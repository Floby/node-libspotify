/*
 * =====================================================================================
 *
 *       Filename:  player.cc
 *
 *    Description:  Bindings for the player part of the session module
 *
 *        Version:  1.0
 *        Created:  24/12/2012 18:16:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Florent Jaby (FJ), florent.jaby@gmail.com
 *        Company:  Florent Jaby
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include "common.h"
#include "audio.h"

using namespace v8;
using namespace nsp;
using namespace node;

#define BUFFER_SIZE 44100
// one second buffer

static audio_fifo_t g_audiofifo;
static bool alive = false;
static bool workeralive = false;

/**
 * spotify callback for the end_of_track event. It's in here because at some point we'll need
 * to be able to check the audio buffer to see if it's been sent fully before
 * sending the end_of_track event
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
extern void call_end_of_track_callback(sp_session* session) {
  ObjectHandle<sp_session>* s = (ObjectHandle<sp_session>*) sp_session_userdata(session);
  Local<Object> o = Nan::New(s->object);
  Local<Value> cbv = o->Get(Nan::GetCurrentContext(), Nan::New<String>("end_of_track").ToLocalChecked()).ToLocalChecked();
  if(!cbv->IsFunction()) {
    return;
  }
  Nan::Callback *cb = new Nan::Callback(cbv.As<Function>());

  const unsigned int argc = 0;
  Local<Value> argv[argc] = {};
  cb->Call(argc, argv);
}

/**
 * spotify callback for the music_delivery event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 * In this, we are called from an internal spotify thread. We buffer the data in the audio_fifo structure
 */
extern int call_music_delivery_callback(sp_session* session, const sp_audioformat *format, const void *frames, int num_frames) {
  audio_fifo_t *af = &g_audiofifo;
  audio_fifo_data_t *afd;
  size_t s;

  if (num_frames == 0) {
    return 0; // Audio discontinuity, do nothing
  }

  // make sure we're the only one using the queue
  pthread_mutex_lock(&af->mutex);

  /* Buffer one second of audio */
  // if more, tell spotify we didn't use any samples
  if (af->qlen > format->sample_rate) {
    pthread_mutex_unlock(&af->mutex);

    return 0;
  }

  // we calculate the memory we need to allocate for these samples
  s = num_frames * sizeof(int16_t) * format->channels;

  afd = (audio_fifo_data_t*) malloc(sizeof(*afd) + s);
  memcpy(afd->samples, frames, s);

  afd->nsamples = num_frames;
  afd->session = session;
  afd->rate = format->sample_rate;
  afd->channels = format->channels;

  TAILQ_INSERT_TAIL(&af->q, afd, link);
  af->qlen += num_frames;

  pthread_cond_signal(&af->cond);
  pthread_mutex_unlock(&af->mutex);

  return num_frames;
}

static void free_music_data(char* data, void* hint) {
  free(hint);
}


/**
 * Load the given track in the player of the given session
 */
NAN_METHOD(Session_Player_Load) {


  assert(info.Length() == 2);
  assert(info[0]->IsObject());
  assert(info[1]->IsObject());

  ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(info[0]);
  ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(info[1]);


  sp_error error = sp_session_player_load(session->pointer, track->pointer);
  NSP_THROW_IF_ERROR(error);

  info.GetReturnValue().SetUndefined();
}

/**
 * starts playing
 */
NAN_METHOD(Session_Player_Play) {


  assert(info.Length() == 2);
  assert(info[0]->IsObject());
  assert(info[1]->IsBoolean());

  ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(info[0]);

  sp_error error = sp_session_player_play(session->pointer, info[1]->BooleanValue());
  NSP_THROW_IF_ERROR(error);

  info.GetReturnValue().SetUndefined();
}

/**
 * async class
 */
class NSPWorker : public Nan::AsyncWorker {
public:
 audio_fifo_t* af;
 audio_fifo_data_t* afd;

 NSPWorker () : Nan::AsyncWorker(NULL) {
  af = &g_audiofifo;
  afd = NULL;
 }

 ~NSPWorker () { }

 void Execute () {
  if (alive && af->qlen > 0) {
    afd = audio_get(af);
  }
 }

protected:
 void HandleOKCallback () {
  while (afd) {
    sp_session* spsession = afd->session;
    ObjectHandle<sp_session>* session = (ObjectHandle<sp_session>*) sp_session_userdata(spsession);

    Local<Value> cbv = Nan::New(session->object)->Get(Nan::New<String>("music_delivery").ToLocalChecked());
    if(!cbv->IsFunction()) {
      workeralive = false;
      return;
    }
    Nan::Callback *callback = new Nan::Callback(cbv.As<Function>());

    Local<Object> buffer = Nan::NewBuffer((char*) afd->samples, afd->nsamples * sizeof(int16_t)* afd->channels, free_music_data, afd).ToLocalChecked();
    buffer->Set(Nan::New<String>("channels").ToLocalChecked(), Nan::New<Number>(afd->channels));
    buffer->Set(Nan::New<String>("rate").ToLocalChecked(), Nan::New<Number>(afd->rate));

    Local<Value> argv[1] = { buffer };
    Local<Value> send_more_data = callback->Call(1, argv);

    assert(send_more_data->IsBoolean());

    // Pause the delivery of data because we have been told that no more data can be handled,
    // it's up to whoever told us to stop to call Session_Player_Stream_Resume to resume data
    if (!send_more_data->ToBoolean()->Value()) {
      alive = false;
    }

    afd = NULL;
  }

  if (alive) {
    Nan::AsyncQueueWorker(new NSPWorker());
    workeralive = true;
  } else {
    workeralive = false;
  }
 }

 void HandleErrorCallback () {}
};

/**
 * starts player thread
 */
NAN_METHOD(Session_Player_Stream_Resume) {


  alive = true;

  if (alive && !workeralive) {
    Nan::AsyncQueueWorker(new NSPWorker());
  }

  info.GetReturnValue().SetUndefined();
}

/**
 * stops player thread
 */
NAN_METHOD(Session_Player_Stop) {


  alive = false;

  info.GetReturnValue().SetUndefined();
}

void nsp::init_player(Local<Object> target) {
  Nan::Export(target, "session_player_load", Session_Player_Load);
  Nan::Export(target, "session_player_play", Session_Player_Play);
  Nan::Export(target, "session_player_stream_resume", Session_Player_Stream_Resume);
  Nan::Export(target, "session_player_stop", Session_Player_Stop);

  audio_fifo_t* af = &g_audiofifo;
  TAILQ_INIT(&af->q);
  af->qlen = 0;

  pthread_mutex_init(&af->mutex, NULL);
  pthread_cond_init(&af->cond, NULL);
}

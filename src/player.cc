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
static int pause_delivery;

/**
 * spotify callback for the end_of_track event. It's in here because at some point we'll need
 * to be able to check the audio buffer to see if it's been sent fully before
 * sending the end_of_track event
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
extern void call_end_of_track_callback(sp_session* session) {
    ObjectHandle<sp_session>* s = (ObjectHandle<sp_session>*) sp_session_userdata(session);
    Handle<Object> o = s->object;
    Handle<Value> cbv = o->Get(String::New("end_of_track"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 0;
    Local<Value> argv[argc] = {};
    cb->Call(Context::GetCurrent()->Global(), argc, argv);
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

static void read_delivered_music(uv_timer_t* handle, int status) {
    audio_fifo_t* af = &g_audiofifo;
    audio_fifo_data_t* afd;

    if (af->qlen == 0) {
		return;
    }
	
    while(af->qlen > 0 && !pause_delivery) {
        afd = audio_get(af);
        if(!afd) {
            break;
        }
		
		HandleScope scope;
		
        sp_session* spsession = afd->session;
        ObjectHandle<sp_session>* session = (ObjectHandle<sp_session>*) sp_session_userdata(spsession);

        Handle<Value> cbv = session->object->Get(String::New("music_delivery"));
        if(!cbv->IsFunction()) {
            return;
        }
        Handle<Function> cb = Local<Function>(Function::Cast(*cbv));

        Buffer* buffer = Buffer::New((char*) afd->samples, afd->nsamples * sizeof(int16_t)* afd->channels, free_music_data, afd);
        buffer->handle_->Set(String::New("channels"), Number::New(afd->channels));
        buffer->handle_->Set(String::New("rate"), Number::New(afd->rate));

        Local<Value> argv[1] = { Local<Value>::New(buffer->handle_) };
        Handle<Value> send_more_data = cb->Call(Context::GetCurrent()->Global(), 1, argv);
        
		assert(send_more_data->IsBoolean());
        
        // Pause the delivery of data because we have been told that no more data can be handled,
        // it's up to whoever told us to stop to call Session_Player_Stream_Resume to resume data
        if (!send_more_data->ToBoolean()->Value()) {
			pause_delivery = 1;
        }
        
        scope.Close(Undefined());
    }

    return;
}


/**
 * Load the given track in the player of the given session
 */
static Handle<Value> Session_Player_Load(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 2);
    assert(args[0]->IsObject());
    assert(args[1]->IsObject());

    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);
    ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(args[1]);


    sp_error error = sp_session_player_load(session->pointer, track->pointer);
    NSP_THROW_IF_ERROR(error);

    return scope.Close(Undefined());
}

/**
 * starts playing
 */
static Handle<Value> Session_Player_Play(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 2);
    assert(args[0]->IsObject());
    assert(args[1]->IsBoolean());

    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);

    sp_error error = sp_session_player_play(session->pointer, args[1]->BooleanValue());
    NSP_THROW_IF_ERROR(error);
	
	pause_delivery = 0;
	
    return scope.Close(Undefined());
}

/**
 *  more data is required by the stream
 */
static Handle<Value> Session_Player_Stream_Resume(const Arguments& args) {
    HandleScope scope;
	
	pause_delivery = 0;
	
    return scope.Close(Undefined());
}

static uv_timer_t read_music_handle;

void nsp::init_player(Handle<Object> target) {
    NODE_SET_METHOD(target, "session_player_load", Session_Player_Load);
    NODE_SET_METHOD(target, "session_player_play", Session_Player_Play);
    NODE_SET_METHOD(target, "session_player_stream_resume", Session_Player_Stream_Resume);

    audio_fifo_t* af = &g_audiofifo;
	TAILQ_INIT(&af->q);
	af->qlen = 0;

	pthread_mutex_init(&af->mutex, NULL);
	pthread_cond_init(&af->cond, NULL);

    uv_timer_init(uv_default_loop(), &read_music_handle);
    uv_timer_start(&read_music_handle, read_delivered_music, 100, 100);
    uv_unref((uv_handle_t*) &read_music_handle);
}

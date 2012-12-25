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

#include <math.h>
#include "common.h"

using namespace v8;
using namespace nsp;
using namespace node;

#define BUFFER_SIZE 44100
// one second buffer


// we make a circular buffer
static int16_t buffer[BUFFER_SIZE];
static unsigned int read_index = 0;
static unsigned int write_index = 0;
static bool buffer_full = false;

static sp_session* current_session;


/**
 * spotify callback for the music_delivery event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
extern int call_music_delivery_callback(sp_session* session, const sp_audioformat *format, const void *frames, int num_frames) {
    //copy these values here
    unsigned int ri = read_index;
    unsigned int wi = write_index;

    // if there is no frame to process, there is an audio discontinuity, let's do nothing
    if(num_frames == 0) return 0;

    // if our buffer is full, let's just do nothing
    if(buffer_full == true) return 0;
    
    // I don't know how to switch audio format, so let's just ignore what we don't know how to process
    if(format->channels != 2 || format->sample_rate != 44100) {
        return 0;
    }

    // calculate how much room we have in our buffer
    int available = (ri - wi); 
    if(available == 0 && false == buffer_full) available = BUFFER_SIZE;
    // if the read index is above its limit, we'll have to copy in two times
    if(wi > ri) {
        // available is negative
        available = (BUFFER_SIZE + available);
    }
    int frames_consumed = fmin(num_frames, (available / format->channels));
    int frames_ignored = num_frames - frames_consumed;

    if(wi < ri) {
        memcpy(&buffer[wi], frames, sizeof(int16_t) * frames_consumed * format->channels);
    }
    // if the read index is above its limit (read index), we'll have to copy in two times
    else {
        int frames_tail = fmin(frames_consumed, (BUFFER_SIZE - wi) / format->channels);
        int frames_head = fmin(0, frames_consumed - frames_tail);
        int size_frame_tail = sizeof(int16_t) * frames_tail * format->channels;
        memcpy(&buffer[wi], frames, size_frame_tail);
        if(frames_head) {
            memcpy(buffer, (int16_t*)frames + size_frame_tail*format->channels, sizeof(int16_t) * frames_head * format->channels);
        }
    }
    wi = (wi + (frames_consumed * format->channels)) % BUFFER_SIZE;

    write_index = wi;
    buffer_full = (frames_ignored > 0);

    current_session = session;
    return frames_ignored;
}

static void free_music_data(char* data, void* hint) {
    delete [] data;
}

static void read_delivered_music(uv_idle_t* handle, int status) {
    unsigned int wi = write_index, ri = read_index;
    if(wi == ri && buffer_full == false) return;

    int to_read = wi - ri;
    if(to_read < 0) {
        to_read = BUFFER_SIZE + to_read;
    }

    int16_t* data = new int16_t[to_read];

    if(ri < wi) {
        memcpy(data, &buffer[ri], to_read);
    }
    else {
        int read_tail = BUFFER_SIZE - to_read;
        memcpy(data, &buffer[ri], read_tail);
        memcpy(data + read_tail, buffer, to_read - read_tail);
    }
    ri = (ri+to_read) % BUFFER_SIZE;
    read_index = ri;
    buffer_full = false;

    Buffer* buffer = Buffer::New((char*)data, to_read * sizeof(int16_t), free_music_data, NULL);

    sp_session* spsession = current_session;
    ObjectHandle<sp_session>* session = (ObjectHandle<sp_session>*) sp_session_userdata(spsession);
    
    Handle<Value> cbv = session->object->Get(String::New("music_delivery"));
    if(!cbv->IsFunction()) return;

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 1;
    Local<Value> argv[argc] = { Local<Value>::New(buffer->handle_) };
    cb->Call(Context::GetCurrent()->Global(), argc, argv);

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

    return scope.Close(Undefined());
}

static uv_idle_t read_music_handle;

void nsp::init_player(Handle<Object> target) {
    NODE_SET_METHOD(target, "session_player_load", Session_Player_Load);
    NODE_SET_METHOD(target, "session_player_play", Session_Player_Play);

    uv_idle_init(uv_default_loop(), &read_music_handle);
    uv_idle_start(&read_music_handle, read_delivered_music);
    uv_unref((uv_handle_t*) &read_music_handle);
}

/*
 * =====================================================================================
 *
 *       Filename:  session.cc
 *
 *    Description:  Bindings for the sp_session subsystem
 *
 *        Version:  1.0
 *        Created:  16/12/2012 21:38:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Florent Jaby (FJ), florent.jaby@gmail.com
 *        Company:  Florent Jaby
 *
 * =====================================================================================
 */

#include "common.h"
#include "playlistcallbacks.cc"
#include <stdlib.h>

using namespace v8;
using namespace nsp;

Handle<Value> nsp::JsNoOp(const Arguments& args) {
    return args.This();
}

/*
 * The following callback function do nothing more than getting the session object and calling their
 * Javascript counterparts
 */

/**
 * spotify callback for the logged_in event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_logged_in_callback(sp_session* session, sp_error error) {
    ObjectHandle<sp_session>* s = (ObjectHandle<sp_session>*) sp_session_userdata(session);
    Handle<Object> o = s->object;
    Handle<Value> cbv = o->Get(String::New("logged_in"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 1;
    Handle<Value> err = Null();
    if(error != SP_ERROR_OK) {
        err = Exception::Error(String::New(sp_error_message(error)));
    }
    Local<Value> argv[argc] = { Local<Value>::New(err) };
    cb->Call(Context::GetCurrent()->Global(), argc, argv);

    return;
}

/**
 * spotify callback for the logged_out event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_logged_out_callback(sp_session* session) {
    ObjectHandle<sp_session>* s = (ObjectHandle<sp_session>*) sp_session_userdata(session);
    Handle<Object> o = s->object;
    Handle<Value> cbv = o->Get(String::New("logged_out"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 0;
    Local<Value> argv[argc] = {};
    cb->Call(Context::GetCurrent()->Global(), argc, argv);

    return;
}

/**
 * spotify callback for the metadata_updated event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_metadata_updated_callback(sp_session* session) {
  	ObjectHandle<sp_session>* s = (ObjectHandle<sp_session>*) sp_session_userdata(session);
    Handle<Object> o = s->object;
    Handle<Value> cbv = o->Get(String::New("metadata_updated"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 0;
    Local<Value> argv[argc] = {};
    cb->Call(Context::GetCurrent()->Global(), argc, argv);

    return;
}

/**
 * spotify callback for the connection_error event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_connection_error_callback(sp_session* session, sp_error error) {
}

/**
 * spotify callback for the message_to_user event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_message_to_user_callback(sp_session* session, const char* message) {
}


uv_timer_t do_notify_handle; ///> uv loop handle for notifying main thread

/**
 * since the notify_main_thread is not called from the main thread
 * we have to set a timer in order to execute the JS callback at the right moment
 */
static void do_call_notify_main_thread_callback(uv_timer_t* handle, int status) {
    sp_session* session = (sp_session*) handle->data;
    ObjectHandle<sp_session>* s = (ObjectHandle<sp_session>*) sp_session_userdata(session);
    Handle<Object> o = s->object;
    Handle<Value> cbv = o->Get(String::New("notify_main_thread"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 0;
    Local<Value> argv[argc] = {};
    cb->Call(Context::GetCurrent()->Global(), argc, argv);

    return;
}

/**
 * spotify callback for the notify_main_thread event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_notify_main_thread_callback(sp_session* session) {
    uv_timer_init(uv_default_loop(), &do_notify_handle);
    do_notify_handle.data = session;

    // set the loop to call our JS callback in 3 ms
    // TODO how about next tick ?
    uv_timer_start(&do_notify_handle, &do_call_notify_main_thread_callback, 1, 0);
}

/**
 * spotify callback for the music_delivery event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 * implemented in player.cc
 */
extern int call_music_delivery_callback(sp_session* session, const sp_audioformat *format, const void *frames, int num_frames);

/**
 * spotify callback for the play_token_lost event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_play_token_lost_callback(sp_session* session) {
	ObjectHandle<sp_session>* s = (ObjectHandle<sp_session>*) sp_session_userdata(session);
    Handle<Object> o = s->object;
    Handle<Value> cbv = o->Get(String::New("play_token_lost"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 0;
    Local<Value> argv[argc] = {};
    cb->Call(Context::GetCurrent()->Global(), argc, argv);

    return;
}

/**
 * spotify callback for the log_message event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_log_message_callback(sp_session* session, const char* data) {
}

/**
 * spotify callback for the end_of_track event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 * implemented in player.cc
 */
extern void call_end_of_track_callback(sp_session* session);

/**
 * spotify callback for the streaming_error event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_streaming_error_callback(sp_session* session, sp_error error) {
}

/**
 * spotify callback for the userinfo_updated event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_userinfo_updated_callback(sp_session* session) {
}

/**
 * spotify callback for the start_playback event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_start_playback_callback(sp_session* session) {
}

/**
 * spotify callback for the stop_playback event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_stop_playback_callback(sp_session* session) {
}

/**
 * spotify callback for the get_audio_buffer_stats event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_get_audio_buffer_stats_callback(sp_session* session, sp_audio_buffer_stats* stats) {
}

/**
 * spotify callback for the offline_status_updated event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_offline_status_updated_callback(sp_session* session) {
}

/**
 * spotify callback for the offline_error event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_offline_error_callback(sp_session* session, sp_error error) {
}

/**
 * spotify callback for the credentials_blob_updated event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_credentials_blob_updated_callback(sp_session* session, const char* blob) {
}

/**
 * spotify callback for the connectionstate_updated event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_connectionstate_updated_callback(sp_session* session) {
}

/**
 * spotify callback for the scrobble_error event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_scrobble_error_callback(sp_session* session, sp_error error) {
}

/**
 * spotify callback for the private_session_mode_changed event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__session__callbacks.html
 */
static void call_private_session_mode_changed_callback(sp_session* session, bool is_private) {
}

static sp_session_callbacks spcallbacks = {
    &call_logged_in_callback,
    &call_logged_out_callback,
    &call_metadata_updated_callback,
    &call_connection_error_callback,
    &call_message_to_user_callback,
    &call_notify_main_thread_callback,
    &call_music_delivery_callback,
    &call_play_token_lost_callback,
    &call_log_message_callback,
    &call_end_of_track_callback,
    &call_streaming_error_callback,
    &call_userinfo_updated_callback,
    &call_start_playback_callback,
    &call_stop_playback_callback,
    &call_get_audio_buffer_stats_callback,
    &call_offline_status_updated_callback,
    &call_offline_error_callback,
    &call_credentials_blob_updated_callback,
    &call_connectionstate_updated_callback,
    &call_scrobble_error_callback,
    &call_private_session_mode_changed_callback
};




/**
 * JS session_config implementation. This just creates a sp_session_config struct
 * from a JS object values and wraps it in a new JS object
 */
static Handle<Value> Session_Config(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // create the handle for this object
    ObjectHandle<sp_session_config>* session_config = new ObjectHandle<sp_session_config>("sp_session_config");

    // allocate the data structure
    sp_session_config* ptr = session_config->pointer = new sp_session_config;

    // set 0 in every field so that spotify doesn't complain
    memset(ptr, 0, sizeof(sp_session_config));

    Handle<Object> obj = args[0]->ToObject();

    ptr->api_version                    = SPOTIFY_API_VERSION;
    ptr->cache_location                 = NSP_STRING_KEY(obj, "cache_location");
    ptr->settings_location              = NSP_STRING_KEY(obj, "settings_location");
    ptr->user_agent                     = NSP_STRING_KEY(obj, "user_agent");
    ptr->compress_playlists             = NSP_BOOL_KEY(obj, "compress_playlists");
    ptr->dont_save_metadata_for_playlists = NSP_BOOL_KEY(obj, "dont_save_metadata_for_playlists");
    ptr->initially_unload_playlists     = NSP_BOOL_KEY(obj, "initially_unload_playlists");
    ptr->device_id                      = NSP_STRING_KEY(obj, "device_id");
    ptr->proxy                          = NSP_STRING_KEY(obj, "proxy");
    ptr->proxy_username                 = NSP_STRING_KEY(obj, "proxy_username");
    ptr->proxy_password                 = NSP_STRING_KEY(obj, "proxy_password");
    // ptr->ca_certs_filename              = NSP_STRING_KEY(obj, "ca_certs_filename");
    ptr->tracefile                      = NSP_STRING_KEY(obj, "tracefile");

    ptr->application_key                = NSP_BUFFER_KEY(obj, "application_key");
    ptr->application_key_size           = NSP_BUFFERLENGTH_KEY(obj, "application_key");
    ptr->callbacks = &spcallbacks;


    // copy everything from the original JS object into the new one
    // so that it can be read later
    Handle<Array> properties = obj->GetOwnPropertyNames();
    for (unsigned int i = 0; i < properties->Length(); ++i) {
        session_config->object->Set(
            properties->Get(i),
            obj->Get(properties->Get(i))
        );
    }

    return scope.Close(session_config->object);
}

/**
 * JS session_create implementation. This unwraps the config argument and calls
 * sp_session_create. The session is then wrapped in a JS object for use in JS land
 */
static Handle<Value> Session_Create(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);

    // create a new handle for the session struct
    ObjectHandle<sp_session>* session = new ObjectHandle<sp_session>("sp_session");

    // unwraps config struct from the first arguments
    ObjectHandle<sp_session_config>* session_config = ObjectHandle<sp_session_config>::Unwrap(args[0]);

    // set the current session ObjectHandle as session userdata for later retrieval
    session_config->pointer->userdata = session;

    // actually call session_create
    sp_error error = sp_session_create(session_config->pointer, &session->pointer);
    NSP_THROW_IF_ERROR(error);

    return scope.Close(session->object);
}

/**
 * JS session_release implementation. This function unwraps the session for the given object
 * and calls sp_session_release on it
 */
static Handle<Value> Session_Release(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);

    // unwrap the ObjectHandle for the session
    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);

    // that would be unfortunate...
    assert(NULL != session->pointer);

    // actually call sp_session_release
    sp_error error = sp_session_release(session->pointer);
    NSP_THROW_IF_ERROR(error);

    // make sure we won't be used this pointer ever again
    session->pointer = NULL;

    return scope.Close(Undefined());
}

/*
 * JS session_login implementation. This function unwraps the session from the given object
 * and calls sp_session_login with the given credentials
 * TODO support for remember_me and credential blobs
 */
static Handle<Value> Session_Login(const Arguments& args) {
    HandleScope scope;

    // check parameters sanity
    assert(args.Length() == 3);
    assert(args[0]->IsObject());
    assert(args[1]->IsString());
    assert(args[2]->IsString());

    // unwrap the session from the given object
    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);

    // actually call sp_session_login
    sp_error error = sp_session_login(
        session->pointer,
        *(String::Utf8Value(args[1]->ToString())),
        *(String::Utf8Value(args[2]->ToString())),
        false,
        NULL
    );
    NSP_THROW_IF_ERROR(error);

    return scope.Close(Undefined());
}

/**
 * JS session_logout implementation
 */
static Handle<Value> Session_Logout(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);
    sp_error error = sp_session_logout(session->pointer);
    NSP_THROW_IF_ERROR(error);

    return scope.Close(Undefined());
}

/**
 * JS session_process_events implementation. This function unwraps the session handle
 * and calls sp_session_process_events. libspotify uses this to process all pending events
 * from the main thread
 * @return next_timeout when in milliseconds to call this function again
 */
static Handle<Value> Session_Process_Events(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);
    int next_timeout = 0;
    sp_error error = sp_session_process_events(session->pointer, &next_timeout);
    NSP_THROW_IF_ERROR(error);

    return scope.Close(Number::New(next_timeout));
}

/**
 * JS session_playlistcontainer implementation. This function unwraps the session handle
 * and calls sp_session_playlistcontainer. this will return the sp_playlistcontainer for the currently logged in user
 */
static Handle<Value> Session_PlaylistContainer(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);

    sp_playlistcontainer* spplaylistcontainer = sp_session_playlistcontainer(session->pointer);
       
    ObjectHandle<sp_playlistcontainer>* playlistcontainer = new ObjectHandle<sp_playlistcontainer>("sp_playlistcontainer");
    playlistcontainer->pointer = spplaylistcontainer;
       
	// actually call sp_playlistcontainer_add_callbacks
    sp_error error = sp_playlistcontainer_add_callbacks(spplaylistcontainer, &nsp_playlistcontainer_callbacks, playlistcontainer);
	NSP_THROW_IF_ERROR(error);
   
    return scope.Close(playlistcontainer->object);
}

void nsp::init_session(Handle<Object> target) {
    NODE_SET_METHOD(target, "session_config", Session_Config);
    NODE_SET_METHOD(target, "session_create", Session_Create);
    NODE_SET_METHOD(target, "session_release", Session_Release);
    NODE_SET_METHOD(target, "session_login", Session_Login);
    NODE_SET_METHOD(target, "session_logout", Session_Logout);
    NODE_SET_METHOD(target, "session_process_events", Session_Process_Events);
    NODE_SET_METHOD(target, "session_playlistcontainer", Session_PlaylistContainer);
}

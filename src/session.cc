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
#include <stdlib.h>

using namespace v8;
using namespace nsp;

Handle<Value> nsp::JsNoOp(const Arguments& args) {
    return args.This();
}

static void call_logged_in_callback(sp_session* session, sp_error error) {
    fprintf(stderr, "KIKOO\n");
    exit(1);
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
static void call_logged_out_callback(sp_session* session) {
}
static void call_metadata_updated_callback(sp_session* session) {
}
static void call_connection_error_callback(sp_session* session, sp_error error) {
}
static void call_message_to_user_callback(sp_session* session, const char* message) {
}
static void call_notify_main_thread_callback(sp_session* session) {
}
static int call_music_delivery_callback(sp_session* session, const sp_audioformat *format, const void *frames, int num_frames) {
    return 0;
}
static void call_play_token_lost_callback(sp_session* session) {
}
static void call_log_message_callback(sp_session* session, const char* data) {
}
static void call_end_of_track_callback(sp_session* session) {
}
static void call_streaming_error_callback(sp_session* session, sp_error error) {
}
static void call_userinfo_updated_callback(sp_session* session) {
}
static void call_start_playback_callback(sp_session* session) {
}
static void call_stop_playback_callback(sp_session* session) {
}
static void call_get_audio_buffer_stats_callback(sp_session* session, sp_audio_buffer_stats* stats) {
}
static void call_offline_status_updated_callback(sp_session* session) {
}
static void call_offline_error_callback(sp_session* session, sp_error error) {
}
static void call_credentials_blob_updated_callback(sp_session* session, const char* blob) {
}
static void call_connectionstate_updated_callback(sp_session* session) {
}
static void call_scrobble_error_callback(sp_session* session, sp_error error) {
}
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
    &call_private_session_mode_changed_callback,
};

static Handle<Value> Session_Config(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // create the handle for this object
    ObjectHandle<sp_session_config>* session_config = new ObjectHandle<sp_session_config>("sp_session_config");

    // allocate the data structure
    //sp_session_config* ptr = &sconfig;
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
    ptr->ca_certs_filename              = NSP_STRING_KEY(obj, "ca_certs_filename");
    ptr->tracefile                      = NSP_STRING_KEY(obj, "tracefile");

    ptr->application_key                = NSP_BUFFER_KEY(obj, "application_key");
    ptr->application_key_size           = NSP_BUFFERLENGTH_KEY(obj, "application_key");
    ptr->callbacks = &spcallbacks;
    // TODO callbacks


    Handle<Array> properties = obj->GetOwnPropertyNames();
    for (unsigned int i = 0; i < properties->Length(); ++i) {
        session_config->object->Set(
            properties->Get(i),
            obj->Get(properties->Get(i))
        );
    }

    return scope.Close(session_config->object);
}

static Handle<Value> Session_Create(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);

    ObjectHandle<sp_session>* session = new ObjectHandle<sp_session>("sp_session");
    ObjectHandle<sp_session_config>* session_config = ObjectHandle<sp_session_config>::Unwrap(args[0]);
    session_config->pointer->userdata = session;

    sp_error error = sp_session_create(session_config->pointer, &session->pointer);
    NSP_THROW_IF_ERROR(error);

    return scope.Close(session->object);
}

static Handle<Value> Session_Release(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);

    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);
    sp_error error = sp_session_release(session->pointer);
    NSP_THROW_IF_ERROR(error);

    delete session;

    return scope.Close(Undefined());
}

static Handle<Value> Session_Login(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 3);
    assert(args[0]->IsObject());
    assert(args[1]->IsString());
    assert(args[2]->IsString());

    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);
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

static Handle<Value> Session_Process_Events(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);
    int next_timeout = 0;
    fprintf(stderr, "processing events from C...\n");
    sp_error error = sp_session_process_events(session->pointer, &next_timeout);
    NSP_THROW_IF_ERROR(error);

    return scope.Close(Number::New(next_timeout));
}

void nsp::init_session(Handle<Object> target) {
    NODE_SET_METHOD(target, "session_config", Session_Config);
    NODE_SET_METHOD(target, "session_create", Session_Create);
    NODE_SET_METHOD(target, "session_release", Session_Release);
    NODE_SET_METHOD(target, "session_login", Session_Login);
    NODE_SET_METHOD(target, "session_process_events", Session_Process_Events);
}

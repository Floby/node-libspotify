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

using namespace v8;
using namespace nsp;

Handle<Value> nsp::JsNoOp(const Arguments& args) {
    return args.This();
}

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

void nsp::init_session(Handle<Object> target) {
    NODE_SET_METHOD(target, "session_config", Session_Config);
    NODE_SET_METHOD(target, "session_create", Session_Create);
    NODE_SET_METHOD(target, "session_release", Session_Release);
    NODE_SET_METHOD(target, "session_login", Session_Login);
}

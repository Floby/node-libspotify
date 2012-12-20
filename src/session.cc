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

#include "session.h"
#include "common.h"

using namespace v8;
using namespace nsp;
static unsigned int scount = 0;

void Session::onMainThreadNotified(sp_session* spsession) {
    Session* session = static_cast<Session*>(sp_session_userdata(spsession));
    session->do_schedule_processing_events = 100;
}

void Session::Idler(uv_idle_t* idler_handle, int status) {
    Session* session = (Session*) idler_handle->data;
    if(session->do_schedule_processing_events > 0) {
        session->scheduleProcessingEvents(session->do_schedule_processing_events);
        session->do_schedule_processing_events = 0;
    }
}


void Session::onLoggedIn(sp_session* spsession, sp_error error) {
    HandleScope scope;

    if(error != SP_ERROR_OK) {
        fprintf(stderr, "Error %s\n", sp_error_message(error));
        exit(1);
    }
    printf("LOGGED IN!\n");
    
    Handle<Value> argv[1] = {
        String::New("login")
    };

    Session* session = static_cast<Session*>(sp_session_userdata(spsession));

    node::MakeCallback(session->handle_, "emit", 1, argv);
}

static void process_events_timer_callback(uv_timer_t* handle, int status) {
    Session* session = static_cast<Session*>(handle->data);
    session->processEvents();
}

void Session::processEvents() {
    int next_timeout = 1000;
    fprintf(stderr, "processing events...\n");
    sp_session_process_events(this->spsession, &next_timeout);

    this->scheduleProcessingEvents(next_timeout);

    fprintf(stderr, "processing again in %dms\n", next_timeout);
}

void Session::scheduleProcessingEvents(unsigned int timeout) {
    if(processing_scheduled == true) {
        uv_timer_stop(&process_events_handle);
        processing_scheduled = false;
    }
    uv_timer_init(uv_default_loop(), &process_events_handle);
    process_events_handle.data = this;
    uv_timer_start(&process_events_handle, process_events_timer_callback, timeout, 0);
    processing_scheduled = true;
}


Session::Session(v8::Handle<v8::Object> buffer) : spsession(NULL) {
    this->number = ++scount;
    processing_scheduled = false;
    do_schedule_processing_events = 0;

    // register idle watcher
    uv_idle_init(uv_default_loop(), &idler_handle);
    idler_handle.data = this;
    uv_idle_start(&idler_handle, Idler);


    g_appkey = (uint8_t*) node::Buffer::Data(buffer);
    g_appkey_size = node::Buffer::Length(buffer);

    //initAppKey();
    this->initCallbacks();
    this->initConfig();

    sp_error error;
    error = sp_session_create(&spconfig, &spsession);
    if(error != SP_ERROR_OK) {
        fprintf(stderr, "Error: %s\n", sp_error_message(error));
        exit(1);
    }

    this->scheduleProcessingEvents(10);
}

Session::~Session() {
};

v8::Handle<v8::Value> Session::Login(const v8::Arguments& args) {
    HandleScope scope;
    String::Utf8Value login(args[0]->ToString());
    String::Utf8Value password(args[1]->ToString());

    Session* session = ObjectWrap::Unwrap<Session>(args.This());

    sp_session_login(session->spsession, *login, *password, true, NULL);

    return scope.Close(v8::Undefined());
}

Handle<Value> Session::Close(const Arguments& args) {
    HandleScope scope;

    Session* session = node::ObjectWrap::Unwrap<Session>(args.This());

    if(session->processing_scheduled) {
        uv_timer_stop(&session->process_events_handle);
    }
    uv_idle_stop(&session->idler_handle);

    sp_error error = sp_session_release(session->spsession);
    if(error != SP_ERROR_OK) {
        return ThrowException(
            Exception::Error(String::New(sp_error_message(error)))
        );
    }
    session->spsession = NULL;

    return scope.Close(v8::Undefined());
}

Handle<Value> Session::HasSession(const Arguments& args) {
    HandleScope scope;
    
    Session* session = node::ObjectWrap::Unwrap<Session>(args.This());
    bool res = (NULL != session->spsession);

    return scope.Close(v8::Boolean::New(res));
}

static v8::Handle<v8::Value> bidule(const v8::Arguments& args) {
    printf("chose\n");
    return v8::Null();
}
void Session::Init(Handle<Object> target) {
    // prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(Session::New);
    tpl->SetClassName(String::NewSymbol("Session"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // prototype
    tpl->PrototypeTemplate()->Set(
            String::NewSymbol("bidule"),
            FunctionTemplate::New(bidule)->GetFunction()
    );
    tpl->PrototypeTemplate()->Set(
            String::NewSymbol("login"),
            FunctionTemplate::New(Session::Login)->GetFunction()
    );
    tpl->PrototypeTemplate()->Set(
            String::NewSymbol("close"),
            FunctionTemplate::New(Session::Close)->GetFunction()
    );
    tpl->PrototypeTemplate()->Set(
            String::NewSymbol("hasSession"),
            FunctionTemplate::New(Session::HasSession)->GetFunction()
    );

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("Session"), constructor);
}

Handle<Value> Session::New(const Arguments& args) {
    HandleScope scope;
    Session* session;
    if(args.Length() > 0 && args[0]->IsObject()) {
        session = new Session(args[0]->ToObject());
    }
    else {
        return ThrowException(
            Exception::TypeError(String::New("Session constructor expects a spotify app key"))
        );
    }
    session->Wrap(args.This());

    return args.This();
}

void Session::initCallbacks() {
    memset(&spcallbacks, 0, sizeof(sp_session_callbacks));
    spcallbacks.logged_in = onLoggedIn;
    spcallbacks.notify_main_thread = onMainThreadNotified;
}


void Session::initConfig() {
    memset(&spconfig, 0, sizeof(sp_session_config));
    spconfig.api_version = SPOTIFY_API_VERSION;
    spconfig.cache_location = "tmp";
    spconfig.settings_location = "tmp";
    spconfig.application_key = g_appkey;
    spconfig.application_key_size = g_appkey_size;
    spconfig.user_agent = "node-libspotify";
    spconfig.userdata = this;
    spconfig.callbacks = &spcallbacks;
}

static sp_session_config sconfig;

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

    delete session;

    return scope.Close(Undefined());
}

void nsp::init_session(Handle<Object> target) {
    NODE_SET_METHOD(target, "session_config", Session_Config);
    NODE_SET_METHOD(target, "session_create", Session_Create);
    NODE_SET_METHOD(target, "session_release", Session_Release);
    NODE_SET_METHOD(target, "session_login", Session_Login);
}

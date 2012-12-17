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
    if(error != SP_ERROR_OK) {
        fprintf(stderr, "Error %s\n", sp_error_message(error));
        exit(1);
    }
    printf("LOGGED IN!\n");
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


Session::Session(v8::Handle<v8::Object> buffer) {
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

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("Session"), constructor);
}

Handle<Value> Session::New(const Arguments& args) {
    HandleScope scope;
    Session* session;
    if(args.Length() > 0 && args[0]->IsObject()) {
        fprintf(stderr, "Length: %d\n", node::Buffer::Length(args[0]->ToObject()));
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



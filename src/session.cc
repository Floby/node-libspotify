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

Session::Session() {
};

Session::~Session() {
};

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

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("Session"), constructor);
}

Handle<Value> Session::New(const Arguments& args) {
    HandleScope scope;
    Session* session = new Session();
    session->Wrap(args.This());


    return args.This();
}

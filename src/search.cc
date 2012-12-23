/*
 * =====================================================================================
 *
 *       Filename:  search.cc
 *
 *    Description: bindings to the spotify search submodule 
 *
 *        Version:  1.0
 *        Created:  23/12/2012 16:59:00
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

/**
 * Spotify callback when a search query completed
 */
static void on_search_complete(sp_search* result, void* userdata) {
    ObjectHandle<sp_search>* search = static_cast<ObjectHandle<sp_search>* >(userdata);
    Handle<Value> cbv = search->object->Get(String::New("on_search_complete"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 2;
    sp_error error = sp_search_error(result);
    Handle<Value> err = Null();
    if(error != SP_ERROR_OK) {
        err = Exception::Error(String::New(sp_error_message(error)));
    }
    Local<Value> argv[argc] = { Local<Value>::New(err), Local<Object>::New(search->object) };
    cb->Call(Context::GetCurrent()->Global(), argc, argv);

    return;
}

/**
 * JS search_create implementation. calls sp_search_create
 */
static Handle<Value> Search_Create(const Arguments& args) {
    HandleScope scope;

    // check arguments sanity
    assert(args.Length() == 10);
    assert(args[0]->IsObject());
    assert(args[1]->IsString());
    assert(args[2]->IsNumber());
    assert(args[3]->IsNumber());
    assert(args[4]->IsNumber());
    assert(args[5]->IsNumber());
    assert(args[6]->IsNumber());
    assert(args[7]->IsNumber());
    assert(args[8]->IsNumber());
    assert(args[9]->IsNumber());

    // unwrap the session handle from the given object
    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);

    // create the new handle for the sp_search we are creating
    ObjectHandle<sp_search>* search = new ObjectHandle<sp_search>("sp_search");
    String::Utf8Value query(args[1]);

    // actually call sp_search_create
    search->pointer = sp_search_create(
        session->pointer,
        *query,
        args[2]->ToNumber()->Int32Value(),
        args[3]->ToNumber()->Int32Value(),
        args[4]->ToNumber()->Int32Value(),
        args[5]->ToNumber()->Int32Value(),
        args[6]->ToNumber()->Int32Value(),
        args[7]->ToNumber()->Int32Value(),
        args[8]->ToNumber()->Int32Value(),
        args[9]->ToNumber()->Int32Value(),
        SP_SEARCH_STANDARD,
        &on_search_complete,
        search
    );

    return scope.Close(search->object);
}

/**
 * JS search_num_tracks implementation.
 */
static Handle<Value> Search_Num_Tracks(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    ObjectHandle<sp_search>* search = ObjectHandle<sp_search>::Unwrap(args[0]);
    int num = sp_search_num_tracks(search->pointer);

    return scope.Close(Number::New(num));
}

void nsp::init_search(Handle<Object> target) {
    NODE_SET_METHOD(target, "search_create", Search_Create);
    NODE_SET_METHOD(target, "search_num_tracks", Search_Num_Tracks);
}

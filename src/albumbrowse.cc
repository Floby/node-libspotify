/*
 * =====================================================================================
 *
 *       Filename:  albumbrowse.cc
 *
 *    Description:  bindings for the album subsystem
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Linus Unnebäck, linus@folkdatorn.se
 *        Company:  LinusU AB
 *
 * =====================================================================================
 */


#include "common.h"

using namespace v8;
using namespace nsp;

void cb_albumbrowse_complete (sp_albumbrowse *result, void *userdata) {
    Persistent<Function> callback = static_cast<Function*>(userdata);

    callback->Call(callback, 0, NULL);
    callback.Dispose();
}

static Handle<Value> AlbumBrowse_Create(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 3);
    assert(args[0]->IsObject());  // sp_session
    assert(args[1]->IsObject());  // sp_album
    assert(args[2]->IsFunction()); // callback

    ObjectHandle<sp_session> *session = ObjectHandle<sp_session>::Unwrap(args[0]);
    ObjectHandle<sp_album> *album = ObjectHandle<sp_album>::Unwrap(args[1]);
    Handle<Function> callback = Persistent<Function>::New(Handle<Function>::Cast(args[2]));

    ObjectHandle<sp_albumbrowse>* albumbrowse = new ObjectHandle<sp_albumbrowse>("sp_albumbrowse");
    albumbrowse->pointer = sp_albumbrowse_create(session->pointer, album->pointer, cb_albumbrowse_complete, *callback);

    return scope.Close(albumbrowse->object);
}

static Handle<Value> AlbumBrowse_Num_Tracks(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject()); // sp_albumbrowse

    ObjectHandle<sp_albumbrowse> *albumbrowse = ObjectHandle<sp_albumbrowse>::Unwrap(args[0]);
    const int num = sp_albumbrowse_num_tracks(albumbrowse->pointer);

    return scope.Close(Number::New(num));
}

static Handle<Value> AlbumBrowse_Track(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 2);
    assert(args[0]->IsObject()); // sp_albumbrowse
    assert(args[1]->IsNumber()); // index

    // input
    ObjectHandle<sp_albumbrowse> *albumbrowse = ObjectHandle<sp_albumbrowse>::Unwrap(args[0]);
    int index = args[2]->ToNumber()->Int32Value();

    // output
    sp_track* sptrack = sp_albumbrowse_track(albumbrowse->pointer, index);
    ObjectHandle<sp_track>* track = new ObjectHandle<sp_track>("sp_track");
    track->pointer = sptrack;

    return scope.Close(track->object);
}

static Handle<Value> AlbumBrowse_Release(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject()); // sp_albumbrowse

    ObjectHandle<sp_albumbrowse> *albumbrowse = ObjectHandle<sp_albumbrowse>::Unwrap(args[0]);
    sp_error error = sp_albumbrowse_release(albumbrowse->pointer);
    NSP_THROW_IF_ERROR(error);

    return scope.Close(Undefined());
}

void nsp::init_albumbrowse(Handle<Object> target) {
    NODE_SET_METHOD(target, "albumbrowse_create", AlbumBrowse_Create);
    NODE_SET_METHOD(target, "albumbrowse_num_tracks", AlbumBrowse_Num_Tracks);
    NODE_SET_METHOD(target, "albumbrowse_track", AlbumBrowse_Track);
    NODE_SET_METHOD(target, "albumbrowse_release", AlbumBrowse_Release);
}

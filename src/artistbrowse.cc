/*
 * =====================================================================================
 *
 *       Filename:  artistbrowse.cc
 *
 *    Description:  bindings for the artist subsystem
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

void cb_artistbrowse_complete (sp_artistbrowse *result, void *userdata) {
    Persistent<Function> callback = static_cast<Function*>(userdata);

    callback->Call(callback, 0, NULL);
    callback.Dispose();
}

static Handle<Value> ArtistBrowse_Create(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 3);
    assert(args[0]->IsObject());  // sp_session
    assert(args[1]->IsObject());  // sp_artist
    assert(args[2]->IsFunction()); // callback

    ObjectHandle<sp_session> *session = ObjectHandle<sp_session>::Unwrap(args[0]);
    ObjectHandle<sp_artist> *artist = ObjectHandle<sp_artist>::Unwrap(args[1]);
    Handle<Function> callback = Persistent<Function>::New(Handle<Function>::Cast(args[2]));

    ObjectHandle<sp_artistbrowse>* artistbrowse = new ObjectHandle<sp_artistbrowse>("sp_artistbrowse");
    artistbrowse->pointer = sp_artistbrowse_create(session->pointer, artist->pointer, SP_ARTISTBROWSE_NO_TRACKS, cb_artistbrowse_complete, *callback);

    return scope.Close(artistbrowse->object);
}

static Handle<Value> ArtistBrowse_Num_Albums(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject()); // sp_artistbrowse

    ObjectHandle<sp_artistbrowse> *artistbrowse = ObjectHandle<sp_artistbrowse>::Unwrap(args[0]);
    const int num = sp_artistbrowse_num_albums(artistbrowse->pointer);

    return scope.Close(Number::New(num));
}

static Handle<Value> ArtistBrowse_Album(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 2);
    assert(args[0]->IsObject()); // sp_artistbrowse
    assert(args[1]->IsNumber()); // index

    // input
    ObjectHandle<sp_artistbrowse> *artistbrowse = ObjectHandle<sp_artistbrowse>::Unwrap(args[0]);
    int index = args[1]->ToNumber()->Int32Value();

    // output
    sp_album* spalbum = sp_artistbrowse_album(artistbrowse->pointer, index);
    ObjectHandle<sp_album>* album = new ObjectHandle<sp_album>("sp_album");
    album->pointer = spalbum;

    return scope.Close(album->object);
}

static Handle<Value> ArtistBrowse_Release(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject()); // sp_artistbrowse

    ObjectHandle<sp_artistbrowse> *artistbrowse = ObjectHandle<sp_artistbrowse>::Unwrap(args[0]);
    sp_error error = sp_artistbrowse_release(artistbrowse->pointer);
    NSP_THROW_IF_ERROR(error);

    return scope.Close(Undefined());
}

void nsp::init_artistbrowse(Handle<Object> target) {
    NODE_SET_METHOD(target, "artistbrowse_create", ArtistBrowse_Create);
    NODE_SET_METHOD(target, "artistbrowse_num_albums", ArtistBrowse_Num_Albums);
    NODE_SET_METHOD(target, "artistbrowse_album", ArtistBrowse_Album);
    NODE_SET_METHOD(target, "artistbrowse_release", ArtistBrowse_Release);
}

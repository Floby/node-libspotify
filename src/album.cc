/*
 * =====================================================================================
 *
 *       Filename:  album.cc
 *
 *    Description:  bindings for the album subsystem
 *
 *        Version:  1.0
 *        Created:  03/01/2013 15:18:14
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
 * JS album_is_loaded implementation. checks if a given album is loaded
 */
static Handle<Value> Album_Is_Loaded(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_album pointer from given object
    ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(args[0]);

    // actually call sp_album_is_loaded
    bool loaded = sp_album_is_loaded(album->pointer);

    return scope.Close(Boolean::New(loaded));
}

/**
 * JS album_name implementation. checks if a given album is loaded
 */
static Handle<Value> Album_Name(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_album pointer from given object
    ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(args[0]);

    const char* name = sp_album_name(album->pointer);

    return scope.Close(String::New(name));
}

/**
 * JS album_artist implementation. checks if a given album is loaded
 */
static Handle<Value> Album_Artist(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_album pointer from given object
    ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(args[0]);

    sp_artist* spartist = sp_album_artist(album->pointer);
    ObjectHandle<sp_artist>* artist = new ObjectHandle<sp_artist>("sp_artist");
    artist->pointer = spartist;

    return scope.Close(artist->object);
}


void nsp::init_album(Handle<Object> target) {
    NODE_SET_METHOD(target, "album_is_loaded", Album_Is_Loaded);
    NODE_SET_METHOD(target, "album_name", Album_Name);
    NODE_SET_METHOD(target, "album_artist", Album_Artist);
}


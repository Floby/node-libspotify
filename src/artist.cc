/*
 * =====================================================================================
 *
 *       Filename:  artist.cc
 *
 *    Description:  bindings for the artist subsystem
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
 * JS artist_is_loaded implementation. checks if a given artist is loaded
 */
static Handle<Value> Artist_Is_Loaded(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_artist pointer from given object
    ObjectHandle<sp_artist>* artist = ObjectHandle<sp_artist>::Unwrap(args[0]);

    // actually call sp_artist_is_loaded
    bool loaded = sp_artist_is_loaded(artist->pointer);

    return scope.Close(Boolean::New(loaded));
}

/**
 * JS artist_name implementation. checks if a given artist is loaded
 */
static Handle<Value> Artist_Name(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_artist pointer from given object
    ObjectHandle<sp_artist>* artist = ObjectHandle<sp_artist>::Unwrap(args[0]);

    const char* name = sp_artist_name(artist->pointer);

    return scope.Close(String::New(name));
}

void nsp::init_artist(Handle<Object> target) {
    NODE_SET_METHOD(target, "artist_is_loaded", Artist_Is_Loaded);
    NODE_SET_METHOD(target, "artist_name", Artist_Name);
}

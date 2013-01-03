/*
 * =====================================================================================
 *
 *       Filename:  track.cc
 *
 *    Description:  Bindings for the tracks submodule
 *
 *        Version:  1.0
 *        Created:  24/12/2012 15:32:58
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
 * JS track_is_loaded implementation. checks if a given track is loaded
 */
static Handle<Value> Track_Is_Loaded(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_track pointer from given object
    ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(args[0]);

    // actually call sp_track_is_loaded
    bool loaded = sp_track_is_loaded(track->pointer);

    return scope.Close(Boolean::New(loaded));
}

/**
 * JS track_duration implementation. checks if a given track is loaded
 */
static Handle<Value> Track_Duration(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_track pointer from given object
    ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(args[0]);

    // actually call sp_track_is_loaded
    int duration = sp_track_duration(track->pointer);

    return scope.Close(Number::New(duration));
}

/**
 * JS track_num_artists implementation. checks if a given track is loaded
 */
static Handle<Value> Track_Num_Artists(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_track pointer from given object
    ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(args[0]);

    int num_artists = sp_track_num_artists(track->pointer);

    return scope.Close(Number::New(num_artists));
}

/**
 * JS track_name implementation. checks if a given track is loaded
 */
static Handle<Value> Track_Name(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_track pointer from given object
    ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(args[0]);

    const char* name = sp_track_name(track->pointer);

    return scope.Close(String::New(name));
}

/**
 * JS track_artist implementation. checks if a given track is loaded
 */
static Handle<Value> Track_Artist(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 2);
    assert(args[0]->IsObject());
    assert(args[1]->IsNumber());

    // gets sp_track pointer from given object
    ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(args[0]);
    int index = args[1]->ToNumber()->Int32Value();
    assert(index >= 0);
    assert(index < sp_track_num_artists(track->pointer));

    sp_artist* spartist = sp_track_artist(track->pointer, index);
    ObjectHandle<sp_artist>* artist = new ObjectHandle<sp_artist>("sp_artist");
    artist->pointer = spartist;

    return scope.Close(artist->object);
}

void nsp::init_track(Handle<Object> target) {
    NODE_SET_METHOD(target, "track_is_loaded", Track_Is_Loaded);
    NODE_SET_METHOD(target, "track_duration", Track_Duration);
    NODE_SET_METHOD(target, "track_num_artists", Track_Num_Artists);
    NODE_SET_METHOD(target, "track_name", Track_Name);
    NODE_SET_METHOD(target, "track_artist", Track_Artist);
}

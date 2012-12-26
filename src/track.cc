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

void nsp::init_track(Handle<Object> target) {
    NODE_SET_METHOD(target, "track_is_loaded", Track_Is_Loaded);
    NODE_SET_METHOD(target, "track_duration", Track_Duration);
}

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


/**
 * JS track_album implementation. checks if a given track is loaded
 */
static Handle<Value> Track_Album(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_track pointer from given object
    ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(args[0]);

    sp_album* spalbum = sp_track_album(track->pointer);
    ObjectHandle<sp_album>* album = new ObjectHandle<sp_album>("sp_album");
    album->pointer = spalbum;

    return scope.Close(album->object);
}

/**
 * JS track_get_availability implementation. checks if a given track is loaded
 */
static Handle<Value> Track_Get_Availability(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 2);
    assert(args[0]->IsObject());
    assert(args[1]->IsObject());

    // gets sp_track pointer from given object
    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);
    ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(args[1]);

    sp_availability availability = sp_track_get_availability(session->pointer, track->pointer);
    Handle<Value> res;
    switch (availability) {
        case SP_TRACK_AVAILABILITY_AVAILABLE:
            res = String::New("AVAILABLE");
            break;
        case SP_TRACK_AVAILABILITY_NOT_STREAMABLE:
            res = String::New("NOT_STREAMABLE");
            break;
        case SP_TRACK_AVAILABILITY_BANNED_BY_ARTIST:
            res = String::New("BANNED_BY_ARTIST");
            break;
        case SP_TRACK_AVAILABILITY_UNAVAILABLE:
        default:
            res = String::New("UNAVAILABLE");
            break;
    }

    return scope.Close(res);
}

static Handle<Value> Track_Is_Starred(const Arguments& args) {
    HandleScope scope;
    
    // test arguments sanity
    assert(args.Length() == 2);
    assert(args[0]->IsObject());
    assert(args[1]->IsObject());

    // get session and track pointers from object
    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);
    ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(args[1]);

    bool starred = sp_track_is_starred(session->pointer, track->pointer);

    return scope.Close(Boolean::New(starred));
}

static Handle<Value> Track_Set_Starred(const Arguments& args) {
    HandleScope scope;
    
    // test arguments sanity
    assert(args.Length() == 3);
    assert(args[0]->IsObject());
    assert(args[1]->IsArray());
    assert(args[2]->IsBoolean());

    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);

    Handle<Array> arr = Local<Array>(Array::Cast(*args[1]));
    unsigned int length = arr->Length();
    sp_track* tracks[length];
    for(unsigned int i=0; i<length; ++i) {
        tracks[i] = ObjectHandle<sp_track>::Unwrap(arr->Get(i))->pointer;
    }

    sp_error error = sp_track_set_starred(session->pointer, tracks, length, args[2]->ToBoolean()->BooleanValue());
    NSP_THROW_IF_ERROR(error);

    return scope.Close(Undefined());
}

/**
 * JS track_popularity implementation. checks if a given track is loaded
 */
static Handle<Value> Track_Popularity(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_track pointer from given object
    ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(args[0]);

    // actually call sp_track_popularity
    int popularity = sp_track_popularity(track->pointer);

    return scope.Close(Number::New(popularity));
}

void nsp::init_track(Handle<Object> target) {
    NODE_SET_METHOD(target, "track_is_loaded", Track_Is_Loaded);
    NODE_SET_METHOD(target, "track_duration", Track_Duration);
    NODE_SET_METHOD(target, "track_num_artists", Track_Num_Artists);
    NODE_SET_METHOD(target, "track_name", Track_Name);
    NODE_SET_METHOD(target, "track_artist", Track_Artist);
    NODE_SET_METHOD(target, "track_album", Track_Album);
    NODE_SET_METHOD(target, "track_get_availability", Track_Get_Availability);
    NODE_SET_METHOD(target, "track_is_starred", Track_Is_Starred);
    NODE_SET_METHOD(target, "track_popularity", Track_Popularity);
    NODE_SET_METHOD(target, "track_set_starred", Track_Set_Starred);
}

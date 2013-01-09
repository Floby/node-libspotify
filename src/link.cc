/*
 * =====================================================================================
 *
 *       Filename:  link.cc
 *
 *    Description: bindings for links subsystem 
 *
 *        Version:  1.0
 *        Created:  07/01/2013 12:37:03
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

static Handle<Value> Link_Create_From_Track(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_track pointer from given object
    ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(args[0]);

    // TODO handle length in ms
    sp_link* link = sp_link_create_from_track(track->pointer, 0);
    char url[256];
    // TODO handle truncated urls
    sp_link_as_string(link, url, 256);

    return scope.Close(String::New(url));
}

static Handle<Value> Link_Create_From_Artist(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_artist pointer from given object
    ObjectHandle<sp_artist>* artist = ObjectHandle<sp_artist>::Unwrap(args[0]);

    sp_link* link = sp_link_create_from_artist(artist->pointer);
    char url[256];
    // TODO handle truncated urls
    sp_link_as_string(link, url, 256);

    return scope.Close(String::New(url));
}

static Handle<Value> Link_As_Track(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsString());

    String::Utf8Value url(args[0]);

    sp_link* link = sp_link_create_from_string(*url);
    assert(sp_link_type(link) == SP_LINKTYPE_TRACK);

    ObjectHandle<sp_track>* track = new ObjectHandle<sp_track>("sp_track");
    track->pointer = sp_link_as_track(link);

    return scope.Close(track->object);
}

static Handle<Value> Link_As_Artist(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsString());

    String::Utf8Value url(args[0]);

    sp_link* link = sp_link_create_from_string(*url);
    assert(sp_link_type(link) == SP_LINKTYPE_ARTIST);

    ObjectHandle<sp_artist>* artist = new ObjectHandle<sp_artist>("sp_artist");
    artist->pointer = sp_link_as_artist(link);

    return scope.Close(artist->object);
}


void nsp::init_link(Handle<Object> target) {
    NODE_SET_METHOD(target, "link_create_from_track", Link_Create_From_Track);
    NODE_SET_METHOD(target, "link_create_from_artist", Link_Create_From_Artist);
    NODE_SET_METHOD(target, "link_as_track", Link_As_Track);
    NODE_SET_METHOD(target, "link_as_artist", Link_As_Artist);
}

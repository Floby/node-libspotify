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
#include "imagecallbacks.cc"
#include "playlistcallbacks.cc"

using namespace v8;
using namespace nsp;

template <typename sp_type>
static Handle<Value> Link_Create(const Arguments& args, sp_link* (*link_fn)(sp_type*)) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_type pointer from given object
    ObjectHandle<sp_type>* obj = ObjectHandle<sp_type>::Unwrap(args[0]);

    sp_link* link = (*link_fn)(obj->pointer);

    if(link == NULL) {
        return scope.Close(Null());
    } else {
        // TODO handle truncated urls
        char url[256];
        sp_link_as_string(link, url, 256);

        return scope.Close(String::New(url));
    }
}

template <typename sp_type, typename sp_param>
static Handle<Value> Link_Create(const Arguments& args, sp_link* (*link_fn)(sp_type*, sp_param)) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 2);
    assert(args[0]->IsObject());
    assert(args[1]->IsNumber());

    // gets sp_type pointer from given object
    ObjectHandle<sp_type>* obj = ObjectHandle<sp_type>::Unwrap(args[0]);

    sp_param param = static_cast<sp_param>(args[1]->ToNumber()->Int32Value());
    sp_link* link = (*link_fn)(obj->pointer, param);

    if(link == NULL) {
        return scope.Close(Null());
    } else {
        // TODO handle truncated urls
        char url[256];
        sp_link_as_string(link, url, 256);

        return scope.Close(String::New(url));
    }
}

template <typename sp_type>
static Handle<Value> Link_As(const Arguments& args, sp_linktype type, sp_type* (*link_fn)(sp_link*), const char* name) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsString());

    String::Utf8Value url(args[0]);

    sp_link* link = sp_link_create_from_string(*url);
    assert(sp_link_type(link) == type);

    ObjectHandle<sp_type>* obj = new ObjectHandle<sp_type>(name);
    obj->pointer = (*link_fn)(link);

    return scope.Close(obj->object);
}

static Handle<Value> Link_As(const Arguments& args, sp_linktype type, sp_playlist* (*link_fn)(sp_session*, sp_link*), const char* name) {
    HandleScope scope;

    // test arguments sanity
    assert(type == SP_LINKTYPE_PLAYLIST);
    assert(args.Length() == 2);
    assert(args[0]->IsString());
    assert(args[1]->IsObject());

    String::Utf8Value url(args[0]);

    sp_link* link = sp_link_create_from_string(*url);
    assert(sp_link_type(link) == type);

    // gets sp_session pointer from given object
    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[1]);

    ObjectHandle<sp_playlist>* obj = new ObjectHandle<sp_playlist>(name);
    obj->pointer = link_fn(session->pointer, link);

    // Add callbacks
    sp_error error = sp_playlist_add_callbacks(obj->pointer, &nsp_playlist_callbacks, obj);
    NSP_THROW_IF_ERROR(error);

    return scope.Close(obj->object);
}

static Handle<Value> Link_As(const Arguments& args, sp_linktype type, sp_image* (*link_fn)(sp_session*, sp_link*), const char* name) {
    HandleScope scope;

    // test arguments sanity
    assert(type == SP_LINKTYPE_IMAGE);
    assert(args.Length() == 2);
    assert(args[0]->IsString());
    assert(args[1]->IsObject());

    String::Utf8Value url(args[0]);

    sp_link* link = sp_link_create_from_string(*url);
    assert(sp_link_type(link) == type);

    // gets sp_session pointer from given object
    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[1]);

    ObjectHandle<sp_image>* obj = new ObjectHandle<sp_image>(name);
    obj->pointer = link_fn(session->pointer, link);

    // Add callbacks
    sp_error error = sp_image_add_load_callback(obj->pointer, &cb_image_loaded, obj);
    NSP_THROW_IF_ERROR(error);

    return scope.Close(obj->object);
}

static Handle<Value> Link_Create_From_Track(const Arguments& args) {
    return Link_Create(args, sp_link_create_from_track);
}

static Handle<Value> Link_Create_From_Album(const Arguments& args) {
    return Link_Create(args, sp_link_create_from_album);
}

static Handle<Value> Link_Create_From_Album_Cover(const Arguments& args) {
    return Link_Create(args, sp_link_create_from_album_cover);
}

static Handle<Value> Link_Create_From_Artist(const Arguments& args) {
    return Link_Create(args, sp_link_create_from_artist);
}

static Handle<Value> Link_Create_From_Artist_Portrait(const Arguments& args) {
    return Link_Create(args, sp_link_create_from_artist_portrait);
}

static Handle<Value> Link_Create_From_Search(const Arguments& args) {
    return Link_Create(args, sp_link_create_from_search);
}

static Handle<Value> Link_Create_From_Playlist(const Arguments& args) {
    return Link_Create(args, sp_link_create_from_playlist);
}

static Handle<Value> Link_Create_From_User(const Arguments& args) {
    return Link_Create(args, sp_link_create_from_user);
}

static Handle<Value> Link_Create_From_Image(const Arguments& args) {
    return Link_Create(args, sp_link_create_from_image);
}

static Handle<Value> Link_As_Track(const Arguments& args) {
    return Link_As(args, SP_LINKTYPE_TRACK, sp_link_as_track, "sp_track");
}

static Handle<Value> Link_As_Album(const Arguments& args) {
    return Link_As(args, SP_LINKTYPE_ALBUM, sp_link_as_album, "sp_album");
}

static Handle<Value> Link_As_Artist(const Arguments& args) {
    return Link_As(args, SP_LINKTYPE_ARTIST, sp_link_as_artist, "sp_artist");
}

static Handle<Value> Link_As_User(const Arguments& args) {
    return Link_As(args, SP_LINKTYPE_PROFILE, sp_link_as_user, "sp_user");
}

static Handle<Value> Link_As_Playlist(const Arguments& args) {
    return Link_As(args, SP_LINKTYPE_PLAYLIST, sp_playlist_create, "sp_playlist");
}

static Handle<Value> Link_As_Image(const Arguments& args) {
    return Link_As(args, SP_LINKTYPE_IMAGE, sp_image_create_from_link, "sp_image");
}

static Handle<Value> Link_Type(const Arguments& args) {
    HandleScope scope;
    const char* type = NULL;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsString());

    String::Utf8Value url(args[0]);
    sp_link* link = sp_link_create_from_string(*url);

    if(link) {
        switch(sp_link_type(link)) {
            case SP_LINKTYPE_INVALID:    type = NULL;         break;
            case SP_LINKTYPE_TRACK:      type = "track";      break;
            case SP_LINKTYPE_ALBUM:      type = "album";      break;
            case SP_LINKTYPE_ARTIST:     type = "artist";     break;
            case SP_LINKTYPE_SEARCH:     type = "search";     break;
            case SP_LINKTYPE_PLAYLIST:   type = "playlist";   break;
            case SP_LINKTYPE_PROFILE:    type = "profile";    break;
            case SP_LINKTYPE_STARRED:    type = "starred";    break;
            case SP_LINKTYPE_LOCALTRACK: type = "localtrack"; break;
            case SP_LINKTYPE_IMAGE:      type = "image";      break;
            default: assert(false); break;
        }
    }

    if(type) {
        return scope.Close(String::New(type));
    } else {
        return scope.Close(Null());
    }
}



void nsp::init_link(Handle<Object> target) {
    NODE_SET_METHOD(target, "link_create_from_track", Link_Create_From_Track);
    NODE_SET_METHOD(target, "link_create_from_album", Link_Create_From_Album);
    NODE_SET_METHOD(target, "link_create_from_album_cover", Link_Create_From_Album_Cover);
    NODE_SET_METHOD(target, "link_create_from_artist", Link_Create_From_Artist);
    NODE_SET_METHOD(target, "link_create_from_artist_portrait", Link_Create_From_Artist_Portrait);
    NODE_SET_METHOD(target, "link_create_from_search", Link_Create_From_Search);
    NODE_SET_METHOD(target, "link_create_from_playlist", Link_Create_From_Playlist);
    NODE_SET_METHOD(target, "link_create_from_user", Link_Create_From_User);
    NODE_SET_METHOD(target, "link_create_from_image", Link_Create_From_Image);
    NODE_SET_METHOD(target, "link_as_track", Link_As_Track);
    NODE_SET_METHOD(target, "link_as_album", Link_As_Album);
    NODE_SET_METHOD(target, "link_as_artist", Link_As_Artist);
    NODE_SET_METHOD(target, "link_as_user", Link_As_User);
    NODE_SET_METHOD(target, "link_as_playlist", Link_As_Playlist);
    NODE_SET_METHOD(target, "link_as_image", Link_As_Image);
    NODE_SET_METHOD(target, "link_type", Link_Type);
}

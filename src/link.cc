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
#include "playlistcallbacks.cc"

using namespace v8;
using namespace nsp;

NAN_METHOD(Link_Create_From_Track) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_track pointer from given object
  ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(info[0]);

  // TODO handle length in ms
  sp_link* link = sp_link_create_from_track(track->pointer, 0);
  char url[256];
  // TODO handle truncated urls
  sp_link_as_string(link, url, 256);

  info.GetReturnValue().Set(Nan::New<String>(url).ToLocalChecked());
}

NAN_METHOD(Link_Create_From_Artist) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_artist pointer from given object
  ObjectHandle<sp_artist>* artist = ObjectHandle<sp_artist>::Unwrap(info[0]);

  sp_link* link = sp_link_create_from_artist(artist->pointer);
  char url[256];
  // TODO handle truncated urls
  sp_link_as_string(link, url, 256);

  info.GetReturnValue().Set(Nan::New<String>(url).ToLocalChecked());
}

NAN_METHOD(Link_Create_From_Playlist) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_playlist pointer from given object
  ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(info[0]);

  sp_link* link = sp_link_create_from_playlist(playlist->pointer);
  char url[256];
  // TODO handle truncated urls
  sp_link_as_string(link, url, 256);

  info.GetReturnValue().Set(Nan::New<String>(url).ToLocalChecked());
}

NAN_METHOD(Link_As_Track) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsString());

  String::Utf8Value url(info[0]);

  sp_link* link = sp_link_create_from_string(*url);
  assert(sp_link_type(link) == SP_LINKTYPE_TRACK);

  ObjectHandle<sp_track>* track = new ObjectHandle<sp_track>("sp_track");
  track->pointer = sp_link_as_track(link);

  info.GetReturnValue().Set(Nan::New(track->object));
}

NAN_METHOD(Link_As_Artist) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsString());

  String::Utf8Value url(info[0]);

  sp_link* link = sp_link_create_from_string(*url);
  assert(sp_link_type(link) == SP_LINKTYPE_ARTIST);

  ObjectHandle<sp_artist>* artist = new ObjectHandle<sp_artist>("sp_artist");
  artist->pointer = sp_link_as_artist(link);

  info.GetReturnValue().Set(Nan::New(artist->object));
}

NAN_METHOD(Link_As_Playlist) {


  // test arguments sanity
  assert(info.Length() == 2);
  assert(info[0]->IsString());
  assert(info[1]->IsObject());

  String::Utf8Value url(info[0]);

  sp_link* link = sp_link_create_from_string(*url);
  assert(sp_link_type(link) == SP_LINKTYPE_PLAYLIST);

  // gets sp_session pointer from given object
  ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(info[1]);

  ObjectHandle<sp_playlist>* playlist = new ObjectHandle<sp_playlist>("sp_playlist");
  playlist->pointer = sp_playlist_create(session->pointer, link);

  // Add callbacks
  sp_error error = sp_playlist_add_callbacks(playlist->pointer, &nsp_playlist_callbacks, playlist);
  NSP_THROW_IF_ERROR(error);

  info.GetReturnValue().Set(Nan::New(playlist->object));
}


NAN_METHOD(Link_Type) {

  const char* type = NULL;

  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsString());

  String::Utf8Value url(info[0]);
  sp_link* link = sp_link_create_from_string(*url);
  if(!link) {
    info.GetReturnValue().Set(Nan::New<Boolean>(false));
  }

  switch(sp_link_type(link)) {
    case SP_LINKTYPE_PLAYLIST:
      type = "playlist";
      break;
    case SP_LINKTYPE_ARTIST:
      type = "artist";
      break;
    case SP_LINKTYPE_TRACK:
      type = "track";
      break;
    default:
      info.GetReturnValue().Set(Nan::New<Boolean>(false));
  }

  info.GetReturnValue().Set(Nan::New<String>(type).ToLocalChecked());
}



void nsp::init_link(Local<Object> target) {
  Nan::Export(target, "link_create_from_track", Link_Create_From_Track);
  Nan::Export(target, "link_create_from_artist", Link_Create_From_Artist);
  Nan::Export(target, "link_create_from_playlist", Link_Create_From_Playlist);
  Nan::Export(target, "link_as_track", Link_As_Track);
  Nan::Export(target, "link_as_artist", Link_As_Artist);
  Nan::Export(target, "link_as_playlist", Link_As_Playlist);
  Nan::Export(target, "link_type", Link_Type);
}

/*
 * =====================================================================================
 *
 *       Filename:  playlist.cc
 *
 *    Description:  Bindings for the playlist submodule
 *
 *        Version:  1.0
 *        Created:  30/03/2013 11:58:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Iain Cole, iaincole1@gmail.com
 *        Company:  Iain Cole
 *
 * =====================================================================================
 */

#include "common.h"
#include "playlistcallbacks.cc"
#include <stdlib.h>

using namespace v8;
using namespace nsp;

/*
 * PLAYLIST CONTAINER
 */

/**
 * JS playlistcontainer_is_loaded implementation. checks if a given playlistcontainer is loaded
 */
NAN_METHOD(PlaylistContainer_Is_Loaded) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_playlistcontainer pointer from given object
  ObjectHandle<sp_playlistcontainer>* playlistcontainer = ObjectHandle<sp_playlistcontainer>::Unwrap(info[0]);

  // actually call sp_playlistcontainer_is_loaded
  bool loaded = sp_playlistcontainer_is_loaded(playlistcontainer->pointer);

  info.GetReturnValue().Set(Nan::New<Boolean>(loaded));
}

/**
 * JS num playlists implementation.
 */
NAN_METHOD(PlaylistContainer_Num_Playlists) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_playlistcontainer pointer from given object
  ObjectHandle<sp_playlistcontainer>* playlistcontainer = ObjectHandle<sp_playlistcontainer>::Unwrap(info[0]);

  // actually call sp_playlistcontainer_num_playlists
  int numPlaylists = sp_playlistcontainer_num_playlists(playlistcontainer->pointer);

  info.GetReturnValue().Set(Nan::New<Number>(numPlaylists));
}

/**
 * JS playlist implementation. Gets the playlist at index for the playlist container
 */
NAN_METHOD(PlaylistContainer_Playlist) {


  // test arguments sanity
  assert(info.Length() == 3);
  assert(info[0]->IsObject());
  assert(info[1]->IsObject());
  assert(info[2]->IsNumber());

  // gets sp_playlistcontainer pointer from given object
  ObjectHandle<sp_playlistcontainer>* playlistcontainer = ObjectHandle<sp_playlistcontainer>::Unwrap(info[0]);

  // gets sp_session pointer from given object
  ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(info[1]);

  int index = info[2].As<Number>()->Int32Value();

  assert(index >= 0);
  assert(index < sp_playlistcontainer_num_playlists(playlistcontainer->pointer));

  // actually call sp_playlistcontainer_playlist
  sp_playlist* spplaylist = sp_playlistcontainer_playlist(playlistcontainer->pointer, index);

  // Set the playlist in RAM
  sp_playlist_set_in_ram(session->pointer, spplaylist, true);

  ObjectHandle<sp_playlist>* playlist = new ObjectHandle<sp_playlist>("sp_playlist");
  playlist->pointer = spplaylist;

  sp_error error = sp_playlist_add_callbacks(spplaylist, &nsp_playlist_callbacks, playlist);
  NSP_THROW_IF_ERROR(error);

  info.GetReturnValue().Set(Nan::New(playlist->object));
}

void nsp::init_playlistcontainer(Local<Object> target) {
  Nan::Export(target, "playlistcontainer_is_loaded", PlaylistContainer_Is_Loaded);  
  Nan::Export(target, "playlistcontainer_num_playlists", PlaylistContainer_Num_Playlists);
  Nan::Export(target, "playlistcontainer_playlist", PlaylistContainer_Playlist);
}

/*
 * PLAYLIST
 */

/**
 * JS playlist_is_loaded implementation. checks if a given playlist is loaded
 */
NAN_METHOD(Playlist_Is_Loaded) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_playlistcontainer pointer from given object
  ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(info[0]);

  // actually call sp_playlistcontainer_is_loaded
  bool loaded = sp_playlist_is_loaded(playlist->pointer);

  info.GetReturnValue().Set(Nan::New<Boolean>(loaded));
}

/**
 * JS playlist_name implementation. return the playlist name
 */
NAN_METHOD(Playlist_Name) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_playlistcontainer pointer from given object
  ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(info[0]);

  // actually call sp_playlist_name
  const char* name = sp_playlist_name(playlist->pointer);

  info.GetReturnValue().Set(Nan::New<String>(name).ToLocalChecked());
}

/**
 * JS num tracks implementation.
 */
NAN_METHOD(Playlist_Num_Tracks) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_playlist pointer from given object
  ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(info[0]);

  // actually call sp_playlist_num_tracks
  int numTracks = sp_playlist_num_tracks(playlist->pointer);

  info.GetReturnValue().Set(Nan::New<Number>(numTracks));
}

/**
 * JS playlist track implementation. Gets the track at index for the playlist
 */
NAN_METHOD(Playlist_Track) {


  // test arguments sanity
  assert(info.Length() == 2);
  assert(info[0]->IsObject());
  assert(info[1]->IsNumber());

  // gets sp_playlist pointer from given object
  ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(info[0]);

  int index = info[1]->ToNumber()->Int32Value();

  assert(index >= 0);
  assert(index < sp_playlist_num_tracks(playlist->pointer));

  // actually call sp_playlist_track
  sp_track* sptrack = sp_playlist_track(playlist->pointer, index);

  ObjectHandle<sp_track>* track = new ObjectHandle<sp_track>("sp_track");
  track->pointer = sptrack;

  info.GetReturnValue().Set(Nan::New(track->object));
}

/**
 * JS playlist num subscribers. Gets the number of subscribers for the playlist
 */
NAN_METHOD(Playlist_Update_Subscribers) {


  // test arguments sanity
  assert(info.Length() == 2);
  assert(info[0]->IsObject());
  assert(info[1]->IsObject());

  // gets sp_session pointer from given object
  ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(info[0]);

  // gets sp_playlist pointer from given object
  ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(info[1]);

  sp_error error = sp_playlist_update_subscribers(session->pointer, playlist->pointer);
  NSP_THROW_IF_ERROR(error);

  info.GetReturnValue().SetUndefined();
}

/**
 * JS playlist num subscribers. Gets the number of subscribers for the playlist
 */
NAN_METHOD(Playlist_Num_Subscribers) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_playlist pointer from given object
  ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(info[0]);

  int numSubscribers = sp_playlist_num_subscribers(playlist->pointer);

  info.GetReturnValue().Set(Nan::New<Number>(numSubscribers));
}


void nsp::init_playlist(Local<Object> target) {
  Nan::Export(target, "playlist_is_loaded", Playlist_Is_Loaded);
  Nan::Export(target, "playlist_name", Playlist_Name);
  Nan::Export(target, "playlist_num_tracks", Playlist_Num_Tracks);
  Nan::Export(target, "playlist_track", Playlist_Track);
  Nan::Export(target, "playlist_update_subscribers", Playlist_Update_Subscribers);
  Nan::Export(target, "playlist_num_subscribers", Playlist_Num_Subscribers);
}

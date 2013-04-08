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
static Handle<Value> PlaylistContainer_Is_Loaded(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_playlistcontainer pointer from given object
    ObjectHandle<sp_playlistcontainer>* playlistcontainer = ObjectHandle<sp_playlistcontainer>::Unwrap(args[0]);

    // actually call sp_playlistcontainer_is_loaded
    bool loaded = sp_playlistcontainer_is_loaded(playlistcontainer->pointer);

    return scope.Close(Boolean::New(loaded));
}

/**
 * JS num playlists implementation.
 */
static Handle<Value> PlaylistContainer_Num_Playlists(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_playlistcontainer pointer from given object
    ObjectHandle<sp_playlistcontainer>* playlistcontainer = ObjectHandle<sp_playlistcontainer>::Unwrap(args[0]);

    // actually call sp_playlistcontainer_num_playlists
    int numPlaylists = sp_playlistcontainer_num_playlists(playlistcontainer->pointer);
	
	return scope.Close(Number::New(numPlaylists));
}

/**
 * JS playlist implementation. Gets the playlist at index for the playlist container
 */
static Handle<Value> PlaylistContainer_Playlist(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 3);
    assert(args[0]->IsObject());
    assert(args[1]->IsObject());
    assert(args[2]->IsNumber());

	// gets sp_playlistcontainer pointer from given object
    ObjectHandle<sp_playlistcontainer>* playlistcontainer = ObjectHandle<sp_playlistcontainer>::Unwrap(args[0]);

	// gets sp_session pointer from given object
    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[1]);

	int index = args[2]->ToNumber()->Int32Value();

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
	
	return scope.Close(playlist->object);
}

void nsp::init_playlistcontainer(Handle<Object> target) {
  NODE_SET_METHOD(target, "playlistcontainer_is_loaded", PlaylistContainer_Is_Loaded);  
  NODE_SET_METHOD(target, "playlistcontainer_num_playlists", PlaylistContainer_Num_Playlists);
  NODE_SET_METHOD(target, "playlistcontainer_playlist", PlaylistContainer_Playlist);
}

/*
 * PLAYLIST
*/

/**
 * JS playlist_is_loaded implementation. checks if a given playlist is loaded
 */
static Handle<Value> Playlist_Is_Loaded(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_playlistcontainer pointer from given object
    ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(args[0]);

    // actually call sp_playlistcontainer_is_loaded
    bool loaded = sp_playlist_is_loaded(playlist->pointer);

    return scope.Close(Boolean::New(loaded));
}

/**
 * JS playlist_name implementation. return the playlist name
 */
static Handle<Value> Playlist_Name(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_playlistcontainer pointer from given object
    ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(args[0]);

    // actually call sp_playlist_name
	const char* name = sp_playlist_name(playlist->pointer);

    return scope.Close(String::New(name));
}

/**
 * JS num tracks implementation.
 */
static Handle<Value> Playlist_Num_Tracks(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    // gets sp_playlist pointer from given object
    ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(args[0]);

    // actually call sp_playlist_num_tracks
    int numTracks = sp_playlist_num_tracks(playlist->pointer);
	
	return scope.Close(Number::New(numTracks));
}

/**
 * JS playlist track implementation. Gets the track at index for the playlist
 */
static Handle<Value> Playlist_Track(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 2);
    assert(args[0]->IsObject());
    assert(args[1]->IsNumber());

	// gets sp_playlist pointer from given object
    ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(args[0]);

	int index = args[1]->ToNumber()->Int32Value();

    assert(index >= 0);
    assert(index < sp_playlist_num_tracks(playlist->pointer));

    // actually call sp_playlist_track
    sp_track* sptrack = sp_playlist_track(playlist->pointer, index);
    
    ObjectHandle<sp_track>* track = new ObjectHandle<sp_track>("sp_track");
    track->pointer = sptrack;
    
	return scope.Close(track->object);
}

/**
 * JS playlist num subscribers. Gets the number of subscribers for the playlist
 */
static Handle<Value> Playlist_Update_Subscribers(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 2);
    assert(args[0]->IsObject());
    assert(args[1]->IsObject());

	// gets sp_session pointer from given object
    ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(args[0]);

	// gets sp_playlist pointer from given object
    ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(args[1]);

	sp_error error = sp_playlist_update_subscribers(session->pointer, playlist->pointer);
	NSP_THROW_IF_ERROR(error);
	
	return scope.Close(Undefined());
}

/**
 * JS playlist num subscribers. Gets the number of subscribers for the playlist
 */
static Handle<Value> Playlist_Num_Subscribers(const Arguments& args) {
    HandleScope scope;

    // test arguments sanity
    assert(args.Length() == 1);
    assert(args[0]->IsObject());

	// gets sp_playlist pointer from given object
    ObjectHandle<sp_playlist>* playlist = ObjectHandle<sp_playlist>::Unwrap(args[0]);

	int numSubscribers = sp_playlist_num_subscribers(playlist->pointer);

	return scope.Close(Number::New(numSubscribers));
}


void nsp::init_playlist(Handle<Object> target) {
	NODE_SET_METHOD(target, "playlist_is_loaded", Playlist_Is_Loaded);
	NODE_SET_METHOD(target, "playlist_name", Playlist_Name);
	NODE_SET_METHOD(target, "playlist_num_tracks", Playlist_Num_Tracks);
	NODE_SET_METHOD(target, "playlist_track", Playlist_Track);
	NODE_SET_METHOD(target, "playlist_update_subscribers", Playlist_Update_Subscribers);
	NODE_SET_METHOD(target, "playlist_num_subscribers", Playlist_Num_Subscribers);
}

/*
 * =====================================================================================
 *
 *       Filename:  playlistcallbacks.h
 *
 *    Description:  Callbacks for the playlist subsystem
 *
 *        Version:  1.0
 *        Created:  31/03/2013 18:45:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Iain Cole iaincole1@gmail.com
 *        Company:  Iain Cole
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include "common.h"

using namespace v8;
using namespace nsp;

/*
 * PLAYLIST CONTAINER CALLBACKS
*/

/**
 * spotify callback for the playlist_added event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlistcontainer__callbacks.html
 */
static void call_playlist_added_callback(sp_playlistcontainer* pc, sp_playlist* spplaylist, int position, void* userdata) {
	ObjectHandle<sp_playlistcontainer>* playlistcontainer = (ObjectHandle<sp_playlistcontainer>*) userdata;

    Handle<Object> o = playlistcontainer->object;
    Handle<Value> cbv = o->Get(String::New("playlist_added"));
    if(!cbv->IsFunction()) {
        return;
    }

	Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
	const unsigned int argc = 0;
    Local<Value> argv[argc] = {};
    cb->Call(Context::GetCurrent()->Global(), argc, argv);
}

/**
 * spotify callback for the playlist_removed event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlistcontainer__callbacks.html
 */
static void call_playlist_removed_callback(sp_playlistcontainer* pc, sp_playlist* playlist, int position, void* userdata) {
	ObjectHandle<sp_playlistcontainer>* playlistcontainer = (ObjectHandle<sp_playlistcontainer>*) userdata;
    
    Handle<Object> o = playlistcontainer->object;
    Handle<Value> cbv = o->Get(String::New("playlist_removed"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 0;
    Local<Value> argv[argc] = {};
    cb->Call(Context::GetCurrent()->Global(), argc, argv);
}

/**
 * spotify callback for the playlist_moved event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlistcontainer__callbacks.html
 */
static void call_playlist_moved_callback(sp_playlistcontainer* pc, sp_playlist* playlist, int position, int new_position, void* userdata) {
	ObjectHandle<sp_playlistcontainer>* playlistcontainer = (ObjectHandle<sp_playlistcontainer>*) userdata;
    
    Handle<Object> o = playlistcontainer->object;
    Handle<Value> cbv = o->Get(String::New("playlist_moved"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 0;
    Local<Value> argv[argc] = {};
    cb->Call(Context::GetCurrent()->Global(), argc, argv);
}

/**
 * spotify callback for the container loaded event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlistcontainer__callbacks.html
 */
static void call_container_loaded_callback(sp_playlistcontainer* pc, void* userdata) {
	ObjectHandle<sp_playlistcontainer>* playlistcontainer = (ObjectHandle<sp_playlistcontainer>*)userdata;

    Handle<Object> o = playlistcontainer->object;
    Handle<Value> cbv = o->Get(String::New("container_loaded"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 0;
    Local<Value> argv[argc] = {};
    cb->Call(Context::GetCurrent()->Global(), argc, argv);
}

static sp_playlistcontainer_callbacks nsp_playlistcontainer_callbacks = {
    &call_playlist_added_callback,
    &call_playlist_removed_callback,
    &call_playlist_moved_callback,
    &call_container_loaded_callback
};


/*
 * PLAYLIST CALLBACKS
*/



/**
 * spotify callback for the tracks_added event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_tracks_added_callback(sp_playlist* pl, sp_track* const* tracks, int num_tracks, int position, void* userdata) {
}

/**
 * spotify callback for the tracks_removed event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_tracks_removed_callback(sp_playlist* pl, const int* tracks, int num_tracks, void* userdata) {
}

/**
 * spotify callback for the tracks_moved event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_tracks_moved_callback(sp_playlist* pl, const int* tracks, int num_tracks, int new_position, void* userdata) {
}

/**
 * spotify callback for the playlist_renamed event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_playlist_renamed_callback(sp_playlist* pl, void* userdata) {
}

/**
 * spotify callback for the playlist_state_changed event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_playlist_state_changed_callback(sp_playlist* pl, void* userdata) {
	ObjectHandle<sp_playlist>* playlist = (ObjectHandle<sp_playlist>*)userdata;
    
    Handle<Object> o = playlist->object;
    Handle<Value> cbv = o->Get(String::New("state_changed"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 0;
    Local<Value> argv[argc] = {};
    cb->Call(Context::GetCurrent()->Global(), argc, argv);
}

/**
 * spotify callback for the playlist_update_in_progress event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_playlist_update_in_progress_callback(sp_playlist* pl, bool done, void* userdata) {
}

/**
 * spotify callback for the playlist_metadata_updated event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_playlist_metadata_updated_callback(sp_playlist* pl, void* userdata) {
}

/**
 * spotify callback for the track_created_changed event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_track_created_changed_callback(sp_playlist* pl, int position, sp_user* user, int when, void* userdata) {
}

/**
 * spotify callback for the track_seen_changed event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_track_seen_changed_callback(sp_playlist* pl, int position, bool seen, void* userdata) {
}

/**
 * spotify callback for the description_changed event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_description_changed_callback(sp_playlist* pl, const char* desc, void* userdata) {
}

/**
 * spotify callback for the image_changed event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_image_changed_callback(sp_playlist* pl, const byte* image, void* userdata) {
}

/**
 * spotify callback for the track_message_changed event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_track_message_changed_callback(sp_playlist* pl, int position, const char* message, void* userdata) {
}

/**
 * spotify callback for the subscribers_changed event.
 * See https://developer.spotify.com/technologies/libspotify/docs/12.1.45/structsp__playlist__callbacks.html
 */
static void call_subscribers_changed_callback(sp_playlist* pl, void* userdata) {
	ObjectHandle<sp_playlist>* playlist = (ObjectHandle<sp_playlist>*)userdata;
    
    Handle<Object> o = playlist->object;
    Handle<Value> cbv = o->Get(String::New("subscribers_changed"));
    if(!cbv->IsFunction()) {
        return;
    }

    Handle<Function> cb = Local<Function>(Function::Cast(*cbv));
    const unsigned int argc = 0;
    Local<Value> argv[argc] = {};
    cb->Call(Context::GetCurrent()->Global(), argc, argv);
}

static sp_playlist_callbacks nsp_playlist_callbacks = {
    &call_tracks_added_callback,
    &call_tracks_removed_callback,
    &call_tracks_moved_callback,
    &call_playlist_renamed_callback,
    &call_playlist_state_changed_callback,
    &call_playlist_update_in_progress_callback,
    &call_playlist_metadata_updated_callback,
    &call_track_created_changed_callback,
    &call_track_seen_changed_callback,
	&call_description_changed_callback,
	&call_image_changed_callback,
	&call_track_message_changed_callback,
	&call_subscribers_changed_callback
};
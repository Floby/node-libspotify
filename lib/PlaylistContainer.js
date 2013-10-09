var b = require('bindings')('spotify.node');
var sp = require('./libspotify');
var util = require('util');
var SpObject = require('./SpObject');

function PlaylistContainer (sp_playlistcontainer) {
    this._sp_object = sp_playlistcontainer;
    // We need to set up the callbacks first because we are using one of them
    // to notify us when the object is ready rather than using a backoff
    // isReady check
    this._setupNativeCallbacks();
    SpObject.apply(this);

}
util.inherits(PlaylistContainer, SpObject);
PlaylistContainer.__proto__ = SpObject;

function playlistcontainer_object_type() { return 'playlistcontainer'; };
PlaylistContainer.__defineGetter__('_object_type', playlistcontainer_object_type);
PlaylistContainer.prototype.__defineGetter__('_object_type', playlistcontainer_object_type);

module.exports = PlaylistContainer;

PlaylistContainer.prototype.PLAYLIST_TYPE_PLAYLIST = b.SP_PLAYLIST_TYPE_PLAYLIST;
PlaylistContainer.prototype.PLAYLIST_TYPE_START_FOLDER = b.SP_PLAYLIST_TYPE_START_FOLDER;
PlaylistContainer.prototype.PLAYLIST_TYPE_END_FOLDER = b.SP_PLAYLIST_TYPE_END_FOLDER;
PlaylistContainer.prototype.PLAYLIST_TYPE_PLACEHOLDER = b.SP_PLAYLIST_TYPE_PLACEHOLDER;

PlaylistContainer.prototype._populateAttributes = function _populateAttributes() {
    return this.constructor.super_.prototype._populateAttributes();
};

/**
 * gets the number of playlists in the playlist container
 */
PlaylistContainer.prototype.getNumPlaylists = function getNumPlaylists() {
    this._readyOrThrow();
    return b.playlistcontainer_num_playlists(this._sp_object);
};

/**
 * gets the type of the playlist at index in the playlist container
 */
PlaylistContainer.prototype.getPlaylistType = function getPlaylistType(idx) {
    this._readyOrThrow();
    return b.playlistcontainer_playlist_type(this._sp_object, idx);
};

/**
 * gets the id of the playlist folder at index in the playlist container
 */
PlaylistContainer.prototype.getPlaylistFolderID = function getPlaylistFolderID(idx) {
    this._readyOrThrow();
    return b.playlistcontainer_playlist_folder_id(this._sp_object, idx);
};

/**
 * gets the name of the playlist folder at index in the playlist container
 */
PlaylistContainer.prototype.getPlaylistFolderName = function getPlaylistFolderName(idx) {
    this._readyOrThrow();
    return b.playlistcontainer_playlist_folder_name(this._sp_object, idx);
};

/**
 * gets a playlist from the specified index
 */
PlaylistContainer.prototype.getPlaylistAtIndex = function getPlaylistAtIndex(idx) {
    return new sp.Playlist(b.playlistcontainer_playlist(this._sp_object, sp.Session.currentSession._sp_session,  idx))
}

/**
 * gets a list of all the playlists in the playlist container
 */
PlaylistContainer.prototype.getPlaylists = function getPlaylists(callback) {
    this._readyOrThrow();
    var i, numReady = 0;

    var numPlaylists = this.getNumPlaylists();
    var playlists = new Array();

    for (i = 0; i < numPlaylists; i++) {
        var type = this.getPlaylistType(i);
        switch (type) {
            case b.SP_PLAYLIST_TYPE_PLAYLIST:
                playlists.push(this.getPlaylistAtIndex(i));
                break;
        }
    }

    for (i = 0; i < playlists.length; i++) {
        if (playlists[i].isReady()) {
            checkPlaylistsLoaded(++numReady, playlists, callback);
        } else {
            playlists[i].once('ready', function() {
                checkPlaylistsLoaded(++numReady, playlists, callback);
            });
        }
    }
};

var checkPlaylistsLoaded = function(numReady, array, callback) {
    if (numReady >= array.length) {
        callback(array);
    }
};

/**
 * C callbacks for spotify events do nothing more than calling their JS equivalent
 * here we setup the functions that must be called from C upon these events
 */
PlaylistContainer.prototype._setupNativeCallbacks = function _setupNativeCallbacks() {
    var self = this;
    this._sp_object.container_loaded = function(data) {
        self.emit('ready', data);
    };
    this._sp_object.playlist_added = function(data) {
        self.emit('playlist_added', data);
    };
    this._sp_object.playlist_removed = function(data) {
        self.emit('playlist_removed', data);
    };
    this._sp_object.playlist_moved = function(data) {
        self.emit('playlist_moved', data);
    };
};

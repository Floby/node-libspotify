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

function playlistcontainer_object_type() { return 'playlistcontainer' };
PlaylistContainer.__defineGetter__('_object_type', playlistcontainer_object_type);
PlaylistContainer.prototype.__defineGetter__('_object_type', playlistcontainer_object_type);

module.exports = PlaylistContainer;

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
 * gets a list of all the playlists in the playlist container
 */
PlaylistContainer.prototype.getPlaylists = function getPlaylists(callback) {
    this._readyOrThrow();
    var i, numReady = 0;

    var numPlaylists = this.getNumPlaylists();
    var playlists = new Array(numPlaylists);
    
    for (i = 0; i < playlists.length; i++) {
        var playlist = new sp.Playlist(b.playlistcontainer_playlist(this._sp_object, sp.Session.currentSession._sp_session,  i));
        playlists[i] = playlist;
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


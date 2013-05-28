var b = require('bindings')('spotify.node');
var sp = require('./libspotify');
var util = require('util');
var SpObject = require('./SpObject');

function Playlist (sp_playlist) {
    this._sp_object = sp_playlist;
    // We need to set up the callbacks first because we are using
    // one of them to notify us when the object is ready rather
    // than using a backoff isReady check
    this._setupNativeCallbacks();
    SpObject.apply(this);
}

util.inherits(Playlist, SpObject);
Playlist.__proto__ = SpObject;

function playlist_object_type() { return 'playlist' };
Playlist.__defineGetter__('_object_type', playlist_object_type);
Playlist.prototype.__defineGetter__('_object_type', playlist_object_type);


Playlist.getFromUrl = function getFromUrl(url) {
    return new this(b.link_as_playlist(url, sp.Session.currentSession._sp_session));
}

Playlist.prototype._populateAttributes = function _populateAttributes() {
	this.name = b.playlist_name(this._sp_object);
	this.numSubscribers = b.playlist_num_subscribers(this._sp_object);
	
    return this.constructor.super_.prototype._populateAttributes();
};

/**
 * gets the number of tracks in the playlist
 */
Playlist.prototype.getNumTracks = function getNumTracks() {
    this._readyOrThrow();
    return b.playlist_num_tracks(this._sp_object);
};

/**
 * gets a list of all the tracks in the playlist
 */
Playlist.prototype.getTracks = function getTracks(callback) {
    this._readyOrThrow();
    var i, numReady = 0;
    
    var numTracks = this.getNumTracks();
    var tracks = new Array(numTracks);
    
    for (i = 0; i < tracks.length; i++) {
        var track = new sp.Track(b.playlist_track(this._sp_object, i));
        tracks[i] = track;
    }
    
    for (i = 0; i < tracks.length; i++) {
        if (tracks[i].isReady()) {
            checkTracksLoaded(++numReady, tracks, callback);
        } else {
            tracks[i].once('ready', function() {
                checkTracksLoaded(++numReady, tracks, callback);
            });
        }
    }
};

var checkTracksLoaded = function(numReady, array, callback) {
	if (numReady >= array.length) {
		callback(array);
	}
};

/**
 * C callbacks for spotify events do nothing more than calling their JS equivalent
 * here we setup the functions that must be called from C upon these events
 */
Playlist.prototype._setupNativeCallbacks = function _setupNativeCallbacks() {
    var self = this;
   		
   	var checkReady = function() {
		if (self.isReady() && !self.__readyEventFired === true) {
			self.__readyEventFired = true;
			self._populateAttributes();
			self.emit('ready');
		    b.playlist_update_subscribers(sp.Session.currentSession._sp_session, self._sp_object); // We want the number of subscribers for the playlist so tell spotify to get it for us (this is an async call so is dealt with in the callbacks)
		}
	};
   			
    this._sp_object.state_changed = function() {
    	// Check the possible reasons that state changed has been triggered
    	
    	// 1. Collaboration turned on / off
    	
    	// 2. Pending Changes started / complete
    	
    	// 3. Playlist started loading / finished loading
   		checkReady();
	};
	
	var t = 0;
	this._sp_object.subscribers_changed = function() {
		self.numSubscribers = b.playlist_num_subscribers(self._sp_object);
	}

	checkReady();
};

module.exports = Playlist;


var b = require('bindings')('spotify.node');
var Session = require('./Session');
var Track = require('./Track');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var format = require('format').format;

/**
 * This class allows to run searches on the spotify database
 * You can either create it from a string or specify single fields
 * with the adequate methods
 * @constructor
 * @param {Session} session, the session to attach this search to (optional)
 * @param {String} search, a query to run (optional)
 */
function Search (session, query) {
    if(session && session instanceof Session) {
        this._session = session;
    }
    else {
        this._session = Session.currentSession;
    }

    this._query = query || (typeof session == 'string') ? session : null;

    this.trackOffset    = 0,
    this.trackCount     = 10,
    this.albumOffset    = 0,
    this.albumCount     = 0,
    this.artistOffset   = 0,
    this.artistCount    = 0,
    this.playlistOffset = 0,
    this.playlistCount  = 0

    this._sp_search = null;
}
util.inherits(Search, EventEmitter);

/**
 * actually run the search. When the search is complete
 * the event 'ready' is triggered
 */
Search.prototype.execute = function execute(cb) {
    this._sp_search = b.search_create(
        this._session._sp_session,
        this._query,
        this.trackOffset || 0,
        this.trackCount || 10,
        this.albumOffset || 0,
        this.albumCount || 0,
        this.artistOffset || 0,
        this.artistCount || 0,
        this.playlistOffset || 0,
        this.playlistCount || 0
    );

    var self = this;
    this._sp_search.on_search_complete = function(err, search) {
        if(err) {
            return self.emit('error', err);
        }
        if(search != self._sp_search) {
            return;
        }

        self._processResults(search);

        return self.emit('ready', self);
    }

    if(typeof cb == 'function') {
        this.once('ready', cb);
    }
};

Search.prototype._processResults = function _processResults(search) {
    this.tracks = new Array(b.search_num_tracks(this._sp_search));
    
    for (var i = 0; i < this.tracks.length; ++i) {
        this.tracks[i] = new Track(b.search_track(this._sp_search, i));
    }

    this.artists = [];
    this.albums = [];
    this.playlists = [];
};


module.exports = Search;

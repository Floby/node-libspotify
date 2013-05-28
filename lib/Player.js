var b = require('bindings')('spotify.node');
var sp = require('./libspotify');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var assert = require('assert');
var stream = require('stream');

function Player (session) {
    assert(undefined !== session, "missing parameter session");
    assert(session instanceof sp.Session, "parameter is not a session");
    this._session = session;
	
	var self = this;
	
	// Tell spotify that we're ready for more data
	this._read = function() {
		b.session_player_stream_resume();
	};
	
    this._session._sp_session.music_delivery = function(buffer) {
		// Readable.push returns whether or not we should push more data,
        // so we're returning that value so the underlying code knows what to do
        return self.push(buffer);
    }
    
    this._session._sp_session.end_of_track = function() {
		self.emit('track-end');
    }
    
    stream.Readable.call(this);
}
util.inherits(Player, stream.Readable);


Player.prototype.load = function load(track) {
    assert(track instanceof sp.Track);
    b.session_player_load(this._session._sp_session, track._sp_object);
};

Player.prototype.play = function play() {
    b.session_player_play(this._session._sp_session, true);
};

Player.prototype.stop = function stop() {
    b.session_player_play(this._session._sp_session, false);
};
Player.prototype.flush = function flush() {
    b.player_flush_audio(this._session._sp_session);
};

module.exports = Player;

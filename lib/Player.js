var b = require('bindings')('spotify.node');
var sp = require('./libspotify');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var assert = require('assert');


function Player (session) {
    assert(undefined !== session, "missing parameter session");
    assert(session instanceof sp.Session, "parameter is not a session");
    this._session = session;

    var self = this;
    this._session._sp_session.music_delivery = function(buffer) {
        self.emit('data', buffer);
    }
}
util.inherits(Player, EventEmitter);

Player.prototype.load = function load(track) {
    assert(track instanceof sp.Track);
    b.session_player_load(this._session._sp_session, track._sp_track);
};

Player.prototype.play = function play() {
    b.session_player_play(this._session._sp_session, true);
};

Player.prototype.stop = function stop() {
    b.session_player_play(this._session._sp_session, false);
};

module.exports = Player;

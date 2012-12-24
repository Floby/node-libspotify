var b = require('bindings')('spotify.node');
var Session = require('./Session');
var util = require('util');
var EventEmitter = require('events').EventEmitter;

function Track (sp_track) {
    this._sp_track = sp_track;
}
util.inherits(Track, EventEmitter);

Track.prototype.isReady = function isReady() {
    return b.track_is_loaded(this._sp_track);
};


module.exports = Track;

var b = require('bindings')('spotify.node');
var Session = require('./Session');
var util = require('util');
var EventEmitter = require('events').EventEmitter;

function Track (sp_track) {
    this._sp_track = sp_track;
    if(this.isReady()) {
        this._populateAttributes();
    }
}
util.inherits(Track, EventEmitter);

Track.prototype.isReady = function isReady() {
    return b.track_is_loaded(this._sp_track);
};

Track.prototype._populateAttributes = function _populateAttributes() {
    var dms = this.duration = b.track_duration(this._sp_track);
    // get seconds instead
    dms = Math.floor(dms / 1000);
    var min = Math.floor(dms / 60);
    var sec = (dms % 60); if(sec < 10) sec = '0'+sec;
    this.humanDuration = min+':'+sec;
};


module.exports = Track;

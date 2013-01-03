var b = require('bindings')('spotify.node');
var sp = require('./libspotify');
var util = require('util');
var SpObject = require('./SpObject');

function Track (sp_track) {
    this._sp_object = sp_track;
    SpObject.apply(this);
}
util.inherits(Track, SpObject);

Track.prototype._object_type = 'track';

Track.prototype._populateAttributes = function _populateAttributes() {
    var dms = this.duration = b.track_duration(this._sp_object);
    // get seconds instead
    dms = Math.floor(dms / 1000);
    var min = Math.floor(dms / 60);
    var sec = (dms % 60); if(sec < 10) sec = '0'+sec;
    this.humanDuration = min+':'+sec;

    this.name = this.title = b.track_name(this._sp_object);
    this.artists = [];
    for (var i = 0; i < b.track_num_artists(this._sp_object); ++i) {
        this.artists.push(b.track_artist(this._sp_object, i));
    }
    this.artist = new sp.Artist(b.track_artist(this._sp_object, 0));
};


module.exports = Track;

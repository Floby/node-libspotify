var b = require('bindings')('spotify.node');
var sp = require('./libspotify');
var util = require('util');
var SpObject = require('./SpObject');

function Track (sp_track) {
    this._sp_object = sp_track;
    SpObject.apply(this);
}
util.inherits(Track, SpObject);
Track.__proto__ = SpObject;

function track_object_type() { return 'track' };
Track.__defineGetter__('_object_type', track_object_type);
Track.prototype.__defineGetter__('_object_type', track_object_type);

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
        this.artists.push(new sp.Artist(b.track_artist(this._sp_object, i)));
    }
    this.artist = this.artists[0];
    this.album = new sp.Album(b.track_album(this._sp_object));

    this.availability = b.track_get_availability(this.getSession()._sp_session, this._sp_object);
    this.popularity = b.track_popularity(this._sp_object);
    return this.constructor.super_.prototype._populateAttributes();
};

Track.prototype.isAvailable = function isAvailable() {
    this._readyOrThrow();
    return this.availability === 'AVAILABLE';
};

Track.prototype.isStarred = function isStarred() {
    this._readyOrThrow();
    return b.track_is_starred(this.getSession()._sp_session, this._sp_object);
};

Track.prototype.star = function star() {
    this._readyOrThrow();
    return b.track_set_starred(
        this.getSession()._sp_session,
        [this._sp_object],
        true
    );
};
Track.prototype.unstar = function unstar() {
    this._readyOrThrow();
    return b.track_set_starred(
        this.getSession()._sp_session,
        [this._sp_object],
        false
    );
};

module.exports = Track;

var b = require('bindings')('spotify.node');
var sp = require('./libspotify');
var util = require('util');
var SpObject = require('./SpObject');

function Artist (sp_artist) {
    this._sp_object = sp_artist;
    SpObject.apply(this);
}
util.inherits(Artist, SpObject);
Artist.__proto__ = SpObject;

function artist_object_type() { return 'artist' };
Artist.__defineGetter__('_object_type', artist_object_type);
Artist.prototype.__defineGetter__('_object_type', artist_object_type);


Artist.prototype._object_type = 'artist';

Artist.prototype._populateAttributes = function _populateAttributes() {
    this.name = b.artist_name(this._sp_object);
    return this.constructor.super_.prototype._populateAttributes();
};

Artist.prototype.toString = function toString() {
    this._readyOrThrow();
    return this.name;
};


module.exports = Artist;


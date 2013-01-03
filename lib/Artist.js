var b = require('bindings')('spotify.node');
var sp = require('./libspotify');
var util = require('util');
var SpObject = require('./SpObject');

function Artist (sp_artist) {
    this._sp_object = sp_artist;
    SpObject.apply(this);
}
util.inherits(Artist, SpObject);

Artist.prototype._object_type = 'artist';

Artist.prototype._populateAttributes = function _populateAttributes() {
    this.name = b.artist_name(this._sp_object);
};

Artist.prototype.toString = function toString() {
    return this.name;
};


module.exports = Artist;


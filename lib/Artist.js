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

function artist_object_type() { return 'artist'; };
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

Artist.prototype.getAlbums = function getAlbums(cb) {
    var browser = b.artistbrowse_create(this.getSession()._sp_session, this._sp_object, function () {
        albums = new Array(b.artistbrowse_num_albums(browser));

        for(var i = 0; i<albums.length; i++) {
            albums[i] = new sp.Album(b.artistbrowse_album(browser, i));
        }

        b.artistbrowse_release(browser);
        cb(null, albums);
    });
};

Artist.prototype.getAvailableAlbums = function getAvailableAlbums(cb) {
    this.getAlbums(function (err, albums) {
        if(err) {
            cb(err);
        } else {
            var set = {};
            var filter = function (e) {
                /**
                 FIXME
                 When the album has multiple artists, loop all songs
                 and check if it belongs to current artist and is
                 available, if none the filter should return false.
                 **/
                if(!e.isAvailable()) { return false; }
                if(set[e.name]) { return false; }
                return (set[e.name] = true);
            };

            cb(null, albums.filter(filter));
        }
    });
};

module.exports = Artist;

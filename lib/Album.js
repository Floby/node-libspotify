var b = require('bindings')('spotify.node');
var sp = require('./libspotify');
var util = require('util');
var SpObject = require('./SpObject');

/**
 * an object representing an album
 */
function Album (sp_album) {
    this._sp_object = sp_album;
    SpObject.apply(this);
}
util.inherits(Album, SpObject);
Album.__proto__ = SpObject;

function album_object_type() { return 'album'; };
Album.__defineGetter__('_object_type', album_object_type);
Album.prototype.__defineGetter__('_object_type', album_object_type);

/**
 * @override SpObject._populateAttributes
 */
Album.prototype._populateAttributes = function _populateAttributes() {
    this.name = b.album_name(this._sp_object);
    this.year = b.album_year(this._sp_object);
    this.type = b.album_type(this._sp_object);
    this.artist = new sp.Artist(b.album_artist(this._sp_object));
    return this.constructor.super_.prototype._populateAttributes();
};

/**
 * checks availbility of this album
 * @return boolean
 */
Album.prototype.isAvailable = function isAvailable() {
    this._readyOrThrow();
    return b.album_is_available(this._sp_object);
};

Album.prototype.toString = function toString() {
    this._readyOrThrow();
    return this.name;
};

/**
 * Loads the cover image of this album. After the async fetch over the
 * libspotify finished, onImageLoaded callback gets executed.
 */
Album.prototype.coverImage = function coverImage(onImageLoaded, imageSize) {
    var size = imageSize || this.CoverImageSize.NORMAL;

    b.album_cover(
        this.getSession()._sp_session
        , this._sp_object
        , onImageLoaded
        , size
    );
};

Album.prototype.smallCoverImage = function smallCoverImage(onImageLoaded) {
    this.coverImage(onImageLoaded, this.CoverImageSize.SMALL);
}

Album.prototype.largeCoverImage = function smallCoverImage(onImageLoaded) {
    this.coverImage(onImageLoaded, this.CoverImageSize.LARGE);
}

Album.prototype.CoverImageSize = {
    NORMAL: 0
    , SMALL: 1
    , LARGE: 2
};

module.exports = Album;

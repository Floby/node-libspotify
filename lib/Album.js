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

Album.prototype.IMAGE_SIZE_NORMAL = b.SP_IMAGE_SIZE_NORMAL;
Album.prototype.IMAGE_SIZE_SMALL = b.SP_IMAGE_SIZE_SMALL;
Album.prototype.IMAGE_SIZE_LARGE = b.SP_IMAGE_SIZE_LARGE;

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
Album.prototype.coverImage = function coverImage(imageSize, cb) {
    var sp_image, image;
    var deprecated = function () {
        console.log('`cb` parameter to `coverImage` is deprecated, please use the `Image` class instead');
    };

    if (typeof(imageSize) == 'function') {
        cb = imageSize;
        imageSize = this.IMAGE_SIZE_NORMAL;
    }

    if (typeof(imageSize) == 'string') {
        switch(imageSize) {
            case 'small': imageSize = this.IMAGE_SIZE_SMALL; break;
            case 'normal': imageSize = this.IMAGE_SIZE_NORMAL; break;
            case 'large': imageSize = this.IMAGE_SIZE_LARGE; break;
            default: throw new Error('Unknown image size');
        }
    }

    if (typeof(imageSize) == 'undefined') {
        imageSize = this.IMAGE_SIZE_NORMAL;
    }

    var wrap = function () {
        var buffer = image.getData();
        if (buffer.length == 0) {
            cb(new Error('Cover image is empty'));
        } else {
            cb(null, buffer);
        }
    };

    sp_image = b.album_cover(this.getSession()._sp_session, this._sp_object, imageSize);

    if(sp_image === null) {
        if(typeof(cb) == 'function') {
            deprecated();
            process.nextTick(function () { cb(new Error('Album has no cover image')); });
        }
        return null;
    } else {
        image = new sp.Image(sp_image);
        if(typeof(cb) == 'function') {
            deprecated();
            image.whenReady(wrap);
        }
        return image;
    }
};

Album.prototype.smallCoverImage = function (cb) { return this.coverImage(this.IMAGE_SIZE_SMALL, cb); }
Album.prototype.normalCoverImage = function (cb) { return this.coverImage(this.IMAGE_SIZE_NORMAL, cb); }
Album.prototype.largeCoverImage = function (cb) { return this.coverImage(this.IMAGE_SIZE_LARGE, cb); }

Album.prototype.coverImageUrl = function coverImageUrl(imageSize) {

    if (typeof(imageSize) == 'string') {
        switch(imageSize) {
            case 'small': imageSize = this.IMAGE_SIZE_SMALL; break;
            case 'normal': imageSize = this.IMAGE_SIZE_NORMAL; break;
            case 'large': imageSize = this.IMAGE_SIZE_LARGE; break;
            default: throw new Error('Unknown image size');
        }
    }

    if (typeof(imageSize) == 'undefined') {
        imageSize = this.IMAGE_SIZE_NORMAL;
    }

    return b.link_create_from_album_cover(this._sp_object, imageSize);
}

Album.prototype.getTracks = function (cb) {
    var browser = b.albumbrowse_create(this.getSession()._sp_session, this._sp_object, function () {
        tracks = new Array(b.albumbrowse_num_tracks(browser));

        for(var i = 0; i<tracks.length; i++) {
            tracks[i] = new sp.Track(b.albumbrowse_track(browser, i));
        }

        b.albumbrowse_release(browser);
        cb(null, tracks);
    });
}

module.exports = Album;

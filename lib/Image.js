var b = require('bindings')('spotify.node');
var sp = require('./libspotify');
var util = require('util');
var SpObject = require('./SpObject');

function Image (sp_image) {
    this._sp_object = sp_image;
    this._setupNativeCallbacks();
    SpObject.apply(this);
}
util.inherits(Image, SpObject);
Image.__proto__ = SpObject;

function image_object_type() { return 'image'; };
Image.__defineGetter__('_object_type', image_object_type);
Image.prototype.__defineGetter__('_object_type', image_object_type);

Image.prototype.getData = function getData() {
    this._readyOrThrow();
    return b.image_data(this._sp_object);
};

/**
 * C callbacks for spotify events do nothing more than calling their JS equivalent
 * here we setup the functions that must be called from C upon these events
 */
Image.prototype._setupNativeCallbacks = function _setupNativeCallbacks() {
    var self = this;
    this._sp_object.image_loaded = function(data) {
        self.emit('ready', data);
    };
};

module.exports = Image;

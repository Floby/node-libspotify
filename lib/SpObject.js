var b = require('bindings')('spotify.node');
var sp = require('./libspotify');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var backoff = require('backoff');


/**
 * A JS Class for all spotify object
 * of which we don't own the pointer to in C++
 * so that we have to register refs for it
 */
function SpObject () {
    EventEmitter.apply(this);
    var self = this;

    // if our reference is already loaded
    // populate attributes and trigger ready event as
    // soon as possible
    if(this.isReady()) {
        this._populateAttributes();
        setTimeout(function() {self.emit('ready')}, 1);
    }
    else {
        // We could be doing this with the updated_metadata callback
        // but it doesn't tell us which metadata has been updated
        // so we test it regularly to check if our object is loaded yet
        var bo = backoff.fibonacci({
            initialDelay: 10,
            maxDelay: 1000
        });
        bo.failAfter(10);
        bo.on('ready', function(number, delay) {
            if(!self.isReady()) {
                bo.backoff();
            }
            else {
                self._populateAttributes();
                self.emit('ready');
            }
        });
        bo.on('fail', function() {
            self._loadingTimeout();
        });
        bo.backoff();
    }
}
util.inherits(SpObject, EventEmitter);


/**
 * called when the spotify reference is loaded 
 * in order to populate metadata in its javascript
 * representation
 */
SpObject.prototype._populateAttributes = function _populateAttributes() {
};

/**
 * retrieve an object from the given url
 * this assumes child constructore define an _object_type attribute
 * on themselves
 * @param url the url from which to retrieve the content
 * @returns SpObject an object of the correct type but not loaded
 * @throws TypeError when the given uri doesn't describe an object of this type
 */
SpObject.getFromUrl = function getFromUrl(url) {
    // `this` is actually the constructor here
    var method_name = 'link_as_' + this._object_type;
    return new this(b[method_name](url));
}

/**
 * check if the object is ready to be used.
 * we wait for spotify to load metadata about this instance
 * @return boolean
 */
SpObject.prototype.isReady = function isReady() {
    return this._is_loaded(this._sp_object);
};

SpObject.prototype._is_loaded = function _is_loaded(obj) {
    return b[this._object_type+'_is_loaded'](obj);
};

/**
 * called when loading metadata for this object timed out
 */
SpObject.prototype._loadingTimeout = function _loadingTimeout() {
    this.emit('load-timeout');
};

/**
 * get the session to which this object is attached
 * @returns Session
 */
SpObject.prototype.getSession = function getSession() {
    return sp.Session.currentSession;
};

/**
 * gets a spotify URI for this content
 * @return String
 * @throws URIError
 */
SpObject.prototype.getUrl = function getUrl() {
    var method_name = 'link_create_from_' + this._object_type;
    if(!b[method_name]) {
        throw new URIError('This object can not be represented with a URL');
    }
    return b[method_name](this._sp_object);
};

/**
 * gets a http URI for this content
 * @return String
 * @throws URIError
 */
SpObject.prototype.getHttpUrl = function getHttpUrl() {
    var url = this.getUrl();
    url = url.replace(/:/g, '/');
    url = url.replace(/^spotify/, 'http://open.spotify.com');
    return url;
};


module.exports = SpObject;

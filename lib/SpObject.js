var b = require('bindings')('spotify.node');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var backoff = require('backoff');


function SpObject () {
    EventEmitter.apply(this);
    var bo = backoff.fibonacci({
        initialDelay: 10,
        maxDelay: 1000
    });
    bo.failAfter(10);
    var self = this;
    if(this.isReady()) {
        this._populateAttributes();
        setTimeout(function() {self.emit('ready')}, 1);
    }
    else {
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
    }
}
util.inherits(SpObject, EventEmitter);

SpObject.prototype.isReady = function isReady() {
    return this._is_loaded(this._sp_object);
};

SpObject.prototype._is_loaded = function _is_loaded(obj) {
    return b[this._object_type+'_is_loaded'](obj);
};

SpObject.prototype._loadingTimeout = function _loadingTimeout() {
    this.emit('load-timeout');
};


module.exports = SpObject;

var b = require('bindings')('spotify.node');
var EventEmitter = require('events').EventEmitter;
var fs = require('fs');
var util = require('util');


function inherits (target, source) {
    for(var k in source.prototype) {
        target.prototype[k] = source.prototype[k];
    }
}
inherits(b.Session, EventEmitter);


function Session (params) {
    var base = {
    }
    params = params || {};
    inherits(params, base);

    var key = null;
    if(params.appKey instanceof Buffer) {
        key = params.appKey;
    }
    else if(typeof params.appKey == 'string') {
        key = fs.readFileSync(params.appKey);
    }
    else {
        throw new TypeError('No appKey specified for this spotify session');
    }
    this._session = new b.Session(key);
    var self = this;
    this._session.on('login', function() {
        self.emit('login');
    });
}

util.inherits(Session, EventEmitter);

Session.prototype.login = function login(login, password) {
    this._session.login(login, password);
};
Session.prototype.close = function close() {
    this._session.close();
};
Session.prototype.hasSession = function hasSession() {
    return this._session.hasSession();
};
Session.prototype.isOpen = function isOpen() {
    return this.hasSession();
};
Session.prototype.isClosed = function isClosed() {
    return !this.hasSession();
};


exports.Session = Session;

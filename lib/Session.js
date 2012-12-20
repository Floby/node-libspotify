var assert = require('assert');
var b = require('bindings')('spotify.node');
var i = require('i')(false);
var fs = require('fs');
var util = require('util');
var EventEmitter = require('events').EventEmitter;

var defaultSessionConfig = {
    cache_location:             "/tmp/node-libspotify",
    settings_location:          "/tmp/node-libspotify",
    application_key:            new Buffer(0),
    user_agent:                 "node-libspotify",
    callbacks:                  null,
    compress_playlists:         false,
    dont_save_metadata_for_playlists: true,
    initially_unload_playlists: true,
    device_id:                  "node-libspotify",
    proxy:                      null,
    proxy_username:             null,
    proxy_password:             null,
    ca_certs_filename:          null,
    tracefile:                  null
};

function Session (params) {
    params = params || {};
    var cparams = new Object(defaultSessionConfig);
    for(var key in params) {
        var k = i.underscore(key);
        cparams[k] = params[key];
    }

    var key = null;
    if(typeof cparams.application_key == 'string') {
        cparams.application_key = fs.readFileSync(cparams.application_key);
    }
    else if(! (cparams.application_key instanceof Buffer)) {
        throw new TypeError('No application key specified for this spotify session');
    }
    else if(cparams.application_key.length == 0) {
        console.log(cparams.application_key);
        throw new TypeError('Empty application key specified for this spotify session');
    }
    
    var session_config = b.session_config(cparams);
    console.log('config');
    this._sp_session = b.session_create(b.session_config(cparams));
    //console.log('session');

}
util.inherits(Session, EventEmitter);

Session.prototype.close = function close() {
    b.session_release(this._sp_session);
    this._sp_session = null;
};

Session.prototype.isClosed = function isClosed() {
    return this._sp_session == null;
};

Session.prototype.isOpen = function isOpen() {
    return !this.isClosed();
};

Session.prototype.login = function login(login, password) {
    assert.equal('string', typeof login, 'login should be a string');
    assert.equal('string', typeof password, 'password should be a string');
    b.session_login(this._sp_session, login, password);
};

module.exports = Session;

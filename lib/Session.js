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
    if (Session.currentSession) {
        throw new Error('A spotify session is currently in use. libspotify does not support multiple sessions');
    }

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
        throw new TypeError('Empty application key specified for this spotify session');
    }
    
    var session_config = b.session_config(cparams);
    this._sp_session = b.session_create(session_config);


    Session.currentSession = this;

    this._setupNativeCallbacks();
    this._startProcessingEvents();

}
util.inherits(Session, EventEmitter);

Session.prototype._setupNativeCallbacks = function _setupNativeCallbacks() {
    var self = this;
    this._sp_session.logged_in = function(err) {
        console.log('JS LOGIN CALLBACK!!!');
        self.emit('login', err);
    }
    this._sp_session.logged_out = function() {
        console.log('JS LOGOUT CALLBACK')
        self.emit('logout');
    }
};

Session.prototype._startProcessingEvents = function _startProcessingEvents() {
    var self = this;
    this._process_events_timeout = setInterval(function() {
        console.log('processing events', self.name);
        var next_timeout = b.session_process_events(self._sp_session);
    }, 800);
};

Session.prototype.close = function close() {
    if(this._sp_session) {
        //console.log('there was a session to release')
        b.session_release(this._sp_session);
        this._sp_session = null;
    }
    if(Session.currentSession === this) {
        Session.currentSession = null;
    }
    console.log('clearing interval');
    clearInterval(this._process_events_timeout);
    this._process_events_timeout = null;
};

Session.prototype.isClosed = function isClosed() {
    return this._sp_session == null;
};

Session.prototype.isOpen = function isOpen() {
    return !this.isClosed();
};

Session.prototype.login = function login(login, password) {
    if('string' != typeof login) throw new TypeError('login should be a string')
    if('string' != typeof password) throw new TypeError('password should be a string')
    b.session_login(this._sp_session, login, password);
};
Session.prototype.logout = function logout(cb) {
    b.session_logout(this._sp_session);
    if(typeof cb === 'function') {
        this.on('logout', cb)
    }
};

module.exports = Session;

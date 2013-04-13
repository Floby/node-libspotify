var assert = require('assert');
var b = require('bindings')('spotify.node');
var i = require('i')(false);
var fs = require('fs');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var sp = require('./libspotify');

/**
 * Default settings for the session_config struct
 * see libspotify documentation for more info on which is what
 * https://developer.spotify.com/technologies/libspotify/docs/12.1.45/annotated.html
 */
var defaultSessionConfig = {
    cache_location:             "/tmp/node-libspotify",
    settings_location:          "/tmp/node-libspotify",
    application_key:            new Buffer(0),
    user_agent:                 "node-libspotify",
    // we could completely remove callbacks here, because
    // we don't create a callbacks struct from JS at all
    callbacks:                  null,
    compress_playlists:         false,
    dont_save_metadata_for_playlists: true,
    initially_unload_playlists: false,
    device_id:                  "node-libspotify",
    proxy:                      null,
    proxy_username:             null,
    proxy_password:             null,
    ca_certs_filename:          null,
    tracefile:                  null
};

/**
 * a Spotify Session object. Everything in spotify occurs within a given session
 * currently, the C libspotify library provided by spotify is _really_ buggy when using more
 * than one session per process (like really really). So in effect, this JS class
 * makes sure that no C session struct gets created and instead re-uses the same JS object
 * over and over. Cleaning it each time new is called
 * @constructor
 * @param
 */
function Session (params) {
    // since we only authorize one session object,
    // make sure we do
    if (Session.currentSession) {
        // throw if current session is still open
        if(Session.currentSession.isOpen()) {
            throw new Error('Only one spotify session can live at once');
        }
        Session.currentSession.close();
        // increment current session id
        Session.currentSession._id++;
        // cleaning everything
        Session.currentSession.removeAllListeners();
        Session.currentSession._setupNativeCallbacks();
        Session.currentSession._startProcessingEvents();
        return Session.currentSession;
    }

    this._id = 0; ///> the id of the current session. this is used to make sure we only have one at a given time

    params = params || {};
    // read parameters from the config object, changing them to a C-style name
    var cparams = new Object(defaultSessionConfig);
    for(var key in params) {
        var k = i.underscore(key);
        cparams[k] = params[key];
    }

    // try to read application key from the given parameter
    var key = null;
    // if the parameter is a string, then it's a path to the key file
    if(typeof cparams.application_key == 'string') {
        cparams.application_key = fs.readFileSync(cparams.application_key);
    }
    // if the parameter is not a node buffer, then it's probably not what we want
    else if(! (cparams.application_key instanceof Buffer)) {
        throw new TypeError('No application key specified for this spotify session');
    }
    // if the parameter is an empty buffer, we've got a problem
    else if(cparams.application_key.length == 0) {
        throw new TypeError('Empty application key specified for this spotify session');
    }
    
    // create a config object from these parameters
    var session_config = b.session_config(cparams);

    // create the underlying sp_session object
    this._sp_session = b.session_create(session_config);

    // set this session as new current session
    Session.currentSession = this;

    // setup callbacks from spotify events called from C
    this._setupNativeCallbacks();

    // start the loop to process pending events
    this._startProcessingEvents();

}
util.inherits(Session, EventEmitter);

/**
 * wraps a callback in a function that checks if the callbacks is still relevant for 
 * the current session
 */
function sessionCallback (session, cb) {
    var sid = session._id;
    return function() {
        if(sid != session._id) return;
        return cb.apply(session, arguments);
    }
}

/**
 * C callbacks for spotify events do nothing more than calling their JS equivalent
 * here we setup the functions that must be called from C upon these events
 */
Session.prototype._setupNativeCallbacks = function _setupNativeCallbacks() {
    var self = this;
    this._sp_session.logged_in = sessionCallback(this, function(err) {
        self._logged_in = true;
        self.emit('login', err);
    });
    this._sp_session.logged_out = sessionCallback(this, function() {
        self._logged_in = false;
        self.emit('logout');
    });
    this._sp_session.metadata_updated = sessionCallback(this, function() {
        self.emit('metadata_updated');
    });
    this._sp_session.play_token_lost = sessionCallback(this, function() {
        self.emit('play_token_lost');
    });
};

/**
 * call spotify sp_session_start_processing_events so that pending events
 * can be processed. Current implementation processes events every 0.8 seconds
 * regardless of what libspotify returns as next timeout.
 * TODO ^
 */
Session.prototype._startProcessingEvents = function _startProcessingEvents() {
    this._sp_session.notify_main_thread = sessionCallback(this, function() {
        if(this.isClosed()) return;
        clearTimeout(this._process_events_timeout);
        this._scheduleProcessingEvents(1);
    })
    if(this._process_events_timeout) return;
    this._scheduleProcessingEvents(10);
};

Session.prototype._scheduleProcessingEvents = function _scheduleProcessingEvents(timeout) {
    var self = this;
    this._process_events_timeout = setTimeout(sessionCallback(this, function() {
        var next_timeout = b.session_process_events(self._sp_session);
        if(this.isClosed()) return;
        clearTimeout(self._process_events_timeout);
        self._scheduleProcessingEvents(next_timeout);
    }), timeout);
};

/**
 * close this session. In effect, just stopping processing events
 * TODO maybe make sure we are logged out before considering ourselves closed
 * then fire a 'close' event
 */
Session.prototype.close = function close() {
    clearInterval(this._process_events_timeout);
    this._process_events_timeout = null;
};

/**
 * tests if the current session is closed
 */
Session.prototype.isClosed = function isClosed() {
    return this._process_events_timeout === null;
};

/**
 * tests if the current session is open
 */
Session.prototype.isOpen = function isOpen() {
    return !this.isClosed();
};

/**
 * tests if the current session is logged in
 */
Session.prototype.isLoggedIn = function isLoggedIn() {
    return this._logged_in == true;
};

/**
 * try to login with the given credentials
 * when this request is processed by libspotify, the 'login' event is
 * fired with an error or null as first argument
 * @param {String} login
 * @param {String} password
 */
Session.prototype.login = function login(login, password) {
    if('string' != typeof login) throw new TypeError('login should be a string')
    if('string' != typeof password) throw new TypeError('password should be a string')
    b.session_login(this._sp_session, login, password);
};

/**
 * try to logout.
 * when this request is processed by libspotify, the 'logout' event is triggered
 * @param {Function} cb. a callback to automatically attach to the logout event
 */
Session.prototype.logout = function logout(cb) {
    b.session_logout(this._sp_session);
    if(typeof cb === 'function') {
        this.once('logout', cb)
    }
};

/**
 * gets the audio player for this session. The player really only
 * just emits audio samples
 * @return {Player} the player for further use
 */
Session.prototype.getPlayer = function getPlayer() {
    if(this._player) return this._player;
    this._player = new sp.Player(this);
    return this._player;
};

/**
 * override event emitter method in order to attach callbacks to a specific session
 * @override EventEmitter.addListener
 */
Session.prototype.addListener = function addListener(event, listener) {
    EventEmitter.prototype.addListener.call(this, event, sessionCallback(this, listener));
};
Session.prototype.on = Session.prototype.addListener

/**
 * get the playlistcontainer object for the current session
 */
Session.prototype.getPlaylistcontainer = function getPlaylistcontainer() {
    return new sp.PlaylistContainer(b.session_playlistcontainer(this._sp_session));
};

// exports this Class
module.exports = Session;

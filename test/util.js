var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');

var session = null
exports.getDefaultTestSession = function(cb) {
    if(session instanceof sp.Session) {
        if(session.isLoggedIn()) {
            setTimeout(function() {
                cb(session);
            });
            return;
        }
    }
    else {
        session = new sp.Session({
            applicationKey: __dirname + '/../spotify_key/spotify_appkey.key'
        });
        session.login(cred.login, cred.password);
    }
    session.once('login', function(err) {
        if(err) throw err;
        cb(session);
    });
}

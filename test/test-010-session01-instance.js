var sp = require('../lib/libspotify2');
var session = null;

exports.testInstanciatingSessionWithAppKeyPath = function(test) {
    test.doesNotThrow(function() {
        session = new sp.Session({
            applicationKey: __dirname + '/../spotify_key/spotify_appkey.key'
        });
    }, "Opening a session with a valid spotify_key_app should not throw");
    test.done();
}
 exports.testStopSession = function(test) {
    test.doesNotThrow(function() {
        session.close();
    }, 'Closing a session should not throw');
    test.ok(session.isClosed(), 'Session isClosed should return true');
    test.ok(!session.isOpen(), 'Session isOpen should return false');

    test.done();
 }

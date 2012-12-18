var sp = require('../lib/libspotify');
var session = null;

exports.testInstanciatingSessionWithAppKeyPath = function(test) {
    test.doesNotThrow(function() {
        session = new sp.Session({
            appKey: __dirname + '/../spotify_key/spotify_appkey.key'
        });
    }, "Opening a session with a valid spotify_key_app should not throw");
    test.done();
}
 exports.testStopSession = function(test) {
    test.doesNotThrow(function() {
        session.close();
    }, 'Closing a session should not throw');
    test.ok(session.isClosed());
    test.ok(!session.isOpen());
    delete session;

    test.done();
 }

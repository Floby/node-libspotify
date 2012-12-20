var sp = require('../lib/libspotify2');

exports.testLoginDoesntThrow = function(test) {
    var session = new sp.Session({
        applicationKey: __dirname + '/../spotify_key/spotify_appkey.key'
    });
    test.doesNotThrow(function() {
        session.login('login', 'password');
    })
    test.done();
}

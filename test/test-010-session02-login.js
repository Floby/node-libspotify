var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');

exports.testLoginDoesntThrow = function(test) {
    var session = new sp.Session({
        applicationKey: __dirname + '/../spotify_key/spotify_appkey.key'
    });
    session.name = 'bidule';
    test.doesNotThrow(function() {
        session.login('login', 'password');
    })
    session.close();
    test.done();
}
exports.testLoginIsSucessful = function(test) {
    var session = new sp.Session({
        applicationKey: __dirname + '/../spotify_key/spotify_appkey.key'
    });
    session.login(cred.login, cred.password);
    console.log('trying to login, this may take a while');
    session.on('login', function(err) {
        if(err) test.done(err);
        test.ok(session.isLoggedIn(), 'session should now it is now logged in');
        session.logout(function() {
            session.close();
            test.done();
        });
    });
}

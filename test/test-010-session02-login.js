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
    session.name = 'chose';
    session.login(cred.login, cred.password);
    session.on('login', function(err) {
        test.equal(err, null, 'There should be no error');
        session.close();
        test.done();
    });
}

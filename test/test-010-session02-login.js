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
    console.log('trying to login, this may take a while');
    var logged = false;
    setTimeout(function() {
        if(!logged) {
            test.done(new Error("Waited too long for login"));
        }
    }, 10000);
    session.on('login', function(err) {
        test.equal(null, err, err + ' occured on login');
        logged = true;
        session.logout(function() {
            session.close();
            test.done();
        });
    });
}

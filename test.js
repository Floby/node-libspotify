var sp = require('./lib/libspotify');

var s = new sp.Session({
    appKey: __dirname + '/spotify_key/spotify_appkey.key'
});

var cred = require('./spotify_key/passwd');
s.login(cred.login, cred.password);
s.on('login', function() {
    console.log('LOGGED FROM JS!')
});

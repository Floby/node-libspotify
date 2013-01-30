var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');
var Speaker = require('speaker');

var session = new sp.Session({
    applicationKey: __dirname + '/../spotify_key/spotify_appkey.key'
});
session.login(cred.login, cred.password);
session.once('login', function(err) {
    if(err) this.emit('error', err);

    var track = sp.Track.getFromUrl('spotify:track:6JEK0CvvjDjjMUBFoXShNZ');
    track.on('ready', function() {
        var player = session.getPlayer();
        player.load(track);
        player.play();

        var speaker = new Speaker();
        player.pipe(speaker);

        console.log('playing track. end in %s', track.humanDuration);
        player.once('track-end', function() {
            player.stop();
            session.close();
        });
    });
});


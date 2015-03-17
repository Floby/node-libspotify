var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');
var fs = require('fs');

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

        var f = fs.createWriteStream('/tmp/audio.raw');
        player.pipe(f);

        console.log('playing track. end in %s', track.humanDuration);
        player.play();

        player.once('track-end', function() {
            console.log('track ended');
            f.end();
            player.stop();
            session.close();
        });
    });
});


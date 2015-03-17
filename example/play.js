var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');
var spawn = require('child_process').spawn;

var session = new sp.Session({
    applicationKey: __dirname + '/../spotify_key/spotify_appkey.key'
});
session.login(cred.login, cred.password);
session.once('login', function(err) {
    if(err) this.emit('error', err);

    var search = new sp.Search('artist:"rick astley" track:"never gonna give you up"');
    search.trackCount = 1; // we're only interested in the first result;
    search.execute();
    search.once('ready', function() {
        if(!search.tracks.length) {
            console.log('there is no track to play :[');
            session.logout();
        }

        var track = search.tracks[0];
        var player = session.getPlayer();
        player.load(track);
        player.play();

        var play = spawn('aplay', ['-c', 2, '-f', 'S16_LE', '-r', '44100']);
        player.pipe(play.stdin);

        console.log('playing track. end in %s', track.humanDuration);
        player.on('data', function(buffer) {
            // buffer.length
            // buffer.rate
            // buffer.channels
            // 16bit samples
        });
        player.once('track-end', function() {
            console.log('track ended');
            player.stop();
            session.close();
        });
    });
});

var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');
var testutil = require('./util');

var session = null;

exports.setUp = function(cb) {
    testutil.getDefaultTestSession(function(s) {
        session = s;
        cb();
    })
};
exports.testEndOfShortTrack = function(test) {
    var search = new sp.Search('album:"Indie Rock to the Blues" track:"short song I"');
    search.execute(function() {
        test.ok(search.tracks.length > 0);
        var track = search.tracks[0];
        var player = session.getPlayer();
        player.load(track);
        player.play();
        console.log('playing track, enjoy it, waiting for it to end in %s', track.humanDuration);
        var called_end = false;
        player.once('track-end', function() {
            called_end = true;
            player.stop();
        });
        player.on('data', function() {});
        setTimeout(function() {
            test.ok(called_end);
            player.stop();
            test.done();
        }, track.duration + 2000); // 2 seconds margin
    });
}

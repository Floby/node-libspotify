var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');
var testutil = require('./util');
var trycatch = require('trycatch');
var portaudio = null;
try {
    portaudio = require('portaudio');
}
catch(e) {
}
var fs = require('fs');

var session = null;
var pa = null;
var f = null;

exports.setUp = function(cb) {
    if(portaudio) {
        portaudio.open({
            sampleFormat: portaudio.SampleFormat16Bit,
            channelCount: 2,
            sampleRate: 44100
        }, function(err, p) {
            if(err) return cb(err);
            pa = p;
            testutil.getDefaultTestSession(function(s) {
                session = s;
                cb();
            })
        });
    }
    else {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        })
    }
};

exports.testPlaySingleAlJArreauTrack = function(test) {
    var search = new sp.Search('artist:"Al Jarreau" track:"Boogie down"');
    var f = fs.createWriteStream('/tmp/node-libspotify/al-jarreau.raw');
    f.on('open', function() {
        trycatch(function() {
            search.execute(function() {
                var track = search.tracks[0];
                test.ok(track.isReady(), 'track should be loaded');
                var player = session.getPlayer();
                test.doesNotThrow(function() {
                    player.load(track);
                }, "loading a track should not throw");
                player.play();
                console.log('playing track, this may take some time');
                if(pa) player.pipe(pa);
                player.pipe(f);
                var got_data = false;
                player.once('data', function(chunk) {
                    test.ok(chunk.length > 0, "we should get samples from the player");
                    got_data = true;
                });
                setTimeout(function() {
                    if(pa) pa.start();
                }, 500);
                setTimeout(function() {
                    if(pa) pa.stop();
                    player.stop();
                    f.end();
                    test.ok(got_data, "we should have received some data from the player");
                    test.done();
                }, 5000);
            });
        }, /* catch */ function(e) {
            test.done(e);
        })
    });
}

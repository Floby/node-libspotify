var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');
var testutil = require('./util');
var trycatch = require('trycatch');
var portaudio = require('portaudio');


var session = null;
var pa = null;

//exports.setUp = function(cb) {
    //portaudio.open({
        //sampleFormat: portaudio.SampleFormat16Bit,
        //channelCount: 2,
        //sampleRate: 44100
    //}, function(err, p) {
        //if(err) return cb(err);
        //pa = p;
        //testutil.getDefaultTestSession(function(s) {
            //session = s;
            //cb();
        //})
    //});
//};

exports.setUp = function(cb) {
    testutil.getDefaultTestSession(function(s) {
        session = s;
        cb();
    })
};
exports.testPlaySingleGuillemotsTrack = function(test) {
    var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
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
            //player.pipe(pa);
            var got_data = false;
            player.once('data', function(chunk) {
                test.ok(chunk.length > 0, "we should get samples from the player");
                got_data = true;
            });
            setTimeout(function() {
                //pa.start();
            }, 500);
            setTimeout(function() {
                //pa.stop();
                player.stop();
                test.ok(got_data, "we should have received some data from the player");
                test.done();
            }, 2000);
        });
    }, /* catch */ function(e) {
        test.done(e);
    })
}

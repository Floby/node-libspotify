var sp = require('../lib/libspotify');
var testutil = require('./util');
var timed = testutil.timed;

var session = null;

exports.links = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        })
    },
    'test ready fired': timed(15000, function(test) {
        var track = sp.Track.getFromUrl('spotify:track:1PtgkkCdJ9rGNN2vHkBDdp');
        track.whenReady(function() {
            test.ok(true);
            test.equal(track.name, 'Qwerty Finger', "Track should be Qwerty Finger");
            test.done();
        });
    })
};





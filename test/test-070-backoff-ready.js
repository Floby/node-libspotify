var sp = require('../lib/libspotify');
var testutil = require('./util');

var session = null;

exports.links = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        })
    },
    'test ready fired': function(test) {
        var track = sp.Track.getFromUrl('spotify:track:0UTb7djhwT0nUqcSFs1ITT');
        test.ok(!track.isReady(), 'Track should not be loaded yet');
        var ready = false;
        track.on('ready', function() {
            ready = true;
        });
        setTimeout(function() {
            test.ok(ready, 'track should be loaded');
            test.done();
        }, 1000);
    },
};





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
    'test ready fired': timed(20000, function(test) {
        var track = sp.Track.getFromUrl('spotify:track:6H5NpNmdThi0mp6oRFMu5p');
        test.ok(!track.isReady(), 'Track should not be loaded yet');
        track.on('ready', function() {
            test.ok(true);
            test.done();
        });
    })
};





var sp = require('../lib/libspotify');
var testutil = require('./util');

var session = null;

exports.track = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        })
    },
    testAttributesAreMapped: function(test) {
        var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
        search.execute(function() {
            test.ok(search.tracks.length > 0, "the search should return at least one result");
            var first = search.tracks[0];
            test.ok(first instanceof sp.Track, "the track results should be loaded track objects");
            test.ok(first.isReady());
            test.ok(first.artist.isReady(), 'the artist of the track should be loaded');
            test.equal('Guillemots', first.artist.name, "the track should be a guillemots song");
            test.equal('Guillemots', first.artist, "the track should be a guillemots song");
            test.equal('Fleet', first.title, "the track should be a guillemots song");
            test.ok(first.duration > 0, 'The track should have a non zero duration');
            return test.done();
        });
    }
};



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
    'attributes are mapped': function(test) {
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
            test.ok(first.isAvailable(), 'The track should be available');
            test.equal('AVAILABLE', first.availability);
            test.equal(typeof first.isStarred(), 'boolean', 'isStarred should return a boolean');
            test.equal(typeof first.popularity, 'number', 'Popularity should be a number');
            test.ok(first.popularity >= 0, 'Popularity should be greater than zero');
            test.ok(first.popularity <= 100, 'Popularity should be less than 100');
            
            test.doesNotThrow(function() {
                var starred = first.isStarred();
                first.star();
                test.ok(first.isStarred(), 'Track should be starred');
                first.unstar();
                test.ok(!first.isStarred(), 'Track should not be starred');
                if(starred) first.star();
            }, 'starring and unstarring a track should not throw');
            return test.done();
        });
    }
};



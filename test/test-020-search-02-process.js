var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');
var testutil = require('./util');
var trycatch = require('trycatch');

var session = null;

exports.setUp = function(cb) {
    testutil.getDefaultTestSession(function(s) {
        session = s;
        cb();
    })
};

exports.testGetTrackFromSearchResult = function(test) {
    var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
    search.execute(function() {
        test.doesNotThrow(function() {
            test.ok(search.tracks.length > 0, "the search should return at least one result");
            var first = search.tracks[0];
            test.ok(first instanceof sp.Track, "the track results should be loaded track objects");
            test.ok(first.isReady());
            test.equal('Guillemots', first.artist, "the track should be a guillemots song");
            test.equal('Fleet', first.title, "the track should be a guillemots song");
            test.equal('Hello Land!', first.album, "the album should be Hello land");
        });
        test.done();
    });
}


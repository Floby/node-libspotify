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
    console.log('executing search')
    search.execute(function() {
        console.log('got search results');
        test.doesNotThrow(function() {
            test.ok(search.tracks.length > 0, "the search should return at least one result");
            test.ok(search.tracks[0] instanceof sp.Track, "the track results should be loaded track objects");
            test.ok(search.tracks[0].isReady());
        });
        test.done();
    });
}


var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');
var testutil = require('./util');
var trycatch = require('trycatch');

var session = null;

exports.setUp = function(cb) {
    testutil.getDefaultTestSession(function(s) {
        session = s;
        cb();
    });
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
};

exports.testGetAlbumFromSearchResult = function(test) {
    var search = new sp.Search('artist:"Hurts" album:"Exile"');
    search.trackCount = 0;
    search.albumCount = 1;
    search.execute(function() {
        test.doesNotThrow(function() {
            test.ok(search.albums.length > 0, "the search should return at least one result");
            var first = search.albums[0];
            test.ok(first instanceof sp.Album, "the album results should be loaded album objects");
            test.ok(first.isReady());
            test.equal('Hurts', first.artist, "the album should be a Hurts album");
            test.equal('Exile (Deluxe)', first.name, "the album should be Exile (Deluxe)");
        });
        test.done();
    });
};

exports.testGetArtistFromSearchResult = function(test) {
    var search = new sp.Search('artist:"Coldplay"');
    search.trackCount = 0;
    search.artistCount = 1;
    search.execute(function() {
        test.doesNotThrow(function() {
            test.ok(search.artists.length > 0, "the search should return at least one result");
            var first = search.artists[0];
            test.ok(first instanceof sp.Artist, "the artist results should be loaded artist objects");
            test.ok(first.isReady());
            test.equal('Coldplay', first.name, "the artist should be Coldplay");
        });
        test.done();
    });
};

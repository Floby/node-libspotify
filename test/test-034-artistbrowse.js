var sp = require('../lib/libspotify');
var testutil = require('./util');

var getArtist = function(test, cb) {
    var search = new sp.Search('artist:"Hurts"');
    search.trackCount = 1;
    search.execute(function() {
        test.ok(search.tracks.length > 0, 'the track was found');
        test.ok(search.tracks[0] instanceof sp.Track, 'track is an track');
        test.ok(search.tracks[0].album instanceof sp.Album, 'album is an album');
        test.ok(search.tracks[0].album.artist instanceof sp.Artist, 'artist is an artist');
        cb(search.tracks[0].album.artist);
    });
};

var session = null;

exports.artistbrowse = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        });
    },
    'get albums from artist': function(test) {
        getArtist(test, function(artist) {
            artist.getAvailableAlbums(function(err, albums) {
                test.ifError(err);
                /* FIXME: Should be 22, see comment in lib/Artist.js line 50 */
                test.equal(albums.length, 23, 'the artist has 23 available albums');
                test.equal(albums.map(function(e) {return e instanceof sp.Album;}).indexOf(false), -1, 'It should only contain albums');
                test.equal(albums.reduce(function(prev, current) {
                    return prev && current.isReady();
                }, true), true, 'All albums should be loaded');
                test.done();
            });
        });
    }
}

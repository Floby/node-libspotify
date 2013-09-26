var sp = require('../lib/libspotify');
var testutil = require('./util');

var getAlbum = function(test, cb) {
    var search = new sp.Search('artist:"Hurts" album:"Exile"');
    search.trackCount = 1;
    search.execute(function() {
        test.ok(search.tracks.length > 0, 'the album was found');
        test.ok(search.tracks[0] instanceof sp.Track, 'track is an track');
        test.ok(search.tracks[0].album instanceof sp.Album, 'album is an album');
        cb(search.tracks[0].album);
    });
};

var session = null;

exports.albumbrowse = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        });
    },
    'get tracks from album': function(test) {
        getAlbum(test, function(album) {
            album.getTracks(function(err, tracks) {
                test.ifError(err);
                test.equal(tracks.length, 14, 'the album has 14 tracks');
                test.equal(tracks.map(function(e) {return e instanceof sp.Track;}).indexOf(false), -1, 'It should only contain tracks');
                test.equal(tracks.reduce(function(prev, current) {
                    return prev && current.isReady();
                }, true), true, 'All tracks should be loaded');
                test.done();
            });
        });
    }
}

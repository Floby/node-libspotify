var sp = require('../lib/libspotify');
var testutil = require('./util');

var session = null;

exports.album = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        });
    },
    'attributes are mapped': function(test) {
        var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
        search.execute(function() {
            test.ok(search.tracks.length > 0, "the search should return at least one result");
            var first = search.tracks[0];
            var album = first.album;

            test.ok(album instanceof sp.Album, 'album should be an instance of Album');
            test.equal('Hello Land!', album.name, 'album should be Hello Land');
            test.equal('Guillemots', album.artist, 'album artist should be Guillemots');
            test.equal(2012, album.year, 'album came out in 2012');
            test.equal('UNKNOWN', album.type, 'album type is UNKNOWN'); // why is not ? :[

            return test.done();
        });
    },
    'cover image - default size': function(test) {
        var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
        search.execute(function() {
            test.ok(search.tracks.length > 0, "the search should return at least one result");
            var first = search.tracks[0];
            var album = first.album;

            test.doesNotThrow(function () {
                var img = album.coverImage()
                img.whenReady(function() {
                    return test.done();
                });
            });
        });
    },
    'cover image - normal size (function)': function(test) {
        var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
        search.execute(function() {
            test.ok(search.tracks.length > 0, "the search should return at least one result");
            var first = search.tracks[0];
            var album = first.album;

            test.doesNotThrow(function () {
                var img = album.normalCoverImage();
                img.whenReady(function() {
                    return test.done();
                });
            });
        });
    },
    'cover image - small size (string)': function(test) {
        var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
        search.execute(function() {
            test.ok(search.tracks.length > 0, "the search should return at least one result");
            var first = search.tracks[0];
            var album = first.album;

            test.doesNotThrow(function () {
                var img = album.coverImage('small');
                img.whenReady(function() {
                    return test.done();
                });
            });
        });
    },
    'cover image - large size (constant)': function(test) {
        var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
        search.execute(function() {
            test.ok(search.tracks.length > 0, "the search should return at least one result");
            var first = search.tracks[0];
            var album = first.album;

            test.doesNotThrow(function () {
                img = album.coverImage(album.IMAGE_SIZE_LARGE);
                img.whenReady(function() {
                    return test.done();
                });
            });
        });
    },
    'cover image - unknown size': function(test) {
        var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
        search.execute(function() {
            test.ok(search.tracks.length > 0, "the search should return at least one result");
            var first = search.tracks[0];
            var album = first.album;

            test.throws(function () {
                album.coverImage('very-strange-size');
            }, Error, 'Should fail with unknown size');

            return test.done();
        });
    }
};

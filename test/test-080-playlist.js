var sp = require('../lib/libspotify');
var testutil = require('./util');
var timed = testutil.timed;

var session = null;

exports.playlist = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        });
    },
    'get playlist from URI': function(test) {
        var playlist;
        test.doesNotThrow(function() {
            playlist = sp.Playlist.getFromUrl('spotify:user:flobyiv:playlist:5ZMnMnJWGXZ9qm4gacHpQF');
            test.ok(playlist instanceof sp.Playlist, 'We should get a playlist object');
            test.done();
        }, 'getting playlist from url should not throw');
    },
    'get playlist from Starred': function(test) {
        var playlist;
        test.doesNotThrow(function() {
            playlist = session.getStarred()
            test.ok(playlist instanceof sp.Playlist, 'We should get a playlist object');
            test.done();
        }, 'getting playlist from starred should not throw');
    },
    'attributes are mapped': function(test) {
        var playlist = sp.Playlist.getFromUrl('spotify:user:flobyiv:playlist:5ZMnMnJWGXZ9qm4gacHpQF');
        playlist.whenReady(function() {
            test.doesNotThrow(function() {
                test.equal(playlist.name, "Daft Punk - Random Access Memories", "The title should be Daft Punk - Randome Access Memories");
                test.equal(playlist.numSubscribers, 0, "There would be no subscribers");
                test.equal(playlist.getNumTracks(), 13, 'There should be 13 tracks on Daft Punk - RAM');
                test.done();
            }, "getting attributes should not throw");
        });
    },
    'get tracks from URI': function(test) {
        var playlist = sp.Playlist.getFromUrl('spotify:user:flobyiv:playlist:5ZMnMnJWGXZ9qm4gacHpQF');
        playlist.whenReady(function() {
            playlist.getTracks(function(tracks) {
                test.ok(Array.isArray(tracks), 'tracks should be an array');
                test.equal(tracks.length, 13, 'There should be 13 tracks in the array');
                test.equal(tracks.map(function(e) {return e instanceof sp.Track;}).indexOf(false), -1, 'It should only contain tracks');
                test.equal(tracks.reduce(function(prev, current) {
                    return prev && current.isReady();
                }, true), true, 'All tracks should be loaded');
                test.done();
            });
        });
    }.timed(10000),
    'get tracks from Starred': function(test) {
        var playlist = session.getStarred();
        playlist.whenReady(function() {
            playlist.getTracks(function(tracks) {
                test.ok(Array.isArray(tracks), 'tracks should be an array');
                test.ok(tracks.length > 0, 'There should be tracks in the array');
                test.equal(tracks.map(function(e) {return e instanceof sp.Track;}).indexOf(false), -1, 'It should only contain tracks');
                test.equal(tracks.reduce(function(prev, current) {
                    return prev && current.isReady();
                }, true), true, 'All tracks should be loaded');
                test.done();
            });
        });
    }.timed(40000),
    'get playlist image': function(test) {
        var playlist = sp.Playlist.getFromUrl('spotify:user:digster.se:playlist:1jGWX65tQPpwkuqG2OaRCN');
        playlist.whenReady(function () {
            var img = playlist.getImage();
            test.ok(img !== null, 'the playlist has an image');
            img.whenReady(function () {
                var buffer = img.getData();
                test.ok(buffer instanceof Buffer, 'image data is a buffer');
                test.ok(buffer.length > 0, 'image buffer contains data');
                test.done();
            });
        });
    },
};

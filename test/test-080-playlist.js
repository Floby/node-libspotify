var sp = require('../lib/libspotify');
var testutil = require('./util');
var timed = testutil.timed;

var session = null;

exports.playlist = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        })
    },
    'get playlist from URI': function(test) {
        var playlist;
        test.doesNotThrow(function() {
            playlist = sp.Playlist.getFromUrl('spotify:user:flobyiv:playlist:5ZMnMnJWGXZ9qm4gacHpQF');
            test.ok(playlist instanceof sp.Playlist, 'We should get a playlist object');
            test.done()
        }, 'getting playlist from url should not throw')
    },
    'attributes are mapped': function(test) {
        var playlist = sp.Playlist.getFromUrl('spotify:user:flobyiv:playlist:5ZMnMnJWGXZ9qm4gacHpQF');
        playlist.whenReady(function() {
            test.doesNotThrow(function() {
                test.equal(playlist.name, "Daft Punk - Random Access Memories", "The title should be Daft Punk - Randome Access Memories");
                test.equal(playlist.numSubscribers, 0, "There would be no subscribers");
                test.equal(playlist.getNumTracks(), 13, 'There should be 13 tracks on Daft Punk - RAM');
                test.done();
            }, "getting attributes should not throw")
        })
    },
    'get tracks': timed(10000, function(test) {
        var playlist = sp.Playlist.getFromUrl('spotify:user:flobyiv:playlist:5ZMnMnJWGXZ9qm4gacHpQF');
        playlist.whenReady(function() {
            playlist.getTracks(function(tracks) {
                test.ok(Array.isArray(tracks), 'tracks should be an array');
                test.equal(tracks.length, 13, 'There should be 13 tracks in the array');
                test.equal(tracks.map(function(e) {return e instanceof sp.Track}).indexOf(false), -1, 'It should only contain tracks');
                test.equal(tracks.reduce(function(prev, current) {
                    return prev && current.isReady();
                }, true), true, 'All tracks should be loaded');
                test.done();
            })
        })
    })
};




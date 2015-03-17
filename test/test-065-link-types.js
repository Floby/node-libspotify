var sp = require('../lib/libspotify');
var testutil = require('./util');

var session = null;

exports.links = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        });
    },
    'get link type with non-string throw': function(test) {
        test.throws(function() {
            sp.getLinkType({});
        }, "Getting link type from anything else than string should throw");
        test.done();
    },
    'get link type with invalid url throws': function(test) {
        test.throws(function() {
            sp.getLinkType('not a url');
        }, "Getting link type should throw with invalid links");
        test.done();
    },
    'get link type': function (test) {
        test.throws(function() {
            sp.getLinkType({});
        }, "Getting link type from anything else than string should throw");

        var track_link = 'spotify:track:4BdSLkzKO6iMVCgw7A7JBl';
        var artist_link = 'spotify:artist:3zD5liDjbqljSRorrrcEjs';
        var playlist_link = 'spotify:user:flobyiv:playlist:5ZMnMnJWGXZ9qm4gacHpQF';
        test.doesNotThrow(function() {
            test.equal('track', sp.getLinkType(track_link), "Link type should be 'track'");
            test.equal('artist', sp.getLinkType(artist_link), "Link type should be 'artist'");
            test.equal('playlist', sp.getLinkType(playlist_link), "Link type should be 'playlist'");
        }, "Getting link types should not throw");
        test.done();
    }
};


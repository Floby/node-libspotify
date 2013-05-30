var sp = require('../lib/libspotify');
var testutil = require('./util');

var session = null;

function testLink (Class, url) {
    return function(test) {
        var object = Class.getFromUrl(url);
        object.whenReady(function() {
            test.doesNotThrow(function() {
                var link = object.getLink();
                test.equal('string', typeof link, 'Link should be a string');
                test.ok(object instanceof Class, "retrieved object should be an instance of the class");
                test.equal(Class._object_type, sp.getLinkType(link), "Class and link should have the same type");
                test.equal(link, url, "Retrieved link should be equal to the given url");
            }, 'getting link from object should not throw');
            test.done();
        })
    }
}

exports.links = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        });
    },
    "get link from track": testLink(sp.Track, 'spotify:track:4BdSLkzKO6iMVCgw7A7JBl'),
    "get link from artist": testLink(sp.Artist, 'spotify:artist:4ZCLbhEKI7019HKbk5RsUq'),
    "get link from playlist": testLink(sp.Playlist, 'spotify:user:flobyiv:playlist:2t8yWR57SFWSKHtOlWr095'),
    'get artist link from artist': function(test) {
        var track = sp.Track.getFromUrl('spotify:track:2Uqs7Gi9F0BHg3H7yhbpWm');
        track.on('ready', function() {
            var artist = track.artist;
            test.ok(artist instanceof sp.Artist, 'artist should be an instance of Artist');
            test.equal('Guillemots', artist.name, 'this should be Guillemots');
            test.equal(typeof artist.getUrl(), 'string', 'url should be a string');
            test.done();
        });
    },
    'get artist from link': function(test) {
        var track = sp.Artist.getFromUrl('spotify:artist:3zD5liDjbqljSRorrrcEjs');
        test.ok(track instanceof sp.Artist, 'the returned object should be an artist');
        track.on('ready', function() {
            test.equal('Guillemots', track.name, 'this should be a guillemots track');
            test.done();
        });
    },
};



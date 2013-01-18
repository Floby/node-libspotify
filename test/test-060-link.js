var sp = require('../lib/libspotify');
var testutil = require('./util');

var session = null;

exports.links = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        })
    },
    'track link from search': function(test) {
        var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
        search.execute(function() {
            test.ok(search.tracks.length > 0, 'the search should return at least one result');
            var first = search.tracks[0];
            test.doesNotThrow(function() {
                first.getUrl();
            }, URIError, 'getting a url from a track should not throw')
            test.equal(typeof first.getUrl(), 'string', 'the url should be a string');
            test.equal(first.getUrl(), 'spotify:track:2Uqs7Gi9F0BHg3H7yhbpWm', 'the track should be this one');
            test.done();
        });
    },
    'get track from link': function(test) {
        var track = sp.Track.getFromUrl('spotify:track:2Uqs7Gi9F0BHg3H7yhbpWm');
        test.ok(track instanceof sp.Track, 'the returned object should be a track');
        track.on('ready', function() {
            test.equal('Guillemots', track.artist, 'this should be a guillemots track');
            test.equal('Fleet', track.name, 'this should be a guillemots track named Fleet');
            test.done();
        });
    },
    'get http url from track': function(test) {
        var track = sp.Track.getFromUrl('spotify:track:2Uqs7Gi9F0BHg3H7yhbpWm');
        test.equal(track.getHttpUrl(), 'http://open.spotify.com/track/2Uqs7Gi9F0BHg3H7yhbpWm');
        test.done();
    },
    'get artist link from artist': function(test) {
        var track = sp.Track.getFromUrl('spotify:track:2Uqs7Gi9F0BHg3H7yhbpWm');
        track.on('ready', function() {
            var artist = track.artist;
            test.ok(artist instanceof sp.Artist, 'artist should be an instance of Artist')
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




var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');
var testutil = require('./util');
var trycatch = require('trycatch');

var session = null;

var search = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        })
    },

    testSearchFromString: function(test) {
        var search = null;
        test.doesNotThrow(function() {
            search = new sp.Search("Bogus search terms");
            search.on('ready', function() {
                test.ok(search.tracks.length == 0, "there should be not many results");
                test.done();
            });
            search.execute();
        });
    },

    testSearchGuillemotsSongFound: function(test) {
        var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
        search.on('ready', function() {
            test.ok(search.tracks.length > 0, "the search should return at least one result");
            test.done();
        });
        search.execute();
    },
};

exports.search = search;

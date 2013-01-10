var sp = require('../lib/libspotify');
var testutil = require('./util');

var session = null;

exports.album = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        })
    },
    'attributes are mapped': function(test) {
        var search = new sp.Search('artist:"Guillemots" track:"Fleet"');
        search.execute(function() {
            test.ok(search.tracks.length > 0, "the search should return at least one result");
            var first = search.tracks[0];
            var album = first.album;
            
            test.ok(album instanceof sp.Album, 'album should be an instance of Album');
            test.equal('Hello Land!', album.name, 'album should be Hello Land')
            test.equal('Guillemots', album.artist, 'album artist should be Guillemots')
            test.equal(2012, album.year, 'album came out in 2012');
            test.equal('UNKNOWN', album.type, 'album type is UNKNOWN'); // why is not ? :[

            return test.done();
        });
    }
};




var fs = require('fs');
var sp = require('../lib/libspotify');

var key = fs.readFileSync(__dirname + '/../spotify_key/spotify_appkey.key');

exports.testInstanciatingSessionWithNoArgumentThrows = function(test) {
    test.expect(2);
    test.throws(function() {
        var session = new sp.Session();
    }, TypeError, "instanciating a session without an application key should throw");
    test.throws(function() {
        var session = new sp.Session({biduleMachinChose:true});
    }, TypeError, "instanciating a session without an application key should throw");
    test.done();
}

exports.testInstanciatingSessionWithKey = function(test) {
    var session = null;
    test.doesNotThrow(function() {
        session = new sp.Session({
            applicationKey: key
        });
        session.close();
    });
    test.notDeepEqual(null, session, 'session should be a session object');
    test.ok(session instanceof sp.Session, 'session should be a session object');
    test.done();
}

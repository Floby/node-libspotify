var fs = require('fs');

exports.testCreatingSessionWithoutKeyFails = function(test) {
    test.expect(1);
    var sp = require('../lib/libspotify');
    test.throws(function() {
        var session = new sp.Session({});
    }, TypeError, 'could create session without application key')
    test.done();
}

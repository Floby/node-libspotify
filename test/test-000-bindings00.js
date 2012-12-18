var assert = require('assert');


exports.testLoadingAddon = function(test) {
    test.expect(3);

    var bindings = null
    test.doesNotThrow(function() {
        bindings = require('bindings')('spotify.node');
    })

    test.equal('object', typeof bindings, 'Bindings module is not an object');
    test.equal('Session', bindings.Session.name, 'Session is not named session');

    test.done();
}
exports.testLoadingModule = function(test) {
    test.expect(3);

    var sp = null
    test.doesNotThrow(function() {
        sp = require('../lib/libspotify');
    })

    test.equal('object', typeof sp, 'Bindings module is not an object');
    test.equal('Session', sp.Session.name, 'Session is not named session');

    test.done();
}

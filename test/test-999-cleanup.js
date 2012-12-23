var u = require('./util');

exports.cleanup = function(test) {
    u.getDefaultTestSession(function(s) {
        s.logout(function() {
            s.close();
            test.done();
        })
    });

}


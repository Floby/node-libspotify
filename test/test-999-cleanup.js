var u = require('./util');

exports["Not actually a test, just cleaning up"] = function(test) {
    u.getDefaultTestSession(function(s) {
        s.logout(function() {
            s.close();
            test.done();
        })
    });

}


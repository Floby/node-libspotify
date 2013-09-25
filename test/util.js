var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');

var session = null;

exports.getDefaultTestSession = function(cb) {
    if(session instanceof sp.Session) {
        if(session.isLoggedIn()) {
            setTimeout(function() {
                cb(session);
            });
            return;
        }
    }
    else {
        session = new sp.Session({
            applicationKey: __dirname + '/../spotify_key/spotify_appkey.key'
        });
        session.login(cred.login, cred.password);
    }
    session.once('login', function(err) {
        if(err) throw err;
        cb(session);
    });
};

exports.timed = function(timeout, cb) {
    return function(test) {
        var to = setTimeout(function() {
            test.fail('test timed out after ' + timeout.toLocaleString() + 'ms');
            test.done();
        }, timeout);

        var fakeTest = Object.create(test, {});
        fakeTest.done = function() {
            clearTimeout(to);
            test.done();
        };

        cb(fakeTest);
    };
};

Function.prototype.timed = function(timeout) {
    return exports.timed(timeout, this);
}

Function.prototype.caught = function() {
    var self = this;
    return function(test) {
        test.doesNotThrow(function() {
            self(test);
        });
    }
}
Function.prototype.withDomain = function(withStack) {
  var fn = this;
  return function(test) {
    var d = domain.create();
    d.on('error', function(e) {
      test.fail('test failed with ' + e.message);
      if(withStack) {
        console.error(e.stack)
      }
      test.done();
    });
    d.run(fn.bind(this, test));
  }
}


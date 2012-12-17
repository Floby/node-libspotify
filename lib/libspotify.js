var b = require('bindings')('spotify.node');
var EventEmitter = require('events').EventEmitter;

console.log(Object.keys(b));
console.log(b.hello());
console.log(b.bidule());

console.log(Object.keys(b.Session));


function inherits (target, source) {
    for(var k in source.prototype) {
        target.prototype[k] = source.prototype[k];
    }
}
inherits(b.Session, EventEmitter);

var fs = require('fs');
var key = fs.readFileSync(__dirname + '/../spotify_key/spotify_appkey.key');

var s = new b.Session(key);
s.bidule();
var cred = require('../spotify_key/passwd');
s.login(cred.login, cred.password);


var b = require('bindings')('spotify.node');

console.log(Object.keys(b));
console.log(b.hello());
console.log(b.bidule());

console.log(Object.keys(b.Session));
var s = new b.Session();
s.bidule();

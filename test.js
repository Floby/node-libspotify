var b = require('bindings')('spotify.node');

console.log(Object.keys(b));
console.log(b.hello());
console.log(b.bidule());

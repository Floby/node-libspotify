var b = require('bindings')('spotify.node');
var EventEmitter = require('events').EventEmitter;
var fs = require('fs');

function inherits (target, source) {
    for(var k in source.prototype) {
        target.prototype[k] = source.prototype[k];
    }
}
inherits(b.Session, EventEmitter);


function Session (params) {
    var base = {
    }
    inherits(params, base);

    console.log(params);
    
    var key = null;
    if(params.appKey instanceof Buffer) {
        key = params.appKey;
    }
    else if(typeof params.appKey == 'string') {
        key = fs.readFileSync(params.appKey);
    }
    else {
        throw new TypeError('No appKey specified for this spotify session');
    }
    this._session = new b.Session(key);
}

Session.prototype.login = function login(login, password) {
    this._session.login(login, password);
};


exports.Session = Session;

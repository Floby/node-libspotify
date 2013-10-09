var sp = require('../lib/libspotify');
var testutil = require('./util');

var session = null;

exports.links = {
    setUp: function(cb) {
        testutil.getDefaultTestSession(function(s) {
            session = s;
            cb();
        });
    },
    'get image for album cover': function(test) {
        var img = sp.Image.getFromUrl('spotify:image:1ffad0da52c24ff70554dbc2b70d2265be777816');
        img.whenReady(function () {
            var buffer = img.getData();
            test.ok(buffer instanceof Buffer, 'image data is a buffer');
            test.ok(buffer.length > 0, 'image buffer contains data');
            test.done();
        });
    },
    'get image for artist portrait': function (test) {
        var img = sp.Image.getFromUrl('spotify:image:32222dd4d53a339d92b1d8c72b678a1ec3e1840e');
        img.whenReady(function () {
            var buffer = img.getData();
            test.ok(buffer instanceof Buffer, 'image data is a buffer');
            test.ok(buffer.length > 0, 'image buffer contains data');
            test.done();
        });
    }
};

exports.Album = require('./Album');
exports.Artist = require('./Artist');
exports.Player = require('./Player');
exports.Search = require('./Search');
exports.Session = require('./Session');
exports.Track = require('./Track');
exports.Image = require('./Image');
exports.Playlist = require('./Playlist');
exports.PlaylistContainer = require('./PlaylistContainer');

var b = require('bindings')('spotify.node');

exports.getLinkType = function getLinkType(url) {
    if(typeof url !== 'string') {
        throw new TypeError("Given parameter is not a string. Spotify links are strings.");
    }
    var res = b.link_type(url);
    if(res === null) {
        throw new Error("Given parameter is probably not a valid URI");
    }
    return res;
}

[![Build Status](https://travis-ci.org/Floby/node-libspotify.png)](https://travis-ci.org/Floby/node-libspotify)

node-libspotify
===============

Node bindings for the libspotify C library

_This is still very alpha, but you can already play around I suppose_

Install & test
--------------

Please note that you must have the libspotify library installed on your system
you can get it from [here](https://developer.spotify.com/technologies/libspotify/).

You must provide your own spotify application key, because I'm not giving mine away :]
You also must provide valid credentials for a spotify account.

Once you cloned the repository
run `npm install` then test the module with `npm test`

Main objectives
---------------

As there are already a number of spotify bindings or modules for the REST API, the main goal
of this module is not to give access to the artist and tracks catalog. It's main purpose is
to allow for playback of the tracks. The idea is to expose a `Player` object in which the user
can load tracks, play them, and get decompressed audio data from it (as provided by the libspotify C library).

What the user can do with the audio stream is up to him/her. As it is raw PCM data, it is easy to pipe to
some kind of encoder (like gstreamer) in order to broadcast music or stream it to a web user (as long as it complies
with the Spotify terms of service ;).

The user can also choose to play the song locally with a node module like [speaker](http://github.com/TooTallNate/node-speaker)
or pipe the audio data to another process like [aplay](http://linux.die.net/man/1/aplay)


The main goal is now achieved. Audio data is exposed as the Player object which behaves like a readable stream

Snippet
-------

Here is a code snippet of how to play a track from spotify

```js

var sp = require('../lib/libspotify');
var cred = require('../spotify_key/passwd');
var fs = require('fs');
var spawn = require('child_process').spawn;

var f = fs.createWriteStream('/tmp/bidule.raw');

var session = new sp.Session({
    applicationKey: __dirname + '/../spotify_key/spotify_appkey.key'
});
session.login(cred.login, cred.password);
session.once('login', function(err) {
    if(err) this.emit('error', err);

    var search = new sp.Search('artist:"rick astley" track:"never gonna give you up"');
    search.trackCount = 1; // we're only interested in the first result;
    search.execute();
    search.once('ready', function() {
        if(!search.tracks.length) {
            console.error('there is no track to play :[');
            session.logout();
        }

        var track = search.tracks[0];
        var player = session.getPlayer();
        player.load(track);
        player.play();

        // linux
        var play = spawn('aplay', ['-c', 2, '-f', 'S16_LE', '-r', '44100']);
        // osx with `brew install sox`
        var play = spawn('play', ['-r', 44100, '-b', 16, '-L', '-c', 2, '-e', 'signed-integer', '-t', 'raw', '-']);

        player.pipe(play.stdin);

        console.error('playing track. end in %s', track.humanDuration);
        player.on('data', function(buffer) {
            // buffer.length
            // buffer.rate
            // buffer.channels
            // 16bit samples
        });
        player.once('track-end', function() {
            console.error('track ended');
            f.end();
            player.stop();
            session.close();
        });
    });
});

```


TODO
====

* Bind to the rest of the API...



Credits
=======

Thanks to [IainCole](https://github.com/IainCole/node-libspotify) for is help on the playlist subsystem.

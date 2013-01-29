---
submenu_item: tuto
layout: project
title: Quick Start
---

Because Spotify is far from being an open technology, there are certain limitations while using
this module. Namely:

* Only once session can live at once within the process
* The user must have a Premium Spotify account
* The developper must own a valid application key
* Only a Premium Spotify user can get an application key

#### Getting a Spotify application key

As I said just before, you can get a spotify application key only if you are yourself a premium spotify
user (9.90€/month). I like Spotify so it's not really a problem for me.

Go to [My Keys](https://developer.spotify.com/technologies/libspotify/keys/)
and make sure you are logged in.
You should see your application key waiting to be downloaded. Download it as a file (Binary).

#### Install libspotify
Downloading and installing libspotify is fairly easy. I'll give you the steps for a \*nix system.
Go to the [Libspotify Homepage](https://developer.spotify.com/technologies/libspotify) 
and download the archive according to your system.

Extract it somewhere, go in that folder in a terminal and execute <code>make install</code> with admin rights.

#### Finally getting to write some JS

now that we're all set with the boring stuff, we're finally going to write some Javascript.

##### Create a session and log in

```javascript

var sp = require('libspotify');
var session = new sp.Session({
    applicationKey: '/path/to/spotify/application.key'
});

session.once('login', function() {
    console.log('LOGGED IN TO SPOTIFY');
})
session.login(login, password);

```

As you can see logging in to spotify is fairly simple. Just create a session and call login with valid credentials.

##### Search for tracks

```javascript
var search = new sp.Search("Calvin Harris");
search.trackCount = 10; // get the 10 first results
search.execute(function() {
    console.log(search.tracks);
});
```

With node-libspotify, a search query is an object which you configure and then execute. You can alternatively listen
to the `ready` event.

##### Get a track from its spotify URL

```javascript
var track = sp.Track.getFromUrl('spotify:track:2Uqs7Gi9F0BHg3H7yhbpWm');
track.on('ready', function() {
    console.log(track);
});

```

In this example appears an important aspect of node-libspotify. Nearly all objects are loaded asynchronously. This means
that in this case, the `track` oject is just a placeholder at first and then gets populated with data. The `ready` event
is triggered when these data are loaded.


##### Stream a track from spotify

```javascript
var player = session.getPlayer();
player.load(track);
player.play();
player.on('data', function(buffer) {
    // buffer.length is the length of the buffer as usual
    // buffer.rate is the sample rate of the audio data in this buffer
    // buffer.channels is the number of channels of the audio data (usually 2 for stereo)
    // Samples are 16 bit signed integers
});
```

As you can see, libspotify only feeds the client with raw audio data. All processing and decompressing is done by libspotify
under the hood. You can see an example of how to actually play music from spotify in the
[example folder](http://github.com/Floby/node-libspotify/blob/master/example/play.js)
of the repository


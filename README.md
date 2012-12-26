node-libspotify
===============

Node bindings for the libspotify C library

_/!\ Early development version, don't use just yet /!\_


Main objectives
---------------

As there are already a number of spotify bindings or modules for the REST API, the main goal
of this module is not to give access to the artist and tracks catalog. It's main purpose is
to allow for playback of the tracks. The idea is to expose a `Player` object in which the user
can load tracks, play them, and get decompressed audio data from it (as provided by the libspotify C library).

What the user can do with the audio stream is up to him/her. As it is raw PCM data, it is easy to pipe to
some kind of encoder (like gstreamer) in order to broadcast music or stream it to a web user (as long as it complies
with the Spotify terms of service ;).

The user can also choose to play the song locally with a node module like [node-portaudio](https://github.com/joeferner/node-portaudio)
or pipe the audio data to another process like [play](http://linux.about.com/library/cmd/blcmdl1_play.htm)


The main goal is now achieved. Audio data is exposed as the Player object which behaves like a readable stream

---
submenu_item: home
layout: project
---
<p class="lead">
<strong>Node-libspotify</strong> is a <a href="http://nodejs.org">Node.js</a> module which allows the use from Javascript of the low-level Spotify API.
</p>
    


#### But why ?

Spotify is my favourite place to listen to my music. Most the time I'm only streaming it from the web.
When I'm on my phone it's pretty much all sync'd to my Android app.
I'm [FlobyIV](http://open.spotify.com/user/flobyiv) on Spotify and [FlobyIV](http://last.fm/user/FlobyIV) as well on Last.fm


Because I love Spotify and Node.js so much, I couldn't help but give a try at using the first one with the second one.
Spotify released [libspotify](http://developer.spotify.com/technologies/libspotify/) some time ago, which is a low-level
library (written in C) to use their service. It's not badly documented but it's probably the first time I had to bind to
something without the ability to read the actual code, because libspotify is still proprietary software.


#### Try it out
I suggest you start by reading the [Read me]({{ site.html_pages[0].url }}). Note the you will need to install libspotify
globally on your system so that node-libspotify can link to it during compilation.


#### Current state
I wouldn't call node-libspotify a "finished" project. There are still some parts of the API that are not accessible from
the Node side. But I'm still actively working on it, so it should be there soon enough.

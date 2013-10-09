{
  "targets": [
    {
      "target_name": "libspotify",
      "sources": [
        "src/album.cc",
        "src/albumbrowse.cc",
        "src/artist.cc",
        "src/artistbrowse.cc",
        "src/audio.cc",
        "src/binding.cc",
        "src/image.cc",
        "src/link.cc",
        "src/player.cc",
        "src/search.cc",
        "src/session.cc",
        "src/track.cc",
        "src/playlist.cc"
      ],
      "cflags": ["-Wall"],
      "conditions" : [
        [
          'OS!="win"', {
            "libraries" : [
              '-lspotify'
            ],
          }
        ],
        [
          'OS=="win"', {
            "libraries" : [
              '<(module_root_dir)/gyp/lib/libspotify.dll.a'
            ]
          }
        ]
      ]
    }
  ]
}

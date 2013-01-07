{
  "targets": [
    {
      "target_name": "libspotify",
      "sources": [
        "src/binding.cc",
        "src/session.cc",
        "src/search.cc",
        "src/track.cc",
        "src/artist.cc",
        "src/player.cc",
        "src/audio.cc",
        "src/link.cc",
      ],
      "cflags": ["-Wall", "-g", "-O0"],
      "conditions" : [
        [
          'OS!="win"', {
            "libraries" : [
              '-lspotify',
              '-lm'
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

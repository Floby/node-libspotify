{
  "targets": [
    {
      "target_name": "libspotify",
      "sources": [
        "src/binding.cc",
        "src/session.cc",
        "src/search.cc",
        "src/track.cc",
      ],
      "cflags": ["-Wall", "-g", "-O0"],
      "conditions" : [
        [
          'OS!="win"', {
            "libraries" : [
              '-lspotify',
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

{
  "targets": [
    {
      "target_name": "libspotify",
      "sources": [
        "src/binding.cc",
        "src/spotify.cc",
        "src/session.cc"
      ],
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

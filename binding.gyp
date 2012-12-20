{
  "targets": [
    {
      "target_name": "libspotify",
      "sources": [
        "src/binding.cc",
        "src/session.cc"
      ],
      "cflags": ["-Wall"],
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

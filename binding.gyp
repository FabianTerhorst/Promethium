{
  "targets": [
    {
      "target_name": "promethium",
      "sources": [ "src/addon.cpp", "src/process.cpp" ],
      "include_dirs": [
        "<!@(pkg-config libpromethium --cflags-only-I | sed s/-I//g)"
      ],
      "libraries": [
        "<!@(pkg-config libpromethium --libs)"
      ]
    }
  ]
}

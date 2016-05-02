{
  "targets": [
    {
      "target_name": "promethium",
      "sources": [ "src/addon.cpp", "src/process.cpp" ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}

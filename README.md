# GameGoogleTranslate

This is a collection of tools I have written to assist in making google translate mods for games. Execute the compiled programs to get usage info.

Currently supported games:

1. Minecraft
2. Hades (WIP)
3. Generic recursive translator

### How to build
#### Splitters
This project uses CMake. Simply execute the init script for your platform, or run the following commands:
```sh
mkdir -p build && cd build;
cmake ..
```

#### HyperTranslate
This project uses [CocoaPods](https://cocoapods.org) to load Google's offline translation library. Install CocoaPods, then:
```sh
cd HyperTranslate
pod install
open HyperTranslate.xcworkspace
```
It reads from a file in the Simulator's sandbox because the library is only available for iOS. See `ViewController.swift` for more information
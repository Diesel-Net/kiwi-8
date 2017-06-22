# Kiwi8
**v1.03**

![boot](/images/screenshots/boot.png)

A cross-platform Chip-8 interpreter written 
in C++ using SDL2, ImGui, and OpenGL. 

**[Compatibility Notes](https://github.com/tomdaley92/Kiwi8/issues/9)**

**[Downloads](https://github.com/tomdaley92/Kiwi8/releases)**

**Features**:
 - MacOS and Windows
 - Fully featured GUI
 - Audio
 - Custom color pallete
 - CPU frequency selection
 - Command line support
 - No SCHIP support

## Building on Windows
Microsoft's Visual C++ Build Tools 
(vcvarsall/cl/nmake) are assumed to be 
installed and added to PATH.
1) Open the command prompt and navigate 
   to the Kiwi8/Windows directory.
2) Type `vcvarsall x86` to load the 
   windows development environment.
3) Type `nmake`.

## Building on MacOS
Apple's Xcode command line tools 
(clang++/make) are assumed to be 
installed and added to PATH.
1) Open the terminal and navigate 
   to the Kiwi8/MacOS directory.
2) Type `make`.

## Resources
* [Chip-8 wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
* [Emu-Docs](https://github.com/Emu-Docs/Emu-Docs)
* [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [SDL Wiki](https://wiki.libsdl.org/)
* [ImGui](https://github.com/ocornut/imgui)
* [/r/EmuDev](https://www.reddit.com/r/EmuDev/)

## Usage
    Kiwi8 [filename] [-FMLSV]
    -F      Launch in fullscreen
    -M      Launch with audio muted
    -L      Disable load/store quirk
    -S      Disable shift quirk
    -V      Disable vertical wrapping

_Enjoy!_

# Kiwi8
**v1.03**

A cross-platform Chip-8 interpreter written 
in C++ using SDL2, ImGui, and OpenGL.



I have included pre-built binaries for both Windows
and MacOS (.exe file and .app bundle respectively)

The interpreter passes "SC Test.ch8" up to 
error 23, which is the first SCHIP opcode 
encountered.

**[Compatibility Notes](https://github.com/tomdaley92/Kiwi8/issues/9)**
**[Download](https://github.com/tomdaley92/Kiwi8/releases)**

## Resources
* [Chip-8 wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
* [Emu-Docs](https://github.com/Emu-Docs/Emu-Docs)
* [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [SDL Wiki](https://wiki.libsdl.org/)
* [/r/EmuDev](https://www.reddit.com/r/EmuDev/)

## Building on Windows:
Microsoft's Visual C++ Build Tools 
(vcvarsall/cl/nmake) are assumed to be 
installed and added to PATH.
1) Open the command prompt and navigate 
   to the Kiwi8/Windows directory.
2) Type `vcvarsall x86` to load the 
   windows development environment.
3) Type `nmake`.

## Building on MacOS:
Apple's Xcode command line tools 
(clang++/make) are assumed to be 
installed and added to PATH.
1) Open the terminal and navigate 
   to the Kiwi8/MacOS directory.
2) Type `make`.

## Usage
    Kiwi8 [filename] [-FMLSV]
    -F      Launch in fullscreen
    -M      Launch with audio muted
    -L      Disable load/store quirk
    -S      Disable shift quirk
    -V      Disable vertical wrapping

_Enjoy!_


# Kiwi8
**v1.03**

![Windows_boot](/images/screenshots/Windows_boot.png)
![MacOS_boot](/images/screenshots/MacOS_boot.png)

A cross-platform Chip-8 interpreter written 
in C++ using SDL2, ImGui, and OpenGL.

**[Compatibility Notes](https://github.com/tomdaley92/Kiwi8/issues/9)**

**[Downloads](https://github.com/tomdaley92/Kiwi8/releases)**

## Features
 - Windows and MacOS
 - Graphical user interface
 - Audio
 - Color customizer
 - CPU frequency selection
 - Command line support
 - No SCHIP support

## Usage
    Kiwi8 [filename] [-FMLSV]
    -F      Launch in fullscreen
    -M      Launch with audio muted
    -L      Disable load/store quirk
    -S      Disable shift quirk
    -V      Disable vertical wrapping

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
(clang++/make/install_name_tool) 
are assumed to be installed and 
added to PATH.
1) Open the terminal and navigate 
   to the Kiwi8/MacOS directory.
2) Type `make`.

## Resources
- [Chip-8 wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
- [Emu-Docs](https://github.com/Emu-Docs/Emu-Docs)
- [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [SDL Wiki](https://wiki.libsdl.org/)
- [ImGui](https://github.com/ocornut/imgui)
- [/r/EmuDev](https://www.reddit.com/r/EmuDev/)

---

I can highly recommend this project to anyone 
wanting to get their feet wet with emulator 
programming, video-game programming, or even 
cross-platform development. It has been a great 
educational excercise involving major aspects 
of modern day applications such as input handling, 
user-interface design, 2D graphics rendering, 
audio generation, code profiling and more.

_Enjoy!_

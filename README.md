# Kiwi8
**v1.03**

![boot](/images/screenshots/Kiwi8_Linux_x86_64_ericb_2017_07_06.png)

![boot](/images/screenshots/boot.png)

A cross-platform Chip-8 interpreter written 
in C++ using SDL2, ImGui, and OpenGL.

**[Compatibility Notes](https://github.com/tomdaley92/Kiwi8/issues/9)**

**[Downloads](https://github.com/tomdaley92/Kiwi8/releases)**

## Features
 - Windows, MacOS and Linux**
 - Graphical user interface
 - Audio
 - Color customizer
 - CPU frequency selection
 - Command line support
 - No SCHIP support

**(tested on Intel x86_64, but may work wit other archs)

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

## Building on Linux

1) go into the Linux directory
2) read the README file, and check you got all dependencies installed

3) once everything is installed, you can build the Kiwi8 binary typing :

make

if nothing went wrong, the binary is located in the "Linux/release" subdir.

cd release 
To test : 

./Kiwi8 [options] # see the options above 

Notice : the Linux implementation is a terrible hack, and you use it at your own risks !
-- 
ericb

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

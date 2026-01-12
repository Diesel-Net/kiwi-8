# Kiwi8

![boot](/images/screenshots/boot.png)

A cross-platform Chip-8 interpreter written
in C-style C++ using SDL2, ImGui, and OpenGL.

**[<ins>Compatibility Notes<ins>](https://github.com/tomdaley92/Kiwi8/issues/9)**<br>
**[<ins>Downloads<ins>](https://github.com/tomdaley92/Kiwi8/releases)**

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

Microsoft's Visual C++ Build Tools must be installed and added to your **PATH**:

>vcvarsall<br>
>nmake<br>
>cl<br>

1. Change current working directory

   ```cmd
   cd Windows
   ```

1. Configure environment for 64-bit architecture

   ```cmd
   vcvarsall.bat x86_amd64
   ```

1. Compile with Microsoft's `nmake` utility

   ```cmd
   nmake
   ```

## Building on MacOS

Apple's Xcode command line tools must be installed and added to your **PATH**:

>make<br>
>clang++<br>
>install_name_tool<br>
>otool<br>

1. Change current working directory

   ```bash
   cd MacOS
   ```

1. Compile with GNU's `make` utility

   ```bash
   make
   ```

## Resources

- [Chip-8 wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
- [Emu-Docs](https://github.com/Emu-Docs/Emu-Docs)
- [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [SDL Wiki](https://wiki.libsdl.org/)
- [ImGui](https://github.com/ocornut/imgui)
- [/r/EmuDev](https://www.reddit.com/r/EmuDev/)

---
<br>
<br>

"I can _highly_ recommend this project to anyone
wanting to get their feet wet in reverse-engineering, game-programming, and/or
cross-platform development. It's been a great
educational excercise involving major aspects
of modern day applications such as input handling,
user-interface design, 2D graphics rendering,
audio generation, code profiling and more!"

_Enjoy!_

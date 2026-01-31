

# Kiwi-8

[![Release](https://github.com/Diesel-Net/kiwi-8/actions/workflows/release.yml/badge.svg)](https://github.com/Diesel-Net/kiwi-8/actions/workflows/release.yml)
[![Build](https://github.com/Diesel-Net/kiwi-8/actions/workflows/build.yml/badge.svg)](https://github.com/Diesel-Net/kiwi-8/actions/workflows/build.yml)
[![Lint](https://github.com/Diesel-Net/kiwi-8/actions/workflows/lint.yml/badge.svg)](https://github.com/Diesel-Net/kiwi-8/actions/workflows/lint.yml)
<br>

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

## Controls


    controls        <-->        keybindings

    1 2 3 C                     1 2 3 4
    4 5 6 D                     q w e r
    7 8 9 E                     a s d f
    A 0 B F                     z x c v
    increase speed              page up
    decrease speed              page down
    quit                        esc
    toggle fullscreen           enter
    toggle menu                 left alt
    show fps                    right alt
    soft reset                  f5
    pause                       p
    mute                        m


## Usage

      Kiwi8 [options] [rom]
      Options:
         -F    Fullscreen
         -M    Mute audio
         -L    Disable load/store quirk
         -S    Disable shift quirk
         -V    Disable vertical wrapping
         -H    Enable horizontal wrapping (DXYN)
         -J    Enable jump with VX offset (BNNN)
         -G    Enable logic ops VF=0 (8XY1/2/3)
         -I    Enable I+VX overflow quirk (FX1E)
         -D    Enable draw flag reset quirk
         -h, --help  Show this help message

## Building on Windows

The following must be installed and added to your **PATH**:

>vcvarsall<br>
>cl<br>
>python3<br>
>cmake<br>
>nmake<br>

1. Change current working directory

   ```cmd
   cd Windows
   ```

1. Configure environment for your architecture (x64 or arm64)

   ```cmd
   vcvarsall.bat x64
   ```

1. Compile with Microsoft's `nmake` utility

   ```cmd
   nmake
   ```

## Building on MacOS

The following must be installed and added to your **PATH**:

>install_name_tool<br>
>clang<br>
>python3<br>
>cmake<br>
>make<br>

1. Change current working directory

   ```bash
   cd MacOS
   ```

1. Compile with GNU's `make` utility

   ```bash
   make
   ```

## Building on Linux (Debian)

The following must be added to your **PATH**:

>g++<br>
>make<br>
>cmake<br>
>python3<br>
>pkg-config<br>

You'll also need the GTK3, openGL, PulseAudio/Pipewire headers:

```bash
sudo apt install libgtk-3-dev xorg-dev libgl1-mesa-dev libasound2-dev libpulse-dev libpipewire-0.3-dev libdbus-1-dev
```

1. Change current working directory

   ```bash
   cd linux
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
- [Tobias V. Langhoff's Guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator)

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

_- Tom_

Kiwi8 v1.03

A cross-platform Chip-8 interpreter written 
in C++ using SDL2, ImGui, and OpenGL.

For more information about the Chip-8 visit:
https://en.wikipedia.org/wiki/CHIP-8

I have included pre-built binaries for both Windows
and MacOS (.exe file and .app bundle respectively)

You can download them here:
https://github.com/tomdaley92/Kiwi8/releases

The interpreter passes "SC Test.ch8" up to 
error 23, which is the first SCHIP opcode 
encountered.

ROM Compatibility is discussed here:
https://github.com/tomdaley92/Kiwi8/issues/9


Controls        --->        keybindings

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


How to compile:

    Windows:
        Microsoft's Visual C++ Build Tools 
        (vcvarsall/cl/nmake) are assumed to be 
        installed and added to PATH.
        1) Open the command prompt and navigate 
           to the Kiwi8/Windows directory.
        2) Type "vcvarsall x86" to load the 
           windows development environment.
        3) Type "nmake".

    MacOS:
        Apple's Xcode command line tools 
        (clang++/make) are assumed to be 
        installed and added to PATH.
        1) Open the terminal and navigate 
           to the Kiwi8/MacOS directory.
        2) Type "make".


Usage: Kiwi8 [filename] [-FMLSV]
    -F      Launch in fullscreen
    -M      Launch with audio muted
    -L      Disable load/store quirk
    -S      Disable shift quirk
    -V      Disable vertical wrapping

Enjoy!

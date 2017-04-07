Chip8 v1.02

A CHIP-8 interpreter written in C++ using the 
Simple DirectMedia Layer library.

For more information about the CHIP-8 visit:
https://en.wikipedia.org/wiki/CHIP-8

I have included pre-built binaries for both Windows
and MacOS (.exe file and .app bundle respectively)

Most games are playable, however there are 
still some issues with a select few. 
No audio is implemented. A work in progress.

ROM Compatibility discussed here:
https://github.com/tomdaley92/Chip8/issues/9


Controls        --->        keybindings

1 2 3 C                     1 2 3 4
4 5 6 D                     q w e r
7 8 9 E                     a s d f
A 0 B F                     z x c v
quit                        esc
toggle fullscreen           alt + enter
soft reset                  f5


How to compile:

    Windows:
        Microsoft's Visual C++ Build Tools 
        (vcvarsall/cl/nmake) are assumed to be 
        installed and added to PATH.
        1) Open the command prompt and navigate 
           to the Chip8/Windows directory.
        2) Type "vcvarsall x86" to load the 
           windows development environment.
        3) Type "nmake".

    MacOS:
        Apple's Xcode command line tools 
        (clang++/make) are assumed to be 
        installed and added to PATH.
        1) Open the terminal and navigate 
           to the Chip8/MacOS directory.
        2) Type "make".


Usage: Chip8 PATH_TO_ROM [-F] [R G B]
    -F      Launch in fullscreen
    R G B   Render color in RGB format, 
            3 numbers from 0-255

    Note: If you launch the program with no 
          arguments, you will be prompted to 
          load a ROM file on startup.


Enjoy!

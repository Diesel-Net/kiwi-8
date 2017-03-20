Chip8 v1.0

A Chip-8 interpreter written in C++ using the Simple 
DirectMedia Layer library.

For more information about the Chip-8 visit:
https://en.wikipedia.org/wiki/CHIP-8

I have included pre-built binaries for both Windows
and Mac OS X (.exe file and .app bundle respectively)

Most games are playable, however there are 
still quite a few bugs here and there. 
No audio is implemented. A work in progress.


Controls        --->        keybindings

1 2 3 C                     1 2 3 4
4 5 6 D                     q w e r
7 8 9 E                     a s d f
A 0 B F                     z x c v
quit                        esc
toggle fullscreen           alt + enter
soft reset                  (not yet implemented)


How to compile:

    Windows:
        Microsoft's Visual C++ Build Tools 
        (vcvarsall.bat/nmake) are assumed to be 
        installed and added to PATH.
        1) Open a command prompt window and navigate 
           to Chip8/Windows directory.
        2) Type "vcvarsall" to load the windows 
           development evironment.
        3) Type "nmake".

    MacOS:
        Apple's Xcode command line tools 
        (g++/make) are assumed to be installed and 
        added to PATH.
        1) Open a terminal and navigate 
           to Chip8/MacOS directory.
        2) Type "make".


Usage: Chip8 PATH_TO_ROM [-F] [R G B]
    -F      Launch in Fullscreen
    R G B   Render color in RGB format, 3 numbers
            from 0-255

    Note: If you launch the game with no arguments, 
          you will be prompted to load a ROM file on 
          startup.


Enjoy!

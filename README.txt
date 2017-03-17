Chip8 v1.0

A Chip-8 interpreter written in C++ using the Simple 
DirectMedia Layer library.

I have included pre-built binaries for both Windows
and Mac OS X (.exe file and .app bundle respectively)

Most games seem to work properly, however there are 
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


To compile:

    1) Open a terminal/command-prompt and navigate to either the 
       Windows or MacOS directory.

    On Windows:
        Note: Microsoft Visual C++ Build Tools are assumed to be 
        installed and added to PATH.
        2) Type "vcvarsall x86".
        3) Type "nmake".

    On MacOS:
        Note: GCC is assumed to be installed and added to PATH.
        2) Type "make".

Usage: Chip8 PATH_TO_ROM [-F] [R G B]\n"
    -F      Launch in Fullscreen
    R G B   Render color in RGB format, 3 numbers between 0-255

    * If you launch the game with no arguments, you will be prompted
      to choose a ROM to load on startup.

Enjoy!

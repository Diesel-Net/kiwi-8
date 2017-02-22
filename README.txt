Chip8 v1.0

A Chip-8 interpreter written in C++ using the Simple 
DirectMedia Layer library.

I have included pre-built binaries for both Windows
and Mac OS X (.exe file and .app bundle respectively)

Most games seem to work properly, however there are 
still quite a few bugs here and there. 
No audio is implemented. A work in progress.


Controls		--->		keybindings

1 2 3 C						1 2 3 4          
4 5 6 D 					q w e r 
7 8 9 E 					a s d f
A 0 B F 					z x c v
quit 						esc
toggle fullscreen			alt + enter
soft reset 					(not yet implemented)

	
Note: If for any reason you need to build from scratch,
be aware that the makefiles expect the SDL libraries to 
be in a folder named "frameworks" for everything to 
compile properly.

Enjoy!

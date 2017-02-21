# Author: Thomas Daley
# Date: September 8, 2016
# Purpose: Builds the application into a standalone .app bundle for distribution

CC = g++
CFLAGS = -std=c++11 -Wall
INCS = -I$(CURDIR)/frameworks/sdl/include/
LIBS = -L$(CURDIR)/frameworks/sdl/lib/ -lSDL2-2.0.0 -framework Cocoa -framework Carbon
MAIN = main.mm

# These must be named consistently with Info.plist file:
ICON = chip8.icns
APPNAME = Chip8

# Default target
$(APPNAME): main.o objects
	# Removing previous build
	cd release; $(RM) -r *.app

	# Compiling main executable
	$(CC) $(CFLAGS) $(LIBS) $(INCS) *.o -o $@ $(LFLAGS)

	# Remove leftover objects 
	$(RM) *.o

	# Creating empty .app bundle
	mkdir $@.app
	cd $@.app; mkdir Contents; cd Contents; mkdir MacOS; mkdir Resources; mkdir Frameworks

	# Copying the frameworks into .app bundle
	cp frameworks/sdl/lib/libSDL2-2.0.0.dylib $@.app/Contents/Frameworks/libSDL2-2.0.0.dylib

	# Re-linking the dylibs to newly compiled executable
	install_name_tool -change /usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib $@ 

	# Moving the configured executable and copy other files into .app bundle
	cp resources/$(ICON) $@.app/Contents/Resources/$(ICON)
	cp resources/Info.plist $@.app/Contents/Info.plist
	mv $@ $@.app/Contents/MacOS/$@
	mv $@.app release/$@.app

# Build object files
objects:	src/*.c* ../Core/*.c*
	$(CC) $(CFLAGS) -c $(INCS) $(CFLAGS) $?

main.o:	src/$(MAIN)
	$(CC) $(CFLAGS) -c $(INCS) $(CFLAGS) $<

# Tests the release version
test:
	open ./release/$(APPNAME).app --args "/Users/tomdaley92/Desktop/Chip8/Roms/Astro Dodge [Revival Studios, 2008].ch8" -F 197 23 100

# Runs the program with an attached terminal (Application must be built prior)
debug:
	./release/$(APPNAME).app/Contents/MacOS/$(APPNAME) "../Roms/Astro Dodge [Revival Studios, 2008].ch8" 255 0 0

# Removes latest build and object files
clean:
	$(RM) *.o *.app
	cd release; $(RM) -r *.app

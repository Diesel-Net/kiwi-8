# Author: Thomas Daley
# Date: September 8, 2016
# Purpose: Builds the application into a standalone .app bundle for distribution

CC = clang++
CFLAGS = -std=c++11 -Wall -mmacosx-version-min=10.9
LFLAGS = -mmacosx-version-min=10.9
INCS = -I$(CURDIR)/frameworks/sdl/include/ \
	   -I$(CURDIR)/../frameworks/imgui/

LIBS = -L$(CURDIR)/frameworks/sdl/lib/ -lSDL2-2.0.0 -framework Cocoa -framework OpenGL

# These must be named consistently with Info.plist file:
ICON = chip8.icns
APPNAME = Chip8

# Default target
$(APPNAME): objects
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

	# Moving the configured executable and copying other files into .app bundle
	cp resources/$(ICON) $@.app/Contents/Resources/$(ICON)
	cp src/Info.plist $@.app/Contents/Info.plist
	mv $@ $@.app/Contents/MacOS/$@
	mv $@.app release/$@.app

# Build object files
objects:	src/*.mm ../Core/*.c* ../frameworks/imgui/imgui*.cpp
	$(CC) $(CFLAGS) -c $(INCS) $(CFLAGS) $?

# Tests the release version
test_release:
	open ./release/$(APPNAME).app --args \

# Runs the program with an attached terminal (Application must be built prior)
test_debug:
	./release/$(APPNAME).app/Contents/MacOS/$(APPNAME) \
	"/Users/tomdaley92/Chip8/Roms/Chip-8 Pack/Chip-8 Demos/Zero Demo [zeroZshadow, 2007].ch8" \

# Removes latest build and object files
clean:
	$(RM) *.o *.app
	cd release; $(RM) -r *.app

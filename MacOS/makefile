CC = clang++
CFLAGS = -std=c++17 -Wall -mmacosx-version-min=11.0 -arch x86_64 -arch arm64
LFLAGS = -mmacosx-version-min=11.0
INCS = -I$(CURDIR)/frameworks/sdl/include/ \
	   -I$(CURDIR)/../frameworks/imgui/
LIBS = -L$(CURDIR)/frameworks/sdl/lib/ \
		-lSDL2-2.0.0 \
		-framework Cocoa \
		-framework OpenGL

APPNAME = Kiwi8

# SDL dependency files
SDL_LIB = frameworks/sdl/lib/libSDL2-2.0.0.dylib
SDL_HEADERS = frameworks/sdl/include/SDL2/SDL.h

all: $(APPNAME)
	$(RM) *.o *.app

# Build SDL if not present
sdl: $(SDL_LIB) $(SDL_HEADERS)

$(SDL_LIB) $(SDL_HEADERS):
	bash ../tools/build_sdl_macos.sh

# Default target
$(APPNAME): sdl clean objects
	# Compiling main executable
	$(CC) $(CFLAGS) $(LIBS) $(INCS) *.o -o $@ $(LFLAGS)

	# Creating empty .app bundle
	mkdir $@.app
	cd $@.app; mkdir Contents; cd Contents; mkdir MacOS; mkdir Resources; mkdir Frameworks

	# Copying the frameworks into .app bundle
	cp frameworks/sdl/lib/libSDL2-2.0.0.dylib $@.app/Contents/Frameworks/libSDL2-2.0.0.dylib

	# Set the dylib's install name to be relative to the bundle
	install_name_tool -id @executable_path/../Frameworks/libSDL2-2.0.0.dylib $@.app/Contents/Frameworks/libSDL2-2.0.0.dylib

	# Re-linking the dylibs to newly compiled executable (handle previous absolute path and @rpath)
	install_name_tool -change /usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib $@
	install_name_tool -change @rpath/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib $@

	# Moving the configured executable and copying other files into .app bundle
	cp resources/$@.icns $@.app/Contents/Resources/$@.icns
	cp src/Info.plist $@.app/Contents/Info.plist
	mv $@ $@.app/Contents/MacOS/$@
	mkdir release
	mv $@.app release/$@.app

	# Verification: ensure the executable and dylib have correct references
	otool -L release/$@.app/Contents/MacOS/$@ | sed -n '1,120p' | grep -q '@executable_path/../Frameworks/libSDL2-2.0.0.dylib' || (echo 'Error: executable not linked to bundled SDL' && exit 1)
	otool -L release/$@.app/Contents/Frameworks/libSDL2-2.0.0.dylib | sed -n '1,120p' | grep -q '@executable_path/../Frameworks/libSDL2-2.0.0.dylib' || (echo 'Error: SDL dylib install name not set' && exit 1)

# Builds object files
objects:	src/*.mm ../Core/*.c* ../frameworks/imgui/imgui*.cpp
	$(CC) $(CFLAGS) -c $(INCS) $(CFLAGS) $?

# Tests the release version
test_release:
	open ./release/$(APPNAME).app --args \

# Runs the program with an attached terminal (Application must be built prior)
test_debug:
	./release/$(APPNAME).app/Contents/MacOS/$(APPNAME) \


# Check dylib linking inside release bundle
check-dylibs:
	otool -L release/$(APPNAME).app/Contents/MacOS/$(APPNAME) | sed -n '1,120p'
	otool -L release/$(APPNAME).app/Contents/Frameworks/libSDL2-2.0.0.dylib | sed -n '1,120p'


# Removes build artifacts (objects, app bundle) but keeps SDL
clean:
	$(RM) *.o *.app
	$(RM) -r release

# Removes just SDL (for forcing recompile)
clean-sdl:
	$(RM) -r frameworks/sdl/lib
	$(RM) -r frameworks/sdl/include

# Removes everything including SDL (full reset)
distclean: clean clean-sdl
	$(RM) -r frameworks/sdl/backups
	$(RM) -r ../MacOS/third_party

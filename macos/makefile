APP_NAME = Kiwi8

# Use VERSION if set, otherwise fall back to GITHUB_REF_NAME, then default to develop
VERSION ?= $(GITHUB_REF_NAME)
ifeq ($(VERSION),)
VERSION = develop
endif

# Use SUB_VERSION if set, otherwise fall back to GITHUB_SHA, then default to unknown
SUB_VERSION ?= $(GITHUB_SHA)
ifeq ($(SUB_VERSION),)
SUB_VERSION = unknown
endif

APP_MANIFEST = src/Info.plist
APP_BUNDLE = $(APP_NAME).app
APP_EXE = $(APP_NAME)

CC = clang
CXX = clang++
CFLAGS = -std=c11
CPPFLAGS = -Wall \
	-mmacosx-version-min=11.0 \
	-MMD -MP -DAPP_NAME='"$(APP_NAME)"' \
	-DVERSION='"$(VERSION)"' \
	-DSUB_VERSION='"$(SUB_VERSION)"'

CXXFLAGS = -std=c++11
INCLUDE_DIRS = -I../shared \
	-I../external/sdl/build/include/ \
	-I../external/imgui/ \
	-I../external/stb/ \
	-I../external/lekkit/

LDFLAGS = -mmacosx-version-min=11.0 \
	-L../external/sdl/build/lib/ \
	-lSDL2-2.0.0 \
	-framework Cocoa \
	-framework OpenGL

SDL_LIB = ../external/sdl/build/lib/libSDL2-2.0.0.dylib

# Generated headers
LICENSE_HEADER = ../shared/license.h
BOOTROM_HEADER = ../shared/bootrom.h
BOOTROM_SOURCE = ../roms/Kiwi8_logo_2.ch8

OBJS = audio.o \
	chip8.o \
	display.o \
	gui.o \
	input.o \
	main.o \
	toast.o \
	open_file_dialog.o \
	stb_ds.o \
	profiles.o \
	imgui.o \
	imgui_draw.o \
	imgui_impl_sdl.o \
	sha256.o

DEPS = $(OBJS:.o=.d)

PROFILES_INI = ../shared/profiles.ini

# Include auto-generated dependency files (must be before targets)
-include $(DEPS)

.DEFAULT_GOAL := all

all: debug/profiles.ini debug/$(APP_EXE) release/$(APP_BUNDLE)/Contents/Resources/profiles.ini release/$(APP_BUNDLE)

# Build SDL (force rebuild with: make sdl)
sdl:
	bash build_sdl.sh

# Build SDL if lib doesn't exist (headers come together)
$(SDL_LIB):
	bash build_sdl.sh

# Generate license header from LICENSE file
$(LICENSE_HEADER): ../LICENSE
	python3 ../tools/generate_license_header.py ../LICENSE $(LICENSE_HEADER)

# Generate bootrom header from ROM file
$(BOOTROM_HEADER): $(BOOTROM_SOURCE)
	python3 ../tools/generate_bootrom_header.py $(BOOTROM_SOURCE) $(BOOTROM_HEADER)

# Generate Info.plist from template with version substitution
$(APP_MANIFEST): $(APP_MANIFEST).in
	sed -e 's/@APP_NAME@/$(APP_NAME)/g' -e 's/@VERSION@/$(VERSION)/g' -e 's/@SUB_VERSION@/$(SUB_VERSION)/g' $< > $@

# Pattern rules for incremental compilation
%.o: src/%.mm
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: ../external/imgui/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: ../external/lekkit/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: ../external/stb/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: ../shared/%.c $(LICENSE_HEADER) $(BOOTROM_HEADER)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: ../shared/%.cc $(LICENSE_HEADER) $(BOOTROM_HEADER)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@

debug/profiles.ini: $(PROFILES_INI)
	mkdir -p debug
	cp $(PROFILES_INI) debug/profiles.ini

release/$(APP_BUNDLE)/Contents/Resources/profiles.ini: $(PROFILES_INI)
	mkdir -p release/$(APP_BUNDLE)/Contents/Resources
	cp $(PROFILES_INI) release/$(APP_BUNDLE)/Contents/Resources/profiles.ini

# debug executable
debug/$(APP_EXE): $(SDL_LIB) $(OBJS)
	mkdir -p debug
	cp $(SDL_LIB) debug/libSDL2-2.0.0.dylib
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -g $(INCLUDE_DIRS) $(OBJS) -o $@ $(LDFLAGS)

	# Set the dylib's install name to be relative to the bundle
	install_name_tool -id @executable_path/libSDL2-2.0.0.dylib debug/libSDL2-2.0.0.dylib

	# Fix executable's reference to SDL
	install_name_tool -change @rpath/libSDL2-2.0.0.dylib @executable_path/libSDL2-2.0.0.dylib debug/$(APP_EXE)

# release executable (inside .app bundle)
release/$(APP_BUNDLE)/Contents/MacOS/$(APP_EXE): $(SDL_LIB) $(OBJS)
	mkdir -p release/$(APP_BUNDLE)/Contents/MacOS
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) $(OBJS) -o $@ $(LDFLAGS)

	# Fix executable's reference to SDL (from @rpath to bundle-relative path)
	install_name_tool -change @rpath/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib release/$(APP_BUNDLE)/Contents/MacOS/$(APP_EXE)

# Info.plist in the bundle
release/$(APP_BUNDLE)/Contents/Info.plist: $(APP_MANIFEST)
	mkdir -p release/$(APP_BUNDLE)/Contents
	cp $(APP_MANIFEST) $@

# Icon file in the bundle
release/$(APP_BUNDLE)/Contents/Resources/Kiwi8.icns: resources/Kiwi8.icns
	mkdir -p release/$(APP_BUNDLE)/Contents/Resources
	cp resources/Kiwi8.icns $@

# SDL dylib in the bundle
release/$(APP_BUNDLE)/Contents/Frameworks/libSDL2-2.0.0.dylib: $(SDL_LIB)
	mkdir -p release/$(APP_BUNDLE)/Contents/Frameworks

	# Copy the frameworks into .app bundle
	cp $(SDL_LIB) $@

	# Set the dylib's install name to be relative to the bundle
	install_name_tool -id @executable_path/../Frameworks/libSDL2-2.0.0.dylib release/$(APP_BUNDLE)/Contents/Frameworks/libSDL2-2.0.0.dylib

# .app bundle
release/$(APP_BUNDLE): release/$(APP_BUNDLE)/Contents/MacOS/$(APP_EXE) release/$(APP_BUNDLE)/Contents/Info.plist release/$(APP_BUNDLE)/Contents/Resources/Kiwi8.icns release/$(APP_BUNDLE)/Contents/Frameworks/libSDL2-2.0.0.dylib

run-debug: debug/$(APP_EXE)
	./debug/$(APP_EXE)

run-release: release/$(APP_BUNDLE)
	open release/$(APP_BUNDLE)

# Removes build artifacts (objects, app bundle) but keeps SDL
clean:
	$(RM) *.o *.d
	$(RM) -r debug
	$(RM) -r release
	$(RM) $(LICENSE_HEADER) $(BOOTROM_HEADER)
	$(RM) $(APP_MANIFEST)

# Removes everything including SDL installation and build artifacts (full reset)
distclean: clean
	$(RM) -r ../external/sdl

.PHONY: all sdl clean distclean run-release run-debug

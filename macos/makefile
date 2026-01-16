APP_NAME = Kiwi8

# Use VERSION if set, otherwise fall back to GITHUB_REF_NAME, then default to unknown
VERSION ?= $(GITHUB_REF_NAME)
ifeq ($(VERSION),)
VERSION = unknown
endif

COMMIT_HASH ?= $(GITHUB_SHA)
ifeq ($(COMMIT_HASH),)
COMMIT_HASH = unknown
endif

APP_MANIFEST = src/Info.plist
APP_BUNDLE = $(APP_NAME).app
APP_EXE = $(APP_NAME)

CC = clang++
CFLAGS = -std=c++17 -Wall -mmacosx-version-min=11.0 -arch x86_64 -arch arm64 -MMD -MP -DAPP_NAME='"$(APP_NAME)"' -DVERSION='"$(VERSION)"' -DCOMMIT_HASH='"$(COMMIT_HASH)"'
LFLAGS = -mmacosx-version-min=11.0
INCS = -I../external/sdl/build/include/ -I../external/imgui/ -I../shared/
LIBS = -L../external/sdl/build/lib/ -lSDL2-2.0.0 -framework Cocoa -framework OpenGL

# SDL dependency files
SDL_LIB = ../external/sdl/build/lib/libSDL2-2.0.0.dylib

# Generated headers
LICENSE_HEADER = ../shared/license.h
BOOTROM_HEADER = ../shared/bootrom.h
BOOTROM_SOURCE = ../roms/Kiwi8_logo_2.ch8

# Source files
CORE_SRCS = ../shared/Audio.cc ../shared/Chip8.cc ../shared/Display.cc ../shared/Gui.cc \
            ../shared/imgui_impl_sdl.cc ../shared/Input.cc ../shared/main.cc \
            ../shared/opcodes.cc ../shared/open_file_dialog.cc
IMGUI_SRCS = ../external/imgui/imgui.cpp ../external/imgui/imgui_demo.cpp \
             ../external/imgui/imgui_draw.cpp
MACOS_SRCS = src/file_dialog.mm

# Object files (convert source paths to .o in current directory)
OBJS = $(notdir $(CORE_SRCS:.cc=.o)) $(notdir $(IMGUI_SRCS:.cpp=.o)) $(notdir $(MACOS_SRCS:.mm=.o))
DEPS = $(OBJS:.o=.d)

# Include auto-generated dependency files (must be before targets)
-include $(DEPS)

.DEFAULT_GOAL := all

all: debug/$(APP_EXE) release/$(APP_BUNDLE)

# Build SDL (force rebuild with: make sdl)
sdl:
	bash ../tools/build_sdl_macos.sh

# Build SDL if lib doesn't exist (headers come together)
$(SDL_LIB):
	bash ../tools/build_sdl_macos.sh

# Generate license header from LICENSE file
$(LICENSE_HEADER): ../LICENSE
	python3 ../tools/generate_license_header.py ../LICENSE $(LICENSE_HEADER)

# Generate bootrom header from ROM file
$(BOOTROM_HEADER): $(BOOTROM_SOURCE)
	python3 ../tools/generate_bootrom_header.py $(BOOTROM_SOURCE) $(BOOTROM_HEADER)

# Generate Info.plist from template with version substitution
$(APP_MANIFEST): $(APP_MANIFEST).in
	sed -e 's/@APP_NAME@/$(APP_NAME)/g' -e 's/@VERSION@/$(VERSION)/g' -e 's/@COMMIT_HASH@/$(COMMIT_HASH)/g' $< > $@

# Pattern rules for incremental compilation
%.o: ../shared/%.cc $(LICENSE_HEADER) $(BOOTROM_HEADER)
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.o: ../external/imgui/%.cpp
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.o: src/%.mm
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

# debug executable
debug/$(APP_EXE): $(SDL_LIB) $(OBJS)
	mkdir -p debug
	cp $(SDL_LIB) debug/libSDL2-2.0.0.dylib
	$(CC) $(CFLAGS) -g $(LIBS) $(INCS) $(OBJS) -o $@ $(LFLAGS)

	# Set the dylib's install name to be relative to the bundle
	install_name_tool -id @executable_path/libSDL2-2.0.0.dylib debug/libSDL2-2.0.0.dylib

	# Fix executable's reference to SDL
	install_name_tool -change @rpath/libSDL2-2.0.0.dylib @executable_path/libSDL2-2.0.0.dylib debug/$(APP_EXE)

# release executable (inside .app bundle)
release/$(APP_BUNDLE)/Contents/MacOS/$(APP_EXE): $(SDL_LIB) $(OBJS)
	mkdir -p release/$(APP_BUNDLE)/Contents/MacOS
	$(CC) $(CFLAGS) $(LIBS) $(INCS) $(OBJS) -o $@ $(LFLAGS)

# .app bundle
release/$(APP_BUNDLE): release/$(APP_BUNDLE)/Contents/MacOS/$(APP_EXE) $(SDL_LIB) resources/Kiwi8.icns $(APP_MANIFEST)
	mkdir -p release/$(APP_BUNDLE)/Contents/{Resources,Frameworks}

	# Copying the frameworks into .app bundle
	cp $(SDL_LIB) release/$(APP_BUNDLE)/Contents/Frameworks/libSDL2-2.0.0.dylib

	# Moving other files into .app bundle
	cp resources/Kiwi8.icns release/$(APP_BUNDLE)/Contents/Resources/Kiwi8.icns
	cp $(APP_MANIFEST) release/$(APP_BUNDLE)/Contents/Info.plist

	# Set the dylib's install name to be relative to the bundle
	install_name_tool -id @executable_path/../Frameworks/libSDL2-2.0.0.dylib release/$(APP_BUNDLE)/Contents/Frameworks/libSDL2-2.0.0.dylib

	# Fix executable's reference to SDL (from @rpath to bundle-relative path)
	install_name_tool -change @rpath/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib release/$(APP_BUNDLE)/Contents/MacOS/$(APP_EXE)

	# Update directory timestamp so Make knows bundle is up-to-date
	touch release/$(APP_BUNDLE)

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

APP_NAME = Kiwi8

# Auto-detect version: use GITHUB_REF_NAME if set (CI), else git describe, else git branch
ifeq ($(origin GITHUB_REF_NAME), environment)
VERSION := $(GITHUB_REF_NAME)
else
VERSION := $(shell git describe --tags --exact-match 2>/dev/null || git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown")
endif

APP_NAME_VERSION = $(APP_NAME) $(VERSION)
APP_BUNDLE = release/$(APP_NAME).app
APP_EXE = $(APP_BUNDLE)/Contents/MacOS/$(APP_NAME)

CC = clang++
CFLAGS = -std=c++17 -g -Wall -mmacosx-version-min=11.0 -arch x86_64 -arch arm64 -MMD -MP -DAPPNAME_VERSION='"Kiwi8 $(VERSION)"'
LFLAGS = -mmacosx-version-min=11.0
INCS = -Iframeworks/sdl/include/ -I../frameworks/imgui/
LIBS = -Lframeworks/sdl/lib/ -lSDL2-2.0.0 -framework Cocoa -framework OpenGL

# SDL dependency files
SDL_LIB = frameworks/sdl/lib/libSDL2-2.0.0.dylib

# Source files
CORE_SRCS = ../Core/Audio.cc ../Core/Chip8.cc ../Core/Display.cc ../Core/Gui.cc \
            ../Core/imgui_impl_sdl.cc ../Core/Input.cc ../Core/main.cc \
            ../Core/opcodes.cc ../Core/open_file_dialog.cc
IMGUI_SRCS = ../frameworks/imgui/imgui.cpp ../frameworks/imgui/imgui_demo.cpp \
             ../frameworks/imgui/imgui_draw.cpp
MACOS_SRCS = src/file_dialog.mm

# Object files (convert source paths to .o in current directory)
OBJS = $(notdir $(CORE_SRCS:.cc=.o)) $(notdir $(IMGUI_SRCS:.cpp=.o)) $(notdir $(MACOS_SRCS:.mm=.o))
DEPS = $(OBJS:.o=.d)

# Include auto-generated dependency files (must be before targets)
-include $(DEPS)

.DEFAULT_GOAL := all

all: $(APP_BUNDLE)

# Build SDL (force rebuild with: make sdl)
sdl:
	bash ../tools/build_sdl_macos.sh

# Build SDL if lib doesn't exist (headers come together)
$(SDL_LIB):
	bash ../tools/build_sdl_macos.sh

# Pattern rules for incremental compilation
%.o: ../Core/%.cc
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.o: ../frameworks/imgui/%.cpp
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.o: src/%.mm
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(APP_EXE): $(SDL_LIB) $(OBJS)
	# Create app bundle directory structure
	mkdir -p $(APP_BUNDLE)/Contents/{MacOS,Resources,Frameworks}

	# Compiling main executable
	$(CC) $(CFLAGS) $(LIBS) $(INCS) $(OBJS) -o $@ $(LFLAGS)

$(APP_BUNDLE): $(APP_EXE) $(SDL_LIB) resources/Kiwi8.icns src/Info.plist
	# Copying the frameworks into .app bundle
	cp $(SDL_LIB) $(APP_BUNDLE)/Contents/Frameworks/libSDL2-2.0.0.dylib

	# Set the dylib's install name to be relative to the bundle
	install_name_tool -id @executable_path/../Frameworks/libSDL2-2.0.0.dylib $(APP_BUNDLE)/Contents/Frameworks/libSDL2-2.0.0.dylib

	# Fix executable's reference to SDL (from @rpath to bundle-relative path)
	install_name_tool -change @rpath/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib $(APP_EXE)

	# Moving the configured executable and copying other files into .app bundle
	cp resources/Kiwi8.icns $(APP_BUNDLE)/Contents/Resources/Kiwi8.icns
	cp src/Info.plist $(APP_BUNDLE)/Contents/Info.plist

	# Ad-hoc sign the bundle (framework -> executable -> bundle)
	codesign --force --sign - $(APP_BUNDLE)/Contents/Frameworks/libSDL2-2.0.0.dylib
	codesign --force --sign - $(APP_BUNDLE)/Contents/MacOS/$(APP_NAME)
	codesign --force --sign - $(APP_BUNDLE)

	# Update directory timestamp so Make knows bundle is up-to-date
	touch $(APP_BUNDLE)

run-debug: $(APP_BUNDLE)
	./$(APP_BUNDLE)/Contents/MacOS/$(APP_NAME)

run-release: $(APP_BUNDLE)
	open ./$(APP_BUNDLE)

# Remove just SDL
clean-sdl:
	$(RM) -r frameworks/sdl/lib
	$(RM) -r frameworks/sdl/include

# Removes build artifacts (objects, app bundle) but keeps SDL
clean:
	$(RM) *.o *.d
	$(RM) -r $(APP_BUNDLE)

# Removes everything including SDL (full reset)
distclean: clean clean-sdl
	$(RM) -r frameworks/sdl/backups
	$(RM) -r ../MacOS/third_party

.PHONY: all sdl clean clean-sdl distclean run-release run-debug check-dylibs

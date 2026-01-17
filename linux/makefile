APP_NAME = Kiwi8

# Use VERSION if set, otherwise fall back to GITHUB_REF_NAME, then default to unknown
VERSION ?= $(GITHUB_REF_NAME)
ifeq ($(VERSION),)
VERSION = develop
endif

COMMIT_HASH ?= $(GITHUB_SHA)
ifeq ($(COMMIT_HASH),)
COMMIT_HASH = unknown
endif

APP_EXE = $(APP_NAME)

CC = g++
CFLAGS = -std=c++17 -Wall -MMD -MP -DAPP_NAME='"$(APP_NAME)"' -DVERSION='"$(VERSION)"' -DCOMMIT_HASH='"$(COMMIT_HASH)"'
LFLAGS = -Wl,-rpath,'$$ORIGIN'

# Use pkg-config to get GTK3 flags
GTK3_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK3_LIBS = $(shell pkg-config --libs gtk+-3.0)

CFLAGS += $(GTK3_CFLAGS)
LIBS = -L../external/sdl/build/lib -lSDL2-2.0 $(GTK3_LIBS) -lm -ldl -lpthread

INCS = -I../external/sdl/build/include/ -I../external/imgui/ -I../shared/

# SDL dependency files
SDL_LIB = ../external/sdl/build/lib/libSDL2-2.0.so.0

# Generated headers
LICENSE_HEADER = ../shared/license.h
BOOTROM_HEADER = ../shared/bootrom.h
BOOTROM_SOURCE = ../roms/Kiwi8_logo_2.ch8

# Source files
CORE_SRCS = ../shared/audio.cc ../shared/chip8.cc ../shared/display.cc ../shared/gui.cc \
            ../shared/imgui_impl_sdl.cc ../shared/input.cc ../shared/main.cc \
            ../shared/opcodes.cc ../shared/open_file_dialog.cc
IMGUI_SRCS = ../external/imgui/imgui.cpp ../external/imgui/imgui_draw.cpp
LINUX_SRCS = src/file_dialog.cc

# Object files (convert source paths to .o in current directory)
OBJS = $(notdir $(CORE_SRCS:.cc=.o)) $(notdir $(IMGUI_SRCS:.cpp=.o)) $(notdir $(LINUX_SRCS:.cc=.o))
DEPS = $(OBJS:.o=.d)

# Include auto-generated dependency files (must be before targets)
-include $(DEPS)

.DEFAULT_GOAL := all

all: debug/$(APP_EXE) release/$(APP_EXE)

# Build SDL (force rebuild with: make sdl)
sdl:
	bash ../tools/build_sdl_linux.sh

# Build SDL if lib doesn't exist
$(SDL_LIB):
	bash ../tools/build_sdl_linux.sh

# Generate license header from LICENSE file
$(LICENSE_HEADER): ../LICENSE
	python3 ../tools/generate_license_header.py ../LICENSE $(LICENSE_HEADER)

# Generate bootrom header from ROM file
$(BOOTROM_HEADER): $(BOOTROM_SOURCE)
	python3 ../tools/generate_bootrom_header.py $(BOOTROM_SOURCE) $(BOOTROM_HEADER)

# Pattern rules for incremental compilation
%.o: ../shared/%.cc $(LICENSE_HEADER) $(BOOTROM_HEADER)
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.o: ../external/imgui/%.cpp
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.o: src/%.cc
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

# debug executable
debug/$(APP_EXE): $(SDL_LIB) $(OBJS)
	mkdir -p debug
	cp $(SDL_LIB) debug/libSDL2-2.0.so.0
	$(CC) $(CFLAGS) -g $(INCS) $(LIBDIRS) $(OBJS) -o $@ $(LIBS) $(LFLAGS)

# release executable
release/$(APP_EXE): $(SDL_LIB) $(OBJS)
	mkdir -p release
	cp $(SDL_LIB) release/libSDL2-2.0.so.0
	$(CC) $(CFLAGS) $(INCS) $(LIBDIRS) $(OBJS) -o $@ $(LIBS) $(LFLAGS)

run-debug: debug/$(APP_EXE)
	./debug/$(APP_EXE)

run-release: release/$(APP_EXE)
	./release/$(APP_EXE)

# Removes build artifacts (objects, executables) but keeps SDL
clean:
	$(RM) *.o *.d
	$(RM) -r debug
	$(RM) -r release
	$(RM) $(LICENSE_HEADER) $(BOOTROM_HEADER)

# Removes everything including SDL installation and build artifacts (full reset)
distclean: clean
	$(RM) -r ../external/sdl

.PHONY: all sdl clean distclean run-release run-debug

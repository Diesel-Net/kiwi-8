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

APP_EXE = $(APP_NAME)

CC = gcc
CXX = g++
CFLAGS = -std=gnu11
CPPFLAGS = -Wall \
	-MMD \
	-MP \
	-DAPP_NAME='"$(APP_NAME)"' \
	-DVERSION='"$(VERSION)"' \
	-DSUB_VERSION='"$(SUB_VERSION)"'

CXXFLAGS = -std=gnu++11

INCLUDE_DIRS = -I../shared \
	-I../external/sdl/build/include/ \
	-I../external/imgui/ \
	-I../external/stb/ \
	-I../external/lekkit/

LDFLAGS = -L../external/sdl/build/lib \
	-lSDL2-2.0 \
	-lGL \
	-lm \
	-ldl \
	-lpthread \
	-Wl,-rpath,'$$ORIGIN'

# Use pkg-config to get GTK3 and audio backend flags
GTK3_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK3_LIBS = $(shell pkg-config --libs gtk+-3.0)

# Audio backend pkg-config flags
ALSA_CFLAGS = $(shell pkg-config --cflags alsa)
ALSA_LIBS = $(shell pkg-config --libs alsa)
PULSE_CFLAGS = $(shell pkg-config --cflags libpulse)
PULSE_LIBS = $(shell pkg-config --libs libpulse)
PIPEWIRE_CFLAGS = $(shell pkg-config --cflags libpipewire-0.3)
PIPEWIRE_LIBS = $(shell pkg-config --libs libpipewire-0.3)
JACK_CFLAGS = $(shell pkg-config --cflags jack)
JACK_LIBS = $(shell pkg-config --libs jack)

CPPFLAGS += $(GTK3_CFLAGS) $(ALSA_CFLAGS) $(PULSE_CFLAGS) $(PIPEWIRE_CFLAGS) $(JACK_CFLAGS)
LDFLAGS += $(GTK3_LIBS) $(ALSA_LIBS) $(PULSE_LIBS) $(PIPEWIRE_LIBS) $(JACK_LIBS)

SDL_LIB = ../external/sdl/build/lib/libSDL2-2.0.so.0

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

all: debug/profiles.ini debug/$(APP_EXE) release/profiles.ini release/$(APP_EXE)

# Build SDL (force rebuild with: make sdl)
sdl:
	bash build_sdl.sh

# Build SDL if lib doesn't exist
$(SDL_LIB):
	bash build_sdl.sh

# Generate license header from LICENSE file
$(LICENSE_HEADER): ../LICENSE
	python3 ../tools/generate_license_header.py ../LICENSE $(LICENSE_HEADER)

# Generate bootrom header from ROM file
$(BOOTROM_HEADER): $(BOOTROM_SOURCE)
	python3 ../tools/generate_bootrom_header.py $(BOOTROM_SOURCE) $(BOOTROM_HEADER)

# Pattern rules for incremental compilation
%.o: src/%.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: ../external/imgui/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: ../external/lekkit/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: ../shared/%.c $(LICENSE_HEADER) $(BOOTROM_HEADER)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: ../shared/%.cc $(LICENSE_HEADER) $(BOOTROM_HEADER)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@

debug/profiles.ini: $(PROFILES_INI)
	mkdir -p debug
	cp $(PROFILES_INI) debug/profiles.ini

release/profiles.ini: $(PROFILES_INI)
	mkdir -p release
	cp $(PROFILES_INI) release/profiles.ini

# debug executable
debug/$(APP_EXE): $(SDL_LIB) $(OBJS)
	mkdir -p debug
	cp $(SDL_LIB) debug/libSDL2-2.0.so.0
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -g $(INCLUDE_DIRS)  $(OBJS) -o $@ $(LDFLAGS)

# release executable
release/$(APP_EXE): $(SDL_LIB) $(OBJS)
	mkdir -p release
	cp $(SDL_LIB) release/libSDL2-2.0.so.0
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE_DIRS) $(OBJS) -o $@ $(LDFLAGS)

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

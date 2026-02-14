APP_NAME = Kiwi8

# Use VERSION if set, otherwise fall back to GITHUB_REF_NAME, then default to unknown
VERSION ?= $(GITHUB_REF_NAME)
ifeq ($(VERSION),)
VERSION = develop
endif

SUB_VERSION ?= $(GITHUB_SHA)
ifeq ($(SUB_VERSION),)
SUB_VERSION = unknown
endif

APP_EXE = $(APP_NAME)

CC = g++
CFLAGS = -std=c++17 -Wall -MMD -MP -DAPP_NAME='"$(APP_NAME)"' -DVERSION='"$(VERSION)"' -DSUB_VERSION='"$(SUB_VERSION)"'
LFLAGS = -Wl,-rpath,'$$ORIGIN'

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

CFLAGS += $(GTK3_CFLAGS) $(ALSA_CFLAGS) $(PULSE_CFLAGS) $(PIPEWIRE_CFLAGS) $(JACK_CFLAGS)

INCS = -I../external/sdl/build/include/ -I../external/imgui/ -I../external/stb/ -I../external/lekkit/
LIBS = -L../external/sdl/build/lib -lSDL2-2.0 -lGL $(GTK3_LIBS) $(ALSA_LIBS) $(PULSE_LIBS) $(PIPEWIRE_LIBS) $(JACK_LIBS) $(ESD_LIBS) $(SNDIO_LIBS) -lm -ldl -lpthread

# SDL dependency files
SDL_LIB = ../external/sdl/build/lib/libSDL2-2.0.so.0

# Generated headers
LICENSE_HEADER = ../shared/license.h
BOOTROM_HEADER = ../shared/bootrom.h
BOOTROM_SOURCE = ../roms/Kiwi8_logo_2.ch8

# Source files
CORE_SRCS = ../shared/audio.cc ../shared/chip8.cc ../shared/display.cc ../shared/gui.cc \
			../shared/input.cc ../shared/main.cc \
			../shared/toast.cc ../shared/open_file_dialog.cc ../shared/profiles.cc
IMGUI_SRCS = ../external/imgui/imgui.cpp ../external/imgui/imgui_draw.cpp ../external/imgui/imgui_impl_sdl.cpp
LINUX_SRCS = src/file_dialog.cc
SHA256_SRC = ../external/lekkit/sha256.cc

# Object files (convert source paths to .o in current directory)
OBJS = $(notdir $(CORE_SRCS:.cc=.o)) \
	$(notdir $(IMGUI_SRCS:.cpp=.o)) \
	$(notdir $(LINUX_SRCS:.cc=.o)) \
	$(notdir $(SHA256_SRC:.cc=.o))
DEPS = $(OBJS:.o=.d)

PROFILES_INI = ../shared/profiles.ini

# Include auto-generated dependency files (must be before targets)
-include $(DEPS)

.DEFAULT_GOAL := all

all: debug/profiles.ini debug/$(APP_EXE) release/profiles. release/$(APP_EXE)

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
%.o: ../shared/%.cc $(LICENSE_HEADER) $(BOOTROM_HEADER)
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.o: src/%.cc
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.o: ../external/%.cpp
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.o: ../external/%.cc
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

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

# Author: Eric Bachard
# Date: July 5th, 2017
# Purpose: Builds the application into a standalone Kiwi8 bundle for distribution

CXX = g++
CC = gcc

BUILD_DIR = release
GTK3_LIBS = $(shell pkg-config --cflags --libs gtk+-3.0)
SDL2_LIBS = $(shell sdl2-config --cflags --libs)
GLIB_LIBS= $(shell pkg-config --libs glib-2.0)

# uncomment if you don't want the traditional green style
EXTRA_FLAGS = -DBLUE_STYLE
# uncomment if you prefer the dark blue style
#EXTRA_FLAGS += -DBLUE_DARK_STYLE

OS_FLAGS = -D__Linux__ -O2
CFLAGS = -Wall ${OS_FLAGS} ${EXTRA_FLAGS}
CXXFLAGS = -Wall ${OS_FLAGS} -std=c++11 -L/usr/lib/x86_64-linux-gnu 
LIBS = -L/usr/lib/x86_64-linux-gnu -lGL -lGLU
LFLAGS = ${LIBS} ${GLIB_LIBS} ${GTK3_LIBS} ${SDL2_LIBS}
INCS = -I. -I$(CURDIR)/../frameworks/imgui/ `pkg-config --cflags gtk+-3.0` 

APPNAME = ${BUILD_DIR}/Kiwi8



# TODO : create a package ready to go

# Default target
$(APPNAME): objects objects_c
	# Removing previous build
	cd release; $(RM) -r 

	# Compiling main executable
	$(CXX) $(CXXFLAGS) $(INCS) *.o -o $@ $(LFLAGS)

	# Removing leftover objects 
	$(RM) *.o

# Builds object files
objects:	../Core/*.cc ../frameworks/imgui/imgui*.cpp
	$(CXX) $(CXXFLAGS) -c $(INCS) $(CFLAGS) $?

# Builds object files
objects_c:	src/*.c
	$(CC) $(CFLAGS) -c $(INCS) $(CFLAGS) $?

# Tests the release version
test_release:
	./release/$(APPNAME) --args \


# Removes latest build and object files
clean:
	$(RM) *.o ${APPNAME}
	cd release; $(RM) -r ${APPNAME}

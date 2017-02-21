# Author: Thomas Daley
# Date: September 8, 2016
# Purpose: Builds debug & release version of the application on windows

# Note: Microsoft Visual C++ Build Tools are assumed to be installed and added to PATH

# 1) vcvarsall x86
# 2) nmake

APPNAME = Chip8
ARCH = x86

DEBUG = /SUBSYSTEM:CONSOLE
RELEASE = /SUBSYSTEM:WINDOWS

CC = CL
CFLAGS = /EHsc
INCS = /I$(MAKEDIR)/frameworks/sdl/include/ \
	   /I$(MAKEDIR)/frameworks/sdl2_image/include/ 

LIBS = user32.lib \
	   gdi32.lib \
	   SDL2.lib \
	   SDL2main.lib \
	   SDL2_image.lib \
	   Comdlg32.lib

LFLAGS = /link \
		 /LIBPATH:$(MAKEDIR)/frameworks/sdl/lib/$(ARCH) \
		 /LIBPATH:$(MAKEDIR)/frameworks/sdl2_image/lib/$(ARCH) \
		 /ENTRY:mainCRTStartup

# Default target
all: clean build_debug build_release
	DEL *.exe *.obj *.res

build_debug: objects resource
	$(CC) $(CFLAGS) /Fe$(APPNAME) $(INCS) *.obj *.res $(LIBS) $(LFLAGS) $(DEBUG)
	MOVE $(APPNAME).exe debug\$(APPNAME).exe
	COPY frameworks\sdl\bin\$(ARCH)\SDL2.dll debug\SDL2.dll
	COPY frameworks\sdl2_image\bin\$(ARCH)\SDL2_image.dll debug\SDL2_image.dll
	COPY resources\chip8.ico debug\chip8.ico

build_release: objects resource
	$(CC) $(CFLAGS) /Fe$(APPNAME) $(INCS) *.obj *.res $(LIBS) $(LFLAGS) $(RELEASE)
	MOVE $(APPNAME).exe release\$(APPNAME).exe
	COPY frameworks\sdl\bin\$(ARCH)\SDL2.dll release\SDL2.dll
	COPY frameworks\sdl2_image\bin\$(ARCH)\SDL2_image.dll release\SDL2_image.dll
	COPY resources\chip8.ico release\chip8.ico

test_debug:
	cd debug & $(APPNAME).exe

test_release: 
	cd release & $(APPNAME).exe 


# Build object files
objects:	src/*.c* ../Core/*.c*
	$(CC) $(CFLAGS) /c $(INCS) $(CFLAGS) $?

# Build resource file
resource: src/chip8.rc
	RC src/chip8.rc
	MOVE src\chip8.res chip8.res

# Clean any leftover build files
clean:
	DEL *.exe *.obj *.res
	DEL debug\*.exe debug\*.dll
	DEL release\*.exe release\*.dll

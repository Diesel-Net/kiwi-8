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
CFLAGS = /MD /nologo /W3 /w44996
LFLAGS = /link \
		 /LIBPATH:$(MAKEDIR)/frameworks/sdl/lib/$(ARCH) \
		 /ENTRY:mainCRTStartup

INCS = /I$(MAKEDIR)/frameworks/sdl/include/ \
	   /I$(MAKEDIR)/../frameworks/imgui/

LIBS = user32.lib \
	   SDL2.lib \
	   SDL2main.lib \
	   opengl32.lib \
	   Comdlg32.lib



# Default target
all: clean build_debug build_release
	DEL *.exe *.obj *.res *.pdb

build_debug: objects resource
	$(CC) $(CFLAGS) /Zi /Fe$(APPNAME) $(INCS) *.obj *.res $(LIBS) $(LFLAGS) $(DEBUG)
	MOVE $(APPNAME).exe debug\$(APPNAME).exe
	COPY frameworks\sdl\lib\$(ARCH)\SDL2.dll debug\SDL2.dll
	COPY resources\chip8.ico debug\chip8.ico

build_release: objects resource
	$(CC) $(CFLAGS) /Fe$(APPNAME) $(INCS) *.obj *.res $(LIBS) $(LFLAGS) $(RELEASE)
	MOVE $(APPNAME).exe release\$(APPNAME).exe
	COPY frameworks\sdl\lib\$(ARCH)\SDL2.dll release\SDL2.dll
	COPY resources\chip8.ico release\chip8.ico

test_debug:
	debug\$(APPNAME).exe \
	"C:/Roms/CHIP-8/Chip-8 Games/Blitz [David Winter].ch8" \
	66 134 244

test_release: 
	release\$(APPNAME).exe \


# Build object files
objects:	src\*.c* ..\Core\*.c* ..\frameworks\imgui\imgui*.cpp
	$(CC) $(CFLAGS) /c $(INCS) $(CFLAGS) $?

# Build resource file
resource: src\chip8.rc resources\chip8.ico
	RC src\chip8.rc
	MOVE src\chip8.res chip8.res

# Clean any leftover build files
clean:
	DEL *.exe *.obj *.res *.pdb
	cd debug & DEL *.exe *.ico *.dll *.ini
	cd release & DEL *.exe *.ico *.dll *.ini 

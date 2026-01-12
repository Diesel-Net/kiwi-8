APPNAME = Kiwi8
ARCH = x64

# SDL dependency files
SDL_LIB = frameworks\sdl\lib\SDL2.lib
SDL_DLL = frameworks\sdl\bin\SDL2.dll

DEBUG = /SUBSYSTEM:CONSOLE
RELEASE = /SUBSYSTEM:WINDOWS

CC = CL
CFLAGS = /MD /nologo /W3 /w44996
LFLAGS = /link \
		 /LIBPATH:frameworks\sdl\lib \
		 /ENTRY:mainCRTStartup

INCS = /Iframeworks\sdl\include \
	   /I..\frameworks\imgui
LIBS = user32.lib \
	   SDL2.lib \
	   SDL2main.lib \
	   opengl32.lib \
	   Comdlg32.lib

# Default target
all: clean build_debug build_release
	DEL *.exe *.obj *.res *.pdb

# Build SDL if lib doesn't exist (DLL and headers come together)
$(SDL_LIB):
	powershell -ExecutionPolicy Bypass -File ..\tools\build_sdl_windows.ps1

$(SDL_DLL): $(SDL_LIB)
	@REM DLL is created by same script that creates LIB

build_debug: $(SDL_LIB) $(SDL_DLL) objects resource
	$(CC) $(CFLAGS) /Zi /Fe$(APPNAME) $(INCS) *.obj *.res $(LIBS) $(LFLAGS) $(DEBUG)
	MOVE $(APPNAME).exe debug\$(APPNAME).exe
	COPY frameworks\sdl\bin\SDL2.dll debug\SDL2.dll

build_release: $(SDL_LIB) $(SDL_DLL) objects resource
	$(CC) $(CFLAGS) /Fe$(APPNAME) $(INCS) *.obj *.res $(LIBS) $(LFLAGS) $(RELEASE)
	MOVE $(APPNAME).exe release\$(APPNAME).exe
	COPY frameworks\sdl\bin\SDL2.dll release\SDL2.dll

test_debug:
	debug\$(APPNAME).exe

test_release:
	release\$(APPNAME).exe

# Build object files
objects: src\*.c* ..\Core\*.c* ..\frameworks\imgui\imgui*.cpp
	$(CC) $(CFLAGS) /c $(INCS) $(CFLAGS) $?

# Build resource file
resource: src\$(APPNAME).rc resources\$(APPNAME).ico
	RC src\$(APPNAME).rc
	MOVE src\$(APPNAME).res $(APPNAME).res

# Clean any leftover build files (keeps SDL)
clean:
	DEL *.exe *.obj *.res *.pdb
	cd debug & DEL *.exe *.dll
	cd release & DEL *.exe *.dll

# Remove just SDL (for forcing recompile)
clean-sdl:
	RD /S /Q frameworks\sdl\lib
	RD /S /Q frameworks\sdl\include

# Remove everything including SDL (full reset)
distclean: clean clean-sdl
	RD /S /Q frameworks\sdl\backups
	RD /S /Q ..\Windows\third_party

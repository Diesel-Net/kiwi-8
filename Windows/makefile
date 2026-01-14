APP_NAME = Kiwi8

# Auto-detect version: use GITHUB_REF_NAME if set (CI), else git describe, else git branch
!IFNDEF GITHUB_REF_NAME
# Try git describe for exact tag match
!IF [git describe --tags --exact-match > .version.tmp 2>nul] == 0
!INCLUDE .version.tmp
# Try git branch name
!ELSE IF [git rev-parse --abbrev-ref HEAD > .version.tmp 2>nul] == 0
!INCLUDE .version.tmp
!ELSE
VERSION = unknown
!ENDIF
# Clean up temp file
!IF [del .version.tmp 2>nul]
!ENDIF
!ELSE
VERSION = $(GITHUB_REF_NAME)
!ENDIF

APP_NAME_VERSION = $(APP_NAME) $(VERSION)
APP_EXE = $(APP_NAME).exe
APP_PDB = $(APP_NAME).pdb
APP_RES = $(APP_NAME).res

CC = CL
CFLAGS = /MD /nologo /W3 /w44996 "/DAPPNAME_VERSION=\"$(APP_NAME_VERSION)\""
LFLAGS = /link \
		 /LIBPATH:frameworks\sdl\lib \
		 /ENTRY:mainCRTStartup

INCS = /Iframeworks\sdl\include \
	   /I..\frameworks\imgui \
	   /I..\Core
LIBS = user32.lib \
	   SDL2.lib \
	   SDL2main.lib \
	   opengl32.lib \
	   Comdlg32.lib \
	   Shell32.lib

# SDL dependency files
SDL_LIB = frameworks\sdl\lib\SDL2.lib
SDL_DLL = frameworks\sdl\bin\SDL2.dll

# Generated headers
LICENSE_HEADER = ..\Core\license.h
BOOTROM_HEADER = ..\Core\bootrom.h
BOOTROM_SOURCE = ..\Roms\Kiwi8_logo_2.ch8

# Source files
CORE_SRCS = ..\Core\Audio.cc ..\Core\Chip8.cc ..\Core\Display.cc ..\Core\Gui.cc \
            ..\Core\imgui_impl_sdl.cc ..\Core\Input.cc ..\Core\main.cc \
            ..\Core\opcodes.cc ..\Core\open_file_dialog.cc
IMGUI_SRCS = ..\frameworks\imgui\imgui.cpp ..\frameworks\imgui\imgui_demo.cpp \
             ..\frameworks\imgui\imgui_draw.cpp
WINDOWS_SRCS = src\file_dialog.cc

# Object files (all built in current directory)
OBJS = Audio.obj Chip8.obj Display.obj Gui.obj imgui_impl_sdl.obj Input.obj \
       main.obj opcodes.obj open_file_dialog.obj imgui.obj imgui_demo.obj \
       imgui_draw.obj file_dialog.obj


# Default target
all: debug\$(APP_EXE) release\$(APP_EXE)

# Build SDL (force rebuild with: nmake sdl)
sdl:
	powershell -ExecutionPolicy Bypass -File ..\tools\build_sdl_windows.ps1

# Build SDL if lib doesn't exist (DLL and headers come together)
$(SDL_LIB):
	powershell -ExecutionPolicy Bypass -File ..\tools\build_sdl_windows.ps1

$(SDL_DLL): $(SDL_LIB)
	@REM DLL is created by same script that creates LIB

# Generate license header from LICENSE file
$(LICENSE_HEADER): ..\LICENSE
	python3 ..\tools\generate_license_header.py ..\LICENSE $(LICENSE_HEADER)

# Generate bootrom header from ROM file
$(BOOTROM_HEADER): $(BOOTROM_SOURCE)
	python3 ..\tools\generate_bootrom_header.py $(BOOTROM_SOURCE) $(BOOTROM_HEADER)

# Inference rules for incremental compilation
{..\Core}.cc{}.obj:
	$(CC) $(CFLAGS) /c $(INCS) $<

{..\frameworks\imgui}.cpp{}.obj:
	$(CC) $(CFLAGS) /c $(INCS) $<

{src}.cc{}.obj:
	$(CC) $(CFLAGS) /c $(INCS) $<

# Explicit rules for files depending on generated headers
Gui.obj: ..\Core\Gui.cc $(LICENSE_HEADER)
	$(CC) $(CFLAGS) /c $(INCS) ..\Core\Gui.cc

Chip8.obj: ..\Core\Chip8.cc $(BOOTROM_HEADER)
	$(CC) $(CFLAGS) /c $(INCS) ..\Core\Chip8.cc

$(APP_RES): src\Kiwi8.rc resources\Kiwi8.ico
	RC src\Kiwi8.rc
	MOVE src\Kiwi8.res $@

debug\$(APP_EXE): $(SDL_LIB) $(SDL_DLL) $(OBJS) $(APP_RES)
	$(CC) $(CFLAGS) /Zi /Fe$(APP_NAME) $(INCS) $(OBJS) $(APP_RES) $(LIBS) $(LFLAGS) /SUBSYSTEM:CONSOLE
	IF NOT EXIST debug MKDIR debug
	COPY $(SDL_DLL) debug\SDL2.dll
	MOVE $(APP_EXE) $@

release\$(APP_EXE): $(SDL_LIB) $(SDL_DLL) $(OBJS) $(APP_RES)
	$(CC) $(CFLAGS) /Fe$(APP_NAME) $(INCS) $(OBJS) $(APP_RES) $(LIBS) $(LFLAGS) /SUBSYSTEM:WINDOWS
	IF NOT EXIST release MKDIR release
	COPY $(SDL_DLL) release\SDL2.dll
	MOVE $(APP_EXE) $@

run-debug:
	debug\$(APP_EXE)

run-release:
	release\$(APP_EXE)

# Clean any leftover build files (keeps SDL)
clean:
	-DEL /Q $(OBJS) $(APP_RES) $(APP_PDB) 2>nul
	-DEL /Q $(LICENSE_HEADER) $(BOOTROM_HEADER) 2>nul
	-IF EXIST debug RD /S /Q debug
	-IF EXIST release RD /S /Q release

# Remove everything including SDL installation and build artifacts (full reset)
distclean: clean
	-RD /S /Q frameworks\sdl
	-RD /S /Q ..\Windows\third_party

.PHONY: all sdl run-debug run-release clean distclean

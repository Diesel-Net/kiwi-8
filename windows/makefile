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

# Git commit hash (unique identifier for build)
!IF [powershell -NoProfile -Command "$$hash = git rev-parse --short HEAD 2>$$null; if ($$hash) { Set-Content -Path .hash.tmp -Value ""COMMIT_HASH = $$hash"" -NoNewline; exit 0 } else { exit 1 }"] == 0
!INCLUDE .hash.tmp
!ELSE
COMMIT_HASH = unknown
!ENDIF
# Clean up temp file
!IF [del .hash.tmp 2>nul]
!ENDIF

APP_NAME_VERSION = $(APP_NAME) $(VERSION)
APP_EXE = $(APP_NAME).exe
APP_PDB = $(APP_NAME).pdb
APP_RES = $(APP_NAME).res

CC = CL
CFLAGS = /std:c++17 /W4 /MD /nologo /w44996 "/DAPP_NAME_VERSION=\"$(APP_NAME_VERSION)\"" "/DCOMMIT_HASH=\"$(COMMIT_HASH)\""
LFLAGS = /link \
		 /LIBPATH:..\external\sdl\build\lib \
		 /ENTRY:mainCRTStartup

INCS = /I..\external\sdl\build\include \
	   /I..\external\imgui \
	   /I..\shared
LIBS = user32.lib \
	   SDL2.lib \
	   SDL2main.lib \
	   opengl32.lib \
	   Comdlg32.lib \
	   Shell32.lib

# SDL dependency files
SDL_LIB = ..\external\sdl\build\lib\SDL2.lib
SDL_DLL = ..\external\sdl\build\bin\SDL2.dll

# Generated headers
LICENSE_HEADER = ..\shared\license.h
BOOTROM_HEADER = ..\shared\bootrom.h
BOOTROM_SOURCE = ..\roms\Kiwi8_logo_2.ch8

# Source files
CORE_SRCS = ..\shared\Audio.cc ..\shared\Chip8.cc ..\shared\Display.cc ..\shared\Gui.cc \
            ..\shared\imgui_impl_sdl.cc ..\shared\Input.cc ..\shared\main.cc \
            ..\shared\opcodes.cc ..\shared\open_file_dialog.cc
IMGUI_SRCS = ..\external\imgui\imgui.cpp ..\external\imgui\imgui_demo.cpp \
             ..\external\imgui\imgui_draw.cpp
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
{..\shared}.cc{}.obj:
	$(CC) $(CFLAGS) /c $(INCS) $<

{..\external\imgui}.cpp{}.obj:
	$(CC) $(CFLAGS) /c $(INCS) $<

{src}.cc{}.obj:
	$(CC) $(CFLAGS) /c $(INCS) $<

# Explicit rules for files depending on generated headers
Gui.obj: ..\shared\Gui.cc $(LICENSE_HEADER)
	$(CC) $(CFLAGS) /c $(INCS) ..\shared\Gui.cc

Chip8.obj: ..\shared\Chip8.cc $(BOOTROM_HEADER)
	$(CC) $(CFLAGS) /c $(INCS) ..\shared\Chip8.cc

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
	-RD /S /Q ..\external\sdl

.PHONY: all sdl run-debug run-release clean distclean

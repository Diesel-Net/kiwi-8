APP_NAME = Kiwi8

# Use VERSION if set, otherwise fall back to GITHUB_REF_NAME, then default to develop
!IFNDEF VERSION
!IFDEF GITHUB_REF_NAME
VERSION = $(GITHUB_REF_NAME)
!ELSE
VERSION = develop
!ENDIF
!ENDIF

# Use SUB_VERSION if set, otherwise fall back to GITHUB_SHA, then default to unknown
!IFNDEF SUB_VERSION
!IFDEF GITHUB_SHA
SUB_VERSION = $(GITHUB_SHA)
!ELSE
SUB_VERSION = unknown
!ENDIF
!ENDIF

# Detect architecture from environment (set by MSVC vcvarsall.bat)
# VSCMD_ARG_TGT_ARCH is set to x64, x86, arm64, or arm
!IFNDEF SDL_ARCH
!IFDEF VSCMD_ARG_TGT_ARCH
SDL_ARCH = $(VSCMD_ARG_TGT_ARCH)
!ELSE
SDL_ARCH = x64
!ENDIF
!ENDIF

APP_EXE = $(APP_NAME).exe
APP_PDB = $(APP_NAME).pdb
APP_RES = $(APP_NAME).res

CC = CL
CXX = CL
CFLAGS = /std:c11
CPPFLAGS = /W4 \
	/MD \
	/nologo \
	/w44996 \
	"/DAPP_NAME=\"$(APP_NAME)\"" \
	"/DVERSION=\"$(VERSION)\"" \
	"/DSUB_VERSION=\"$(SUB_VERSION)\""

CXXFLAGS = /std:c++11
INCLUDE_DIRS = /I..\shared \
	/I..\external\sdl\build\include \
	/I..\external\imgui \
	/I..\external\stb \
	/I..\external\lekkit

LDFLAGS = /link \
	/LIBPATH:..\external\sdl\build\lib \
	/ENTRY:mainCRTStartup \
	user32.lib \
	SDL2.lib \
	SDL2main.lib \
	opengl32.lib \
	Comdlg32.lib \
	Shell32.lib

# SDL2.dll is built in the same step as the static libs
SDL_LIB = ..\external\sdl\build\lib\SDL2.lib

# Generated headers
LICENSE_HEADER = ..\shared\license.h
BOOTROM_HEADER = ..\shared\bootrom.h
BOOTROM_SOURCE = ..\roms\Kiwi8_logo_2.ch8

OBJS = audio.obj \
	chip8.obj \
	display.obj \
	gui.obj \
	input.obj \
	main.obj \
	toast.obj \
	profiles.obj \
	open_file_dialog.obj \
	imgui_impl_sdl.obj \
	imgui.obj \
	imgui_draw.obj \
	sha256.obj

PROFILES_INI = ..\shared\profiles.ini

# Default target
all: debug\profiles.ini debug\$(APP_EXE) release\profiles.ini release\$(APP_EXE)


# Build SDL (force rebuild with: nmake sdl)
sdl:
	powershell -ExecutionPolicy Bypass -File build_sdl.ps1 -ARCH $(SDL_ARCH)

# Build SDL if lib doesn't exist (DLL and headers come together)
$(SDL_LIB):
	powershell -ExecutionPolicy Bypass -File build_sdl.ps1 -ARCH $(SDL_ARCH)

# Generate license header from LICENSE file
$(LICENSE_HEADER): ..\LICENSE
	python3 ..\tools\generate_license_header.py ..\LICENSE $(LICENSE_HEADER)

# Generate bootrom header from ROM file
$(BOOTROM_HEADER): $(BOOTROM_SOURCE)
	python3 ..\tools\generate_bootrom_header.py $(BOOTROM_SOURCE) $(BOOTROM_HEADER)

# Inference rules for incremental compilation
{src}.cc{}.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) /c $(INCLUDE_DIRS) $<

{..\external\imgui}.cpp{}.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) /c $(INCLUDE_DIRS) $<

{..\external\lekkit}.c{}.obj:
	$(CC) $(CFLAGS) $(CPPFLAGS) /c $(INCLUDE_DIRS) $<

{..\shared}.c{}.obj:
	$(CC) $(CFLAGS) $(CPPFLAGS) /c $(INCLUDE_DIRS) $<

{..\shared}.cc{}.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) /c $(INCLUDE_DIRS) $<

# Explicit rules for files depending on generated headers
gui.obj: ..\shared\gui.cc $(LICENSE_HEADER)
	$(CC) $(CFLAGS) $(CPPFLAGS) /c $(INCLUDE_DIRS) ..\shared\gui.cc

chip8.obj: ..\shared\chip8.c $(BOOTROM_HEADER)
	$(CC) $(CFLAGS) $(CPPFLAGS) /c $(INCLUDE_DIRS) ..\shared\chip8.c

$(APP_RES): src\Kiwi8.rc resources\Kiwi8.ico
	RC src\Kiwi8.rc
	MOVE src\Kiwi8.res $@

debug\profiles.ini: $(PROFILES_INI)
	IF NOT EXIST debug MKDIR debug
	COPY $(PROFILES_INI) debug\profiles.ini

release\profiles.ini: $(PROFILES_INI)
	IF NOT EXIST release MKDIR release
	COPY $(PROFILES_INI) release\profiles.ini

debug\$(APP_EXE): $(SDL_LIB) $(OBJS) $(APP_RES)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) /Zi /Fe$(APP_NAME) $(INCLUDE_DIRS) $(OBJS) $(APP_RES) $(LDFLAGS) /SUBSYSTEM:CONSOLE
	IF NOT EXIST debug MKDIR debug
	COPY ..\external\sdl\build\bin\SDL2.dll debug\SDL2.dll
	MOVE $(APP_EXE) $@

release\$(APP_EXE): $(SDL_LIB) $(OBJS) $(APP_RES)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) /Fe$(APP_NAME) $(INCLUDE_DIRS) $(OBJS) $(APP_RES) $(LDFLAGS) /SUBSYSTEM:WINDOWS
	IF NOT EXIST release MKDIR release
	COPY ..\external\sdl\build\bin\SDL2.dll release\SDL2.dll
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

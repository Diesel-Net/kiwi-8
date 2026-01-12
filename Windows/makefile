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
	   Comdlg32.lib \
	   Shell32.lib

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

RESOURCE = Kiwi8.res

APP_NAME = Kiwi8
APP_EXE = Kiwi8.exe

APP_DEBUG = debug\$(APP_NAME).exe
APP_RELEASE = release\$(APP_NAME).exe

# Default target
all: $(APP_DEBUG) $(APP_RELEASE)

# Build SDL if lib doesn't exist (DLL and headers come together)
$(SDL_LIB):
	powershell -ExecutionPolicy Bypass -File ..\tools\build_sdl_windows.ps1

$(SDL_DLL): $(SDL_LIB)
	@REM DLL is created by same script that creates LIB

# Inference rules for incremental compilation
{..\Core}.cc{}.obj:
	$(CC) $(CFLAGS) /c $(INCS) $<

{..\frameworks\imgui}.cpp{}.obj:
	$(CC) $(CFLAGS) /c $(INCS) $<

{src}.cc{}.obj:
	$(CC) $(CFLAGS) /c $(INCS) $<

$(RESOURCE): src\Kiwi8.rc resources\Kiwi8.ico
	RC src\Kiwi8.rc
	MOVE src\Kiwi8.res $@

$(APP_DEBUG): $(SDL_LIB) $(SDL_DLL) $(OBJS) $(RESOURCE)
	$(CC) $(CFLAGS) /Zi /Fe$(APP_NAME) $(INCS) *.obj *.res $(LIBS) $(LFLAGS) $(DEBUG)
	IF NOT EXIST debug MKDIR debug
	COPY $(SDL_DLL) debug\SDL2.dll
	MOVE $(APP_NAME).exe $@

$(APP_RELEASE): $(SDL_LIB) $(SDL_DLL) $(OBJS) $(RESOURCE)
	$(CC) $(CFLAGS) /Fe$(APP_NAME) $(INCS) *.obj *.res $(LIBS) $(LFLAGS) $(RELEASE)
	IF NOT EXIST release MKDIR release
	COPY $(SDL_DLL) release\SDL2.dll
	MOVE $(APP_NAME).exe $@

test-debug:
	debug\$(APP_NAME).exe

test-release:
	release\$(APP_NAME).exe

# Clean any leftover build files (keeps SDL)
clean:
	-DEL /Q $(APP_DEBUG) $(APP_RELEASE) *.obj *.res *.pdb 2>nul
	-IF EXIST debug RD /S /Q debug
	-IF EXIST release RD /S /Q release

# Remove just SDL (for forcing recompile)
clean-sdl:
	-RD /S /Q frameworks\sdl\lib
	-RD /S /Q frameworks\sdl\include

# Remove everything including SDL (full reset)
distclean: clean clean-sdl
	-RD /S /Q frameworks\sdl\backups
	-RD /S /Q ..\Windows\third_party

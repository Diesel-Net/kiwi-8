CC = clang++
CFLAGS = -std=c++17 -Wall -mmacosx-version-min=11.0 -arch x86_64 -arch arm64 -MMD -MP
LFLAGS = -mmacosx-version-min=11.0
INCS = -I$(CURDIR)/frameworks/sdl/include/ \
	   -I$(CURDIR)/../frameworks/imgui/
LIBS = -L$(CURDIR)/frameworks/sdl/lib/ \
		-lSDL2-2.0.0 \
		-framework Cocoa \
		-framework OpenGL

# SDL dependency files
SDL_LIB = frameworks/sdl/lib/libSDL2-2.0.0.dylib
SDL_HEADERS = frameworks/sdl/include/SDL2/SDL.h

APP_NAME = Kiwi8
APP_BUNDLE = release/$(APP_NAME).app
APP_EXECUTABLE = $(APP_BUNDLE)/Contents/MacOS/$(APP_NAME)

# Source files
CORE_SRCS = ../Core/Audio.cc ../Core/Chip8.cc ../Core/Display.cc ../Core/Gui.cc \
            ../Core/imgui_impl_sdl.cc ../Core/Input.cc ../Core/main.cc \
            ../Core/opcodes.cc ../Core/open_file_dialog.cc
IMGUI_SRCS = ../frameworks/imgui/imgui.cpp ../frameworks/imgui/imgui_demo.cpp \
             ../frameworks/imgui/imgui_draw.cpp
MACOS_SRCS = src/file_dialog.mm

# Object files (convert source paths to .o in current directory)
OBJS = $(notdir $(CORE_SRCS:.cc=.o)) $(notdir $(IMGUI_SRCS:.cpp=.o)) $(notdir $(MACOS_SRCS:.mm=.o))
DEPS = $(OBJS:.o=.d)

$(APP_BUNDLE): $(APP_EXECUTABLE) $(SDL_LIB)
	# Copying the frameworks into .app bundle
	cp frameworks/sdl/lib/libSDL2-2.0.0.dylib $(APP_BUNDLE)/Contents/Frameworks/libSDL2-2.0.0.dylib

	# Set the dylib's install name to be relative to the bundle
	install_name_tool -id @executable_path/../Frameworks/libSDL2-2.0.0.dylib $(APP_BUNDLE)/Contents/Frameworks/libSDL2-2.0.0.dylib

	# Moving the configured executable and copying other files into .app bundle
	cp resources/Kiwi8.icns $(APP_BUNDLE)/Contents/Resources/Kiwi8.icns
	cp src/Info.plist $(APP_BUNDLE)/Contents/Info.plist

	# modify timestamp for make to handle rebuilding only when deps change
	touch $(APP_BUNDLE)

$(APP_EXECUTABLE): $(SDL_LIB) $(SDL_HEADERS) $(OBJS)
	# Creating empty .app bundle
	mkdir -p $(APP_BUNDLE)
	cd $(APP_BUNDLE); mkdir -p Contents; cd Contents; mkdir -p MacOS; mkdir -p Resources; mkdir -p Frameworks

	# Compiling main executable
	$(CC) $(CFLAGS) $(LIBS) $(INCS) $(OBJS) -o $@ $(LFLAGS)

	# Re-linking the dylibs to newly compiled executable (handle previous absolute path and @rpath)
	install_name_tool -change /usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib $@
	install_name_tool -change @rpath/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib $@

$(SDL_LIB) $(SDL_HEADERS):
	bash ../tools/build_sdl_macos.sh

# Pattern rules for different source file types
%.o: ../Core/%.cc
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.o: ../frameworks/imgui/%.cpp
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.o: src/%.mm
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

test-release: $(APP_BUNDLE)
	open ./$(APP_BUNDLE)

test-debug: $(APP_BUNDLE)
	./$(APP_BUNDLE)/Contents/MacOS/$(APPNAME)


check-dylibs:
	otool -L $(APP_EXECUTABLE) | sed -n '1,120p'
	otool -L $(APP_BUNDLE)/Contents/Frameworks/libSDL2-2.0.0.dylib | sed -n '1,120p'

# Removes just SDL (for forcing recompile)
clean-sdl:
	$(RM) -r frameworks/sdl/lib
	$(RM) -r frameworks/sdl/include

# Removes build artifacts (objects, app bundle) but keeps SDL
clean:
	$(RM) *.o *.d
	$(RM) -r $(APP_BUNDLE)

# Removes everything including SDL (full reset)
distclean: clean clean-sdl
	$(RM) -r frameworks/sdl/backups
	$(RM) -r ../MacOS/third_party

.PHONY: clean clean-sdl distclean test-release test-debug check-dylibs

# Include auto-generated dependency files
-include $(DEPS)

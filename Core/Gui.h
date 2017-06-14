#ifndef GUI_H
#define GUI_H

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include <SDL2/SDL.h>

#define MENU_HEIGHT 19

/*typedef struct GuiData {
	bool *emulation_paused;
	bool *load_store_quirk;
	bool *shift_quirk;
	bool *vwrap;
} GuiData; */

/* Forward Declaraton */
class Display;

class Gui {

	private:

		Display *display;

		bool *emulation_paused;
        bool *load_store_quirk;
		bool *shift_quirk;
		bool *vwrap;

	public:

		bool soft_reset_flag;
		bool load_rom_flag;
		bool quit_flag;
		bool show_menu_flag;
		
		

		Gui();
		~Gui();

		/* Wrapper functions to SDL/ImGui interface */
		void Initialize(Display *display, 
						bool *emulation_paused, 
                        bool *load_store_quirk, 
                        bool *shift_quirk, 
                        bool *vwrap);

		void ProcessEvents(SDL_Event* event);
		void NewFrame(SDL_Window *window);
		void ProcessMenu();
		void Render();

		
};

#endif

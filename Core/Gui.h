#ifndef GUI_H
#define GUI_H

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include <SDL2/SDL.h>

#define MENU_HEIGHT 38

/* Forward Declaraton */
class Display;

class Gui {

	private:

		Display *display;

		bool *emulation_paused;
        bool *load_store_quirk;
		bool *shift_quirk;
		bool *vwrap;

		void MainMenu();

	public:

		bool soft_reset_flag;
		bool load_rom_flag;
		bool quit_flag;
		bool show_menu_flag;
		

		Gui();
		~Gui();

		void Initialize(Display *display, 
						bool *emulation_paused, 
                        bool *load_store_quirk, 
                        bool *shift_quirk, 
                        bool *vwrap);

		void ProcessEvents(SDL_Event* event);
		void NewFrame();
		void Render();

		
};

#endif

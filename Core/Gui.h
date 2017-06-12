#ifndef GUI_H
#define GUI_H

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include <SDL2/SDL.h>

#define MENU_HEIGHT 19

class Gui {

	private:

        float *background_color;
        float *foreground_color;

	public:

		bool load_rom_flag = 0;
		bool quit_flag = 0;
		bool show_menu_flag = 1;
		bool load_store_flag = 0;
		bool shift_flag = 0;
		bool vwrap_flag = 0;
		

		Gui();
		~Gui();

		/* Wrapper functions to SDL/ImGui interface */
		void Initialize(SDL_Window *window, float *background_color, float *foreground_color);
		void ProcessEvents(SDL_Event* event);
		void NewFrame(SDL_Window *window);
		void ProcessMenu();
		void Render();

		
};

#endif

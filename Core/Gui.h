#ifndef GUI_H
#define GUI_H

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include <SDL2/SDL.h>

#define MENU_HEIGHT 38

/* Forward declaration */
class Display;

class Gui {

    private:

        Display *display;

        /* Pointers to chip8 data */
        int *steps;
        bool *emulation_paused;
        bool *load_store_quirk;
        bool *shift_quirk;
        bool *vwrap;

        /* Help-window toggles */
        bool show_controls;
        bool show_license;
        bool show_about;

        void MainMenu();
        void HelpWindows();

    public:

        bool soft_reset_flag;
        bool load_rom_flag;
        bool quit_flag;
        bool show_menu_flag;
        

        Gui();
        ~Gui();

        void Initialize(Display *display,
                        int *steps, 
                        bool *emulation_paused, 
                        bool *load_store_quirk, 
                        bool *shift_quirk, 
                        bool *vwrap);

        void ProcessEvents(SDL_Event* event);
        void NewFrame();
        void Render();

        
};

#endif

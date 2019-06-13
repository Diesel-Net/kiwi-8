#ifndef GUI_H
#define GUI_H

#if defined( __Linux__ )
#include <GL/gl.h>
#endif
#include <imgui.h>
#include "imgui_impl_sdl.h"
#include <SDL2/SDL.h>

#define MENU_HEIGHT 38

/* forward declaration */
class Display;

class Gui {

    private:

        /* pointers to chip-8 data */
        Display *display;

        int *cycles;
        bool *paused;
        bool *load_store_quirk;
        bool *shift_quirk;
        bool *vwrap;
        bool *mute;

        /* help-window toggles */
        bool show_controls;
        bool show_license;
        bool show_about;
        bool show_usage;

        void MainMenu();
        void HelpWindows();

    public:

        bool soft_reset_flag;
        bool load_rom_flag;
        bool quit_flag;
        bool show_menu_flag;
        bool show_fps_flag;

        Gui();
        ~Gui();

        void Initialize(
            Display *display,
            int *cycles, 
            bool *paused, 
            bool *load_store_quirk, 
            bool *shift_quirk, 
            bool *vwrap,
            bool *mute
        );

        void ProcessEvents(SDL_Event *event);
        void NewFrame();
        void Render();

        
};

#endif

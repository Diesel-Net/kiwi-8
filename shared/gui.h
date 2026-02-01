#ifndef GUI_H
#define GUI_H

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include <SDL2/SDL.h>

#define MENU_HEIGHT 38

/* forward declaration */
struct display;

struct gui {

    /* help-window toggles */
    bool show_controls;
    bool show_license;
    bool show_about;
    bool show_usage;

    bool soft_reset_flag;
    bool load_rom_flag;
    bool quit_flag;
    bool show_menu_flag;
    bool show_fps_flag;
};

/* Global gui instance */
extern struct gui gui;

/* Gui functions */
void gui_cleanup(void);
void gui_init(void);
void gui_process_events(SDL_Event *event);
void gui_new_frame(void);
void gui_render(void);

#endif

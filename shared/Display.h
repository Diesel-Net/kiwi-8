#ifndef DISPLAY_H
#define DISPLAY_H

#include "Gui.h"
#include <SDL2/SDL.h>

#define WIDTH 64
#define HEIGHT 32
#define SCALE 12 /* 64x32 Scaled by X amount */

/* default render colors */
#define DEFAULT_BACKGROUND_R 25
#define DEFAULT_BACKGROUND_G 20
#define DEFAULT_BACKGROUND_B 40

#define DEFAULT_FOREGROUND_R 200
#define DEFAULT_FOREGROUND_G 200
#define DEFAULT_FOREGROUND_B 255

struct display {
    SDL_GLContext glcontext;

    /* texture of 32x64x3 bytes (R, G, B) */
    unsigned char texture[HEIGHT][WIDTH][3];

    /* most recent copy of the chip8 vram */
    unsigned char **back_buffer;

    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;

    bool fullscreen_flag;
    bool vsync_flag;
    bool limit_fps_flag;
    bool lost_window_focus;

    /* RGB colors for foreground/background */
    float background_color[3];
    float foreground_color[3];

    SDL_Window *window;
};

/* Global display instance */
extern struct display display;

/* Display functions */
void display_create(void);
void display_destroy(void);
int display_initialize(
    bool fullscreen,
    int *cycles,
    bool *paused,
    bool *load_store_quirk,
    bool *shift_quirk,
    bool *vwrap,
    bool *muted
);
void display_resize(int x, int y);
void display_toggle_fullscreen(void);
void display_toggle_vsync(void);
void display_raise_window(void);
void display_render_frame(unsigned char **frame);

#endif

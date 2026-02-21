#ifndef DISPLAY_H
#define DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>

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

extern struct display display;

void display_destroy(void);
int display_init(bool fullscreen);
void display_resize(int x, int y);
void display_toggle_fullscreen(void);
void display_toggle_vsync(void);
void display_raise_window(void);
void display_render_frame(unsigned char **frame);

#ifdef __cplusplus
}
#endif

#endif

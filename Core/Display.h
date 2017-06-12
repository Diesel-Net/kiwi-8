#ifndef DISPLAY_H
#define DISPLAY_H

#include "Gui.h"
#include <SDL2/SDL.h>

#define WIDTH 64
#define HEIGHT 32
#define SCALE 12 /* 64x32 Scaled by X amount */

class Display {

    private:

        //SDL_Renderer *renderer;
        SDL_GLContext glcontext;

        /* Texture of 32x64x3 bytes (R, G, B) */
        unsigned char texture[HEIGHT][WIDTH][3];

        /* Most recent copy of the chip8 vram */
        unsigned char **back_buffer;
        
        int WINDOW_WIDTH;
        int WINDOW_HEIGHT;

        /* RGB colors for foreground/background */
        float background_color[3];
        float foreground_color[3];

    public:

        /* Exposed publicly for icon fix (see Windows/src/main.cc) */
        SDL_Window *window;

        Gui gui;

        Display();
        ~Display();

        int Initialize( unsigned int fullscreen, 
                        unsigned char R, 
                        unsigned char B, 
                        unsigned char G);

        void Resize(int x, int y);
        void ToggleFullscreen();
        void RenderFrame(unsigned char **frame);
};

#endif

#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

#define WIDTH 64
#define HEIGHT 32
#define SCALE 12 /* 64x32 Scaled by X amount */

class Display {

	private:

		SDL_Renderer *renderer;

		/* A pointer to the chip8 vram */
		unsigned char **vram_ptr;
		
		float SCALE_W;
		float SCALE_H;
		
    	int WINDOW_WIDTH;
    	int WINDOW_HEIGHT;

    	/* RGB color for rendering foreground */
    	int R, G, B;

	public:

		/* Exposed publicly for windows icon fix (see main.cc) */
		SDL_Window *window;

		Display();
		~Display();

		void Initialize(unsigned char **vram_ptr, int fullscreen, int R, int B, int G);
		void Resize(int x, int y);
		void Refresh();
		void ToggleFullscreen();
		void RenderFrame();
};

#endif

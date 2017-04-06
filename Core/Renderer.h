#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>

#define WIDTH 64
#define HEIGHT 32
#define SCALE 12 /* 64x32 Scaled by X amount */

class Renderer {

	private:

		SDL_Renderer *renderer;

		/* A pointer to the chip8 vram */
		unsigned char **vram_ptr;
		
		float SCALE_W = SCALE;
		float SCALE_H = SCALE;
		
    	int WINDOW_WIDTH = WIDTH * (int)SCALE_W;
    	int WINDOW_HEIGHT = HEIGHT * (int)SCALE_H;

    	/* flag set to 1 if aspect ratio should be maintained in the rendering */
    	unsigned int keep_aspect_ratio = 0;

    	/* RGB color for rendering foreground */
    	int R, G, B;

	public:

		SDL_Window *window;

		Renderer();
		~Renderer();

		void Initialize(unsigned char **vram_ptr, int fullscreen, int R, int B, int G);
		void Resize(int x, int y);
		void Refresh();
		void ToggleFullscreen();
		void RenderFrame();
};

#endif

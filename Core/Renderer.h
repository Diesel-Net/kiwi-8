#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>

#define WIDTH 64
#define HEIGHT 32

class Renderer {

	private:

		SDL_Renderer *renderer;

		/* A pointer to the chip8 vram */
		unsigned char **vram_ptr;

		/* 64x32 Scaled by X amount */
		int SCALE = 12;
		
		int RENDER_WIDTH = WIDTH * SCALE;
		int RENDER_HEIGHT = HEIGHT * SCALE;

		int RENDER_OFFSET_W = 0;
    	int RENDER_OFFSET_H = 0;

    	int WINDOW_WIDTH = RENDER_WIDTH;
    	int WINDOW_HEIGHT = RENDER_HEIGHT;

    	int R, G, B;

    	/* For fps measuring */
    	int fps;

    	void UpdateFPS();

	public:

		SDL_Window *window;

		Renderer();
		~Renderer();

		void Initialize(unsigned char **vram_ptr, int fullscreen, int R, int B, int G);
		void UpdateRenderSpace();
		void ToggleFullscreen();
		void RenderFrame();
		int GetFPS();
		void SetFPS(int fps);
};

#endif

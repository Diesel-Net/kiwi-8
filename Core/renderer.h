#include <SDL2/SDL.h>

#ifndef RENDERER
#define RENDERER

#define WIDTH 64
#define HEIGHT 32

class Renderer {

	private:

		SDL_Renderer *renderer;

		/* 64x32 Scaled by X amount */
		int SCALE = 12;
		
		int RENDER_WIDTH = WIDTH * SCALE;
		int RENDER_HEIGHT = HEIGHT * SCALE;

		int RENDER_OFFSET_W = 0;
    	int RENDER_OFFSET_H = 0;

    	int WINDOW_WIDTH = RENDER_WIDTH;
    	int WINDOW_HEIGHT = RENDER_HEIGHT;

    	int R, G, B;

		
		

	public:
		SDL_Window *window;

		/* Constructor */
		Renderer();
		~Renderer();

		/* functions */
		int Initialize(int fullscreen, int R, int B, int G);
		//SDL_window GetWindow();
		int UpdateRenderSpace();
		int SetFullscreen();
		int SetWindowed();
		int RenderFrame(unsigned char **pixels);
		void Quit();
};

#endif
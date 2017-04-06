#include "Renderer.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

#define MIN( a, b ) ( ( a < b) ? a : b )

Renderer::Renderer(){
    frames = 0;
}

Renderer::~Renderer(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
    

void Renderer::Initialize(unsigned char **vram_ptr, int fullscreen, int R, int G, int B){
    int window_mode = SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS;

    /* No need to copy the vram, simply assign a pointer to it */
    this->vram_ptr = vram_ptr;
    
   	window = SDL_CreateWindow("Chip8", 
   							  SDL_WINDOWPOS_CENTERED, 
   							  SDL_WINDOWPOS_CENTERED, 
   							  WINDOW_WIDTH, 
   							  WINDOW_HEIGHT, 
   							  window_mode);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    /*
    SDL_Rect viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.w = WIDTH;
    viewport.h = HEIGHT;
    SDL_RenderSetViewport(renderer, &viewport); */

    /* Set to fullscreen mode if flag present */
    if (fullscreen) { 
        ToggleFullscreen();
    }
   
    this->R = R;
    this->G = G;
    this->B = B;

    Resize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void Renderer::Resize(int x, int y) {
	/* Get the current window size */	
	//SDL_GetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
    if(x > 0) {
        WINDOW_WIDTH = x;
    }

    if(y > 0) {
        WINDOW_HEIGHT = y;
    }

    /* The two lines below are needed When opengl is under the hood (e.g. needed for MacOS not Windows).
       On Windows this is not needed because SDL_Renderer will use Direct3D under the hood */
    if(RENDERER_OPENGL) {
        SDL_DestroyRenderer(renderer);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    }
	
	float ratio_w = (float)WINDOW_WIDTH / WIDTH;
	float ratio_h = (float)WINDOW_HEIGHT / HEIGHT;

	SCALE = MIN(ratio_w, ratio_h);

    SDL_RenderSetScale(renderer, ratio_w, ratio_h);

    /*
	RENDER_OFFSET_W = (WINDOW_WIDTH - RENDER_WIDTH);
	RENDER_OFFSET_H = (WINDOW_HEIGHT - RENDER_HEIGHT);
	if (RENDER_OFFSET_W > 0) {
		RENDER_OFFSET_W /= 2;
	}
	if (RENDER_OFFSET_H > 0) {
		RENDER_OFFSET_H /= 2;
	} */

	//fprintf(stderr, "%d(%d) X %d(%d)\n", WINDOW_WIDTH, RENDER_WIDTH, WINDOW_HEIGHT, RENDER_HEIGHT);
    RenderFrame();
    fprintf(stderr, "Resize()\n");
}

void Renderer::ToggleFullscreen() {

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP) {

        /* Set Windowed */
        SDL_SetWindowFullscreen(window, 0);
        SDL_ShowCursor(SDL_ENABLE);

    } else {
        
        /* Set Fullscreen */
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_ShowCursor(SDL_DISABLE);
    }
}

void Renderer::RenderFrame(){

    /* Clear the screen */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect rectangle; 
    rectangle.x = 0;
    rectangle.y = 0;
    //rectangle.w = SCALE;
    //rectangle.h = SCALE;
    rectangle.w = 1;
    rectangle.h = 1;

    /* Set the foreground color */
    SDL_SetRenderDrawColor(renderer, R, G, B, 0);

    for (int i = 0; i < WIDTH; i++){
    	for (int j = 0; j < HEIGHT; j++){

    		//rectangle.x = (i * SCALE) + RENDER_OFFSET_W;
	        //rectangle.y = (j * SCALE) + RENDER_OFFSET_H;
            rectangle.x = i;
            rectangle.y = j;

	  		if (vram_ptr[i][j]) {

                /* Fill the pixel */
		        SDL_RenderFillRect(renderer, &rectangle);
	        } 
    	}
    }

    /* Draw anything rendered since last call */
    SDL_RenderPresent(renderer);
    frames++;
}

unsigned int Renderer::FPS(unsigned int elapsed) {
    /* elapsed - time in miliseconds between successive calls to this function */
    int fps = (1000 / elapsed) * frames;
    frames = 0;
    return fps;
}

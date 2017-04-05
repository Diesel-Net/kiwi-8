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
    int window_mode = SDL_WINDOW_RESIZABLE;

    /* No need to copy the vram, simply assign a pointer to it */
    this->vram_ptr = vram_ptr;
    
   	window = SDL_CreateWindow("Chip8", 
   							  SDL_WINDOWPOS_CENTERED, 
   							  SDL_WINDOWPOS_CENTERED, 
   							  WINDOW_WIDTH, 
   							  WINDOW_HEIGHT, 
   							  window_mode);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    /* Set to fullscreen mode if flag present */
    if (fullscreen) { 
        ToggleFullscreen();
    }
   
    this->R = R;
    this->G = G;
    this->B = B;
}

void Renderer::UpdateRenderSpace() {

	/* Get the current window size */	
	SDL_GetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
	
	int ratio_w = (WINDOW_WIDTH / WIDTH);
	int ratio_h = (WINDOW_HEIGHT / HEIGHT);

	SCALE = MIN(ratio_w, ratio_h);
    
	RENDER_WIDTH =  WIDTH * SCALE;
	RENDER_HEIGHT = HEIGHT * SCALE;

	RENDER_OFFSET_W = (WINDOW_WIDTH - RENDER_WIDTH);
	RENDER_OFFSET_H = (WINDOW_HEIGHT - RENDER_HEIGHT);
	if (RENDER_OFFSET_W > 0) {
		RENDER_OFFSET_W /= 2;
	}
	if (RENDER_OFFSET_H > 0) {
		RENDER_OFFSET_H /= 2;
	}

	fprintf(stderr, "%d(%d) X %d(%d)\n", WINDOW_WIDTH, RENDER_WIDTH, WINDOW_HEIGHT, RENDER_HEIGHT);
    RenderFrame();
}

void Renderer::ToggleFullscreen() {

    if (SDL_GetWindowFlags(window) & FULLSCREEN) {

        /* Set Windowed */
        SDL_SetWindowFullscreen(window, 0);
        SDL_ShowCursor(SDL_ENABLE);

    } else {
        
        /* Set Fullscreen */
        SDL_SetWindowFullscreen(window, FULLSCREEN);
        SDL_ShowCursor(SDL_DISABLE);
    }

    /* MacOS likes the two lines below, fullscreen toggling is 
       buggy for MacOS in SDL2 right now, this is a temporary 
       fix until I update sdl libs  */
    SDL_DestroyRenderer(renderer);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
}

void Renderer::RenderFrame(){

    /* Clear the screen */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect rectangle; 
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.w = SCALE;
    rectangle.h = SCALE;

    /* Set the foreground color */
    SDL_SetRenderDrawColor(renderer, R, G, B, 0);

    for (int i = 0; i < WIDTH; i++){
    	for (int j = 0; j < HEIGHT; j++){

    		rectangle.x = (i * SCALE) + RENDER_OFFSET_W;
	        rectangle.y = (j * SCALE) + RENDER_OFFSET_H;

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

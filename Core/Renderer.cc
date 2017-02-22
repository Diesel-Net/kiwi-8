#include "Renderer.h"

#include <SDL2/SDL.h>
#include <stdio.h>

#define MIN( a, b ) ( ( a < b) ? a : b )

Renderer::Renderer(){

}

Renderer::~Renderer(){
    SDL_Quit();
}

int Renderer::Initialize(int fullscreen, int R, int G, int B){
    SDL_Init(SDL_INIT_EVERYTHING);
    int window_mode = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    
   	window = SDL_CreateWindow("Chip8", 
   							  SDL_WINDOWPOS_CENTERED, 
   							  SDL_WINDOWPOS_CENTERED, 
   							  WINDOW_WIDTH, 
   							  WINDOW_HEIGHT, 
   							  window_mode);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    /* Set to fullscreen mode if flag present */
    if (fullscreen) { 
        SetFullscreen();
    }
   
    this->R = R;
    this->G = G;
    this->B = B;

    return 0;
}

int Renderer::UpdateRenderSpace() {

    SDL_DestroyRenderer(renderer); 
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

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

	printf("%d(%d) X %d(%d)\n", WINDOW_WIDTH, RENDER_WIDTH, WINDOW_HEIGHT, RENDER_HEIGHT);

    SDL_UpdateWindowSurface(window);

	return 0;
}

int Renderer::SetFullscreen(){
	/* Hide the mouse cursor */
	SDL_ShowCursor(SDL_DISABLE);

	SDL_MaximizeWindow(window);
	
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    UpdateRenderSpace();

	return 0;
}

int Renderer::SetWindowed() {
	/* Show the mouse cursor */
	SDL_ShowCursor(SDL_ENABLE);

    SDL_SetWindowFullscreen(window, 0);

    UpdateRenderSpace();

    return 0;
}

int Renderer::RenderFrame(unsigned char **pixels){
    /* Render the pixels */
    SDL_Rect rectangle; 
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.w = SCALE;
    rectangle.h = SCALE;

    /* Clear the screen */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, R, G, B, 0);

    for (int i = 0; i < WIDTH; i++){
    	for (int j = 0; j < HEIGHT; j++){

    		rectangle.x = (i * SCALE) + RENDER_OFFSET_W;
	        rectangle.y = (j * SCALE) + RENDER_OFFSET_H;
	  		
	  		if (pixels[i][j]) {
	    		SDL_SetRenderDrawColor(renderer, R, G, B, 0);
		        SDL_RenderFillRect(renderer, &rectangle);
	        } else {
	        	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	        	SDL_RenderFillRect(renderer, &rectangle);
                
	        }   
    	}
    } 
    /* Draw */
    SDL_RenderPresent(renderer);
	return 0;
}

void Renderer::Quit() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}
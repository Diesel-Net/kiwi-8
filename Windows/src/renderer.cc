/* Screen renderer */

#include "renderer.h"

#include <windows.h>
#include <iostream> 

#include <SDL2/SDL.h>
#include <SDL_image.h>

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


    /* Load the window icon */
    image = IMG_Load("chip8.ico");
    SDL_SetWindowIcon(window, image);

    if (image == NULL) {
        printf("Error loading window icon...\n");
    }

    /* Set to fullscreen mode if flag present */
    if (fullscreen) { 
        SetFullscreen();
    }

   
    this->R = R;
    this->G = G;
    this->B = B;


    return EXIT_SUCCESS;
}

int Renderer::UpdateRenderSpace() {

    SDL_DestroyRenderer(renderer); 
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	/* Get the current window size */	
	SDL_GetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
	
	int ratio_w = (WINDOW_WIDTH / WIDTH);
	int ratio_h = (WINDOW_HEIGHT / HEIGHT);

	SCALE = min(ratio_w, ratio_h);

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

	return EXIT_SUCCESS;
}

int Renderer::SetFullscreen(){
	/* Hide the mouse cursor */
	SDL_ShowCursor(SDL_DISABLE);

	SDL_MaximizeWindow(window);
	
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    UpdateRenderSpace();

	return EXIT_SUCCESS;
}

int Renderer::SetWindowed() {
	/* Show the mouse cursor */
	SDL_ShowCursor(SDL_ENABLE);

    SDL_SetWindowFullscreen(window, 0);

    UpdateRenderSpace();

    return EXIT_SUCCESS;
}

int Renderer::RenderCycle(unsigned char **pixels){
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
	return EXIT_SUCCESS;
}

int Renderer::CheckInput(unsigned char *key) {

    /* Get the keyboard state */
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (SDL_PollEvent(&event)){

        /* Quit event */
        if (event.type == SDL_QUIT){
            /* Close when the user clicks X */
            SDL_DestroyWindow(window);
            SDL_Quit();
            return EXIT_FAILURE;
        }

        /* Keystroke events */
        if (event.type == SDL_KEYDOWN) {

            //std::cout << "You just pressed the " << (char)event.key.keysym.sym << " key" << std::endl;

            if (state[SDL_SCANCODE_ESCAPE]) {
                /* Close if escape is held down */
                SDL_DestroyWindow(window);
                SDL_Quit();
                return EXIT_FAILURE;
            }
            if ((state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) && state[SDL_SCANCODE_RETURN]) {

                int flag = SDL_GetWindowFlags(window);
                flag = flag ^ SDL_WINDOW_FULLSCREEN;

                if (flag & SDL_WINDOW_FULLSCREEN) {
                    
                    SetFullscreen();
                } else {
                    SetWindowed();
                }

                
            }

            /* Pressed keys */
            if (state[SDL_SCANCODE_1]) {
                key[0x1] = 1;
            }
            if (state[SDL_SCANCODE_2]) {
                key[0x2] = 1;
            }
            if (state[SDL_SCANCODE_3]) {
                key[0x3]= 1;
            }
            if (state[SDL_SCANCODE_4]) {
                key[0xC] = 1;
            }
            if (state[SDL_SCANCODE_Q]) {
                key[0x4] = 1;
            }
            if (state[SDL_SCANCODE_W]) {
                key[0x5] = 1;
            }
            if (state[SDL_SCANCODE_E]) {
                key[0x6] = 1;
            }
            if (state[SDL_SCANCODE_R]) {
                key[0xD] = 1;
            }
            if (state[SDL_SCANCODE_A]) {
                key[0x7] = 1;
            }
            if (state[SDL_SCANCODE_S]) {
                key[0x8] = 1;
            }
            if (state[SDL_SCANCODE_D]) {
                key[0x9] = 1;
            }
            if (state[SDL_SCANCODE_F]) {
                key[0xE] = 1;
            }
            if (state[SDL_SCANCODE_Z]) {
                key[0xA] = 1;
            }
            if (state[SDL_SCANCODE_X]) {
                key[0x0] = 1;
            }
            if (state[SDL_SCANCODE_C]) {
                key[0xB] = 1;
            }
            if (state[SDL_SCANCODE_V]) {
                key[0xF] = 1;
            }
        }

        /* Released keys */
        if (event.type == SDL_KEYUP) {

            //std::cout << "You just released the " << (char)event.key.keysym.sym << " key" << std::endl;

            /* Check that state of the keys */
            if (!state[SDL_SCANCODE_1]) {
                key[0x1] = 0;
            }
            if (!state[SDL_SCANCODE_2]) {
                key[0x2] = 0;
            }
            if (!state[SDL_SCANCODE_3]) {
                key[0x3]= 0;
            }
            if (!state[SDL_SCANCODE_4]) {
                key[0xC] = 0;
            }
            if (!state[SDL_SCANCODE_Q]) {
                key[0x4] = 0;
            }
            if (!state[SDL_SCANCODE_W]) {
                key[0x5] = 0;
            }
            if (!state[SDL_SCANCODE_E]) {
                key[0x6] = 0;
            }
            if (!state[SDL_SCANCODE_R]) {
                key[0xD] = 0;
            }
            if (!state[SDL_SCANCODE_A]) {
                key[0x7] = 0;
            }
            if (!state[SDL_SCANCODE_S]) {
                key[0x8] = 0;
            }
            if (!state[SDL_SCANCODE_D]) {
                key[0x9] = 0;
            }
            if (!state[SDL_SCANCODE_F]) {
                key[0xE] = 0;
            }
            if (!state[SDL_SCANCODE_Z]) {
                key[0xA] = 0;
            }
            if (!state[SDL_SCANCODE_X]) {
                key[0x0] = 0;
            }
            if (!state[SDL_SCANCODE_C]) {
                key[0xB] = 0;
            }
            if (!state[SDL_SCANCODE_V]) {
                key[0xF] = 0;
            }
        }

        /* Window events */
        if (event.window.type == SDL_WINDOWEVENT){

            if (event.window.event == SDL_WINDOWEVENT_RESIZED){  
                /* Update the current rendering screen space */
                UpdateRenderSpace();
            } 
        }
    }
    return EXIT_SUCCESS;
}

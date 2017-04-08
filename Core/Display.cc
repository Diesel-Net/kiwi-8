#include "Display.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

#define MIN( a, b ) ( ( a < b) ? a : b )

Display::Display(){
    SCALE_W = SCALE;
    SCALE_H = SCALE;
    WINDOW_WIDTH = WIDTH * (int)SCALE_W;
    WINDOW_HEIGHT = HEIGHT * (int)SCALE_H;
}

Display::~Display(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
    

void Display::Initialize(unsigned char **vram_ptr, 
                         SDL_mutex *data_lock, 
                         unsigned int fullscreen, 
                         unsigned char R, 
                         unsigned char G, 
                         unsigned char B){

    int window_mode = SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS;

    /* No need to copy the vram, simply assign a pointer to it */
    this->vram_ptr = vram_ptr;
    this->data_lock = data_lock;
    this->R = R;
    this->G = G;
    this->B = B;

    window = SDL_CreateWindow("Chip8", 
                              SDL_WINDOWPOS_CENTERED, 
                              SDL_WINDOWPOS_CENTERED, 
                              WINDOW_WIDTH, 
                              WINDOW_HEIGHT, 
                              window_mode);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Resize(WINDOW_WIDTH, WINDOW_HEIGHT);

    /* Set to fullscreen mode if flag present */
    if (fullscreen) { 
        ToggleFullscreen();
    }
}

void Display::Refresh() {
    /* Destroy the renderer, create a new one, otherwise screen goes black if context is lost */
    SDL_DestroyRenderer(renderer);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    /* Scale up the image */
    SDL_RenderSetScale(renderer, SCALE_W, SCALE_H);

    /* Render the frame */
    RenderFrame();
}

void Display::Resize(int x, int y) {
    /* Get the current window size */
    WINDOW_WIDTH = x;
    WINDOW_HEIGHT = y;
    
    SCALE_W = (float)WINDOW_WIDTH / WIDTH;
    SCALE_H = (float)WINDOW_HEIGHT / HEIGHT;

    /* maintain aspect ratio */
    //SCALE_W = MIN(SCALE_W, SCALE_H);
    //SCALE_H = SCALE_W;

    Refresh();
}

void Display::ToggleFullscreen() {
    /* Check if already fullscreen */
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

void Display::RenderFrame(){

    /* Clear the screen (Set the background color) */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /* Set the foreground color */
    SDL_SetRenderDrawColor(renderer, R, G, B, 0);

    if (SDL_LockMutex(data_lock) == 0) {

        for (int i = 0; i < WIDTH; i++){
            for (int j = 0; j < HEIGHT; j++){

                if (vram_ptr[i][j]) {

                    /* Fill the foreground pixel */
                    SDL_RenderDrawPoint(renderer, i, j);
                } 
            }
        }

        SDL_UnlockMutex(data_lock);
    } else {
        fprintf(stderr, "%s\n", SDL_GetError());
    }

    /* Draw anything rendered since last call */
    SDL_RenderPresent(renderer);
}

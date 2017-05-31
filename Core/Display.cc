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
    back_buffer = NULL;
    renderer = NULL;
    window = NULL;
}

Display::~Display(){
    /* Clean-up */
    if (back_buffer) {
        for (int i = 0; i < WIDTH; i++) {
            free(back_buffer[i]);
        }
        free(back_buffer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
    

int Display::Initialize(SDL_mutex *data_lock, 
                         unsigned int fullscreen, 
                         unsigned char R, 
                         unsigned char G, 
                         unsigned char B){

    int window_mode = SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS;
    this->data_lock = data_lock;
    this->R = R;
    this->G = G;
    this->B = B;

    /* Init the backbuffer */
    back_buffer = (unsigned char **) malloc(WIDTH * sizeof(unsigned char *));
    const char *err_str = "Unable to allocate memory on the heap.\n";

    if(!back_buffer) {
        fprintf(stderr, "%s", err_str);
        return 1;
    }
    memset(back_buffer, 0, WIDTH * sizeof(unsigned char *));

    for (int i = 0; i < WIDTH; i++) {
        back_buffer[i] = (unsigned char *) malloc(HEIGHT * sizeof(unsigned char));
        if(!back_buffer[i]) {
            fprintf(stderr, "%s", err_str);
            return 1;
        }
        memset(back_buffer[i], 0, HEIGHT * sizeof(unsigned char));
    }

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

    return 0;
}

void Display::Refresh() {
    /* Destroy the renderer, create a new one, otherwise screen 
       goes black for a few seconds on MacOS when swithcing to fullscreen */
    SDL_DestroyRenderer(renderer);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    /* Scale up the image */
    SDL_RenderSetScale(renderer, SCALE_W, SCALE_H);

    /* Render the last frame */
    RenderFrame(back_buffer);
}

void Display::Resize(int x, int y) {
    /* Get the current window size */
    WINDOW_WIDTH = x;
    WINDOW_HEIGHT = y;
    
    SCALE_W = (float)(WINDOW_WIDTH / WIDTH);
    SCALE_H = (float)(WINDOW_HEIGHT / HEIGHT);

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

void Display::RenderFrame(unsigned char **frame){

    /* copy the frame to back_buffer */
    for (int i = 0; i < WIDTH; i++) {
        memcpy(back_buffer[i], frame[i], HEIGHT * sizeof(unsigned char));
    }

    /* Clear the screen (Set the background color) */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /* Set the foreground color */
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);

    for (int i = 0; i < WIDTH; i++){
        for (int j = 0; j < HEIGHT; j++){

            if (frame[i][j]) {

                /* Fill the foreground pixel */
                SDL_RenderDrawPoint(renderer, i, j);
            } 
        }
    }

    /* Draw anything rendered since last call */
    SDL_RenderPresent(renderer);
}

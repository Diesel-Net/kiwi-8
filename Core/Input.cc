#include "Input.h"

#include <SDL2/SDL.h>
#include <stdio.h>

Input::Input() {
    /* Empty constructor */
}

Input::~Input() {
    /* Empty deconstructor */
}

int Input::Poll(Renderer *renderer, unsigned char *keys, SDL_mutex *data_lock) {
    int response = 1;
    
    /* Wait indefinitley for the next event */
    if (SDL_WaitEvent(&event)) {
        
        state = SDL_GetKeyboardState(NULL);
        
        if (SDL_LockMutex(data_lock) == 0) {
            CheckKeys(keys);
            response = CheckOS(renderer);
            SDL_UnlockMutex(data_lock);
        } else {
            fprintf(stderr, "Couldn't lock mutex, terminating main thread.\n");
        }
        
    } else {
        fprintf(stderr, "Error waiting for next event.\n");
    }
    return response;
}

void Input::CheckKeys(unsigned char *keys) {

    /* Pressed keys */
    if (event.type == SDL_KEYDOWN) {

        /* Check that state of the keys */
        if (state[SDL_SCANCODE_1]) {
            keys[0x1] = 1;
        } 
        
        if (state[SDL_SCANCODE_2]) {
            keys[0x2] = 1;
        }
        if (state[SDL_SCANCODE_3]) {
            keys[0x3]= 1;
        }
        if (state[SDL_SCANCODE_4]) {
            keys[0xC] = 1;
        }
        if (state[SDL_SCANCODE_Q]) {
            keys[0x4] = 1;
        }
        if (state[SDL_SCANCODE_W]) {
            keys[0x5] = 1;
        }
        if (state[SDL_SCANCODE_E]) {
            keys[0x6] = 1;
        }
        if (state[SDL_SCANCODE_R]) {
            keys[0xD] = 1;
        }
        if (state[SDL_SCANCODE_A]) {
            keys[0x7] = 1;
        }
        if (state[SDL_SCANCODE_S]) {
            keys[0x8] = 1;
        }
        if (state[SDL_SCANCODE_D]) {
            keys[0x9] = 1;
        }
        if (state[SDL_SCANCODE_F]) {
            keys[0xE] = 1;
        }
        if (state[SDL_SCANCODE_Z]) {
            keys[0xA] = 1;
        }
        if (state[SDL_SCANCODE_X]) {
            keys[0x0] = 1;
        }
        if (state[SDL_SCANCODE_C]) {
            keys[0xB] = 1;
        }
        if (state[SDL_SCANCODE_V]) {
            keys[0xF] = 1;
        }
    }

    /* Released keys */
    if (event.type == SDL_KEYUP) {

        /* Check that state of the keys */
        if (!state[SDL_SCANCODE_1]) {
            keys[0x1] = 0;
        }
        if (!state[SDL_SCANCODE_2]) {
            keys[0x2] = 0;
        }
        if (!state[SDL_SCANCODE_3]) {
            keys[0x3]= 0;
        }
        if (!state[SDL_SCANCODE_4]) {
            keys[0xC] = 0;
        }
        if (!state[SDL_SCANCODE_Q]) {
            keys[0x4] = 0;
        }
        if (!state[SDL_SCANCODE_W]) {
            keys[0x5] = 0;
        }
        if (!state[SDL_SCANCODE_E]) {
            keys[0x6] = 0;
        }
        if (!state[SDL_SCANCODE_R]) {
            keys[0xD] = 0;
        }
        if (!state[SDL_SCANCODE_A]) {
            keys[0x7] = 0;
        }
        if (!state[SDL_SCANCODE_S]) {
            keys[0x8] = 0;
        }
        if (!state[SDL_SCANCODE_D]) {
            keys[0x9] = 0;
        }
        if (!state[SDL_SCANCODE_F]) {
            keys[0xE] = 0;
        }
        if (!state[SDL_SCANCODE_Z]) {
            keys[0xA] = 0;
        }
        if (!state[SDL_SCANCODE_X]) {
            keys[0x0] = 0;
        }
        if (!state[SDL_SCANCODE_C]) {
            keys[0xB] = 0;
        }
        if (!state[SDL_SCANCODE_V]) {
            keys[0xF] = 0;
        }
    }
} 

int Input::CheckOS(Renderer *renderer) {

    /* Quit event */
    if (event.type == SDL_QUIT){
        /* Close when the user clicks X */
        return 1;
    }

    /* Keystroke events */
    if (event.type == SDL_KEYDOWN) {
        if (state[SDL_SCANCODE_ESCAPE]) {
            /* Close if escape is held down */
            return 1;
        }

        if (state[SDL_SCANCODE_F5]) {
            /* Soft reset if F5 is held down */
            return -1;
        }

        if ((state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) && state[SDL_SCANCODE_RETURN]) {
            renderer->ToggleFullscreen();
        }
    }

    /* Window events */
    if (event.window.type == SDL_WINDOWEVENT){
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){  
            /* Update the current rendering screen space */
            renderer->UpdateRenderSpace();
        } 
        if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            /* TODO: Resume the emulator, if paused */
            renderer->UpdateRenderSpace();
        } 
        if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            /* TODO: Add a toggle for "pause on focus loss" */
        }
        
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
            return 1;
        }
    }

    return 0;
}

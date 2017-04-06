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

int Input::CheckOS(Renderer *renderer) {

    /* Quit event */
    if (event.type == SDL_QUIT){
        /* Close when the user clicks "X" */
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
            renderer->Resize(event.window.data1, event.window.data2);
        } 
        if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            /* TODO: Resume the emulator, if paused */
            renderer->Refresh();
        } 
        if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            /* TODO: Add a toggle for "pause on focus loss" */
        }
        
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
            /* The window manager requests that the window be closed */
            return 1;
        }
    }

    return 0;
}

void Input::CheckKeys(unsigned char *keys) {
    /* Check that state of the keys */

    /* Pressed keys */
    if (event.type == SDL_KEYDOWN) {
        keys[0x1] = state[SDL_SCANCODE_1];
        keys[0x2] = state[SDL_SCANCODE_2];
        keys[0x3] = state[SDL_SCANCODE_3];
        keys[0xC] = state[SDL_SCANCODE_4];
        keys[0x4] = state[SDL_SCANCODE_Q];
        keys[0x5] = state[SDL_SCANCODE_W];
        keys[0x6] = state[SDL_SCANCODE_E];
        keys[0xD] = state[SDL_SCANCODE_R];
        keys[0x7] = state[SDL_SCANCODE_A];
        keys[0x8] = state[SDL_SCANCODE_S];
        keys[0x9] = state[SDL_SCANCODE_D];
        keys[0xE] = state[SDL_SCANCODE_F];
        keys[0xA] = state[SDL_SCANCODE_Z];
        keys[0x0] = state[SDL_SCANCODE_X];
        keys[0xB] = state[SDL_SCANCODE_C];
        keys[0xF] = state[SDL_SCANCODE_V];
    }

    /* Released keys */
    if (event.type == SDL_KEYUP) {
        keys[0x1] = !state[SDL_SCANCODE_1];
        keys[0x2] = !state[SDL_SCANCODE_2];
        keys[0x3] = !state[SDL_SCANCODE_3];
        keys[0xC] = !state[SDL_SCANCODE_4];
        keys[0x4] = !state[SDL_SCANCODE_Q];
        keys[0x5] = !state[SDL_SCANCODE_W];
        keys[0x6] = !state[SDL_SCANCODE_E];
        keys[0xD] = !state[SDL_SCANCODE_R];
        keys[0x7] = !state[SDL_SCANCODE_A];
        keys[0x8] = !state[SDL_SCANCODE_S];
        keys[0x9] = !state[SDL_SCANCODE_D];
        keys[0xE] = !state[SDL_SCANCODE_F];
        keys[0xA] = !state[SDL_SCANCODE_Z];
        keys[0x0] = !state[SDL_SCANCODE_X];
        keys[0xB] = !state[SDL_SCANCODE_C];
        keys[0xF] = !state[SDL_SCANCODE_V];
    }
}

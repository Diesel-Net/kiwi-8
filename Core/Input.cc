#include "Input.h"
#include "Display.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <String.h>

Input::Input() {
    /* Empty */
}

Input::~Input() {
    /* Empty */
}

void Input::Initialize(Display *display, SDL_mutex *data_lock, SDL_cond *halt_cond) {
    this->display = display;
    this->data_lock = data_lock;
    this->halt_cond = halt_cond;
    Reset();
}

void Input::Reset() {
    memset(keys, 0, NUM_KEYS);
}

int Input::Poll(unsigned int *steps) {
    int response = USER_QUIT;
    
    /* Wait indefinitley for the next event */
    if (SDL_WaitEvent(&event)) {
        state = SDL_GetKeyboardState(NULL);
        if (SDL_LockMutex(data_lock) == 0) {
            response = CheckEvents(steps);
            CheckKeys();
            SDL_UnlockMutex(data_lock);
        } else {
            fprintf(stderr, "%s\n", SDL_GetError());
        }
    } else {
        fprintf(stderr, "%s\n", SDL_GetError());
    }
    return response;
}

int Input::CheckEvents(unsigned int *steps) {
    int response = CONTINUE;

    /* Quit event */
    if (event.type == SDL_QUIT){
        /* Close when the user clicks "X" */
        response = USER_QUIT;
    }

    /* User defined events */
    if (event.type == SDL_USEREVENT) {
        if (event.user.code == SIGNAL_DRAW) {

            unsigned char **data = (unsigned char **)event.user.data1;
            display->RenderFrame(data);
            
            /* Clean-up the data */
            for (int i = 0; i < WIDTH; i++) {
                free(data[i]);
            }
            free(data);
        } 
    }

    /* Keystroke events */
    if (event.type == SDL_KEYDOWN) {
        if (state[SDL_SCANCODE_ESCAPE]){
            /* Close if escape is held down */
            response = USER_QUIT;
        }

        /* For MacOS Compliance */
        #ifdef __APPLE__
        if ((state[SDL_SCANCODE_LGUI] && state[SDL_SCANCODE_Q]) || 
            (state[SDL_SCANCODE_RGUI] && state[SDL_SCANCODE_Q])) {
            response = USER_QUIT;
        }
        #endif

        if (state[SDL_SCANCODE_F5]) {
            /* Soft reset if F5 is held down */
            response = SOFT_RESET;
        }
        if ((state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) && state[SDL_SCANCODE_RETURN]) {
            display->ToggleFullscreen();
        }
        if (state[SDL_SCANCODE_PAGEDOWN]) {
        	/* TODO: Slow emulation speed */
        	if (*steps -1 < MIN_STEPS ) {
        		*steps = MIN_STEPS;
        	} else {
        		*steps -= 1;
        	}
        	//fprintf(stderr, "steps: %u\n", *steps);

        }
        if (state[SDL_SCANCODE_PAGEUP]) {
        	/* TODO: Raise emulation speed */
        	if (*steps +1 > MAX_STEPS ) {
        		*steps = MAX_STEPS;
        	} else {
        		*steps += 1;
        	}
        	//fprintf(stderr, "steps: %u\n", *steps);
        }
    }

    /* Window events */
    if (event.window.type == SDL_WINDOWEVENT){
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){  
            /* Update the current rendering screen space */
            display->Resize(event.window.data1, event.window.data2);
        } 
        if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            /* TODO: Resume the emulator, if paused */
            display->Refresh();
        } 
        if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            /* TODO: Add a toggle for "pause on focus loss" */

        }
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
            /* The window manager requests that the window be closed */
            response = USER_QUIT;
        }
    }

    return response;
}

void Input::CheckKeys() {

    /* Map the state of the keys */
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

    /* Signal if a key was pressed this round */
    if (event.type == SDL_KEYDOWN) {
        int key_pressed = 0;
        for (int i = 0; i < NUM_KEYS; i++) {
            if (keys[i]) {
                key_pressed = 1;
            }
        }
        if (key_pressed) {
            SDL_CondSignal(halt_cond);
        }        
    }
}

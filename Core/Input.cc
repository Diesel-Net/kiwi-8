#include "Input.h"
#include "Display.h"
#include <stdio.h>
#include <String.h>

Input::Input() {
    /* Empty */
}

Input::~Input() {
    /* Empty */
}

void Input::Initialize(Display *display, int *steps, int *cpu_halt) {
    this->steps = steps;
    this->display = display;
    this->cpu_halt = cpu_halt;
    Reset();
}

void Input::Reset() {
    key_pressed = 0;
    memset(keys, 0, NUM_KEYS);
}

int Input::Poll() {
    int response = CONTINUE;
    
    /* Purge any queued events */
    while (SDL_PollEvent(&event)) {
        response = USER_QUIT;

        state = SDL_GetKeyboardState(NULL);

        /* Check Gui */
        display->gui.ProcessEvents(&event);
        if (display->gui.quit_flag) break;   

        /* Check Window */
        response = CheckEvents();
        if (response == USER_QUIT) break; 
        
        /* Check chip-8 input */
        CheckKeys(); 

    } 
    return response;
}

int Input::CheckEvents() {
    int response = CONTINUE;

    /* Quit event */
    if (event.type == SDL_QUIT){
        /* Close when the user clicks "X" */
        response = USER_QUIT;
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
        if (state[SDL_SCANCODE_F12]) {
            display->ToggleFullscreen();
        }
        if (state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) {
            /* Hide/Show menu */
            display->gui.show_menu_flag = !display->gui.show_menu_flag;
        }
        if (state[SDL_SCANCODE_PAGEDOWN]) {
        	/* TODO: Slow emulation speed */
        	if (*steps -1 < MIN_STEPS ) {
        		*steps = MIN_STEPS;
        	} else {
        		*steps -= 1;
        	}
        }
        if (state[SDL_SCANCODE_PAGEUP]) {
        	/* TODO: Raise emulation speed */
        	if (*steps +1 > MAX_STEPS ) {
        		*steps = MAX_STEPS;
        	} else {
        		*steps += 1;
        	}
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

    /* Check if cpu is awaiting a keypress for opcode FX0A */
    if (cpu_halt && event.type == SDL_KEYDOWN) {
        for (int i = 0; i < NUM_KEYS; i++) {
            if (keys[i]) {
                key_pressed = 1;
                break;
            }
        }     
    }
}

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

void Input::Initialize(Display *display, int *steps, bool *cpu_halt, bool *emulation_paused) {
    this->steps = steps;
    this->display = display;
    this->cpu_halt = cpu_halt;
    this->emulation_paused = emulation_paused;
    Reset();
}

void Input::Reset() {
    awaiting_key_press = 0;
    memset(keys, 0, NUM_KEYS);
}

int Input::Poll() {
    int response = CONTINUE;
    
    /* Purge any queued events */
    while (SDL_PollEvent(&event)) {

        state = SDL_GetKeyboardState(NULL);

        /* Check Gui */
        display->gui.ProcessEvents(&event);
        if (display->gui.quit_flag) response |= USER_QUIT;
        if (display->gui.soft_reset_flag) response |= SOFT_RESET;

        /* Check Window */
        response |= CheckEvents();
        
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

        if (state[SDL_SCANCODE_F5]) {
            /* Soft reset if F5 is held down */
            response = SOFT_RESET;
        }
        if (state[SDL_SCANCODE_RETURN]) {
            /* Switch from windowed to fullscreen or vice-versa */
            display->ToggleFullscreen();
        }
        if (state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) {
            /* Hide/Show menu */
            display->gui.show_menu_flag = !display->gui.show_menu_flag;
        }
        if (state[SDL_SCANCODE_P]) {
            *emulation_paused = !*emulation_paused;
        }
        if (state[SDL_SCANCODE_PAGEDOWN]) {
        	/* Slow emulation speed */
        	if (*steps -1 < MIN_STEPS ) {
        		*steps = MIN_STEPS;
        	} else {
        		*steps -= 1;
        	}
        }
        if (state[SDL_SCANCODE_PAGEUP]) {
        	/* Raise emulation speed */
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
    if (cpu_halt && awaiting_key_press) {
        for (int i = 0; i < NUM_KEYS; i++) {
            if (keys[i]) {
                awaiting_key_press = 0;
                break;
            }
        }     
    }
}

#include "input.h"
#include <SDL2/SDL.h>

int check_input(Renderer *renderer, unsigned char *key) {
    SDL_Event event;

    /* Get the keyboard state */
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (SDL_PollEvent(&event)){

        /* Quit event */
        if (event.type == SDL_QUIT){
            /* Close when the user clicks X */
            renderer->Quit();
            return 1;
        }

        /* Keystroke events */
        if (event.type == SDL_KEYDOWN) {

            //std::cout << "You just pressed the " << (char)event.key.keysym.sym << " key" << std::endl;

            if (state[SDL_SCANCODE_ESCAPE]) {
                /* Close if escape is held down */
                renderer->Quit();
                return 1;
            }
            if ((state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) && state[SDL_SCANCODE_RETURN]) {

                int flag = SDL_GetWindowFlags(renderer->window);
                flag = flag ^ SDL_WINDOW_FULLSCREEN;

                if (flag & SDL_WINDOW_FULLSCREEN) {
                    
                    renderer->SetFullscreen();
                } else {
                    renderer->SetWindowed();
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
                renderer->UpdateRenderSpace();
            } 
        }
    }
    return 0;
}

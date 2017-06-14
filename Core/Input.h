#ifndef INPUT_H
#define INPUT_H

#include "Display.h"
#include <SDL2/SDL.h>

#define NUM_KEYS 16
#define USER_QUIT 1
#define SOFT_RESET 2
#define CONTINUE 0
#define MIN_STEPS 1
#define MAX_STEPS 50

class Input {
    private:
        /* For processing window/keyboard events */
        SDL_Event event;
        const Uint8 *state;

        Display *display;
        int *steps;
        bool *cpu_halt;

        int CheckEvents();
        void CheckKeys();

    public:
        Input();
        ~Input();

        /* HEX based keypad (0x0-0xF) */
        unsigned char keys[NUM_KEYS];
        int awaiting_key_press;

        void Initialize(Display *display, int *steps, bool *cpu_halt);
        void Reset();
        int Poll();
        
};

#endif

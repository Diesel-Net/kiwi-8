#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

#define NUM_KEYS 16

/* there is a 4 ( and no 3), so each value 
can represent a unique bit position */
#define CONTINUE 0
#define USER_QUIT 1
#define SOFT_RESET 2
#define LOAD_ROM 4 

/* forward declaration */
class Display;

class Input {
    private:
        /* for processing window/keyboard events */
        SDL_Event event;
        const unsigned char *state;

        Display *display;

        int *cycles;
        bool *cpu_halt;
        bool *paused;
        bool *muted;

        int ProcessEvents();
        void ProcessKeys();

    public:
        Input();
        ~Input();

        /* chip-8 HEX based keypad (0x0-0xF) */
        unsigned char keys[NUM_KEYS];

        /* for opcode 0xFX0A */
        bool awaiting_key_press;

        void Initialize(Display *display, 
                        int *cycles, 
                        bool *cpu_halt, 
                        bool *paused,
                        bool *muted);
        void Reset();
        int Poll();
        
};

#endif

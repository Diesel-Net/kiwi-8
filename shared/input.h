#ifndef INPUT_H
#define INPUT_H

#include "chip8.h"
#include <SDL2/SDL.h>

#define NUM_KEYS 16

/* there is a 4 ( and no 3), so each value
can represent a unique bit position */
#define CONTINUE 0
#define USER_QUIT 1
#define SOFT_RESET 2
#define LOAD_ROM 4
#define SAVE_PROFILE 8

struct input {
    /* for processing window/keyboard events */
    SDL_Event event;
    const unsigned char *state;

    /* chip-8 HEX based keypad (0x0-0xF) */
    unsigned char keys[NUM_KEYS];

    /* for opcode 0xFX0A */
    bool awaiting_key_press;
};

/* Global input instance */
extern struct input input;

/* Input functions */
void input_reset(void);
int input_poll(void);

#endif

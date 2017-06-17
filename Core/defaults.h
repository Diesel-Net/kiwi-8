#ifndef DEFAULTS_H
#define DEFAULTS_H

#define DEFAULT_ROM_SIZE 4

/* Defaults */
typedef struct Defaults {
    const bool fullscreen = 0;
    const bool load_store_quirk = 0;
    const bool shift_quirk = 0;
    const bool vwrap = 1;
    const unsigned char R = 200;
    const unsigned char G = 170;
    const unsigned char B = 255;
} Defaults;


const unsigned char default_rom[DEFAULT_ROM_SIZE] = {
    /* Custom rom image using bmp.ch8 - TO COMPLETE */
    0x00, 0x00, 0x00, 0x00
};

#endif

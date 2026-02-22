#ifndef CHIP8_H
#define CHIP8_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bootrom.h" // Generated at build time from roms/Kiwi8_logo_2.ch8
#include "quirks.h"
#include <stdlib.h>
#include <stdbool.h>

#define MEM_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_DEPTH 16
#define ENTRY_POINT 0x200
#define FONTS_SIZE 80
#define CYCLES_PER_STEP 12 /* ~720 inst/sec if ticking at 60hz */
#define MIN_CYCLES_PER_STEP 1
#define MAX_CYCLES_PER_STEP 50
#define TICKS 60 /* hz - Timer count down rate */

static const unsigned char chip8_fontset[FONTS_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0x20, 0x60, 0x20, 0x20, 0x70, // B
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // C
        0xF0, 0x80, 0xF0, 0x80, 0x80, // D
        0xF0, 0x90, 0x90, 0xF0, 0x90  // F
    };

struct chip8 {
    /* number of cycles per step */
    int cycles;

    /* whether or not cpu is currently halted by opcode FX0A */
    bool cpu_halt;

    /* whether or not emulation is currently paused. */
    bool paused;

    /* All quirk toggles */
    struct quirks quirks;

    /* two bytes for each instruction */
    unsigned short opcode;

    /* memory */
    unsigned char memory[MEM_SIZE];

    /* copy of the rom for soft resetting */
    unsigned char *rom;
    unsigned int rom_size;

    /* rom profile tracking */
    char rom_filename[256];  /* basename of currently loaded ROM */
    int rom_loaded;          /* 1 if user ROM loaded, 0 if bootrom */

    /* registers */
    unsigned char V[NUM_REGISTERS];
    unsigned short I;
    unsigned short PC;

    unsigned char delay_timer;
    unsigned char sound_timer;

    /* stack */
    unsigned short stack[STACK_DEPTH];
    unsigned short sp;

    /* mute audio toggle */
    bool muted;

    /* draw flag */
    int draw_flag;

    /* 1-bit encoded screen pixels (64x32) */
    unsigned char **vram;


};

/* Global chip8 instance */
extern struct chip8 chip8;

void chip8_destroy(void);
int chip8_init(
    bool fullscreen,
    bool muted,
    const char *profiles_path
);
int chip8_load_rom(const char *rom_filepath);
void chip8_run(void);
void chip8_update_timers(void);
void chip8_step_cpu(int cycles);
void chip8_soft_reset(void);
int chip8_load_bootrom(void);
void chip8_fetch_opcode(void);
void chip8_execute_opcode(void);

#ifdef __cplusplus
}
#endif

#endif // CHIP8_H

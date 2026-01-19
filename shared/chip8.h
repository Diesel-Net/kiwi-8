#ifndef CHIP8_H
#define CHIP8_H

#include "bootrom.h" // Generated at build time from roms/Kiwi8_logo_2.ch8
#include "display.h"
#include "input.h"
#include "audio.h"
#include <stdlib.h>

/* Detect OS at compile time */
#if defined(_WIN32) || defined(_WIN64)
    #define BUILD_OS "Windows"
#elif defined(__APPLE__) || defined(__MACH__)
    #define BUILD_OS "macOS"
#elif defined(__linux__)
    #define BUILD_OS "Linux"
#else
    #define BUILD_OS "unknown"
#endif

/* Detect architecture at compile time */
#if defined(__aarch64__) || defined(__arm64__) || defined(_M_ARM64)
    #define BUILD_ARCH "ARM64"
#elif defined(__x86_64__) || defined(_M_X64)
    #define BUILD_ARCH "x86_64"
#elif defined(__i386__) || defined(_M_IX86)
    #define BUILD_ARCH "x86"
#else
    #define BUILD_ARCH "unknown"
#endif

// APP_NAME is defined by the compiler via -DAPP_NAME="..."
#ifndef APP_NAME
#define APP_NAME "Kiwi8"
#endif

// VERSION is defined by the compiler via -DVERSION="..."
#ifndef VERSION
#define VERSION "develop"
#endif

// SUB_VERSION is defined by the compiler via -DSUB_VERSION="..."
#ifndef SUB_VERSION
#define SUB_VERSION "unknown"
#endif

#define MEM_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_DEPTH 16
#define ENTRY_POINT 0x200
#define FONTS_SIZE 80
#define CYCLES_PER_STEP 12 /* ~720 inst/sec if ticking at 60hz */
#define MIN_CYCLES_PER_STEP 1
#define MAX_CYCLES_PER_STEP 50
#define TICKS 60 /* hz - Timer count down rate */

struct chip8 {
    /* number of cycles per step */
    int cycles;

    /* whether or not cpu is currently halted by opcode FX0A */
    bool cpu_halt;

    /* whether or not emulation is currently paused. */
    bool paused;

    /* CPU quirks */
    bool load_store_quirk;
    bool shift_quirk;

    /* vertical wrapping toggle */
    bool vwrap;

    /* two bytes for each instruction */
    unsigned short opcode;

    /* memory */
    unsigned char memory[MEM_SIZE];

    /* copy of the rom for soft resetting */
    unsigned char *rom;
    unsigned int rom_size;

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

    const unsigned char chip8_fontset[FONTS_SIZE] = {
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
};

/* Global chip8 instance */
extern struct chip8 chip8;

void chip8_destroy(void);
int chip8_initialize(
    bool fullscreen,
    bool load_store_quirk,
    bool shift_quirk,
    bool vwrap,
    bool muted
);
int chip8_load(const char *rom_name);
void chip8_run(void);
void chip8_update_timers(void);
void chip8_step_cpu(int cycles);
void chip8_soft_reset(void);
int chip8_load_bootrom(void);
void chip8_fetch_opcode(void);
void chip8_execute_opcode(void);

/* opcode helpers */
void exec00E0(void);
void exec00EE(void);
void exec0NNN(void);
void exec1NNN(void);
void exec2NNN(void);
void exec3XNN(void);
void exec4XNN(void);
void exec5XY0(void);
void exec6XNN(void);
void exec7XNN(void);
void exec8XY0(void);
void exec8XY1(void);
void exec8XY2(void);
void exec8XY3(void);
void exec8XY4(void);
void exec8XY5(void);
void exec8XY6(void);
void exec8XY7(void);
void exec8XYE(void);
void exec9XY0(void);
void execANNN(void);
void execBNNN(void);
void execCXNN(void);
void execDXYN(void);
void execEX9E(void);
void execEXA1(void);
void execFX07(void);
void execFX0A(void);
void execFX15(void);
void execFX18(void);
void execFX1E(void);
void execFX29(void);
void execFX33(void);
void execFX55(void);
void execFX65(void);
void execUnknown(void);

#endif

#ifndef CHIP8_H
#define CHIP8_H

#include "Display.h"
#include "Input.h"
#include <SDL2/SDL.h>

#define TITLE "Chip8 v1.02"
#define MEM_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_DEPTH 16
#define MEM_OFFSET 512
#define FONTS_SIZE 80
#define INSTRUCTIONS_PER_CYCLE 14 /* ~840 inst/sec if running at 60hz */
#define SPEED 60 /* hz - Cycle Speed */


class Chip8 {

    private:

        /* Two quirks of the Chip8 CPU. 
           Some games assume these are enabled to run correctly.

           Load/store quirks - Instructions OxFX55 and 0xF65 increments 
           value of I register but some CHIP-8 programs assumes that 
           they don't. Enabling this quirk causes I register to become 
           unchanged after the instruction.

           Shift quirks - Shift instructions originally shift register 
           VY and store results in register VX. Some CHIP-8 programs 
           incorrectly assumes that the VX register is shifted by this 
           instruction, and VY remains unmodified. Enabling this quirk
           causes VX to become shifted and VY remain untouched. */
        int load_store_quirk;
        int shift_quirk;

        /* Two bytes for each instruction */
        unsigned short opcode;

        /* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
           0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
           0x200-0xFFF - Program ROM and work RAM */
        unsigned char memory[MEM_SIZE];

        /* A copy of the rom for soft resetting */
        unsigned char *rom;
        unsigned int rom_size;

        /* 15 general prupose regsiters, and a carry flag register */
        unsigned char V[NUM_REGISTERS]; 

        /* Index register and program counter */
        unsigned short I;
        unsigned short PC;

        unsigned char delay_timer;
        unsigned char sound_timer;

        /* stack with maximum of 16 levels */
        unsigned short stack[STACK_DEPTH];
        unsigned short sp;

        Input *input;

        /* If this flag is enabled, draw a frame at the end of the cycle */
        int draw_flag;
        unsigned int event_type;


        /* 1-bit encoded screen pixels (32x64) */
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
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

        /* A single thread for processing opcodes */
        SDL_Thread *cpu_thread;

        /* Let's thread-safe-ify things */
        SDL_mutex *data_lock;
        SDL_cond *halt_cond;

        /* For signaling thread termination */
        int terminated;

        void SoftReset();
        void FetchOpcode();
        void ExecuteOpcode();
        void UpdateTimers();
        void SignalTerminate();
        void SignalDraw();

        static int CPUThread(void *data);
        

    public:
        /* Constructor */
        Chip8();
        ~Chip8();

        int Initialize(unsigned int fullscreen, 
                       unsigned int load_store_quirk,
                       unsigned int shift_quirk,
                       unsigned char R, 
                       unsigned char G, 
                       unsigned char B);

        int Load(const char *rom_name);
        void Run();

        /* Exposed publicly for windows icon fix (see main.cc) */
        Display *display;

        /* Exposed publicly for CPU thread */
        int EmulateCycle();
};

#endif

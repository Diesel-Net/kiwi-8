#ifndef CHIP8_H
#define CHIP8_H

#include "Renderer.h"
#include "Input.h"

#include <SDL2/SDL.h>

#define MEM_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_DEPTH 16
#define MEM_OFFSET 512
#define NUM_KEYS 16
#define FONTS_SIZE 80

/* Thread functions  */
int TimerThread(void *data);
int CycleThread(void *data);

class Chip8 {

	private:
		/* Two bytes for each instruction */
		unsigned short opcode;

		/* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
		   0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
		   0x200-0xFFF - Program ROM and work RAM */
		unsigned char memory[MEM_SIZE];

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

		/* HEX based keypad (0x0-0xF) */
		unsigned char keys[NUM_KEYS];
		Input input = Input();

		int draw_flag;

		/* 1-bit encoded screen pixels (32x64) */
		unsigned char **vram;

		/* For soft-resetting */
		unsigned char *rom;
		int rom_size;

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

		/* Threads */
		SDL_Thread *timer_thread;
		SDL_Thread *cycle_thread;

		/* Mutex shared by all the threads */
		SDL_mutex *data_lock = SDL_CreateMutex();

		/* For thread signaling */
		int terminated;

		void SoftReset();
		void FetchOpcode();
		void InterpretOpcode();

	public:
		/* Exposed publicly for windows icon fix (see main.cc) */
		Renderer renderer = Renderer();

		/* Exposed publicly for timer/cycle threads */
		void UpdateTimers();
		void EmulateCycle();

		/* Constructor */
		Chip8();
		~Chip8();

		int Initialize(int fullscreen, int R, int G, int B);
		int Load(const char *rom_name);
		void Run();
		int SignalTerminate();
};

#endif

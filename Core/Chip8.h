#ifndef CHIP8_H
#define CHIP8_H

#include "Renderer.h"

#define MEM_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_DEPTH 16
#define MEM_OFFSET 512
#define NUM_INPUT 16
#define FONTS_SIZE 80

class Chip8 {

	private:
		
		/* Two bytes for each instruction */
		unsigned short opcode = 0;

		/* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
		   0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
		   0x200-0xFFF - Program ROM and work RAM */
		unsigned char memory[MEM_SIZE];

		/* 15 general prupose regsiters, and a carry flag register */
		unsigned char V[NUM_REGISTERS]; 

		/* Index register and program counter */
		unsigned short I = 0;
		unsigned short PC = MEM_OFFSET;

		unsigned char delay_timer = 0;
		unsigned char sound_timer = 0;

		/* stack with maximum of 16 levels */
		unsigned short stack[STACK_DEPTH];
		unsigned short sp = 0;

		/* HEX based keypad (0x0-0xF) */
		unsigned char key[NUM_INPUT];

		int draw_flag = 0;

		/* 1-bit encoded screen pixels (32x64) */
		unsigned char **pixels;

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

		int EmulateCycle();
		int FetchOpcode();
		int InterpretOpcode();
		int UpdateTimers();

	public:
		Renderer renderer = Renderer();

		/* Constructor */
		Chip8();
		~Chip8();

		int Initialize(int fullscreen, int R, int G, int B);
		int Load(const char *rom_name);
		int Run();

};

#endif

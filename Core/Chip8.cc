/*
Author: Thomas Daley
Date: September 18, 2016
chip8.cc
*/

#include "Chip8.h"
#include "Renderer.h"
#include "input.h"

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Constructor */
Chip8::Chip8() {

}

Chip8::~Chip8() {
	free(vram);
}

int Chip8::Initialize(int fullscreen, int R, int G, int B){
	renderer.Initialize(fullscreen, R, G, B);
	vram = (unsigned char **) malloc(WIDTH * sizeof(unsigned char *));
	memset(vram, 0, WIDTH * sizeof(unsigned char *));

	for (int i = 0; i < WIDTH; i++) {
		vram[i] = (unsigned char *) malloc(HEIGHT * sizeof(unsigned char));
		memset(vram[i], 0, HEIGHT * sizeof(unsigned char));
	}

	/* Initialize registers and memory once */
	memset(V, 0 , NUM_REGISTERS);
	memset(memory, 0, MEM_SIZE);
	memset(stack, 0, STACK_DEPTH);
	memset(key, 0, NUM_INPUT);

	/* Load fontset */
	for(int i = 0; i < FONTS_SIZE; ++i) {
		memory[i] = chip8_fontset[i];
	}

	delay_timer = 0;
	sound_timer = 0;

	draw_flag = 1;

	return 0;
}

int Chip8::Run(){
	for(;;) {
		if (EmulateCycle()) {
			return 1;
		}

		SDL_Delay(1.5);
	}
	return 0;
}

int Chip8::Load(const char *rom_name){
	
	FILE *file;
	file = fopen(rom_name, "rb");
	
	if(file == NULL){
		fprintf(stderr, "Error loading file\n");
		return 1;
	}
	/* Jump to the end of the file */
	fseek(file, 0, SEEK_END); 
	/* Get the current byte offset in the file */         
	int len = ftell(file);  
	/* Jump back to the beginning of the file */           
	rewind(file);                     

	fprintf(stderr, "size: %d bytes.\n", len);

	if (len > MEM_SIZE - MEM_OFFSET) {
		fprintf(stderr, "Rom is too large or not formatted properly.\n");
		return 1;
	}


	/* Read the entire file starting from 0x200 */
	if (!fread(memory + MEM_OFFSET, len, sizeof(unsigned char), file)) {
		fprintf(stderr, "Error while reading Rom into memory.\n");
		return 1;
	}

	fclose(file);
	return 0;
}


int Chip8::EmulateCycle(){
	
	if(FetchOpcode() || InterpretOpcode() || UpdateTimers() || check_input(&renderer, key)) {
		return 1;
	}

	/* render the scene */
	if (draw_flag) {
		renderer.RenderFrame(vram);
		draw_flag = 0;
	}

	return 0;
}

int Chip8::FetchOpcode() {
	opcode = memory[PC] << 8 | memory[PC + 1];
	return 0;
}

int Chip8::InterpretOpcode(){
	fprintf(stderr, "Processing opcode: 0x%X\n", opcode);

	/* Decode opcodes */
	switch (opcode & 0xF000) {
		case 0x0000:
			switch (opcode & 0x000F) {
				
				case 0x0000: 
					/* 0x00E0: Clears the screen */
					for (int i = 0; i < WIDTH; i++) {
						memset(vram[i], 0, HEIGHT * sizeof(unsigned char));
					}
					draw_flag = 1;
					PC += 2;  
				break;

				case 0x000E: 
					/* 0x00EE: Returns from subroutine */         
					--sp;			// 16 levels of stack, decrease stack pointer to prevent overwrite
					PC = stack[sp];	// Put the stored return address from the stack back into the program counter					
					PC += 2;		// Don't forget to increase the program counter!
				break; 

				default:
					fprintf(stderr, "Uknown opcode [0x0000]: 0x%X\n", opcode);
				break;
			}
		break;

		/* opcode 0x1NNN */
		case 0x1000:
			/* Jumps to address NNN */
			PC = opcode & 0x0FFF;
		break;

		/* opcode 0x2NNN */
		case 0x2000:
			/* Calls subroutine at NNN */
			stack[sp] = PC;
			++sp;
			PC = opcode & 0x0FFF;
		break;

		/* opcode 0x3XNN */
		case 0x3000:
			/* Skips the next instruction if VX equals NN */
			if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
				PC += 2;
			}
			PC += 2;
		break;
		
		/* opcode 0x4XNN */
		case 0x4000:
			/* Skips the next instruction if VX doesn't equal NN */
			if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
				PC += 2;
			}
			PC += 2;
		break;

		/* opcode 0x5XY0 */
		case 0x5000:
			/* Skips the next instruction if VX equals VY */
			if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
				PC += 2;
			}
			PC += 2;
		break;

		/* opcode 0x6XNN */
		case 0x6000:
			/* Sets VX to NN */
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			PC += 2;
		break;

		/* opcode 0x7XNN */
		case 0x7000:
			/* Adds NN to VX */
			V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			PC += 2;
		break;

		/* opcode 0x8XYN */
		case 0x8000:
			switch (opcode & 0x000F) {
				/* opcode 0x8XY0 */
				case 0x0000:
					/* Sets VX to the value of VY */
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
					PC += 2;
				break;

				/* opcode 0x8XY1 */
				case 0x0001:
					/* Sets VX to VX or VY */
					V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
					PC += 2;
				break;

				/* opcode 0x8XY2 */
				case 0x0002:
					/* Sets VX to VX and VY */
					V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
					PC += 2;
				break;

				/* opcode 0x8XY3 */
				case 0x0003:
					/* Sets VX to VX xor VY */
					V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
					PC += 2;
				break;

				/* opcode 0x8XY4 */
				case 0x0004:
					/* Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't */
					if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
						V[0xF] = 1; //carry
					}
					else {
						V[0xF] = 0;
					}

					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4]; 
					PC += 2;
				break;

				/* opcode 0x8XY5 */
				case 0x0005:
					/* VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't */
					if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
						V[0xF] = 0; // there is a borrow
					}
					else {
						V[0xF] = 1;					
					}

					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
					PC += 2;
				break;

				case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
					V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
					V[(opcode & 0x0F00) >> 8] >>= 1;
					PC += 2;
				break;

				case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
					if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {	// VY-VX
						V[0xF] = 0; // there is a borrow
					}
					else {
						V[0xF] = 1;
					}

					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];				
					PC += 2;
				break;

				case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
					V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
					V[(opcode & 0x0F00) >> 8] <<= 1;
					PC += 2;
				break;

				default:
					fprintf (stderr, "Unknown opcode [0x8000]: 0x%X\n", opcode);
				break;
			}
		break;

		/* opcode 0x9XY0 */
		case 0x9000:
			/* Skips the next instruction if VX doesn't equal VY */
			if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
				PC +=2;
			}
			PC += 2;
		break;

		/* opcode ANNN */
		case 0xA000:
			/* Sets I to the address NNN */
			I = opcode & 0x0FFF;
			PC += 2;
		break;

		/* opcode BNNN */
		case 0xB000:
			/* Jumps to the address NNN plus V0 */
			PC = (opcode & 0x0FFF) + V[0];
		break;

		/* opcode CXNN */
		case 0xC000:
			/* Sets VX to the result of a bitwise and operation on a random number and NN */
			V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
			PC += 2;
		break;

		/* opcode DXYN */
		case 0xD000:
			/* Draws a sprite at coordinate (VX, VY) that has a width of 8 vram and a height of N vram. 
			Each row of 8 vram is read as bit-coded starting from memory location I; 
			I value doesn’t change after the execution of this instruction. As described above, 
			VF is set to 1 if any screen vram are flipped from set to unset when the sprite is drawn, 
			and to 0 if that doesn’t happen */
		{
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;

			V[0xF] = 0;
			for (int yline = 0; yline < height; yline++) {
				
				pixel = memory[I + yline];
				
				for(int xline = 0; xline < 8; xline++) {

					if((pixel & (0x80 >> xline)) != 0) {
						
						int true_x = (x + xline) % WIDTH;
						int true_y = (y + yline) % HEIGHT;
						
						if(vram[true_x][true_y] == 1) {
								fprintf(stderr, "COLLISION!\n");
								V[0xF] = 1;                                    
						}
						
						vram[true_x][true_y] ^= 1;
					}
				}
			}
						
			draw_flag = true;			
			PC += 2;
		}
		break;

		case 0xE000:
			switch(opcode & 0x00FF) {
				case 0x009E:
					/* EX9E:	Skips the next instruction if the key stored in VX is pressed */
					if(key[V[(opcode & 0x0F00) >> 8]] != 0) {
						PC += 2;
					}
					PC += 2;
				break;

				case 0x00A1:
					/* EXA1:	Skips the next instruction if the key stored in VX isn't pressed */
					if(key[V[(opcode & 0x0F00) >> 8]] == 0) {
						PC += 2;
					}
					
					PC += 2;
				break;

				default:
					fprintf (stderr, "Unknown opcode [0xE000]: 0x%X\n", opcode);
				break;
			}
		break;

		case 0xF000:
			switch (opcode & 0x00FF) {
				case 0x0007:
					V[(opcode & 0x0F00) >> 8] = delay_timer;
					PC += 2;
				break;

				case 0x000A:
				{
					bool keyPress = false;

					for(int i = 0; i < 16; ++i)
					{
						if(key[i] != 0)
						{
							V[(opcode & 0x0F00) >> 8] = i;
							keyPress = true;
						}
					}

					/* If we didn't received a keypress, skip this cycle and try again */
					if(!keyPress)						
						return 0;

					PC += 2;					
				}
				break;

				case 0x0015: // FX15: Sets the delay timer to VX
					delay_timer = V[(opcode & 0x0F00) >> 8];
					PC += 2;
				break;

				case 0x0018: // FX18: Sets the sound timer to VX
					sound_timer = V[(opcode & 0x0F00) >> 8];
					PC += 2;
				break;

				case 0x001E: // FX1E: Adds VX to I
					if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
						V[0xF] = 1;
					else
						V[0xF] = 0;
					I += V[(opcode & 0x0F00) >> 8];
					PC += 2;
				break;

				case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
					I = V[(opcode & 0x0F00) >> 8] * 0x5;
					PC += 2;
				break;

				case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
					memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
					memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
					memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;					
					PC += 2;
				break;

				case 0x0055: // FX55: Stores V0 to VX in memory starting at address I					
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
						memory[I + i] = V[i];	
					}

					// On the original interpreter, when the operation is done, I = I + X + 1.
					I += ((opcode & 0x0F00) >> 8) + 1;
					PC += 2;
				break;

				case 0x0065: // FX65: Fills V0 to VX with values from memory starting at address I					
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
						V[i] = memory[I + i];			

					// On the original interpreter, when the operation is done, I = I + X + 1.
					I += ((opcode & 0x0F00) >> 8) + 1;
					PC += 2;
				break;

				default:
					fprintf (stderr, "Unknown opcode [0xF000]: 0x%X\n", opcode);
				break;
			}
		break;

	}
	return 0;
}

int Chip8::UpdateTimers(){
	/* Update timers */
	//printf ("delay: %d, sound: %d\n", delay_timer, sound_timer);
  	if(delay_timer > 0) {
    	--delay_timer;
	}
 
 	if(sound_timer > 0) {
    	if(sound_timer == 1) {
      		fprintf(stderr, "BEEP!\n");
    	}
    	--sound_timer;
	}

	return 0;
}

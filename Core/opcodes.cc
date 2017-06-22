#include "Chip8.h"
#include <stdio.h>
#include <stdlib.h> /* rand() */

/* Decode the instruction */
#define OP ((opcode & 0xF000) >> 12)
#define OP_NNN (opcode & 0x0FFF)
#define OP_NN (opcode & 0x00FF)
#define OP_N (opcode & 0x000F)
#define OP_X ((opcode & 0x0F00) >> 8)
#define OP_Y ((opcode & 0x00F0) >> 4)

inline void Chip8::exec00E0() {
    /* 0x00E0: clears the screen */
    for (int i = 0; i < WIDTH; i++) {
        memset(vram[i], 0, HEIGHT * sizeof(unsigned char));
    }
    draw_flag = 1;
    PC += 2;  
}

inline void Chip8::exec00EE() {
    /* 0x00EE: returns from subroutine */         
    sp--; 
    PC = stack[sp];
    PC += 2;
}

inline void Chip8::exec0NNN() {
    /* 0x0NNN: SYS addr - jump to a machine code routine at nnn. 
    This instruction is only used on the old computers on which 
    Chip-8 was originally implemented. It is ignored by modern 
    interpreters. */
    PC += 2;
}

inline void Chip8::exec1NNN() {
    /* 0x1NNN: jumps to address NNN */
    PC = OP_NNN;
}

inline void Chip8::exec2NNN() {
    /* 0x2NNN: calls subroutine at NNN */
    stack[sp] = PC;
    ++sp;
    PC = OP_NNN;
}

inline void Chip8::exec3XNN() {
    /* 0x3XNN: skips the next instruction if VX equals NN */
    if (V[OP_X] == OP_NN) PC += 2;
    PC += 2;
}

inline void Chip8::exec4XNN() {
    /* 0x4XNN: skips the next instruction if VX doesn't equal NN */
    if (V[OP_X] != OP_NN) PC += 2;
    PC += 2;
}

inline void Chip8::exec5XY0() {
    /* 0x5XY0: skips the next instruction if VX equals VY */
    if (V[OP_X] == V[OP_Y]) PC += 2;
    PC += 2;
}

inline void Chip8::exec6XNN() {
    /* 0x6XNN: sets VX to NN */
    V[OP_X] = OP_NN;
    PC += 2;
}

inline void Chip8::exec7XNN() {
    /* 0x7XNN: adds NN to VX */
    V[OP_X] += OP_NN;
    PC += 2;
}

inline void Chip8::exec8XY0() {
    /* 0x8XY0: sets VX to the value of VY */
    V[OP_X] = V[OP_Y];
    PC += 2;
}

inline void Chip8::exec8XY1() {
    /* 0x8XY1: sets VX to VX or VY */
    V[OP_X] |= V[OP_Y];
    PC += 2;
}

inline void Chip8::exec8XY2() {
    /* 0x8XY2: sets VX to VX and VY */
    V[OP_X] &= V[OP_Y];
    PC += 2;
}

inline void Chip8::exec8XY3() {
    /* 0x8XY3: sets VX to VX xor VY */
    V[OP_X] ^= V[OP_Y];
    PC += 2;
}

inline void Chip8::exec8XY4() {
    /* 0x8XY4: adds VY to VX. VF is set to 1 when there's a 
       carry, and to 0 when there isn't */
    unsigned short sum;
    sum  = V[OP_Y] + V[OP_X];
    (sum > 0xFF) ? V[0xF] = 1 : V[0xF] = 0;

    /* only the lowest 8 bits are kept */
    V[OP_X] = (unsigned char) sum; 
    PC += 2;
}

inline void Chip8::exec8XY5() {
    /* 0x8XY5: VY is subtracted from VX. VF is set to 0 when 
       there's a borrow, and 1 when there isn't */
    (V[OP_Y] > V[OP_X]) ? V[0xF] = 0 : V[0xF] = 1;
    V[OP_X] -= V[OP_Y];
    PC += 2;
}

inline void Chip8::exec8XY6() {
    /*  0x8XY6: shifts VX right by one. VF is set to the value 
        of the least significant bit of VX before the shift. */
    V[0xF] = V[OP_X] & 0x01;
    shift_quirk? V[OP_X] >>= 1 : V[OP_X] = V[OP_Y] >> 1;
    PC += 2;
}
inline void Chip8::exec8XY7() {
    /* 0x8XY7: sets VX to VY minus VX. VF is set to 0 when 
       there's a borrow, and 1 when there isn't. */
    (V[OP_X] > V[OP_Y]) ? V[0xF] = 0 : V[0xF] = 1;
    V[OP_X] = V[OP_Y] - V[OP_X];              
    PC += 2;
}
inline void Chip8::exec8XYE() {
    /* 0x8XYE: shifts VX left by one. VF is set to the value of 
       the most significant bit of VX before the shift. */
    V[0xF] = (V[OP_X] & 0x80) >> 7;
    shift_quirk ? V[OP_X] <<= 1 : V[OP_X] = V[OP_Y] << 1;
    PC += 2;
}

inline void Chip8::exec9XY0() {
    /* 0x9XY0: skips the next instruction if VX doesn't equal VY */
    if (V[OP_X] != V[OP_Y]) PC +=2;
    PC += 2;
}
inline void Chip8::execANNN() {
    /* ANNN: sets I to the address NNN */
    I = OP_NNN;
    PC += 2;
}
inline void Chip8::execBNNN() {
    /* BNNN: jumps to the address NNN plus V0 */
    PC = OP_NNN + V[0];
}
inline void Chip8::execCXNN() {
    /* CXNN: sets VX to the result of a bitwise 
       and operation on a random number and NN */
    V[OP_X] = (rand() % 0xFF) & OP_NN;
    PC += 2;
}
inline void Chip8::execDXYN() {
    /* DXYN: draws a sprite at coordinate (VX, VY) that has a width of 8 
       vram and a height of N vram. Each row of 8 vram is read as bit-coded 
       starting from memory location I; I value doesn’t change after the 
       execution of this instruction. As described above, VF is set to 1 if 
       any screen vram are flipped from set to unset when the sprite is 
       drawn, and to 0 if that doesn’t happen */
    unsigned short x = V[OP_X];
    unsigned short y = V[OP_Y];
    unsigned short height = OP_N;
    unsigned short pixel;
    V[0xF] = 0;

    for (unsigned char yline = 0; yline < height; yline++) {
        pixel = memory[I + yline];
        for(unsigned char xline = 0; xline < 8; xline++) {
            if((pixel & (0x80 >> xline)) != 0) {  
                
                /* note: Blitz - David Winter
                   has sprites with one too many vertical pixel so it ends up 
                   wrapping to the top of the screen if you (y % HEIGHT) */
                unsigned char true_x = (x + xline) % WIDTH;
                unsigned char true_y = (y + yline);
                if(vwrap) true_y = true_y % HEIGHT;

                /* OOB check is needed when vwrap is turned off
                   so some poorly written games won't crash */
                if (true_x < WIDTH && true_y < HEIGHT) {

                    /* collision */
                    if(vram[true_x][true_y] == 1) V[0xF] = 1;                          
                    
                    /* toggle the pixel */
                    vram[true_x][true_y] ^= 1;
                } 
            }
        }
    }          
    draw_flag = 1;  
    PC += 2;
}

inline void Chip8::execEX9E() {
    /* EX9E: skips the next instruction if the key stored in VX is pressed */
    if(input.keys[V[OP_X]] == 1) PC += 2;
    PC += 2;
}

inline void Chip8::execEXA1() {
    /* EXA1: skips the next instruction if the key stored in VX isn't pressed */
    if(input.keys[V[OP_X]] == 0) PC += 2;
    PC += 2;                
}

inline void Chip8::execFX07() {
    /* FX07: sets VX to delay timer */
    V[OP_X] = delay_timer;
    PC += 2;
}
inline void Chip8::execFX0A() {
    /* FX0A: pause execution until a key is pressed and store result in V[X] */
    if (cpu_halt) {
        if (!input.awaiting_key_press) {
            for (int i = 0; i < NUM_KEYS; i++) {
                if (input.keys[i] != 0) V[OP_X] = i;
            }
            cpu_halt = 0;
            PC += 2;
            return;
        }
    }
    cpu_halt = 1;
    input.awaiting_key_press = 1;
}

inline void Chip8::execFX15() {
    /* FX15: sets the delay timer to VX */
    delay_timer = V[OP_X];
    PC += 2;
}

inline void Chip8::execFX18() {
    /* FX18: sets the sound timer to VX */
    sound_timer = V[OP_X];
    PC += 2;
}

inline void Chip8::execFX1E() {
    /* FX1E: adds VX to I
       VF is set to 1 when range overflow (I+VX>0xFFF), 
       and 0 when there isn't. */
    unsigned short sum;
    sum = I + V[OP_X];
    if (sum > 0xFFF) V[0xF] = 1;
    else V[0xF] = 0;
    I += V[OP_X];
    PC += 2;
}

inline void Chip8::execFX29() {
/* FX29: sets I to the location of the sprite for the character in VX. 
   Characters 0-F (in hexadecimal) are represented by a 4x5 font. */
    I = V[OP_X] * 0x05;
    PC += 2;
}

inline void Chip8::execFX33() {
    /* FX33: stores the binary-coded decimal representation of VX 
    at the addresses I, I plus 1, and I plus 2 */
    memory[I] = V[OP_X] / 100;
    memory[I + 1] = (V[OP_X] / 10) % 10;
    memory[I + 2] = (V[OP_X] % 100) % 10;                 
    PC += 2;
}

inline void Chip8::execFX55() {
    /* FX55: stores V0 to VX in memory starting at address I */                
    for (int i = 0; i <= OP_X; i++) {
        memory[I + i] = V[i];   
    }

    /* on the original interpreter, when the operation is done, 
       I = I + X + 1. */
    if (!load_store_quirk) I += OP_X + 1;
    PC += 2;
}

inline void Chip8::execFX65() {
    /* FX65: fills V0 to VX with values from memory starting at address I */              
    for (int i = 0; i <= OP_X; i++) {
        V[i] = memory[I + i];           
    }

    /* on the original interpreter, when the operation is done, 
       I = I + X + 1. */
    if (!load_store_quirk) I += OP_X + 1;
    PC += 2;
}

inline void Chip8::execUnknown() {
    fprintf (stderr, "Unknown opcode: 0x%X\n", opcode);
    PC+=2;
}

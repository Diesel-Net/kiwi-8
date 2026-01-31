#include "chip8.h"
#include <stdio.h>
#include <stdlib.h> /* rand() */

/* Decode the instruction */
#define OP ((chip8.opcode & 0xF000) >> 12)
#define OP_NNN (chip8.opcode & 0x0FFF)
#define OP_NN (chip8.opcode & 0x00FF)
#define OP_N (chip8.opcode & 0x000F)
#define OP_X ((chip8.opcode & 0x0F00) >> 8)
#define OP_Y ((chip8.opcode & 0x00F0) >> 4)

inline void exec00E0() {
    /* 0x00E0: clears the screen */
    for (int i = 0; i < WIDTH; i++) {
        memset(chip8.vram[i], 0, HEIGHT * sizeof(unsigned char));
    }
    chip8.draw_flag = 1;
    chip8.PC += 2;
}

inline void exec00EE() {
    /* 0x00EE: returns from subroutine */
    chip8.sp--;
    chip8.PC = chip8.stack[chip8.sp];
    chip8.PC += 2;
}

inline void exec0NNN() {
    /* 0x0NNN: SYS addr - jump to a machine code routine at nnn.
    This instruction is only used on the old computers on which
    Chip-8 was originally implemented. It is ignored by modern
    interpreters. */
    chip8.PC += 2;
}

inline void exec1NNN() {
    /* 0x1NNN: jumps to address NNN */
    chip8.PC = OP_NNN;
}

inline void exec2NNN() {
    /* 0x2NNN: calls subroutine at NNN */
    chip8.stack[chip8.sp] = chip8.PC;
    chip8.sp++;
    chip8.PC = OP_NNN;
}

inline void exec3XNN() {
    /* 0x3XNN: skips the next instruction if VX equals NN */
    if (chip8.V[OP_X] == OP_NN) chip8.PC += 2;
    chip8.PC += 2;
}

inline void exec4XNN() {
    /* 0x4XNN: skips the next instruction if VX doesn't equal NN */
    if (chip8.V[OP_X] != OP_NN) chip8.PC += 2;
    chip8.PC += 2;
}

inline void exec5XY0() {
    /* 0x5XY0: skips the next instruction if VX equals VY */
    if (chip8.V[OP_X] == chip8.V[OP_Y]) chip8.PC += 2;
    chip8.PC += 2;
}

inline void exec6XNN() {
    /* 0x6XNN: sets VX to NN */
    chip8.V[OP_X] = OP_NN;
    chip8.PC += 2;
}

inline void exec7XNN() {
    /* 0x7XNN: adds NN to VX */
    chip8.V[OP_X] += OP_NN;
    chip8.PC += 2;
}

inline void exec8XY0() {
    /* 0x8XY0: sets VX to the value of VY */
    chip8.V[OP_X] = chip8.V[OP_Y];
    chip8.PC += 2;
}

inline void exec8XY1() {
    /* 0x8XY1: sets VX to VX or VY */
    chip8.V[OP_X] |= chip8.V[OP_Y];
    if (chip8.quirks.logic_vf_quirk) chip8.V[0xF] = 0;
    chip8.PC += 2;
}

inline void exec8XY2() {
    /* 0x8XY2: sets VX to VX and VY */
    chip8.V[OP_X] &= chip8.V[OP_Y];
    if (chip8.quirks.logic_vf_quirk) chip8.V[0xF] = 0;
    chip8.PC += 2;
}

inline void exec8XY3() {
    /* 0x8XY3: sets VX to VX xor VY */
    chip8.V[OP_X] ^= chip8.V[OP_Y];
    if (chip8.quirks.logic_vf_quirk) chip8.V[0xF] = 0;
    chip8.PC += 2;
}

inline void exec8XY4() {
    /* 0x8XY4: adds VY to VX. VF is set to 1 when there's a
       carry, and to 0 when there isn't */
    unsigned short sum;
    sum  = chip8.V[OP_Y] + chip8.V[OP_X];
    (sum > 0xFF) ? chip8.V[0xF] = 1 : chip8.V[0xF] = 0;

    /* only the lowest 8 bits are kept */
    chip8.V[OP_X] = (unsigned char) sum;
    chip8.PC += 2;
}

inline void exec8XY5() {
    /* 0x8XY5: VY is subtracted from VX. VF is set to 0 when
       there's a borrow, and 1 when there isn't */
    (chip8.V[OP_Y] > chip8.V[OP_X]) ? chip8.V[0xF] = 0 : chip8.V[0xF] = 1;
    chip8.V[OP_X] -= chip8.V[OP_Y];
    chip8.PC += 2;
}

inline void exec8XY6() {
    /*  0x8XY6: shifts VX right by one. VF is set to the value
        of the least significant bit of VX before the shift. */
    chip8.V[0xF] = chip8.V[OP_X] & 0x01;
    chip8.quirks.shift_quirk ? chip8.V[OP_X] >>= 1 : chip8.V[OP_X] = chip8.V[OP_Y] >> 1;
    chip8.PC += 2;
}
inline void exec8XY7() {
    /* 0x8XY7: sets VX to VY minus VX. VF is set to 0 when
       there's a borrow, and 1 when there isn't. */
    (chip8.V[OP_X] > chip8.V[OP_Y]) ? chip8.V[0xF] = 0 : chip8.V[0xF] = 1;
    chip8.V[OP_X] = chip8.V[OP_Y] - chip8.V[OP_X];
    chip8.PC += 2;
}
inline void exec8XYE() {
    /* 0x8XYE: shifts VX left by one. VF is set to the value of
       the most significant bit of VX before the shift. */
    chip8.V[0xF] = (chip8.V[OP_X] & 0x80) >> 7;
    chip8.quirks.shift_quirk ? chip8.V[OP_X] <<= 1 : chip8.V[OP_X] = chip8.V[OP_Y] << 1;
    chip8.PC += 2;
}

inline void exec9XY0() {
    /* 0x9XY0: skips the next instruction if VX doesn't equal VY */
    if (chip8.V[OP_X] != chip8.V[OP_Y]) chip8.PC +=2;
    chip8.PC += 2;
}
inline void execANNN() {
    /* ANNN: sets I to the address NNN */
    chip8.I = OP_NNN;
    chip8.PC += 2;
}
inline void execBNNN() {
    /* BNNN: jumps to the address NNN plus V0 or VX (quirk) */
    chip8.PC = OP_NNN + (chip8.quirks.jump_quirk ? chip8.V[OP_X] : chip8.V[0]);
}
inline void execCXNN() {
    /* CXNN: sets VX to the result of a bitwise
       and operation on a random number and NN */
    chip8.V[OP_X] = (rand() % 0xFF) & OP_NN;
    chip8.PC += 2;
}
inline void execDXYN() {
    /* DXYN: draws a sprite at coordinate (VX, VY) that has a width of 8
       vram and a height of N vram. Each row of 8 vram is read as bit-coded
       starting from memory location I; I value doesn’t change after the
       execution of this instruction. As described above, VF is set to 1 if
       any screen vram are flipped from set to unset when the sprite is
       drawn, and to 0 if that doesn’t happen */
    unsigned short x = chip8.V[OP_X];
    unsigned short y = chip8.V[OP_Y];
    unsigned short height = OP_N;
    unsigned short pixel;
    chip8.V[0xF] = 0;

    for (unsigned char yline = 0; yline < height; yline++) {
        pixel = chip8.memory[chip8.I + yline];
        for(unsigned char xline = 0; xline < 8; xline++) {
            if((pixel & (0x80 >> xline)) != 0) {
                unsigned char true_x = chip8.quirks.hwrap ? (x + xline) % WIDTH : (x + xline);
                unsigned char true_y = chip8.quirks.vwrap ? (y + yline) % HEIGHT : (y + yline);
                if ((chip8.quirks.hwrap || true_x < WIDTH) && (chip8.quirks.vwrap || true_y < HEIGHT)) {
                    if(chip8.vram[true_x][true_y] == 1) chip8.V[0xF] = 1;
                    chip8.vram[true_x][true_y] ^= 1;
                }
            }
        }
    }
    chip8.draw_flag = 1;
    chip8.PC += 2;
}

inline void execEX9E() {
    /* EX9E: skips the next instruction if the key stored in VX is pressed */
    if(input.keys[chip8.V[OP_X]] == 1) chip8.PC += 2;
    chip8.PC += 2;
}

inline void execEXA1() {
    /* EXA1: skips the next instruction if the key stored in VX isn't pressed */
    if(input.keys[chip8.V[OP_X]] == 0) chip8.PC += 2;
    chip8.PC += 2;
}

inline void execFX07() {
    /* FX07: sets VX to delay timer */
    chip8.V[OP_X] = chip8.delay_timer;
    chip8.PC += 2;
}
inline void execFX0A() {
    /* FX0A: pause execution until a key is pressed and store result in V[X] */
    if (chip8.cpu_halt) {
        if (!input.awaiting_key_press) {
            for (int i = 0; i < NUM_KEYS; i++) {
                if (input.keys[i] != 0) chip8.V[OP_X] = i;
            }
            chip8.cpu_halt = 0;
            chip8.PC += 2;
            return;
        }
    }
    chip8.cpu_halt = 1;
    input.awaiting_key_press = 1;
}

inline void execFX15() {
    /* FX15: sets the delay timer to VX */
    chip8.delay_timer = chip8.V[OP_X];
    chip8.PC += 2;
}

inline void execFX18() {
    /* FX18: sets the sound timer to VX */
    chip8.sound_timer = chip8.V[OP_X];
    chip8.PC += 2;
}

inline void execFX1E() {
    /* FX1E: adds VX to I
       VF is set to 1 when range overflow (I+VX>0xFFF),
       and 0 when there isn't. */
    ///unsigned short sum;
    //sum = chip8.I + chip8.V[OP_X];

    // TODO: Add quirk toggle for this behavior
    // Commented out to fix compatibility issues with: AnimalRace
    //if (sum > 0xFFF) chip8.V[0xF] = 1;
    //else chip8.V[0xF] = 0;

    if (chip8.quirks.i_overflow_quirk) {
        unsigned short sum = chip8.I + chip8.V[OP_X];
        chip8.I = sum;
        chip8.V[0xF] = (sum > 0xFFF) ? 1 : 0;
    } else {
        chip8.I += chip8.V[OP_X];
    }
    chip8.PC += 2;
}

inline void execFX29() {
    /* FX29: sets I to the location of the sprite for the character in VX.
       Characters 0-F (in hexadecimal) are represented by a 4x5 font. */
    chip8.I = chip8.V[OP_X] * 0x05;
    chip8.PC += 2;
}

inline void execFX33() {
    /* FX33: stores the binary-coded decimal representation of VX
    at the addresses I, I plus 1, and I plus 2 */
    chip8.memory[chip8.I] = chip8.V[OP_X] / 100;
    chip8.memory[chip8.I + 1] = (chip8.V[OP_X] / 10) % 10;
    chip8.memory[chip8.I + 2] = (chip8.V[OP_X] % 100) % 10;
    chip8.PC += 2;
}

inline void execFX55() {
    /* FX55: stores V0 to VX in memory starting at address I */
    for (int i = 0; i <= OP_X; i++) {
        chip8.memory[chip8.I + i] = chip8.V[i];
    }

    /* on the original interpreter, when the operation is done,
       I = I + X + 1. */
    if (!chip8.quirks.load_store_quirk) chip8.I += OP_X + 1;
    chip8.PC += 2;
}

inline void execFX65() {
    /* FX65: fills V0 to VX with values from memory starting at address I */
    for (int i = 0; i <= OP_X; i++) {
        chip8.V[i] = chip8.memory[chip8.I + i];
    }

    /* on the original interpreter, when the operation is done,
       I = I + X + 1. */
    if (!chip8.quirks.load_store_quirk) chip8.I += OP_X + 1;
    chip8.PC += 2;
}

inline void execUnknown() {
    fprintf (stderr, "Unknown opcode: 0x%X\n", chip8.opcode);
    chip8.PC+=2;
}

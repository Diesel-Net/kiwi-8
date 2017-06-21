/*
Author: Thomas Daley
Date: September 18, 2016
*/
#include "Chip8.h"
#include "open_file_dialog.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Chip8::Chip8() {
    cycles = CYCLES_PER_STEP;
    paused = 0;
    muted = 0;
    vwrap = 1;
    display = Display();
    input = Input();    
    vram = NULL;
    rom = NULL;
}

Chip8::~Chip8() {
    /* Clean-up */
    if (vram) {
        for (int i = 0; i < WIDTH; i++) {
            free(vram[i]);
        }
        free(vram);
    }
    free(rom);
    SDL_Quit();
}

int Chip8::Initialize(bool fullscreen, 
                      bool load_store_quirk,
                      bool shift_quirk,
                      bool vwrap){

    if (SDL_Init(  SDL_INIT_TIMER 
                 | SDL_INIT_AUDIO 
                 | SDL_INIT_VIDEO 
                 | SDL_INIT_EVENTS)) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    this-> load_store_quirk = load_store_quirk;
    this-> shift_quirk = shift_quirk;
    this-> vwrap = vwrap;

    /* init vram */
    vram = (unsigned char **) malloc(WIDTH * sizeof(unsigned char *));
    const char *err_str = "Unable to allocate memory on the heap.\n";
    if (!vram) {
        fprintf(stderr, "%s", err_str);
        return 1;
    }
    memset(vram, 0, WIDTH * sizeof(unsigned char *));
    for (int i = 0; i < WIDTH; i++) {
        vram[i] = (unsigned char *) malloc(HEIGHT * sizeof(unsigned char));
        if (!vram[i]) {
            fprintf(stderr, "%s", err_str);
            return 1;
        }
        memset(vram[i], 0, HEIGHT * sizeof(unsigned char));
    }

    /* init audio, display, input */
    audio.Initialize();
    if (display.Initialize(fullscreen, 
                           &this->cycles,
                           &this->paused, 
                           &this->load_store_quirk, 
                           &this->shift_quirk, 
                           &this->vwrap,
                           &this->muted )) {
        return 1;
    }
    input.Initialize(&display, &cycles, &cpu_halt, &paused);
    

    /* init registers and memory once */
    memset(V, 0 , NUM_REGISTERS);
    memset(memory, 0, MEM_SIZE);
    memset(stack, 0, STACK_DEPTH);
    I = 0;
    PC = ENTRY_POINT;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    cpu_halt = 0;
    draw_flag = 1;

    /* Load fontset */
    for(int i = 0; i < FONTS_SIZE; ++i) {
        memory[i] = chip8_fontset[i];
    }

    return LoadBootRom();
}

int Chip8::LoadBootRom() {
    free(rom);
    rom_size = BOOTROM_SIZE;
    rom = (unsigned char *)malloc(rom_size);
    if(!rom) {
        fprintf(stderr, "Unable to allocate memory for rom.\n");
        return 1;
    }
    memset(rom, 0 , rom_size);

    /* Save for later (Soft-resets) */
    memcpy(rom, bootrom, rom_size);

    /* Copy the entire rom to memory starting from 0x200 */
    memcpy(memory + ENTRY_POINT, bootrom, BOOTROM_SIZE);

    return 0;
}

int Chip8::Load(const char *rom_name){
    if (rom_name) {

        /* Open the file */
        FILE *file;
        file = fopen(rom_name, "rb");
        if(file == NULL){
            fprintf(stderr, "File not opened.\n");
            return 1;
        }

        /* Get file size */
        fseek(file, 0, SEEK_END);      
        rom_size = ftell(file);            
        rewind(file);                     
        if (rom_size > MEM_SIZE - ENTRY_POINT) {
            fprintf(stderr, "Rom is too large or not formatted properly.\n");
            return 1;
        }

        /* Allocate or free and reallocate as necessary */
        free(rom);
        rom = (unsigned char *)malloc(rom_size);
        if(!rom) {
            fprintf(stderr, "Unable to allocate memory for rom.\n");
            return 1;
        }
        memset(rom, 0 , rom_size);

        /* Save the rom for later (soft-resets) */
        if (!fread(rom, sizeof(unsigned char), rom_size, file)) {
            fprintf(stderr, "Unable to read Rom file after successfully opening.\n");
            return 1;
        }

        SoftReset();
        fclose(file);
        

    } else {

        /* Load ROM from GUI */
        char new_rom_name[PATH_MAX];
        openFileDialog(new_rom_name) ?
            fprintf(stderr, "User aborted the open file dialog.\n") :
            Load(new_rom_name);

        /* Flip GUI toggle */
        display.gui.load_rom_flag = 0;
    }

    return 0;
}

void Chip8::SoftReset() {
    /* Clear the vram */
    for (int i = 0; i < WIDTH; i++) {
        memset(vram[i], 0, HEIGHT * sizeof(unsigned char));
    }

    /* Reset the state of the input keys */
    input.Reset();

    /* Clear registers and the stack */
    memset(V, 0 , NUM_REGISTERS);
    memset(stack, 0, STACK_DEPTH);
    memset(memory, 0, MEM_SIZE);

    /* Load fontset */
    for(int i = 0; i < FONTS_SIZE; ++i) {
        memory[i] = chip8_fontset[i];
    }

    /* Copy the entire rom to memory starting from 0x200 */
    memcpy(memory + ENTRY_POINT, rom, rom_size);

    /* Re-initialize program counter, stack pointer, timers, etc. */
    I = 0;
    PC = ENTRY_POINT;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    cpu_halt = 0;
    draw_flag = 1;

    /* Un-pause (if paused) whenever we Soft-Reset */
    paused = 0;

    /* Flip the GUI bit */
    display.gui.soft_reset_flag = 0;
    
}

void Chip8::Run(){
    int event;
    unsigned int t1;
    unsigned int t2;
    unsigned int elapsed;
    unsigned int remaining;

    /* Slows execution speed (60hz) ~= 16.66 ms intervals 
       This makes it easy to decrement the Chip8 timers 
       60 times a second */
    unsigned int interval = 1000 / TICKS;

    /* Main run-forever loop */
    for (;;) {

        t1 = SDL_GetTicks();

        event = input.Poll();

        /* Do something based on response... */
        if ((event & USER_QUIT) == USER_QUIT) return;
        if ((event & LOAD_ROM) == LOAD_ROM) Load(NULL);
        if ((event & SOFT_RESET) == SOFT_RESET) SoftReset();
        
        if (!paused) {
            /* Emulate a batch of cycles */
            StepCpu(cycles);

            /* Update Audio */
            if (sound_timer > 0 && !muted) audio.Beep(SAMPLES_PER_FRAME);

            /* Check internal timers */
            UpdateTimers();
        }

        /* Draw a frame if we need to */
        if (draw_flag && display.limit_fps_flag) {
            display.RenderFrame(vram);
            draw_flag = 0;
        } else {
            display.RenderFrame(NULL);
        }

        t2 = SDL_GetTicks();
        
        /* Calculate how long to sleep thread based on remaining frame time */
        elapsed = t2 - t1;
        remaining = interval - elapsed;
        if (elapsed < interval) {
            SDL_Delay(remaining);
            elapsed = interval;
        }
    }
}

void Chip8::UpdateTimers(){
    /* Update timers at 60 Hz */
    if (!cpu_halt) {
        if(delay_timer > 0) delay_timer--;
        if(sound_timer > 0) sound_timer--;
    } 
}

void Chip8::StepCpu(int cycles){
    /* Execute a batch of instructions */
    for (int i = 0; i < cycles; i++) {
        FetchOpcode();
        ExecuteOpcode();

        /* Draw */
        if(draw_flag && !display.limit_fps_flag){
            display.RenderFrame(vram);
            draw_flag = 0;
        }
    }
}

void Chip8::FetchOpcode() {
    /* Fetch two bytes while being careful of byte alignment */
    opcode = memory[PC] << 8 | memory[PC + 1];
}

void Chip8::ExecuteOpcode(){
    /* Decode the instruction */
    unsigned char OP = (opcode & 0xF000) >> 12;
    unsigned short NNN = opcode & 0x0FFF;
    unsigned char NN = opcode & 0x00FF;
    unsigned char N = opcode & 0x000F;
    unsigned char X = (opcode & 0x0F00) >> 8;
    unsigned char Y = (opcode & 0x00F0) >> 4;
    const char *unknown = "Unknown opcode: ";

    /* Execute the instruction */
    switch (OP) {
        case 0x0:
            switch (NNN) {
                
                case 0x00E0: 
                    /* 0x00E0: Clears the screen */
                    for (int i = 0; i < WIDTH; i++) {
                        memset(vram[i], 0, HEIGHT * sizeof(unsigned char));
                    }
                    draw_flag = 1;
                    PC += 2;  
                    break;

                case 0x00EE: 
                    /* 0x00EE: Returns from subroutine */         
                    sp--;           // 16 levels of stack, decrease stack pointer to prevent overwrite
                    PC = stack[sp]; // Put the stored return address from the stack back into the program counter                   
                    PC += 2;        // Don't forget to increase the program counter!
                    break; 

                default:
                    /* 0x0NNN: SYS addr - Jump to a machine code routine at nnn. 
                       This instruction is only used on the old computers on which 
                       Chip-8 was originally implemented. It is ignored by modern 
                       interpreters. */
                    PC += 2;
                    break;
            }
            break;

        /* opcode 0x1NNN */
        case 0x1:
            /* Jumps to address NNN */
            PC = NNN;
            break;

        /* opcode 0x2NNN */
        case 0x2:
            /* Calls subroutine at NNN */
            stack[sp] = PC;
            ++sp;
            PC = NNN;
            break;

        /* opcode 0x3XNN */
        case 0x3:
            /* Skips the next instruction if VX equals NN */
            if (V[X] == NN) {
                PC += 2;
            }
            PC += 2;
            break;
        
        /* opcode 0x4XNN */
        case 0x4:
            /* Skips the next instruction if VX doesn't equal NN */
            if (V[X] != NN) {
                PC += 2;
            }
            PC += 2;
            break;

        /* opcode 0x5XY0 */
        case 0x5:
            /* Skips the next instruction if VX equals VY */
            if (V[X] == V[Y]) {
                PC += 2;
            }
            PC += 2;
            break;

        /* opcode 0x6XNN */
        case 0x6:
            /* Sets VX to NN */
            V[X] = NN;
            PC += 2;
            break;

        /* opcode 0x7XNN */
        case 0x7:
            /* Adds NN to VX */
            V[X] += NN;
            PC += 2;
            break;

        /* opcode 0x8XYN */
        case 0x8:
            switch (N) {
                /* opcode 0x8XY0 */
                case 0x0:
                    /* Sets VX to the value of VY */
                    V[X] = V[Y];
                    PC += 2;
                    break;

                /* opcode 0x8XY1 */
                case 0x1:
                    /* Sets VX to VX or VY */
                    V[X] |= V[Y];
                    PC += 2;
                    break;

                /* opcode 0x8XY2 */
                case 0x2:
                    /* Sets VX to VX and VY */
                    V[X] &= V[Y];
                    PC += 2;
                    break;

                /* opcode 0x8XY3 */
                case 0x3:
                    /* Sets VX to VX xor VY */
                    V[X] ^= V[Y];
                    PC += 2;
                    break;

                /* opcode 0x8XY4 */
                case 0x4:

                	unsigned short sum;
                	sum  = V[Y] + V[X];
                    /* Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't */
                    if(sum > 0xFF) {
                        /* Carry */
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }

                    /* Only the lowest 8 bits are kept */
                    V[X] = (unsigned char) sum; 
                    PC += 2;
                    break;

                /* opcode 0x8XY5 */
                case 0x5:
                    /* VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't */
                    if(V[Y] > V[X]) {
                        /* there is a borrow */
                        V[0xF] = 0; 
                    }
                    else {
                        V[0xF] = 1;                 
                    }
                    V[X] -= V[Y];
                    PC += 2;
                    break;

                /* opcode 0x8XY6 */
                case 0x6: /* Shifts VX right by one. VF is set to the value of the least significant 
                            bit of VX before the shift. */
                    V[0xF] = V[X] & 0x01;
                    if(shift_quirk) {
                        V[X] >>= 1;
                    } else {
                        V[X] = V[Y] >> 1;
                    } 
                    PC += 2;
                    break;

                /* opcode 0x8XY7 */
                case 0x7: /* Sets VX to VY minus VX. VF is set to 0 when there's a borrow, 
                            and 1 when there isn't. */
                    if (V[X] > V[Y]) {
                        V[0xF] = 0; 
                    }
                    else {
                        V[0xF] = 1;
                    }
                    V[X] = V[Y] - V[X];              
                    PC += 2;
                    break;

                /* opcode 0x8XYE */
                case 0xE: /* Shifts VX left by one. VF is set to the value of the most significant 
                            bit of VX before the shift. */
                    V[0xF] = (V[X] & 0x80) >> 7;
                    if(shift_quirk) {
                        V[X] <<= 1;
                    } else {
                        V[X] = V[Y] << 1;
                    }
                    
                    PC += 2;
                    break;

                default:
                    fprintf (stderr, "%s0x%X\n", unknown, opcode);
                    PC+=2;
                    break;
            }
            break;

        /* opcode 0x9XY0 */
        case 0x9:
            /* Skips the next instruction if VX doesn't equal VY */
            if (V[X] != V[Y]) {
                PC +=2;
            }
            PC += 2;
            break;

        /* opcode ANNN */
        case 0xA:
            /* Sets I to the address NNN */
            I = NNN;
            PC += 2;
            break;

        /* opcode BNNN */
        case 0xB:
            /* Jumps to the address NNN plus V0 */
            PC = NNN + V[0];
            break;

        /* opcode CXNN */
        case 0xC:
            /* Sets VX to the result of a bitwise and operation on a random number and NN */
            V[X] = (rand() % 0xFF) & NN;
            PC += 2;
            break;

        /* opcode DXYN */
        case 0xD: {
            /* Draws a sprite at coordinate (VX, VY) that has a width of 8 vram and a height of N vram. 
            Each row of 8 vram is read as bit-coded starting from memory location I; 
            I value doesn’t change after the execution of this instruction. As described above, 
            VF is set to 1 if any screen vram are flipped from set to unset when the sprite is drawn, 
            and to 0 if that doesn’t happen */
            unsigned short x = V[X];
            unsigned short y = V[Y];
            unsigned short height = N;
            unsigned short pixel;

            V[0xF] = 0;
            for (unsigned char yline = 0; yline < height; yline++) {
                pixel = memory[I + yline];
                for(unsigned char xline = 0; xline < 8; xline++) {
                    if((pixel & (0x80 >> xline)) != 0) {  
                        
                        /* Note: The ROM: Blitz - David Winter
                        has sprites with one too many vertical pixel so it ends up 
                        wrapping to the top of the screen if you (y % HEIGHT) */
                        unsigned char true_x = (x + xline) % WIDTH;
                        unsigned char true_y = (y + yline);
                        if(vwrap) true_y = true_y % HEIGHT;
                        

                        /* This OOB check is needed when vwrap is turned off
                           so some poorly written games won't crash */
                        if (true_x < WIDTH && true_y < HEIGHT) {

                            if(vram[true_x][true_y] == 1) {
                                /* Collision */
                                V[0xF] = 1;                          
                            }
                            /* Toggle the pixels */
                            vram[true_x][true_y] ^= 1;
                        }
                        
                    }
                }
            }          
            draw_flag = 1;  
            PC += 2;
            break;
        }

        case 0xE:
            switch(NN) {
                
                case 0x9E:
                    /* EX9E:    Skips the next instruction if the key stored in VX is pressed */
                    if(input.keys[V[X]] == 1) {
                        PC += 2;
                    }
                    PC += 2;
                    break;

                case 0xA1:
                    /* EXA1:    Skips the next instruction if the key stored in VX isn't pressed */
                    if(input.keys[V[X]] == 0) {
                        PC += 2;
                    }
                    PC += 2;
                    break;

                default:
                    fprintf (stderr, "%s0x%X\n", unknown, opcode);
                    PC+=2;
                    break;
            }
            break;

        case 0xF:
            switch (NN) {
                
                case 0x07: /* FX07: Sets VX to delay timer */
                    V[X] = delay_timer;
                    PC += 2;
                    break;

                case 0x0A: {
                    /* FX0A - Pause execution until a key is pressed and store result in V[X] */

                    if (cpu_halt) {
                        if (!input.awaiting_key_press) {
                            for (int i = 0; i < NUM_KEYS; i++) {
                                if (input.keys[i] != 0) V[X] = i;
                            }
                            //input.awaiting_key_press = 0;
                            cpu_halt = 0;
                            PC += 2;
                            break;
                        }
                    }

                    cpu_halt = 1;
                    input.awaiting_key_press = 1;
                    break;
                    
                }                   

                case 0x15: /* FX15: Sets the delay timer to VX */
                    delay_timer = V[X];
                    PC += 2;
                    break;

                case 0x18: /* FX18: Sets the sound timer to VX */
                    sound_timer = V[X];
                    PC += 2;
                    break;

                case 0x1E: /* FX1E: Adds VX to I */
                    /* VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't. */
                    unsigned short sum;
                    sum = I + V[X];
                    if (sum > 0xFFF) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    I += V[X];
                    PC += 2;
                    break;

                case 0x29: /* FX29: Sets I to the location of the sprite for the character in VX. 
                            Characters 0-F (in hexadecimal) are represented by a 4x5 font. */
                    I = V[X] * 0x05;
                    PC += 2;
                    break;

                case 0x33: /* FX33: Stores the Binary-coded decimal representation of VX at the 
                            addresses I, I plus 1, and I plus 2 */
                    memory[I] = V[X] / 100;
                    memory[I + 1] = (V[X] / 10) % 10;
                    memory[I + 2] = (V[X] % 100) % 10;                 
                    PC += 2;
                    break;

                case 0x55: /* FX55: Stores V0 to VX in memory starting at address I */                
                    for (int i = 0; i <= X; i++) {
                        memory[I + i] = V[i];   
                    }
                    /* On the original interpreter, when the operation is done, I = I + X + 1. */
                    if (!load_store_quirk) {
                        I += X + 1;
                    }
                    PC += 2;
                    break;

                case 0x65: /* FX65: Fills V0 to VX with values from memory starting at address I. */              
                    for (int i = 0; i <= X; i++) {
                        V[i] = memory[I + i];           
                    }
                    /* On the original interpreter, when the operation is done, I = I + X + 1. */
                    if (!load_store_quirk) {
                        I += X + 1;
                    }
                    PC += 2;
                    break;

                default:
                    fprintf (stderr, "%s0x%X\n", unknown, opcode);
                    PC += 2;
                    break;
            }
            break;
    }
}

/*
Author: Thomas Daley
Date: September 18, 2016
*/
#include "Chip8.h"
#include "opcodes.cc"
#include "open_file_dialog.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

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
    /* clean-up */
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
                      bool vwrap,
                      bool muted){

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
    this->muted = muted;

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

    /* load fontset */
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

    /* save for later (soft-resets) */
    memcpy(rom, bootrom, rom_size);

    /* copy the entire rom to memory starting from 0x200 */
    memcpy(memory + ENTRY_POINT, bootrom, BOOTROM_SIZE);

    return 0;
}

int Chip8::Load(const char *rom_name){
    if (rom_name) {

        /* open the file */
        FILE *file;
        file = fopen(rom_name, "rb");
        if(file == NULL){
            fprintf(stderr, "File not opened.\n");
            return 1;
        }

        /* get file size */
        fseek(file, 0, SEEK_END);      
        rom_size = ftell(file);            
        rewind(file);                     
        if (rom_size > MEM_SIZE - ENTRY_POINT) {
            fprintf(stderr, "Rom is too large or not formatted properly.\n");
            return 1;
        }

        /* allocate or free and reallocate as necessary */
        free(rom);
        rom = (unsigned char *)malloc(rom_size);
        if(!rom) {
            fprintf(stderr, "Unable to allocate memory for rom.\n");
            return 1;
        }
        memset(rom, 0 , rom_size);

        /* save the rom for later (soft-resets) */
        if (!fread(rom, sizeof(unsigned char), rom_size, file)) {
            fprintf(stderr, "Unable to read Rom file after successfully opening.\n");
            return 1;
        }

        SoftReset();
        fclose(file);
        

    } else {

        /* load ROM from GUI */
        char new_rom_name[PATH_MAX];
        openFileDialog(new_rom_name) ?
            fprintf(stderr, "User aborted the open file dialog.\n") :
            Load(new_rom_name);

        /* flip GUI toggle */
        display.gui.load_rom_flag = 0;
    }

    return 0;
}

void Chip8::SoftReset() {
    /* clear the vram */
    for (int i = 0; i < WIDTH; i++) {
        memset(vram[i], 0, HEIGHT * sizeof(unsigned char));
    }

    /* reset the state of the input keys */
    input.Reset();

    /* clear registers and the stack */
    memset(V, 0 , NUM_REGISTERS);
    memset(stack, 0, STACK_DEPTH);
    memset(memory, 0, MEM_SIZE);

    /* load fontset */
    for(int i = 0; i < FONTS_SIZE; ++i) {
        memory[i] = chip8_fontset[i];
    }

    /* copy the entire rom to memory starting from 0x200 */
    memcpy(memory + ENTRY_POINT, rom, rom_size);

    /* re-initialize program counter, stack pointer, timers, etc. */
    I = 0;
    PC = ENTRY_POINT;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    cpu_halt = 0;
    draw_flag = 1;

    /* un-pause (if paused) whenever we Soft-Reset */
    paused = 0;

    /* flip the GUI bit */
    display.gui.soft_reset_flag = 0;
    
}

void Chip8::Run(){
    int event;
    unsigned int t1;
    unsigned int t2;
    unsigned int elapsed;
    unsigned int remaining;

    /* slows execution speed (60hz) ~= 16.66 ms intervals 
       This makes it easy to decrement the Chip8 timers 
       60 times a second */
    unsigned int interval = 1000 / TICKS;

    /* main run-forever loop */
    for (;;) {

        t1 = SDL_GetTicks();

        event = input.Poll();

        /* do something based on response... */
        if ((event & USER_QUIT) == USER_QUIT) return;
        if ((event & LOAD_ROM) == LOAD_ROM) Load(NULL);
        if ((event & SOFT_RESET) == SOFT_RESET) SoftReset();
        
        if (!paused) {
            /* emulate a number of cycles */
            StepCpu(cycles);

            /* update Audio */
            if (sound_timer > 0 && !muted) audio.Beep(SAMPLES_PER_FRAME);

            /* check internal timers */
            UpdateTimers();
        }

        /* draw a frame if we need to */
        if (draw_flag && display.limit_fps_flag) {
            display.RenderFrame(vram);
            draw_flag = 0;
        } else {
            display.RenderFrame(NULL);
        }

        t2 = SDL_GetTicks();
        
        /* calculate how long to sleep thread based on remaining frame time */
        elapsed = t2 - t1;
        remaining = interval - elapsed;
        if (elapsed < interval) {
            SDL_Delay(remaining);
            elapsed = interval;
        }
    }
}

void Chip8::UpdateTimers(){
    /* update timers at 60 Hz */
    if (!cpu_halt) {
        if(delay_timer > 0) delay_timer--;
        if(sound_timer > 0) sound_timer--;
    } 
}

void Chip8::StepCpu(int cycles){
    /* execute a batch of instructions */
    for (int i = 0; i < cycles; i++) {
        FetchOpcode();
        ExecuteOpcode();

        /* draw */
        if(draw_flag && !display.limit_fps_flag){
            display.RenderFrame(vram);
            draw_flag = 0;
        }
    }
}

void Chip8::FetchOpcode() {
    /* fetch two bytes while being careful of byte alignment */
    opcode = memory[PC] << 8 | memory[PC + 1];
}

void Chip8::ExecuteOpcode(){
    switch (OP) {
        case 0x0:
            switch (OP_NNN) { 
                case 0x0E0: exec00E0(); break;
                case 0x0EE: exec00EE(); break;
                default:     exec0NNN(); break;
            } break;
        case 0x1: exec1NNN(); break;
        case 0x2: exec2NNN(); break;
        case 0x3: exec3XNN(); break;
        case 0x4: exec4XNN(); break;
        case 0x5: exec5XY0(); break;
        case 0x6: exec6XNN(); break;
        case 0x7: exec7XNN(); break;
        case 0x8:
            switch (OP_N) {
                case 0x0: exec8XY0(); break;
                case 0x1: exec8XY1(); break;
                case 0x2: exec8XY2(); break;
                case 0x3: exec8XY3(); break;
                case 0x4: exec8XY4(); break;
                case 0x5: exec8XY5(); break;
                case 0x6: exec8XY6(); break;
                case 0x7: exec8XY7(); break;
                case 0xE: exec8XYE(); break;
                default: execUnknown(); break;
            } break;
        case 0x9: exec9XY0(); break;
        case 0xA: execANNN(); break;
        case 0xB: execBNNN(); break;
        case 0xC: execCXNN(); break;
        case 0xD: execDXYN(); break;
        case 0xE:
            switch(OP_NN) {
                case 0x9E: execEX9E(); break;
                case 0xA1: execEXA1(); break;
                default: execUnknown(); break;
                    
            } break;
        case 0xF:
            switch (OP_NN) {
                case 0x07: execFX07(); break;
                case 0x0A: execFX0A(); break;              
                case 0x15: execFX15(); break;
                case 0x18: execFX18(); break;
                case 0x1E: execFX1E(); break;
                case 0x29: execFX29(); break;
                case 0x33: execFX33(); break;
                case 0x55: execFX55(); break;
                case 0x65: execFX65(); break;
                default: execUnknown(); break;
            } break;
    }
}

#include "chip8.h"
#include "opcodes.h"
#include "profiles.h"
#include "crc32.h"
#include "notifications.h"
#include "open_file_dialog.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

/* Global chip8 instance */
struct chip8 chip8;

void chip8_destroy() {
    /* clean-up */
    if (chip8.vram) {
        for (int i = 0; i < WIDTH; i++) {
            free(chip8.vram[i]);
        }
        free(chip8.vram);
    }
    free(chip8.rom);
    display_destroy();
    SDL_Quit();
}

int chip8_init(
    bool fullscreen,
    bool muted
) {
    chip8.cycles = CYCLES_PER_STEP;
    chip8.paused = 0;
    chip8.muted = 0;
    chip8.vram = NULL;
    chip8.rom = NULL;

    if (SDL_Init(
        SDL_INIT_TIMER |
        SDL_INIT_VIDEO |
        SDL_INIT_EVENTS
    )) {
        fprintf(stderr,"Error: %s\n", SDL_GetError());
        return 1;
    }

    /* Initialize with default quirks (profiles will override on ROM load) */
    chip8.quirks = (struct quirks){
        1, // load_store_quirk
        1, // shift_quirk
        0, // jump_quirk
        0, // logic_vf_quirk
        0, // i_overflow_quirk
        0, // draw_flag_quirk
        1, // vwrap
        0  // hwrap
    };
    chip8.muted = muted;

    /* init vram */
    chip8.vram = (unsigned char **) malloc(WIDTH * sizeof(unsigned char *));
    const char *err_str = "Unable to allocate memory on the heap.\n";
    if (!chip8.vram) {
        fprintf(stderr, "%s", err_str);
        return 1;
    }
    memset(chip8.vram, 0, WIDTH * sizeof(unsigned char *));
    for (int i = 0; i < WIDTH; i++) {
        chip8.vram[i] = (unsigned char *) malloc(HEIGHT * sizeof(unsigned char));
        if (!chip8.vram[i]) {
            fprintf(stderr, "%s", err_str);
            return 1;
        }
        memset(chip8.vram[i], 0, HEIGHT * sizeof(unsigned char));
    }

    /* Initialize notification system first (before audio) */
    notify_init();

    audio_init();

    if (display_init(fullscreen)) return 1;

    input_reset();

    /* init registers and memory once */
    memset(chip8.V, 0 , NUM_REGISTERS);
    memset(chip8.memory, 0, MEM_SIZE);
    memset(chip8.stack, 0, STACK_DEPTH);
    chip8.I = 0;
    chip8.PC = ENTRY_POINT;
    chip8.sp = 0;
    chip8.delay_timer = 0;
    chip8.sound_timer = 0;
    chip8.cpu_halt = 0;
    chip8.draw_flag = 1;

    /* load fontset */
    for(int i = 0; i < FONTS_SIZE; ++i) {
        chip8.memory[i] = chip8.chip8_fontset[i];
    }

    /* Initialize ROM profile database */
    profiles_init();

    return chip8_load_bootrom();
}

int chip8_load_bootrom() {
    free(chip8.rom);
    chip8.rom_size = BOOTROM_SIZE;
    chip8.rom = (unsigned char *)malloc(chip8.rom_size);
    if(!chip8.rom) {
        fprintf(stderr, "Unable to allocate memory for rom.\n");
        return 1;
    }
    memset(chip8.rom, 0 , chip8.rom_size);

    /* save for later (soft-resets) */
    memcpy(chip8.rom, bootrom, chip8.rom_size);

    /* copy the entire rom to memory starting from 0x200 */
    memcpy(chip8.memory + ENTRY_POINT, bootrom, BOOTROM_SIZE);

    /* Mark as bootrom (not user ROM) */
    chip8.rom_loaded = 0;
    chip8.rom_filename[0] = '\0';

    return 0;
}

int chip8_load(const char *rom_name) {
    if (rom_name) {
        /* open the file */
        FILE *file;
        file = fopen(rom_name, "rb");
        if(file == NULL){
            notify_show(NOTIFY_ERROR, "Unable to open ROM file");
            return 1;
        }

        /* get file size */
        fseek(file, 0, SEEK_END);
        chip8.rom_size = ftell(file);
        rewind(file);
        if (chip8.rom_size > MEM_SIZE - ENTRY_POINT) {
            notify_show(NOTIFY_ERROR, "ROM is too large or not formatted properly");
            fclose(file);
            return 1;
        }

        /* allocate or free and reallocate as necessary */
        free(chip8.rom);
        chip8.rom = (unsigned char *)malloc(chip8.rom_size);
        if(!chip8.rom) {
            notify_show(NOTIFY_ERROR, "Unable to allocate memory for ROM");
            fclose(file);
            return 1;
        }
        memset(chip8.rom, 0 , chip8.rom_size);

        /* save the rom for later (soft-resets) */
        if (!fread(chip8.rom, sizeof(unsigned char), chip8.rom_size, file)) {
            notify_show(NOTIFY_ERROR, "Unable to read ROM file");
            fclose(file);
            return 1;
        }

        fclose(file);

        /* Extract basename for profile tracking */
        const char *basename = strrchr(rom_name, '/');
        if (!basename) basename = strrchr(rom_name, '\\'); /* Windows */
        basename = basename ? basename + 1 : rom_name;
        strncpy(chip8.rom_filename, basename, sizeof(chip8.rom_filename) - 1);
        chip8.rom_filename[sizeof(chip8.rom_filename) - 1] = '\0';

        /* Mark as user ROM (not bootrom) */
        chip8.rom_loaded = 1;

        /* Compute CRC32 and lookup profile */
        uint32_t crc = crc32_compute(chip8.rom, chip8.rom_size);
        const struct profile *profile = profile_lookup(crc);

        if (profile) {
            /* Apply profile quirks */
            chip8.quirks = profile->quirks;
            char notif_msg[256];
            snprintf(notif_msg, sizeof(notif_msg), "Profile applied: %s", chip8.rom_filename);
            notify_show(NOTIFY_SUCCESS, notif_msg);
        } else {
            char notif_msg[256];
            snprintf(notif_msg, sizeof(notif_msg), "No profile found: %s", chip8.rom_filename);
            notify_show(NOTIFY_INFO, notif_msg);
        }

        chip8_soft_reset();

    } else {
        /* load ROM from GUI */
        char new_rom_name[PATH_MAX];
        openFileDialog(new_rom_name) ?
            fprintf(stderr, "User aborted the open file dialog.\n") :
            chip8_load(new_rom_name);

        /* flip GUI toggle */
        gui.load_rom_flag = 0;
        display.lost_window_focus = 1;
    }

    return 0;
}

void chip8_soft_reset() {
    /* clear the vram */
    for (int i = 0; i < WIDTH; i++) {
        memset(chip8.vram[i], 0, HEIGHT * sizeof(unsigned char));
    }

    /* reset the state of the input keys */
    input_reset();

    /* clear registers and the stack */
    memset(chip8.V, 0 , NUM_REGISTERS);
    memset(chip8.stack, 0, STACK_DEPTH);
    memset(chip8.memory, 0, MEM_SIZE);

    /* load fontset */
    for(int i = 0; i < FONTS_SIZE; ++i) {
        chip8.memory[i] = chip8.chip8_fontset[i];
    }

    /* copy the entire rom to memory starting from 0x200 */
    memcpy(chip8.memory + ENTRY_POINT, chip8.rom, chip8.rom_size);

    /* re-initialize program counter, stack pointer, timers, etc. */
    chip8.I = 0;
    chip8.PC = ENTRY_POINT;
    chip8.sp = 0;
    chip8.delay_timer = 0;
    chip8.sound_timer = 0;
    chip8.cpu_halt = 0;
    chip8.draw_flag = 1;

    /* un-pause (if paused) whenever we Soft-Reset */
    chip8.paused = 0;

    /* flip the GUI bit */
    gui.soft_reset_flag = 0;

}

void chip8_run(){
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

        event = input_poll();

        /* do something based on response... */
        if (event & USER_QUIT) return;
        if (event & LOAD_ROM) chip8_load(NULL);
        if (event & SOFT_RESET) chip8_soft_reset();
        if (event & SAVE_PROFILE) {
            profiles_save_current();
            gui.save_profile_flag = 0;
        }

        /* Update notification timers */
        notify_update((double)interval / 1000.0);

        if (!chip8.paused) {
            /* emulate a number of cycles */
            chip8_step_cpu(chip8.cycles);

            /* update Audio */
            if (chip8.sound_timer > 0 && !chip8.muted) {
                audio.beep_active = 1;
            } else {
                audio.beep_active = 0;
            }

            /* check internal timers */
            chip8_update_timers();
        }

        /* draw a frame if we need to */
        if (chip8.draw_flag && display.limit_fps_flag) {
            display_render_frame(chip8.vram);
            chip8.draw_flag = 0;
        } else {
            display_render_frame(NULL);
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

void chip8_update_timers(){
    /* update timers at 60 Hz */
    if (!chip8.cpu_halt) {
        if(chip8.delay_timer > 0) chip8.delay_timer--;
        if(chip8.sound_timer > 0) chip8.sound_timer--;
    }
}

void chip8_step_cpu(int cycles){
    /* execute a batch of instructions */
    for (int i = 0; i < cycles; i++) {
        chip8_fetch_opcode();
        chip8_execute_opcode();

        /* draw */
        if(chip8.draw_flag && !display.limit_fps_flag){
            display_render_frame(chip8.vram);
            chip8.draw_flag = 0;
        }
    }
}

void chip8_fetch_opcode() {
    /* fetch two bytes while being careful of byte alignment */
    chip8.opcode = chip8.memory[chip8.PC] << 8 | chip8.memory[chip8.PC + 1];
    chip8.PC += 2;
}

void chip8_execute_opcode(){
    switch (OP) {
        case 0x0:
            switch (OP_NNN) {
                case 0x0E0: exec00E0(); break;
                case 0x0EE: exec00EE(); break;
                default: exec0NNN(); break;
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

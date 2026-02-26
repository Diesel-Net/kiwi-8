#include "chip8.h"
#include "compat.h"
#include "cpu.h"
#include "input.h"
#include "display.h"
#include "gui.h"
#include "toast.h"
#include "audio.h"
#include "profiles.h"
#include "sha256.h"
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
    bool muted,
    const char *profiles_path
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
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    /* Initialize with default quirks (profiles will override on ROM load) */
    chip8.quirks = quirks_get_defaults();
    chip8.muted = muted;

    /* init vram */
    chip8.vram = (unsigned char **) malloc(WIDTH * sizeof(unsigned char *));
    const char *err_str = "Unable to allocate memory on the heap.\n";
    if (!chip8.vram) {
        printf("%s", err_str);
        return 1;
    }
    memset(chip8.vram, 0, WIDTH * sizeof(unsigned char *));
    for (int i = 0; i < WIDTH; i++) {
        chip8.vram[i] = (unsigned char *) malloc(HEIGHT * sizeof(unsigned char));
        if (!chip8.vram[i]) {
            printf("%s", err_str);
            return 1;
        }
        memset(chip8.vram[i], 0, HEIGHT * sizeof(unsigned char));
    }

    /* Initialize toast system first (before audio) */
    toast_init();

    audio_init();

    if (display_init(fullscreen)) return 1;

    input_reset();

    /* init registers and memory once */
    memset(chip8.V, 0 , NUM_REGISTERS);
    memset(chip8.memory, 0, MEM_SIZE);
    memset(chip8.stack, 0, sizeof(chip8.stack));
    chip8.I = 0;
    chip8.PC = ENTRY_POINT;
    chip8.sp = 0;
    chip8.delay_timer = 0;
    chip8.sound_timer = 0;
    chip8.cpu_halt = 0;
    chip8.draw_flag = 1;

    /* load fontset */
    for(int i = 0; i < FONTS_SIZE; ++i) {
        chip8.memory[i] = chip8_fontset[i];
    }

    /* Initialize ROM profile database */
    profiles_init(profiles_path);

    return chip8_load_bootrom();
}

int chip8_load_bootrom() {
    free(chip8.rom);
    chip8.rom_size = BOOTROM_SIZE;
    chip8.rom = (unsigned char *)malloc(chip8.rom_size);
    if(!chip8.rom) {
        printf("Unable to allocate memory for rom.\n");
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

int chip8_load_rom(const char *rom_filepath) {
    if (rom_filepath) {
        /* open the file */
        printf("Opening file: %s\n", rom_filepath);

        FILE *file;
        file = fopen(rom_filepath, "rb");
        if(file == NULL){
            toast_show(TOAST_ERROR, "Unable to open ROM file");
            return 1;
        }

        /* get file size */
        fseek(file, 0, SEEK_END);
        chip8.rom_size = ftell(file);
        rewind(file);
        if (chip8.rom_size > MEM_SIZE - ENTRY_POINT) {
            toast_show(TOAST_ERROR, "ROM is too large or not formatted properly");
            fclose(file);
            return 1;
        }

        /* allocate or free and reallocate as necessary */
        free(chip8.rom);
        chip8.rom = (unsigned char *)malloc(chip8.rom_size);
        if(!chip8.rom) {
            toast_show(TOAST_ERROR, "Unable to allocate memory for ROM");
            fclose(file);
            return 1;
        }
        memset(chip8.rom, 0 , chip8.rom_size);

        /* save the rom for later (soft-resets) */
        if (!fread(chip8.rom, sizeof(unsigned char), chip8.rom_size, file)) {
            toast_show(TOAST_ERROR, "Unable to read ROM file");
            fclose(file);
            return 1;
        }

        fclose(file);

        /* Extract basename for profile tracking */
        const char *basename = strrchr(rom_filepath, '/');
        if (!basename) basename = strrchr(rom_filepath, '\\'); /* Windows */
        basename = basename ? basename + 1 : rom_filepath;
        strncpy(chip8.rom_filename, basename, sizeof(chip8.rom_filename) - 1);
        chip8.rom_filename[sizeof(chip8.rom_filename) - 1] = '\0';

        /* Mark as user ROM (not bootrom) */
        chip8.rom_loaded = 1;
        char notif_msg[TOAST_MSG_MAX];
        snprintf(notif_msg, sizeof(notif_msg), "ROM loaded: %s", chip8.rom_filename);
        toast_show(TOAST_SUCCESS, notif_msg);

        /* Compute SHA256 and lookup profile using upstream function */
        sha256_hash_t sha256;
        sha256_easy_hash(chip8.rom, chip8.rom_size, sha256.bytes);
        const struct profile *profile = profile_lookup(&sha256);

        if (profile) {
            /* Apply profile quirks */
            chip8.quirks = profile->quirks;
            quirks_print(&chip8.quirks, "Applied quirks:");
            toast_show(TOAST_SUCCESS, "Profile applied");
        } else {
            toast_show(TOAST_INFO, "No profile found");
        }

        chip8_soft_reset();

    } else {
        /* load ROM from GUI */
        char new_rom_filepath[PATH_MAX];
        open_file_dialog(new_rom_filepath) ?
            printf("User aborted the open file dialog.\n") :
            chip8_load_rom(new_rom_filepath);

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
    memset(chip8.stack, 0, sizeof(chip8.stack));
    memset(chip8.memory, 0, MEM_SIZE);

    /* load fontset */
    for(int i = 0; i < FONTS_SIZE; ++i) {
        chip8.memory[i] = chip8_fontset[i];
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
        if (event & LOAD_ROM) chip8_load_rom(NULL);
        if (event & SOFT_RESET) {
            chip8_soft_reset();
            toast_show(TOAST_INFO, "Reset");
        }
        if (event & SAVE_PROFILE) {
            profiles_save_current();
            gui.save_profile_flag = 0;
        }

        /* Update toast timers */
        toast_update((double)interval / 1000.0);

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
                case 0x0E0: exec_00E0(); break;
                case 0x0EE: exec_00EE(); break;
                default: exec_0NNN(); break;
            } break;
        case 0x1: exec_1NNN(); break;
        case 0x2: exec_2NNN(); break;
        case 0x3: exec_3XNN(); break;
        case 0x4: exec_4XNN(); break;
        case 0x5: exec_5XY0(); break;
        case 0x6: exec_6XNN(); break;
        case 0x7: exec_7XNN(); break;
        case 0x8:
            switch (OP_N) {
                case 0x0: exec_8XY0(); break;
                case 0x1: exec_8XY1(); break;
                case 0x2: exec_8XY2(); break;
                case 0x3: exec_8XY3(); break;
                case 0x4: exec_8XY4(); break;
                case 0x5: exec_8XY5(); break;
                case 0x6: exec_8XY6(); break;
                case 0x7: exec_8XY7(); break;
                case 0xE: exec_8XYE(); break;
                default: exec_unknown(); break;
            } break;
        case 0x9: exec_9XY0(); break;
        case 0xA: exec_ANNN(); break;
        case 0xB: exec_BNNN(); break;
        case 0xC: exec_CXNN(); break;
        case 0xD: exec_DXYN(); break;
        case 0xE:
            switch(OP_NN) {
                case 0x9E: exec_EX9E(); break;
                case 0xA1: exec_EXA1(); break;
                default: exec_unknown(); break;
            } break;
        case 0xF:
            switch (OP_NN) {
                case 0x07: exec_FX07(); break;
                case 0x0A: exec_FX0A(); break;
                case 0x15: exec_FX15(); break;
                case 0x18: exec_FX18(); break;
                case 0x1E: exec_FX1E(); break;
                case 0x29: exec_FX29(); break;
                case 0x33: exec_FX33(); break;
                case 0x55: exec_FX55(); break;
                case 0x65: exec_FX65(); break;
                default: exec_unknown(); break;
            } break;
    }
}

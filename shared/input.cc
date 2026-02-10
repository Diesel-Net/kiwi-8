#include "chip8.h"
#include "gui.h"
#include "input.h"
#include "display.h"
#include "toast.h"
#include <string.h> /* memset() */

/* Global input instance */
struct input input;

static void input_process_keys(void) {
    /* map the state of the keys */
    input.keys[0x1] = input.state[SDL_SCANCODE_1];
    input.keys[0x2] = input.state[SDL_SCANCODE_2];
    input.keys[0x3] = input.state[SDL_SCANCODE_3];
    input.keys[0xC] = input.state[SDL_SCANCODE_4];
    input.keys[0x4] = input.state[SDL_SCANCODE_Q];
    input.keys[0x5] = input.state[SDL_SCANCODE_W];
    input.keys[0x6] = input.state[SDL_SCANCODE_E];
    input.keys[0xD] = input.state[SDL_SCANCODE_R];
    input.keys[0x7] = input.state[SDL_SCANCODE_A];
    input.keys[0x8] = input.state[SDL_SCANCODE_S];
    input.keys[0x9] = input.state[SDL_SCANCODE_D];
    input.keys[0xE] = input.state[SDL_SCANCODE_F];
    input.keys[0xA] = input.state[SDL_SCANCODE_Z];
    input.keys[0x0] = input.state[SDL_SCANCODE_X];
    input.keys[0xB] = input.state[SDL_SCANCODE_C];
    input.keys[0xF] = input.state[SDL_SCANCODE_V];

    /* check if cpu is awaiting a keypress for opcode FX0A */
    if (chip8.cpu_halt && input.awaiting_key_press) {
        for (int i = 0; i < NUM_KEYS; i++) {
            if (input.keys[i]) {
                input.awaiting_key_press = 0;
            }
        }
    }
}

static int input_process_events(void) {
    int response = CONTINUE;

    /* cose when the user clicks 'X' */
    if (input.event.type == SDL_QUIT) response = USER_QUIT;

    /* keystroke events */
    if (input.event.type == SDL_KEYDOWN) {
        if (input.state[SDL_SCANCODE_ESCAPE]) response = USER_QUIT;
        if (input.state[SDL_SCANCODE_F5]) response = SOFT_RESET;
        if (input.state[SDL_SCANCODE_RETURN]) display_toggle_fullscreen();
        if (input.state[SDL_SCANCODE_P]) {
            chip8.paused = !chip8.paused;
            toast_show(TOAST_INFO, chip8.paused ? "Paused" : "Unpaused");
        }
        if (input.state[SDL_SCANCODE_M]) {
            chip8.muted = !chip8.muted;
            toast_show(TOAST_INFO, chip8.muted ? "Muted" : "Unmuted");
        }
        if (input.state[SDL_SCANCODE_LALT]) gui.show_menu_flag = !gui.show_menu_flag;
        if (input.state[SDL_SCANCODE_RALT]) gui.show_fps_flag = !gui.show_fps_flag;

        /* slow/raise emulation speed */
        if (input.state[SDL_SCANCODE_PAGEDOWN]) (chip8.cycles -1 < MIN_CYCLES_PER_STEP ) ? chip8.cycles = MIN_CYCLES_PER_STEP : chip8.cycles -= 1;
        if (input.state[SDL_SCANCODE_PAGEUP]) (chip8.cycles +1 > MAX_CYCLES_PER_STEP ) ? chip8.cycles = MAX_CYCLES_PER_STEP : chip8.cycles += 1;
    }

    /* window events */
    if (input.event.window.type == SDL_WINDOWEVENT){
        /* update the current rendering screen space */
        if (input.event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) display_resize(input.event.window.data1, input.event.window.data2);

        if (input.event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            /* TODO: resume the emulator, if paused_on_focus_loss */
        }
        if (input.event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            /* focus is lost when a user tries to laod a rom */
            if (display.lost_window_focus) display_raise_window();
        }

        /* the window manager requests that the window be closed */
        if (input.event.window.event == SDL_WINDOWEVENT_CLOSE) response = USER_QUIT;
    }

    return response;
}

void input_reset(void) {
    input.awaiting_key_press = 0;
    memset(input.keys, 0, NUM_KEYS);
}

int input_poll(void) {
    int response = CONTINUE;

    /* purge any queued events */
    while (SDL_PollEvent(&input.event)) {

        input.state = SDL_GetKeyboardState(NULL);

        /* check GUI */
        gui_process_events(&input.event);
        if (gui.quit_flag) response |= USER_QUIT;
        if (gui.soft_reset_flag) response |= SOFT_RESET;
        if (gui.load_rom_flag) response |= LOAD_ROM;
        if (gui.save_profile_flag) response |= SAVE_PROFILE;

        /* check SDL events (window & hotkeys) */
        response |= input_process_events();

        /* check chip-8 input */
        input_process_keys();
    }
    return response;
}

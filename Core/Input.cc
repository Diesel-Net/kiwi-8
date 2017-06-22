#include "Chip8.h"
#include "Input.h"
#include "Display.h"

Input::Input() {
    /* empty */
}

Input::~Input() {
    /* empty */
}

void Input::Initialize(Display *display, 
                       int *cycles, 
                       bool *cpu_halt, 
                       bool *paused,
                       bool *muted) {
    this->cycles = cycles;
    this->display = display;
    this->cpu_halt = cpu_halt;
    this->paused = paused;
    this->muted = muted;
    Reset();
}

void Input::Reset() {
    awaiting_key_press = 0;
    memset(keys, 0, NUM_KEYS);
}

int Input::Poll() {
    int response = CONTINUE;
    
    /* purge any queued events */
    while (SDL_PollEvent(&event)) {

        state = SDL_GetKeyboardState(NULL);

        /* check GUI */
        display->gui.ProcessEvents(&event);
        if (display->gui.quit_flag) response |= USER_QUIT;
        if (display->gui.soft_reset_flag) response |= SOFT_RESET;
        if (display->gui.load_rom_flag) response |= LOAD_ROM;

        /* check SDL events (window & hotkeys) */
        response |= ProcessEvents();

        /* check chip-8 input */
        ProcessKeys(); 
    } 
    return response;
}

int Input::ProcessEvents() {
    int response = CONTINUE;

    /* cose when the user clicks 'X' */
    if (event.type == SDL_QUIT) response = USER_QUIT;

    /* keystroke events */
    if (event.type == SDL_KEYDOWN) {
        if (state[SDL_SCANCODE_ESCAPE]) response = USER_QUIT;
        if (state[SDL_SCANCODE_F5]) response = SOFT_RESET;
        if (state[SDL_SCANCODE_RETURN]) display->ToggleFullscreen();
        if (state[SDL_SCANCODE_P]) *paused = !(*paused);
        if (state[SDL_SCANCODE_M]) *muted = !(*muted);
        if (state[SDL_SCANCODE_LALT]) display->gui.show_menu_flag = !display->gui.show_menu_flag;
        if (state[SDL_SCANCODE_RALT]) display->gui.show_fps_flag = !display->gui.show_fps_flag;
        
        /* slow/raise emulation speed */
        if (state[SDL_SCANCODE_PAGEDOWN]) (*cycles -1 < MIN_CYCLES_PER_STEP ) ? *cycles = MIN_CYCLES_PER_STEP : *cycles -= 1;
        if (state[SDL_SCANCODE_PAGEUP]) (*cycles +1 > MAX_CYCLES_PER_STEP ) ? *cycles = MAX_CYCLES_PER_STEP : *cycles += 1;	
    }

    /* window events */
    if (event.window.type == SDL_WINDOWEVENT){
        /* update the current rendering screen space */
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) display->Resize(event.window.data1, event.window.data2);
        
        if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            /* TODO: resume the emulator, if paused_on_focus_loss */
        } 
        if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            /* TODO: add a toggle for "pause on focus loss" */
        }

        /* the window manager requests that the window be closed */
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) response = USER_QUIT;
    }

    return response;
}

void Input::ProcessKeys() {
    /* map the state of the keys */
    keys[0x1] = state[SDL_SCANCODE_1];
    keys[0x2] = state[SDL_SCANCODE_2];
    keys[0x3] = state[SDL_SCANCODE_3];
    keys[0xC] = state[SDL_SCANCODE_4];
    keys[0x4] = state[SDL_SCANCODE_Q];
    keys[0x5] = state[SDL_SCANCODE_W];
    keys[0x6] = state[SDL_SCANCODE_E];
    keys[0xD] = state[SDL_SCANCODE_R];
    keys[0x7] = state[SDL_SCANCODE_A];
    keys[0x8] = state[SDL_SCANCODE_S];
    keys[0x9] = state[SDL_SCANCODE_D];
    keys[0xE] = state[SDL_SCANCODE_F];
    keys[0xA] = state[SDL_SCANCODE_Z];
    keys[0x0] = state[SDL_SCANCODE_X];
    keys[0xB] = state[SDL_SCANCODE_C];
    keys[0xF] = state[SDL_SCANCODE_V];

    /* check if cpu is awaiting a keypress for opcode FX0A */
    if (cpu_halt && awaiting_key_press) {
        for (int i = 0; i < NUM_KEYS; i++) {
            if (keys[i]) {
                awaiting_key_press = 0;
            }
        }     
    }
}

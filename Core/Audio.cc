#include "Audio.h"
#include <stdio.h>

/* This callback function should NOT be declared in any header files */
static void audio_callback(void *userdata, unsigned char *stream, int len) {
    /* TO COMPLETE */
    Audio *audio = (Audio *) userdata;
    audio->SineWave(stream, len);
}

Audio::Audio() {
    /* TO COMPLETE */
    wave_position = 0;
    wave_increment = 2 * M_PI * 1000 / FREQUENCY;
    /* Start muted? */
    mute_flag = 1;
}

Audio::~Audio() {
    if (device) SDL_CloseAudioDevice(device); 
}

int Audio::Initialize() {
    /* TO COMPLETE */
    audiospec.freq = FREQUENCY;
    audiospec.format = AUDIO_U8;
    audiospec.channels = 1;
    audiospec.samples = 2048;
    audiospec.callback = audio_callback;
    audiospec.userdata = this;

    /* Open default audio device */
    device = SDL_OpenAudioDevice(NULL, 0, &audiospec, NULL, 0);

    if (!device) {
        fprintf(stderr, "Error: %s\n", SDL_GetError());
        return 1;
    }

    /* Start playing Audio */
    SDL_PauseAudioDevice(device, 0);

    return 0;
}

void Audio::SineWave(unsigned char *stream, int len) {
    /* TO COMPLETE */
    //if (mute_flag) return;

    for (int i = 0; i < len; i++) {
        stream[i] = (unsigned char) (7 * sin(wave_position) + 127);
        wave_position += wave_increment;
    }
}

void Audio::Update(bool beep) {
    /* TO COMPLETE */

    /*
    if (mute_flag == beep) {
        SDL_LockAudioDevice(device);
        mute_flag = !beep;
        SDL_UnlockAudioDevice(device);
    } */
}


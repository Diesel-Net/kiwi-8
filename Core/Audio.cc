#include "Audio.h"
#include <stdio.h>

#define FREQUENCY 44100

/* This callback function should NOT be declared in any header files */
static void audio_callback(void *userdata, unsigned char *stream, int len) {
    Audio *audio = (Audio *) userdata;
    audio->SineWave(stream, len);
}

Audio::Audio() {
    wave_position = 0;
    wave_increment = 2 * M_PI * 1000 / FREQUENCY;
    mute_flag = 0;
}

Audio::~Audio() {
    if (device) SDL_CloseAudioDevice(device); 
}

int Audio::Initialize() {
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
    //SDL_PauseAudioDevice(device, 0);

    return 0;
}

void Audio::SineWave(unsigned char *stream, int len) {
    for (int i = 0; i < len; i++) {
        stream[i] = (unsigned char) (7 * sin(wave_position) + 127);
        wave_position += wave_increment;
    }
}

void Audio::Update(bool play) {
    /* TO COMPLETE */
}


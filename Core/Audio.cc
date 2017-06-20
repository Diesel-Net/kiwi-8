#include "Audio.h"
#include <stdio.h>

Audio::Audio() {
    wave_position = 0;
    wave_increment = ((double) TONE * (2.0 * M_PI)) / (double) FREQUENCY;
}

Audio::~Audio() {
    /* Pause the audio */
    SDL_PauseAudioDevice(device, 1);
    if (device) SDL_CloseAudioDevice(device); 
    free(audio_buffer);
}

int Audio::Initialize() {
    audiospec.freq = FREQUENCY;
    audiospec.format = AUDIO_U8; /* unsigned 8-bit data stream */
    audiospec.channels = 1; /* mono */
    audiospec.samples = 2048; /* must be a power of 2 */
    audiospec.callback = NULL;
    audiospec.userdata = NULL;

    /* Open default audio device (Allow audio changes?) */
    device = SDL_OpenAudioDevice(NULL, 0, &audiospec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);

    if (!device) {
        fprintf(stderr, "Error: %s\n", SDL_GetError());
        return 1;
    }

    audio_buffer = (unsigned char *)malloc(SAMPLES_PER_FRAME * 30); /* ~.5 seconds worth of audio */
    if (!audio_buffer) {
        fprintf(stderr, "Unable to allocate memory for audio buffer.\n");
        return 1;
    } 

    /* Start playing Audio */
    SDL_PauseAudioDevice(device, 0);

    return 0;
}

void Audio::SineWave(int length) {
    for (int i = 0; i < length; i++) {
        audio_buffer[i] = (unsigned char) ((7 * sin(wave_position)) + 127);
        wave_position += wave_increment;
    }
}

void Audio::Beep(int length) {
    if (SDL_GetQueuedAudioSize(device) < (SAMPLES_PER_FRAME * 2)) {
        SineWave(length);
        SDL_QueueAudio(device, audio_buffer, length); 
    }
}


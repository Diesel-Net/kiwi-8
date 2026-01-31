#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>

#define TAU (2.0 * M_PI)
#define AMPLITUDE 28000 // Max amplitude for signed 16-bit
#define SAMPLE_RATE 44100 // Standard CD quality sample rate in Hz
#define TONE 440.0 // Frequency of the beep (e.g., A4 note)


struct audio {
    SDL_AudioSpec audiospec;
    SDL_AudioDeviceID device;

    double phase;
    double phase_increment;

    int beep_active;
};

extern struct audio audio;

void audio_destroy(void);
int audio_initialize(void);

#endif

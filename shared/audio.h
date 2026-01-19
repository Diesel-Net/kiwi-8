#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <stdlib.h>

#define FREQUENCY 48000
#define TONE 440
#define AMPLITUDE 7
#define BIAS 127
#define SAMPLES_PER_FRAME ((FREQUENCY / 60) * 10)

struct audio {
    SDL_AudioSpec audiospec;
    SDL_AudioDeviceID device;

    double wave_position;
    double wave_increment;

    int beep_active;
    int beep_length;
};

extern struct audio audio;

void audio_beep(void);
void audio_destroy(void);
int audio_initialize(void);


#endif

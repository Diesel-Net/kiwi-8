#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <stdlib.h>

#define FREQUENCY 48000
#define TONE 440
#define AMPLITUDE 7
#define BIAS 127
#define SAMPLES_PER_FRAME ((FREQUENCY / 60) * 3)

struct audio {
    SDL_AudioSpec audiospec;
    SDL_AudioDeviceID device;
    unsigned char *audio_buffer;

    double wave_position;
    double wave_increment;
};

/* Global audio instance */
extern struct audio audio;

/* Audio functions */
void audio_create(void);
void audio_destroy(void);
int audio_initialize(void);
void audio_beep(int length);

#endif

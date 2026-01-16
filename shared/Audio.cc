#include "Audio.h"
#include <stdio.h>

/* Global audio instance */
struct audio audio;

void audio_create(void) {
    audio.wave_position = 0;
    audio.wave_increment = ((double) TONE * (2.0 * M_PI)) / (double) FREQUENCY;
}

void audio_destroy(void) {
    /* pause & close the audio */
    SDL_PauseAudioDevice(audio.device, 1);
    if (audio.device) SDL_CloseAudioDevice(audio.device);
    free(audio.audio_buffer);
}

int audio_initialize(void) {
    audio.audiospec.freq = FREQUENCY;
    audio.audiospec.format = AUDIO_U8; /* unsigned 8-bit data stream */
    audio.audiospec.channels = 1; /* mono */
    audio.audiospec.samples = 2048; /* must be a power of 2 */
    audio.audiospec.callback = NULL;
    audio.audiospec.userdata = NULL;

    /* open default audio device (allow audio changes) */
    audio.device = SDL_OpenAudioDevice(NULL, 0, &audio.audiospec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);

    if (!audio.device) {
        fprintf(stderr, "Error: %s\n", SDL_GetError());
        return 1;
    }

    /* ~.5 seconds worth of audio (probably overkill) */
    audio.audio_buffer = (unsigned char *)malloc(SAMPLES_PER_FRAME * 30);
    if (!audio.audio_buffer) {
        fprintf(stderr, "Unable to allocate memory for audio buffer.\n");
        return 1;
    }

    /* start playing audio */
    SDL_PauseAudioDevice(audio.device, 0);

    return 0;
}

static void audio_sine_wave(int length) {
    for (int i = 0; i < length; i++) {
        /* sine wave varies from 120 - 134 */
        audio.audio_buffer[i] = (unsigned char) ((AMPLITUDE * sin(audio.wave_position)) + BIAS);
        audio.wave_position += audio.wave_increment;
    }
}

void audio_beep(int length) {
    if (SDL_GetQueuedAudioSize(audio.device) < (SAMPLES_PER_FRAME * 2)) {
        audio_sine_wave(length);
        SDL_QueueAudio(audio.device, audio.audio_buffer, length);
    }
}

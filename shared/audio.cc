#include "audio.h"
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL_audio.h>
#include <math.h>

/* audio instance */
struct audio audio;

static void audio_callback(void *userdata, Uint8 *stream, int len) {
    struct audio *a = (struct audio *) userdata;

    if (a->beep_active && a->beep_length > 0) {
        for (int i = 0; i < len; i++) {
            stream[i] = (unsigned char)((AMPLITUDE * sin(a->wave_position)) + BIAS);
            a->wave_position += a->wave_increment;
        }
        a->beep_length -= len;
        if (a->beep_length <= 0) {
            a->beep_active = 0;
        }
    } else {
        memset(stream, BIAS, len); // Silence (BIAS value)
    }
}

void audio_destroy(void) {
    SDL_PauseAudioDevice(audio.device, 1);
    if (audio.device) SDL_CloseAudioDevice(audio.device);
}

int audio_initialize(void) {
    audio.wave_position = 0;
    audio.wave_increment = ((double) TONE * (2.0 * M_PI)) / (double) FREQUENCY;
    audio.beep_active = 0;
    audio.beep_length = 0;

    audio.audiospec.freq = FREQUENCY;
    audio.audiospec.format = AUDIO_U8;
    audio.audiospec.channels = 1;
    audio.audiospec.samples = 512;
    audio.audiospec.callback = audio_callback;
    audio.audiospec.userdata = &audio;

    const char* drivers[] = {
        "wasapi", "directsound", "winmm", "xaudio2", "coreaudio", "pulseaudio", "pipewire", "jack", "alsa", "dsp", "dummy",
    };
    int driver_found = 0;
    for (int i = 0; i < sizeof(drivers) / sizeof(drivers[0]); i++) {
        if (SDL_AudioInit(drivers[i]) == 0) {
            fprintf(stdout, "Successfully initialized audio with driver: %s\n", drivers[i]);
            driver_found = 1;
            break;
        }
    }
    if (!driver_found) {
        fprintf(stderr, "Warning: Could not initialize any audio driver, continuing without sound.\n");
    }
    audio.device = SDL_OpenAudioDevice(NULL, 0, &audio.audiospec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (!audio.device) {
        fprintf(stderr, "Warning: No audio device available, using dummy driver. %s\n", SDL_GetError());
        SDL_setenv("SDL_AUDIODRIVER", "dummy", 1);
        audio.device = SDL_OpenAudioDevice(NULL, 0, &audio.audiospec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
        if (!audio.device) {
            fprintf(stderr, "Error: Failed to initialize audio (even with dummy driver): %s\n", SDL_GetError());
            return 1;
        }
    }
    SDL_PauseAudioDevice(audio.device, 0);
    return 0;
}

void audio_beep(int length) {
    audio.beep_active = 1;
    audio.beep_length = length;
}

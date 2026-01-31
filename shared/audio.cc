#include "audio.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL_audio.h>

struct audio audio;

void audio_destroy(void) {
    SDL_PauseAudioDevice(audio.device, 1);
    if (audio.device) SDL_CloseAudioDevice(audio.device);
}

void audio_callback(void* userdata, Uint8* stream, int length) {
    struct audio *audio = (struct audio *)userdata;

    Sint16* samples = (Sint16*)stream;
    int sample_count = length / sizeof(Sint16);

    if (audio->beep_active) {
        for (int i = 0; i < sample_count; ++i) {
            // Generate a square wave: high for phase < PI, low for phase >= PI
            double value = (audio->phase < M_PI) ? 1.0 : -1.0;
            samples[i] = (Sint16)(value * AMPLITUDE);
            audio->phase += audio->phase_increment;
            if (audio->phase >= TAU) {
                audio->phase -= TAU;
            }
        }
    } else {
        memset(stream, 0, length); // silence
    }
}


int audio_initialize(void) {
    audio.phase = 0.0;
    audio.phase_increment = TAU * TONE / SAMPLE_RATE;
    audio.beep_active = 0;

    audio.audiospec.freq = SAMPLE_RATE;
    audio.audiospec.format = AUDIO_S16SYS; // Use signed 16-bit system endian format
    audio.audiospec.channels = 1; // mono
    audio.audiospec.samples = 128; // buffer size (lower: lower latency)
    audio.audiospec.callback = audio_callback;
    audio.audiospec.userdata = &audio;

    const char* drivers[] = {
        "wasapi",
        "directsound",
        "winmm",
        "xaudio2",
        "coreaudio",
        "pulseaudio",
        "pipewire",
        "jack",
        "alsa",
        "dsp",
        "dummy",
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

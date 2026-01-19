#include "audio.h"
#include <stdio.h>
#include <SDL2/SDL_audio.h>

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

    /* try initializing audio using available drivers */
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

    if (!driver_found) {
        fprintf(stderr, "Warning: Could not initialize any audio driver, continuing without sound.\n");
    }

    /* open default audio device (allow audio changes) */
    audio.device = SDL_OpenAudioDevice(NULL, 0, &audio.audiospec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);

    if (!audio.device) {
        fprintf(stderr, "Warning: No audio device available, using dummy driver. %s\n", SDL_GetError());

        /* Try to use dummy audio driver as fallback */
        SDL_setenv("SDL_AUDIODRIVER", "dummy", 1);
        audio.device = SDL_OpenAudioDevice(NULL, 0, &audio.audiospec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);

        if (!audio.device) {
            fprintf(stderr, "Error: Failed to initialize audio (even with dummy driver): %s\n", SDL_GetError());
            return 1;
        }
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

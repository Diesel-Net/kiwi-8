#include "Audio.h"
#include <stdio.h>

/* This callback function should NOT be declared in any header files
static void audio_callback(void *userdata, unsigned char *stream, int len) {
    Audio *audio = (Audio *) userdata;
    audio->SineWave(stream, len);
} */

Audio::Audio() {
    /* TO COMPLETE */
    wave_position = 0;
    wave_increment = ((double) TONE * (2.0 * M_PI)) / (double) FREQUENCY;
    mute_flag = 1;
}

Audio::~Audio() {
    /* Pause the audio */
    SDL_PauseAudioDevice(device, 1);
    if (device) SDL_CloseAudioDevice(device); 
    free(audio_buffer);
}

int Audio::Initialize() {
    /* TO COMPLETE */
    audiospec.freq = FREQUENCY;
    audiospec.format = AUDIO_U8;
    audiospec.channels = 1;
    audiospec.samples = 2048; /* must be a power of 2 */
    audiospec.callback = NULL; //audio_callback;
    audiospec.userdata = NULL; //this;

    /* Open default audio device (Allow audio changes?) */
    device = SDL_OpenAudioDevice(NULL, 0, &audiospec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);

    if (!device) {
        fprintf(stderr, "Error: %s\n", SDL_GetError());
        return 1;
    }

    audio_buffer = (unsigned char *)malloc(SAMPLES_PER_FRAME * 30); /* A half a second (overkill until i figure out whats best) */
    if (!audio_buffer) {
        fprintf(stderr, "Unable to allocate memory for audio buffer.\n");
        return 1;
    } 

    /* Start playing Audio */
    SDL_PauseAudioDevice(device, 0);

    /* Things to note: latency issues
        -if buffer is too big, audio will appear more and more delayed (queue increasing in size)
        -it would be a good idea to check the size of the queue every once in awhile
        - need to generate a sine wave "as we go" to keep track of wave position so it sounds normal
    */

    return 0;
}

void Audio::SineWave(int length) {
    /* TO COMPLETE */
    for (int i = 0; i < length; i++) {
        //audio_buffer[i] = (unsigned char) (7 * sin(wave_position) + 127);
        audio_buffer[i] = (unsigned char) ((7 * sin(wave_position)) + 127);
        wave_position += wave_increment;
    }
    //wave_position = 0;
    
}

void Audio::Update(int length) {
    /* TO COMPLETE */
    if (SDL_GetQueuedAudioSize(device) < (SAMPLES_PER_FRAME * 5)) {
        SineWave(length);
        SDL_QueueAudio(device, audio_buffer, length); 
    }
}


#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>

#define FREQUENCY 44100

class Audio {
    private:
        SDL_AudioSpec audiospec;
        SDL_AudioDeviceID device;

        double wave_position;
        double wave_increment; 

    public:

        void SineWave(unsigned char *stream, int len);
        bool mute_flag;

        Audio();
        ~Audio();
        int Initialize();
        void Update(bool play);
};

#endif

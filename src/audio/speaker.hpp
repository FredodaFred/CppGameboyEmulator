#pragma once

#include <SDL2/SDL_audio.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <ostream>

#define UNPAUSE_AUDIO 0
#define PAUSE_AUDIO 1


class Speaker {
    public:
        void init();

        void play_sample(int16_t left, int16_t right);

        void pause();

        void unpause();

        void close();
    private:
        SDL_AudioDeviceID device_id;
};

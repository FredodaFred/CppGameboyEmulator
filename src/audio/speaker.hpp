#pragma once

#include <SDL2/SDL_audio.h>
#include <SDL2/SDL.h>

class Speaker {
    public:
        static void init();
        static void close();
        static void SDLCALL audio_callback(void* userdata, Uint8* stream, int len);
};

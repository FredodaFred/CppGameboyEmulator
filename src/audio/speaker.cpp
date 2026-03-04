#include "speaker.hpp"

void init() {
    SDL_Init(SDL_INIT_AUDIO);
}

void close() {
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
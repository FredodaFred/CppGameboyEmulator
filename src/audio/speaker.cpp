#include "speaker.hpp"

void Speaker::init() {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE);

    SDL_AudioSpec want;
    SDL_zero(want);
    want.freq = 48000;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 1024;
    device_id = SDL_OpenAudioDevice(nullptr, 0, &want, nullptr, 0);

    if (device_id == 0) {
        std::cout << "Failed to open audio device" << std::endl;
    } else {
        SDL_PauseAudioDevice(device_id, UNPAUSE_AUDIO);
    }

}

/**
 * This is essentially processing a left stereo and right stereo value and playing it
 * @param left
 * @param right
 */
void Speaker::play_sample(int16_t left, int16_t right) {
    int16_t frame[2] = { left, right };
    SDL_QueueAudio(device_id, frame, sizeof(frame));
}
void Speaker::pause() {
    SDL_PauseAudioDevice(device_id, PAUSE_AUDIO);
}
void Speaker::unpause() {
    SDL_PauseAudioDevice(device_id, UNPAUSE_AUDIO);
}

void Speaker::close() {
    if (device_id != 0) {
        SDL_CloseAudioDevice(device_id);
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
#include "speaker.hpp"

void Speaker::init() {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE);

    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 48000;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 128; // do NOT make this higher than 512 unless u want fried audio
    device_id = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);

    if (device_id == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_PauseAudioDevice(device_id, UNPAUSE_AUDIO);
}

/**
 * This is essentially processing a left stereo and right stereo value and playing it
 * @param left
 * @param right
 */
void Speaker::play_sample(int16_t left, int16_t right) {
    if (device_id == 0) return;

    // This logic somehow saves us
    while (SDL_GetQueuedAudioSize(device_id) > 3840) {
        std::this_thread::yield();
    }

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
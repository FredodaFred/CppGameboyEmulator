#pragma once
#include <cstdint>
#include <SDL2/SDL_audio.h>
#include "square_channel.hpp"
#include "speaker.hpp"
#include "wave_channel.hpp"
#include "noise_channel.hpp"

class APU {
    public:
        APU(Speaker& speaker);
        uint8_t apu_io_read(uint16_t addr);
        void apu_io_write(uint16_t addr, uint8_t data);
        void init();
        void tick(int cycle, bool apu_div_tick);

        // 21.8453 M Cycles = 1048576.0 Hz / M Cycles ÷ 48,000 HZ (sample rate we desire)
        static constexpr float SAMPLE_RATE = 1048576.0f / 48000.0f;
        static constexpr int MAX_CHANNEL_VOL_FACTOR = 500;

        uint8_t apu_div{0};
    private:
        Speaker& speaker;
        void tick_cycle();

        void mix_and_sample();

        float sample_accumulator{0};

        SquareChannel channel1;
        SquareChannel channel2;
        WaveChannel channel3;
        NoiseChannel channel4;

       // channel 2
        uint8_t nr21{0x3F};
        uint8_t nr22{0x00};
        uint8_t nr23{0xFF};
        uint8_t nr24{0xBF};

        // channel 3
        uint8_t nr30{0x7F};
        uint8_t nr31{0xFF};
        uint8_t nr32{0x9F};
        uint8_t nr33{0xFF};
        uint8_t nr34{0xBF};

        // channel 4
        uint8_t nr41{0xFF};
        uint8_t nr42{0x00};
        uint8_t nr43{0x00};
        uint8_t nr44{0xBF};


        uint8_t nr50{0x77};
        uint8_t nr51{0xF3};
        uint8_t nr52{0xF1};
    ;

        bool check_master_enable() const;

        void mixer();
};

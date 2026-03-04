#pragma once
#include <cstdint>
#include <SDL2/SDL_audio.h>
#include "channel1.hpp"

class APU {
    public:
        uint8_t apu_io_read(uint16_t addr);
        void apu_io_write(uint16_t addr, uint8_t data);
        void init();
        void tick(int cycle);

        // 87 M Cycles = 4,194,304 Hz / Cycles ÷ 48,000 HZ (sample rate we desire)
        static constexpr int SAMPLE_RATE = 87;


    private:
        void tick_cycle();
        int sample_timer{0};

        uint8_t WAVE_RAM[16]; // 16 byte ram

        Channel1 channel1;

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


        void toggleDACs();

        void trigger_channel1();
        void play_channel1();

        void trigger_channel2();

        double get_wave_duty(uint8_t nrx1);

        uint8_t get_initial_timer_length(uint8_t nrx1);

        bool check_master_enable() const;

        void mixer();

        bool DAC1{true};
        bool DAC2{true};
        bool DAC3{true};
        bool DAC4{true};

        bool ch1_enable{false};
        bool ch2_enable{false};
        bool ch3_enable{false};
        bool ch4_enable{false};
};

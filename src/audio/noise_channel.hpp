#pragma once
#include <cstdint>
#include <cstdio>

#define LENGTH_TIMER_MAX 64

class NoiseChannel {
    public:
        void write_nr41(uint8_t data);

        void write_nr42(uint8_t data);
        uint8_t read_nr42();

        void write_nr43(uint8_t data);
        uint8_t read_nr43();

        void write_nr44(uint8_t data);
        uint8_t read_nr44();

        void tick();

        int16_t sample();
        void length_timer_tick();

        void env_sweep_tick();

    private:
        uint8_t initial_length_timer;
        uint8_t clock_shift;
        bool lsfr_width;
        uint8_t clock_div;

        bool trigger_val;
        bool length_timer_enable;
        uint16_t period;
        uint16_t period_div;

        bool DAC{true};
        bool enabled{true};

        uint8_t length_timer;
        uint8_t initial_volume;
        uint8_t current_volume;
        bool env_dir;
        uint8_t env_sweep_pace;
        uint8_t internal_env_sweep_pace_counter;

        uint16_t lsfr;
        bool last_right_bit{false};
        uint16_t div{0};
        void update_lsfr();
        void trigger();

};
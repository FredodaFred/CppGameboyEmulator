#pragma once
#include <cstdint>
#include <stdexcept>

class Channel1 {

    public:
        Channel1() = default;

        uint8_t sample();

        void write_nr10(uint8_t data);
        uint8_t read_nr10();

        void write_nr11(uint8_t data);
        uint8_t read_nr11();

        void write_nr12(uint8_t data);
        uint8_t read_nr12();

        void write_nr13(uint8_t data);
        uint8_t read_nr13();

        void write_nr14(uint8_t data);
        uint8_t read_nr14();

        bool DAC{true};

    private:
        uint8_t pace;
        bool direction;
        uint8_t individual_step;

        uint8_t initial_volume;
        bool env_dir;
        uint8_t sweep_pace;

        double wave_duty;
        uint8_t initial_timer_length;

        uint16_t period_value;

        bool trigger;
        bool length_enable;

};

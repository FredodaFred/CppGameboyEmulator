#pragma once
#include <cstdint>
#include <stdexcept>

#define LENGTH_TIMER_MAX 64
#define LENGTH_TIMER_M_CYCLES_TICK_RATE 4096

class Channel1 {

    public:
        Channel1() = default;

        void tick();

        int16_t sample();

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

        void trigger();

        void length_timer_tick();

        void period_tick();

        bool DAC{true};
        bool enabled = true;

    private:
        /**
         * Let's convert 1 "period sweep" using our pace value. An M cycle, what we tick(), is 1048576Hz
         * A sweep iteration is the pace * 128Hz.
         * So 1048576 Hz/ 1 M cycle and 128 Hz / sweep. Let's find M cycles per sweep assuming pace is 1!
         * 1048576 Hz/ 1 M-Cycle = x M-cycles / 1 sweep
         * Solve the equation, we get 8192 M cycles per 1 sweep, assuming pace is 1.
         * So to get iterations we would do 1048576 Hz / (128Hz / pace)
         * If pace is zero, there are no iterations! it's disabled/
         */
        uint8_t pace;
        int direction;

        uint8_t initial_volume;
        uint8_t current_volume;

        bool env_dir;
        uint8_t sweep_pace;


        /**
         * Length timer counter wil tick once every 256Hz, when it reaches 64, the channel is disabled.
         * 1048576 Hz / 256Hz would be 4096 M cycles per length timer tick.
         */
        uint8_t initial_length_timer;
        uint8_t length_timer;
        bool length_timer_enable;
        uint8_t length_timer_counter;


        // This will determine pitch
        uint16_t period;

        int internal_m_cycle_counter = 0;
        int sweep_rate = 0;

        /**
         * Think of this like timbre. Creates the square waves by deciding how long we stay high or low.
         * in a 16 bit cycle, every one of these would oscillate twice (move high to low), meaning the pitch
         * for all of these is the same. This is taken directly from the waveform diagram in the pandocs
         * Note: visual diagram is inverted. 12.5% is low 12.5% of the time...
         * https://gbdev.io/pandocs/Audio_Registers.html#ff11--nr11-channel-1-length-timer--duty-cycle
         */
        uint8_t wave_duty;
        uint8_t individual_step;
        static constexpr uint8_t DUTY_TABLES[4][8] = {
            {0,0,0,0,0,0,0,1}, // 12.5%
            {1,0,0,0,0,0,0,1}, // 25%
            {1,0,0,0,0,1,1,1}, // 50%
            {0,1,1,1,1,1,1,0}  // 75%
        };

};

#include <cstdint>
#define LENGTH_TIMER_MAX 256

class WaveChannel {
    public:
        uint8_t read_nr30();
        void write_nr30(uint8_t data);

        uint8_t read_nr31();
        void write_nr31(uint8_t data);

        uint8_t read_nr32();
        void write_nr32(uint8_t data);

        uint8_t read_nr33();
        void write_nr33(uint8_t data);

        uint8_t read_nr34();
        void write_nr34(uint8_t data);

        uint8_t read_WRAM(uint16_t addr);
        void write_WRAM(uint16_t addr, uint8_t data);

        void length_timer_tick();

        void trigger();
        void tick();

        int16_t sample();

        uint8_t get_sample();

    private:
        bool DAC{true};
        uint8_t initial_length_timer;
        uint8_t output_level; // or volume
        uint16_t period;
        uint16_t period_div;

        bool trigger_val;
        bool length_timer_enable;
        uint8_t WAVE_RAM[16]; // 16 byte ram
        uint8_t wave_index{1};
        bool enabled{true};
        uint8_t length_timer;

        uint8_t current_volume = output_level;
};



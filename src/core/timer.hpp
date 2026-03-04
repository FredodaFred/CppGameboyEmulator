#pragma once
#include <cstdint>

// https://gbdev.io/pandocs/Timer_and_Divider_Registers.html#timer-and-divider-registers
class Timer {
    public:
        void write_timer(uint16_t addr, uint8_t data);
        uint8_t read_timer(uint16_t addr);
        void tick(int clock_cycles);
        //This will request and interrupt
        bool interrupt = false;
    private:
        uint8_t DIV{0x18}; // 0xFF04
        uint8_t TIMA{0}; // 0xFF05
        uint8_t TMA{0}; // 0xFF06
        uint8_t TAC{0}; // 0xFF07

        int tima_remainder{0};
        int div_remainder{0};
        int tima_overflow_pending = 0;
        void tick_tima(int clock_cycles);
        void tick_div(int clock_cycles);
};
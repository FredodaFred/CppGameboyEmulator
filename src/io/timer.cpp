#include "timer.hpp"

#include <stdexcept>

// NOTE 16384hz = 64 M (or clock_cycles)

void Timer::tick(int clock_cycles) {
    tick_div(clock_cycles + div_remainder);
    if (TAC & 0x04) {
        tick_tima(clock_cycles + tima_remainder);
    }
}

void Timer::tick_tima(int clock_cycles) {
    int divisor = 0;
    switch (TAC & 0x03) {
        case 0b00: divisor = 1024; break;
        case 0b01: divisor = 16;   break;
        case 0b10: divisor = 64;   break;
        case 0b11: divisor = 256;  break;
    }

    int ticks = clock_cycles / divisor;
    tima_remainder = clock_cycles % divisor;

    for (int i = 0; i < ticks; i++) {
        if (TIMA == 0xFF) {
            TIMA = TMA;
            interrupt = true; // Stay true until Emulator.tick handles it
        } else {
            TIMA++;
        }
    }
}

void Timer::tick_div(int clock_cycles) {
    DIV += clock_cycles / 64;
    div_remainder += clock_cycles % 64;
}

void Timer::write_timer(uint16_t addr, uint8_t data) {
    if (addr == 0xFF04) {
        DIV = 0x00;
        div_remainder = 0x00;
    } else if (addr == 0xFF05) {
        TIMA = data;
        tima_remainder = 0x00;
    } else if (addr == 0xFF06) {
        TMA = data;
    } else if (addr == 0xFF07) {
        TAC = data & 0x0F;
    }
}

uint8_t Timer::read_timer(uint16_t addr) {
    if (addr == 0xFF04) {
        return DIV;
    } else if (addr == 0xFF05) {
        return TIMA;
    } else if (addr == 0xFF06) {
        return TMA;
    } else if (addr == 0xFF07) {
        return TAC;
    }
    std::runtime_error("Invalid timer read");
}

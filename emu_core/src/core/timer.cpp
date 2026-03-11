#include "timer.hpp"

#include <stdexcept>


bool Timer::tick(int clock_cycles) {
    bool apu_div_tick = tick_div(clock_cycles);
    if (TAC & 0x04) {
        tick_tima(clock_cycles + tima_remainder);
    }

    return apu_div_tick;
}

void Timer::tick_tima(int clock_cycles) {

    if (tima_overflow_pending > 0) {
        tima_overflow_pending -= clock_cycles;
        if (tima_overflow_pending <= 0) {
            TIMA = TMA;
            interrupt = true;
            tima_overflow_pending = 0;
        }
    }

    int divisor = 0;
    switch (TAC & 0x03) {
        case 0b00: divisor = 256; break;
        case 0b01: divisor = 4;   break;
        case 0b10: divisor = 16;   break;
        case 0b11: divisor = 64;  break;
    }

    int ticks = clock_cycles / divisor;
    tima_remainder = clock_cycles % divisor;

    for (int i = 0; i < ticks; i++) {
        if (TIMA == 0xFF) {
            TIMA = TMA;
            tima_overflow_pending = 4;
        } else {
            TIMA++;
        }
    }
}

/**
 * 1048576Hz is 1 M cycle
 * The timer increments every 16384Hz
 * 16384hz = 64 M (or clock_cycles)
 * Essentially, DIV will increment once every 64 M cycles
 * @returns apu_div_tick
 */
bool Timer::tick_div(int clock_cycles) {
    int total_cycles = clock_cycles + div_remainder;

    // This indicates our div will actually increment, so now we should to see if apu_div should be updated.
    // We check that be seeing if bit 4 ( 0b00010000) changed from 1 to 0
    if (total_cycles >= 64) {
        bool div_bit4_0 = DIV & 0b00010000;
        DIV++;
        bool div_bit4_1 = DIV & 0b00010000;
        if (div_bit4_0 && !div_bit4_1) {
            return true;
        }
    }

    div_remainder = total_cycles % 64;
    return false;
}

/**
 * We also need to handle the apu-div falling edge case here
 */
bool Timer::write_timer(uint16_t addr, uint8_t data) {
    if (addr == 0xFF04) {
        bool apu_div_tick = DIV & 0b00010000;
        DIV = 0x00;
        div_remainder = 0x00;
        if (apu_div_tick) return true;
    } else if (addr == 0xFF05) {
        TIMA = data;
        tima_remainder = 0x00;
    } else if (addr == 0xFF06) {
        TMA = data;
    } else if (addr == 0xFF07) {
        TAC = data & 0x0F;
    }
    return false;
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

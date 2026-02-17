#include "timer.hpp"

// NOTE 16384hz = 64 M (or clock_cycles)

void Timer::tick(int clock_cycles) {
    tick_div(clock_cycles + div_remainder);
    
    if (TAC & 0x04) {
        tick_tima(clock_cycles + tima_remainder);
    }
}

void Timer::tick_tima(int clock_cycles) {
    int tima_incremented;
    if ((TAC & 0x03) == 0b00) {
        tima_incremented = TIMA + clock_cycles / 256;
        tima_remainder = clock_cycles % 256;
    } else if ((TAC & 0x03) == 0b01) {
        tima_incremented = TIMA + clock_cycles /  4;
        tima_remainder = clock_cycles % 4;
    } else if ((TAC & 0x03) == 0b10) {
        tima_incremented = TIMA + clock_cycles /  16;
        tima_remainder = clock_cycles % 16;
    } else { // 0b11
        tima_incremented = TIMA + clock_cycles /  64;
        tima_remainder = clock_cycles % 64;
    }

    // overflow
    if (tima_incremented > 0xFF) {
        TIMA = TMA;
        //interrupt request
        interrupt = true;
    }
}

void Timer::tick_div(int clock_cycles) {
    DIV += clock_cycles / 64;
    div_remainder += clock_cycles % 64;
}

void Timer::write_timer(uint16_t addr, uint8_t data) {
    if (addr == 0xFF04) {
        DIV = 0x00;
    } else if (addr == 0xFF05) {
        TIMA = data;
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
}

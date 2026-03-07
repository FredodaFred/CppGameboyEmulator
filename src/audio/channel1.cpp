#include "channel1.hpp"

//https://gbdev.io/pandocs/Audio_Registers.html#ff10--nr10-channel-1-sweep


void Channel1::tick() {
    internal_m_cycle_counter++;

    period_tick();
}


int16_t Channel1::sample() {
    if (!DAC || !enabled) return 0x0;
    bool is_high = DUTY_TABLES[wave_duty][individual_step];
    uint8_t sound = is_high ? initial_volume : 0x0;
    return static_cast<int16_t>(sound * 500);
}


void Channel1::write_nr10(uint8_t data) {
    pace = (data >> 4) & 0x07;
    direction = ((data & 0x10) != 0) ? -1 : 1; // Direction: 0 = Addition (period increases); 1 = Subtraction (period decreases)
    individual_step = data & 0x07;
}

uint8_t Channel1::read_nr10() {
    return ((pace << 4) | (individual_step << 3) | individual_step);
}

void Channel1::write_nr11(uint8_t data) {
    initial_length_timer = data & 0b00111111;
    length_timer = initial_length_timer;
    wave_duty = data >> 6;
}

uint8_t Channel1::read_nr11() {
    uint8_t wave_duty_binary;
    if (wave_duty == 12.5) {
         wave_duty_binary = 0;
    } else if (wave_duty == 25) {
        wave_duty_binary = 0b01000000;
    } else if (wave_duty == 50) {
        wave_duty_binary = 0b10000000;
    } else {
        wave_duty_binary = 0b11000000;
    }
    return wave_duty_binary | individual_step;
}

void Channel1::write_nr12(uint8_t data) {
    initial_volume = (data >> 4);
    current_volume = initial_volume;
    env_dir =  (data & 0x08) != 0;
    sweep_pace = data & 0x07;
    DAC = (data & 0xF8) != 0;
    if (!DAC) enabled = false;
}

uint8_t Channel1::read_nr12() {
    return (individual_step << 4) | (static_cast<uint8_t>(env_dir) << 3) | sweep_pace;
}

void Channel1::write_nr13(uint8_t data) {
    period = (period & 0xFF00) | static_cast<uint16_t>(data);
}

uint8_t Channel1::read_nr13() {
    return static_cast<uint8_t>(period & 0x00FF);
}

void Channel1::write_nr14(uint8_t data) {
    period = (period | 0x00FF) | static_cast<uint16_t>(data & 0x07) << 8;
    length_timer_enable = data & 0b01000000;
    bool trigger = data & 0b10000000;
    if (trigger) this->trigger();
}

uint8_t Channel1::read_nr14() {
    return (static_cast<uint8_t>(env_dir) << 7) |
           (static_cast<uint8_t>(env_dir) << 6) |
            (static_cast<uint8_t>(period >> 8) & 0x07);
}

/**
 * Ch1 is enabled.
 * If length timer expired it is reset.
 * The period divider is set to the contents of NR13 and NR14.
 * Envelope timer is reset.
 * Volume is set to contents of NR12 initial volume.
 */
void Channel1::trigger() {
    enabled = true;
    length_timer = initial_length_timer;
    // period divider
    current_volume = initial_volume;
    internal_m_cycle_counter = 0;
}

void Channel1::length_timer_tick() {
    if (length_timer_enable) {
        if ((internal_m_cycle_counter % LENGTH_TIMER_M_CYCLES_TICK_RATE ) == 0) {
            length_timer++;
        }

        if (length_timer >= LENGTH_TIMER_M_CYCLES_TICK_RATE) {
            enabled = false;
        }
    }
}

void Channel1::period_tick() {
    if (pace != 0) {
        int sweep_rate = 1049576/ (128 * pace);
        if (internal_m_cycle_counter >= sweep_rate) {
            // remember 1 << x is the same as 2 to the power of x
            uint16_t period1 = period + direction*(period / (1 << individual_step));
            if ((direction == 1) && period1 > 0x07FF) {
                //handle overflow for addition.
                enabled = false;
            } else {
                period = period1;
            }
        }
    }
}


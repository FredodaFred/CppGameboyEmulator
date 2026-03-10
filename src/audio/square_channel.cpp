#include "square_channel.hpp"

//https://gbdev.io/pandocs/Audio_Registers.html#ff10--nr10-channel-1-sweep


void SquareChannel::tick() {
    period_div++; // period dividers are clocked at 1048576 Hz (1 M Cycle)
    if (period_div >= (2048 - period)) {
        period_div = 0;
        duty_step = (duty_step + 1) & 0x7;
    }
}


int16_t SquareChannel::sample() {
    if (!DAC || !enabled) return 0x0;
    bool is_high = SQUARE_WAVE_DUTY[wave_duty][duty_step];
    int16_t volume_output = is_high ? static_cast<int16_t>(current_volume) : -static_cast<int16_t>(current_volume);
    return volume_output*600;
}

/**
 * Ch1 is enabled.
 * If length timer expired it is reset.
 * The period divider is set to the contents of NR13 and NR14.
 * Envelope timer is reset.
 * Volume is set to contents of NR12 initial volume.
 */
void SquareChannel::trigger() {
    if (!DAC) {
        enabled = false;
        return;
    }

    enabled = true;

    if (length_timer >= LENGTH_TIMER_MAX) {
        length_timer = initial_length_timer;
    }

    period_div = 0;
    current_volume = initial_volume;
    internal_env_sweep_pace_counter = 0;
    pace_counter = (pace == 0) ? 8 : pace;
}

void SquareChannel::length_timer_tick() {
    if (length_timer_enable) {
        length_timer++;
        if (length_timer >= LENGTH_TIMER_MAX) {
            enabled = false;
        }
    }
}

/**
 * Handles period sweep and period
 * This function is called every 128 Hz
 */
void SquareChannel::period_sweep_tick() {
    if (pace == 0) return;
    pace_counter--;
    if (pace_counter == 0) {
        // direction is -1 or 1, the bit shift represents dividing period by a power of 2 indicated by step
        if (individual_step > 0) {
            uint16_t period0 = !direction ? (period + direction*(period >> individual_step)) : (period - direction*(period >> individual_step));

            if (!direction && period0 > 0x07FF) {
                enabled = false;
            } else {
                period = period0;
            }
        }
        pace_counter = pace;
    }
}

void SquareChannel::volume_envelope_tick() {
    if (env_sweep_pace == 0) return;

    internal_env_sweep_pace_counter--;
    if (internal_env_sweep_pace_counter <= 0) {
        if (!env_dir  && current_volume < 15) {
            current_volume++;
        } else if (env_dir && current_volume > 0) {
            current_volume--;
        }
        internal_env_sweep_pace_counter = env_sweep_pace;
    }
}

void SquareChannel::write_nrx0(uint8_t data) {
    pace = (data >> 4) & 0x07;
    pace_counter = pace;
    direction = data & 0x10; // Direction: 0 = Addition (period increases); 1 = Subtraction (period decreases)
    individual_step = data & 0x07;
}

uint8_t SquareChannel::read_nrx0() {
    return ((pace << 4) | (direction << 3) | individual_step);
}

void SquareChannel::write_nrx1(uint8_t data) {
    initial_length_timer = data & 0b00111111;
    length_timer = initial_length_timer;
    wave_duty = data >> 6;
}

uint8_t SquareChannel::read_nrx1() {
    uint8_t wave_duty_binary;
    if (wave_duty == 0) {
         wave_duty_binary = 0;
    } else if (wave_duty == 1) {
        wave_duty_binary = 0b01000000;
    } else if (wave_duty == 2) {
        wave_duty_binary = 0b10000000;
    } else {
        wave_duty_binary = 0b11000000;
    }
    return wave_duty_binary | individual_step;
}

void SquareChannel::write_nrx2(uint8_t data) {
    initial_volume = (data >> 4);
    current_volume = initial_volume;
    env_dir = data & 0x08; // The envelope’s direction; 0 = decrease volume over time, 1 = increase volume over time.
    env_sweep_pace = data & 0x07;
    DAC = (data & 0xF8) != 0;
    if (!DAC) enabled = false;
}

uint8_t SquareChannel::read_nrx2() {
    return (initial_volume << 4) | (static_cast<uint8_t>(env_dir) << 3) | env_sweep_pace;
}

void SquareChannel::write_nrx3(uint8_t data) {
    period = (period & 0xFF00) | static_cast<uint16_t>(data);
}

uint8_t SquareChannel::read_nrx3() {
    return static_cast<uint8_t>(period & 0x00FF);
}

void SquareChannel::write_nrx4(uint8_t data) {
    period = (period & 0x00FF) | (static_cast<uint16_t>(data & 0x07) << 8);
    length_timer_enable = data & 0b01000000;
    trigger_val = data & 0b10000000;
    if (trigger_val) this->trigger();
}

uint8_t SquareChannel::read_nrx4() {
    return (static_cast<uint8_t>(trigger_val) << 7) |
           (static_cast<uint8_t>(length_timer_enable) << 6) |
            (static_cast<uint8_t>(period >> 8) & 0x07);
}

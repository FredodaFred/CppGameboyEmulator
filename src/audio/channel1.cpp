#include "channel1.hpp"

//https://gbdev.io/pandocs/Audio_Registers.html#ff10--nr10-channel-1-sweep


void Channel1::tick() {
    period_sweep_tick();
    length_timer_tick();
    volume_envelope_tick();
    period_div++; // period dividers are clocked at 1048576 Hz (1 M Cycle)
    if (period_div >= 2048) {
        // when it overflows (being clocked when it’s already 2047, or $7FF), its value is set from the contents of NR13 and NR14.
        period_div = period;
        duty_step = (duty_step + 1) & 0x7;
    }
}


int16_t Channel1::sample() {
    if (!DAC || !enabled) return 0x0;
    bool is_high = DUTY_TABLES[wave_duty][duty_step];
    int16_t volume_output = is_high ? static_cast<int16_t>(current_volume) : -static_cast<int16_t>(current_volume);
    return volume_output * 500;
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
    env_dir =  ((data & 0x08) == 0 ) ? -1 : 1; // The envelope’s direction; 0 = decrease volume over time, 1 = increase volume over time.
    env_sweep_pace = data & 0x07;
    internal_env_sweep_pace_counter = env_sweep_pace;
    DAC = (data & 0xF8) != 0;
    if (!DAC) enabled = false;
}

uint8_t Channel1::read_nr12() {
    return (individual_step << 4) | (static_cast<uint8_t>(env_dir) << 3) | env_sweep_pace;
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
    period_div = period;
    current_volume = initial_volume;
    internal_m_cycle_counter_period_sweep = 0;
}

void Channel1::length_timer_tick() {
    if (length_timer_enable) {
        internal_m_cycle_counter_length_enable++;
        if ((internal_m_cycle_counter_length_enable == LENGTH_TIMER_M_CYCLES_TICK_RATE ) == 0) {
            length_timer++;
        }

        if (length_timer >= LENGTH_TIMER_M_CYCLES_TICK_RATE) {
            enabled = false;
        }
    } else {
        internal_m_cycle_counter_length_enable = 0;
    }
}

/**
 * Handles period sweep and period
 */
void Channel1::period_sweep_tick() {
    internal_m_cycle_counter_period_sweep++;
    int sweep_rate = 1048576/ (128 * pace);
    if (internal_m_cycle_counter_period_sweep >= sweep_rate) {
        internal_m_cycle_counter_period_sweep -= sweep_rate;
        if (pace == 0) return;
        // remember the bit shift represents dividing by a power of 2
        uint16_t period1 = period + direction*(period_div >> individual_step);
        if ((direction == 1) && period1 > 0x07FF) {
            //handle overflow for addition.
            enabled = false;
        } else {
            period = period1;
        }
    }
}

void Channel1::volume_envelope_tick() {
    internal_m_cycle_counter_env++;
    if (internal_m_cycle_counter_env >= ENV_RATE_M_CYCLES) {
        internal_m_cycle_counter_env -= ENV_RATE_M_CYCLES;
        if (env_sweep_pace == 0) return;
        internal_env_sweep_pace_counter--;
        if (internal_env_sweep_pace_counter == 0) {
            // 4 bits store volume. Volume can be 0 to 15. These are the bounds checks
            if (!((current_volume + env_dir) > 15 && env_dir == 1)) {
                current_volume += env_dir;
            } else if (!((current_volume + env_dir) < 0 && env_dir == -1)) {
                current_volume += env_dir;
            }

            internal_env_sweep_pace_counter = env_sweep_pace;
        }
    }
}


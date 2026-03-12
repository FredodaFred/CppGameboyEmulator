#include "noise_channel.hpp"

void NoiseChannel::tick() {
    // this is clocked 1/4 an M cycle
    int tick_rate = (clock_div == 0 ? 8 : clock_div * 16) << clock_shift;

    div++;
    if (div >= tick_rate) {
        div = 0;
        update_lsfr();
    }

}

int16_t NoiseChannel::sample() {
    if (!DAC || !enabled) return 0x0;
    bool is_high = last_right_bit;
    int16_t volume_output = is_high ? static_cast<int16_t>(current_volume) : -static_cast<int16_t>(current_volume);
    return volume_output*600;
}

void NoiseChannel::trigger() {
    if (!DAC) {
        enabled = false;
        return;
    }
    if (length_timer >= LENGTH_TIMER_MAX) {
        length_timer = initial_length_timer;
    }

    enabled = true;
    internal_env_sweep_pace_counter = 0;
    current_volume = initial_volume;
    lsfr = 0x0;
}

void NoiseChannel::update_lsfr() {
    uint8_t lsfr0 = lsfr & 0x01;
    uint8_t lsfr1 = (lsfr & 0x02) >> 1;
    if (lsfr0 == lsfr1) {
        //write 1 to bit 15
        lsfr |= 0x8000;
        // bit 7
        if (lsfr_width) {
            lsfr |= 0x0080;
        }
    } else {
        //write 0 to bit 15
        lsfr &= 0x7FFF;
        // bit 7
        if (lsfr_width) {
            lsfr &= 0xFF7F;
        }
    }
    lsfr = lsfr >> 1;
    last_right_bit = lsfr & 0x01;
}

void NoiseChannel::length_timer_tick() {
    if (length_timer_enable) {
        length_timer++;
        if (length_timer >= LENGTH_TIMER_MAX) {
            enabled = false;
        }
    }
}

void NoiseChannel::env_sweep_tick() {
    if (env_sweep_pace == 0) return;

    internal_env_sweep_pace_counter++;
    if (internal_env_sweep_pace_counter >= env_sweep_pace) {
        internal_env_sweep_pace_counter = 0;
        if (env_dir) {
            if (current_volume < 15) current_volume++;
        } else {
            if (current_volume > 0) current_volume--;
        }
    }
}

void NoiseChannel::write_nr41(uint8_t data) {
    initial_length_timer = data & 0b00111111;
}

void NoiseChannel::write_nr42(uint8_t data) {
    initial_volume = (data >> 4);
    env_dir = (data >> 3) & 0x01; // The envelope’s direction; 0 = decrease volume over time, 1 = increase volume over time.
    env_sweep_pace = data & 0x07;
    internal_env_sweep_pace_counter = env_sweep_pace;
    DAC = (data & 0xF8) != 0;
    if (!DAC) enabled = false;
}

uint8_t NoiseChannel::read_nr42() {
    return (initial_volume << 4) | (static_cast<uint8_t>(env_dir) << 3) | env_sweep_pace;
}

void NoiseChannel::write_nr43(uint8_t data) {
    clock_div = data & 0b00000111;
    lsfr_width = data & 0b00001000;
    clock_shift = data >> 4;
}

uint8_t NoiseChannel::read_nr43() {
    return clock_shift << 4 | lsfr_width << 3 | clock_div;
}

void NoiseChannel::write_nr44(uint8_t data) {
    length_timer_enable = data & 0b01000000;
    if (length_timer_enable && length_timer >= LENGTH_TIMER_MAX) {
        enabled = false;
    }
    trigger_val = data & 0b10000000;
    if (trigger_val) {
        trigger();
    }
}

uint8_t NoiseChannel::read_nr44() {
    return trigger_val | length_timer_enable;
}

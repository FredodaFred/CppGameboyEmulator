#include "wave_channel.hpp"

void WaveChannel::tick() {
    period_div++; // period dividers are clocked at 1048576 Hz (1 M Cycle)
    if (period_div >= (2048 - period)/2) { //
        period_div = 0;
        wave_index = (wave_index + 1) & 0x1F;  // & 0x1F = % 32
    }
}

int16_t WaveChannel::sample() {
    if (!DAC || !enabled) return 0x0;
    uint8_t sample = get_sample();
    // Apply volume control
    if (output_level == 0) {
        sample = 0;
    } else {
        // think of this like dividing to the correct output. output level 1, max vol (100%), then 50%, then 25%
        sample = sample >> (output_level - 1);
    }
    // since we are at a range of 0-15, we can't just use a is_high, we need to scale here too
    // 7.5 is the middle of 0-15, so subtracting 8 centers the wave.
    int16_t volume_output = (static_cast<int16_t>(sample) - 8);
    return volume_output*600;
}

uint8_t WaveChannel::get_sample() {
    uint8_t byte  = WAVE_RAM[wave_index/2];
    // Read upper nibble
    if (wave_index % 2 == 0) {
        return byte >> 4;
    }
    // Read lower nibble
    return byte & 0x0F;
}

void WaveChannel::length_timer_tick() {
    if (length_timer_enable) {
        length_timer++;
        if (length_timer >= LENGTH_TIMER_MAX) {
            enabled = false;
        }
    }
}

void WaveChannel::trigger() {
    enabled = true;
    if (length_timer >= LENGTH_TIMER_MAX) {
        length_timer = initial_length_timer;
    }
    period_div = period;
}

uint8_t WaveChannel::read_nr30() {
    return static_cast<uint8_t>(DAC)<< 7;
}

void WaveChannel::write_nr30(uint8_t data) {
    DAC = data >> 7;
    if (!DAC) enabled = false;
}

uint8_t WaveChannel::read_nr31() {
    return initial_length_timer;

}
void WaveChannel::write_nr31(uint8_t data) {
    initial_length_timer = data;
}
uint8_t WaveChannel::read_nr32() {
    return output_level << 5;

}
void WaveChannel::write_nr32(uint8_t data) {
    output_level = (data << 1) >> 6;
}

uint8_t WaveChannel::read_nr33() {
    return static_cast<uint8_t>(period & 0x00FF);

}
void WaveChannel::write_nr33(uint8_t data) {
    period = (period & 0xFF00) | static_cast<uint16_t>(data);
}

void WaveChannel::write_nr34(uint8_t data) {
    period = (period & 0x00FF) | (static_cast<uint16_t>(data & 0x07) << 8);
    length_timer_enable = data & 0b01000000;
    trigger_val = data & 0b10000000;
    if (trigger_val) this->trigger();
}

uint8_t WaveChannel::read_nr34() {
    return (static_cast<uint8_t>(trigger_val) << 7) |
           (static_cast<uint8_t>(length_timer_enable) << 6) |
            (static_cast<uint8_t>(period >> 8) & 0x07);
}

uint8_t WaveChannel::read_WRAM(uint16_t addr) {
    return WAVE_RAM[addr - 0xFF30];
}

void WaveChannel::write_WRAM(uint16_t addr, uint8_t data) {
    WAVE_RAM[addr - 0xFF30] = data;
}

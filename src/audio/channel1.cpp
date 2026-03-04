#include "channel1.hpp"

//https://gbdev.io/pandocs/Audio_Registers.html#ff10--nr10-channel-1-sweep
void Channel1::write_nr10(uint8_t data) {
    pace = (data >> 4) & 0x07;
    direction = (data & 0x10) != 0;
    individual_step = data & 0x07;
}

uint8_t Channel1::read_nr10() {
    return ((pace << 4) | (individual_step << 3) | individual_step);
}

void Channel1::write_nr11(uint8_t data) {
    initial_timer_length = data & 0b00111111;
    uint8_t val = data >> 6;
    switch (val) {
        case(0b00): wave_duty =  12.5; break;
        case(0b01): wave_duty =  25; break;
        case(0b10): wave_duty =  50; break;
        case(0b11): wave_duty =  75; break;
        default: throw std::runtime_error("Invalid value wave duty");
    }
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
    env_dir =  (data & 0x08) != 0;
    sweep_pace = data & 0x07;
    DAC = (data & 0xF8) != 0;
}

uint8_t Channel1::read_nr12() {
    return (individual_step << 4) | (static_cast<uint8_t>(env_dir) << 3) | sweep_pace;
}

void Channel1::write_nr13(uint8_t data) {
    period_value = (period_value & 0xFF00) | static_cast<uint16_t>(data);
}

uint8_t Channel1::read_nr13() {
    return static_cast<uint8_t>(period_value & 0x00FF);
}

void Channel1::write_nr14(uint8_t data) {
    period_value = (period_value | 0x00FF) | static_cast<uint16_t>(data & 0x07) << 8;
    length_enable = data & 0b01000000;
    trigger = data & 0b10000000;
}

uint8_t Channel1::read_nr14() {
    return (static_cast<uint8_t>(env_dir) << 7) |
           (static_cast<uint8_t>(env_dir) << 6) |
            (static_cast<uint8_t>(period_value >> 8) & 0x07);
}


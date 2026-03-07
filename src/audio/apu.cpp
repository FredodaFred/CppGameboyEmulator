#include "apu.hpp"

#include <stdexcept>

/**
 * To do our calculations for the channels, we need to translate M cycles to Hz.
 *
 * 1 M-Cycle runs at the frequency of the System clock.
 * Therefore 1048576 hz is one M Cycle
 *
 * https://gbdev.io/pandocs/Specifications.html#dmg_clk
 * https://oh4.co/site/gameboy.html
 */


APU::APU(Speaker& speaker) : speaker(speaker) {};

void APU::init() {
    //write values for PC = 0x0100
    channel1.write_nr10(0x80);
    channel1.write_nr11(0xBF);
    channel1.write_nr12(0xF3);
    channel1.write_nr13(0xFF);
    channel1.write_nr14(0xBF);
    speaker.init();
}

void APU::tick(int cycle, bool apu_div_tick) {
    if (apu_div_tick) apu_div++;

    bool enabled = check_master_enable();
    if (!enabled) {
        speaker.pause();
    } else {
        speaker.unpause();
    }

    for (int i = 0; i < cycle; i++) {
        tick_cycle();
    }
}

void APU::tick_cycle() {

    // collect samples
     sample_accumulator += 1.0;
    if (sample_accumulator >= SAMPLE_RATE) {
        mix_and_sample();
        sample_accumulator -=  SAMPLE_RATE;
    }
}

void APU::mix_and_sample() {
    int16_t left_stereo = 0;
    int16_t right_stereo = 0;
    int16_t ch1_sample = channel1.sample();

    // Check NR51 for left and right enables
    if (nr51 & 0x10) left_stereo += ch1_sample; // Bit 4: Ch1 Left
    if (nr51 & 0x01) right_stereo += ch1_sample; // Bit 0: Ch1 Right
    speaker.play_sample(left_stereo, right_stereo);

}

uint8_t APU::apu_io_read(uint16_t addr) {
    switch (addr) {

        case 0xFF10: return channel1.read_nr10();
        case 0xFF11: return channel1.read_nr11();
        case 0xFF12: return channel1.read_nr12();
        case 0xFF13: return channel1.read_nr13();
        case 0xFF14: return channel1.read_nr14();

        case 0xFF16: return nr21;
        case 0xFF17: return nr22;
        case 0xFF18: return nr23;
        case 0xFF19: return nr24;

        case 0xFF1A: return nr30;
        case 0xFF1B: return nr31;
        case 0xFF1C: return nr32;
        case 0xFF1D: return nr33;
        case 0xFF1E: return nr34;

        case 0xFF20: return nr41;
        case 0xFF21: return nr42;
        case 0xFF22: return nr43;
        case 0xFF23: return nr44;

        case 0xFF24: return nr50;
        case 0xFF25: return nr51;
        case 0xFF26: return nr52;
        default:
    }

    if (addr  >= 0xFF30 && addr <= 0xFF3F) {
        return WAVE_RAM[addr - 0xFF30];
    }
}
void APU::apu_io_write(uint16_t addr, uint8_t data) {
    switch (addr) {

        case 0xFF10: channel1.write_nr10(data); break;
        case 0xFF11: channel1.write_nr11(data); break;
        case 0xFF12: channel1.write_nr12(data); break;
        case 0xFF13: channel1.write_nr13(data); break;
        case 0xFF14: channel1.write_nr14(data); break;

        case 0xFF16: nr21 = data; break;
        case 0xFF17: nr22 = data; break;
        case 0xFF18: nr23 = data; break;
        case 0xFF19: nr24 = data; if (nr24 & 0x80) break;

        case 0xFF1A: nr30 = data; break;
        case 0xFF1B: nr31 = data; break;
        case 0xFF1C: nr32 = data; break;
        case 0xFF1D: nr33 = data; break;
        case 0xFF1E: nr34 = data; break; // write to MSB triggers a channel

        case 0xFF20: nr41 = data; break;
        case 0xFF21: nr42 = data; break;
        case 0xFF22: nr43 = data; break;
        case 0xFF23: nr44 = data; break;

        case 0xFF26: nr52 = data; break;
        case 0xFF25: nr51 = data; break;
        case 0xFF24: nr50 = data; break;
        default:
    }

    if (addr  >= 0xFF30 && addr <= 0xFF3F) {
        WAVE_RAM[addr - 0xFF30] = data;
    }
}

bool APU::check_master_enable() const {
    return (nr52 & 0x80) != 0;
}
#include "apu.hpp"

#include <stdexcept>

// NOTE 1 M (clock cycle) = 256hz

void APU::init() {
    //write values for PC = 0x0100
    channel1.write_nr10(0x80);
    channel1.write_nr11(0xBF);
    channel1.write_nr12(0xF3);
    channel1.write_nr13(0xFF);
    channel1.write_nr14(0xBF);
}

void APU::tick(int cycle) {
    for (int i = 0; i < cycle; i++) {
        tick_cycle();
    }
}

void APU::tick_cycle() {
    // uint8_t ch1_sample = channel1.sample();
    // mixer(); // mixes samples form all 4 channels
    // //speaker.play()
    sample_timer++;
    if (sample_timer == SAMPLE_RATE) {
        sample_timer = 0;
    }
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
        case 0xFF19: nr24 = data; if (nr24 & 0x80) trigger_channel2(); break;

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

void APU::toggleDACs() {
    DAC2 = (nr22 & 0xF8) != 0;
    DAC3 = (nr30 & 0x20) != 0; // bit 7 of nr30 instead
    DAC4 = (nr42 & 0xF8) != 0;
}

/**
* Trigger (Write-only): Writing any value to NR14 with this bit set triggers the channel, causing the following to occur:

    Ch1 is enabled.
    If length timer expired it is reset.
    The period divider is set to the contents of NR13 and NR14.
    Envelope timer is reset.
    Volume is set to contents of NR12 initial volume.
    Sweep does several things.
    Length enable (Read/Write): Takes effect immediately upon writing to this register.

    Period (Write-only): The upper 3 bits of the period value; the lower 8 bits are stored in NR13.

 */
void APU::trigger_channel1() {
    ch1_enable = true;
}
void APU::play_channel1() {
    if (DAC1 && ch1_enable && check_master_enable()) { // length master checks
        //speaker play ()
    }
}

void APU::trigger_channel2() {
    ch2_enable = true;
}

bool APU::check_master_enable() const {
    return (nr52 & 0x80) != 0;
}
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
    channel1.write_nrx0(0x80);
    channel1.write_nrx1(0xBF);
    channel1.write_nrx2(0xF3);
    channel1.write_nrx3(0xFF);
    channel1.write_nrx4(0xBF);

    channel2.write_nrx1(0x3F);
    channel2.write_nrx2(0x00);
    channel2.write_nrx3(0xFF);
    channel2.write_nrx4(0xBF);

    channel3.write_nr30(0x7F);
    channel3.write_nr31(0xFF);
    channel3.write_nr32(0x9F);
    channel3.write_nr33(0xFF);
    channel3.write_nr34(0xBF);

    channel4.write_nr41(0xFF);
    channel4.write_nr42(0x00);
    channel4.write_nr43(0x00);
    channel4.write_nr44(0xBF);

    speaker.init();
}

void APU::tick(int cycle, bool apu_div_tick) {
    if (apu_div_tick) {
        apu_div++;
        // every 2 ticks
        if (apu_div % 2 == 0) {
            channel1.length_timer_tick();
            channel2.length_timer_tick();
            channel3.length_timer_tick();
            channel4.length_timer_tick();
        }

        // Sweep (128 Hz) every 4 ticks
        if (apu_div == 2 || apu_div == 6) {
            channel1.period_sweep_tick();
        }

        // Envelope (64 Hz) every 8 ticks
        if (apu_div == 7) {
            channel1.volume_envelope_tick();
            channel2.volume_envelope_tick();
            channel4.env_sweep_tick();
            apu_div = 0;
        }
    }

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
    channel1.tick();
    channel2.tick();
    channel3.tick();
    channel4.tick();

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
    int16_t ch2_sample = channel2.sample();
    int16_t ch3_sample = channel3.sample();
    int16_t ch4_sample = channel4.sample();

    uint8_t master_volume_right = nr50 & 0x07; // first 3 bits
    uint8_t master_volume_left = ((nr50 & (0b01110000)) >> 4); //value of 6,5,4

    bool vin_right = nr50 & 0x08; // value 4th bit
    bool vin_left = (nr50 & 0b10000000); //MSB

    // Check NR51 and NR50 for left and right enables
    if (nr51 & 0x10) left_stereo += ch1_sample; // Bit 4: Ch1 Left
    if (nr51 & 0x01) right_stereo += ch1_sample; // Bit 0: Ch1 Right

    if (nr51 & 0x20) left_stereo += ch2_sample; // Bit 5: Ch2 Left
    if (nr51 & 0x02) right_stereo += ch2_sample; // Bit 1: Ch2 Right

    if (nr51 & 0x40) left_stereo += ch3_sample; // Bit 6: Ch3 Left
    if (nr51 & 0x04) right_stereo += ch3_sample; // Bit 2: Ch3 Right

    if (nr51 & 0x80) left_stereo += ch4_sample; // Bit 7: Ch4 Left
    if (nr51 & 0x08) right_stereo += ch4_sample; // Bit 3: Ch4 Right

    // apply nr50
    // for 2 channels max is 1200, so each vale
    left_stereo = (left_stereo * (master_volume_left + 1)) / 8;
    right_stereo = (right_stereo * (master_volume_right + 1)) / 8;
    speaker.play_sample(left_stereo, right_stereo);
}

uint8_t APU::apu_io_read(uint16_t addr) {
    switch (addr) {

        case 0xFF10: return channel1.read_nrx0();
        case 0xFF11: return channel1.read_nrx1();
        case 0xFF12: return channel1.read_nrx2();
        case 0xFF13: return channel1.read_nrx3();
        case 0xFF14: return channel1.read_nrx4();

        case 0xFF16: return channel2.read_nrx1();
        case 0xFF17: return channel2.read_nrx2();
        case 0xFF18: return channel2.read_nrx3();
        case 0xFF19: return channel2.read_nrx4();

        case 0xFF1A: return channel3.read_nr30();
        case 0xFF1B: return channel3.read_nr31();
        case 0xFF1C: return channel3.read_nr32();
        case 0xFF1D: return channel3.read_nr33();
        case 0xFF1E: return channel3.read_nr34();

            // nr_41 is write only
        case 0xFF21: return channel4.read_nr42();
        case 0xFF22: return channel4.read_nr43();
        case 0xFF23: return channel4.read_nr42();

        case 0xFF24: return nr50;
        case 0xFF25: return nr51;
        case 0xFF26: return nr52;
        default:
    }

    if (addr >= 0xFF30 && addr <= 0xFF3F) {
        return channel3.read_WRAM(addr);
    }
}

void APU::apu_io_write(uint16_t addr, uint8_t data) {
    switch (addr) {

        case 0xFF10: channel1.write_nrx0(data); break;
        case 0xFF11: channel1.write_nrx1(data); break;
        case 0xFF12: channel1.write_nrx2(data); break;
        case 0xFF13: channel1.write_nrx3(data); break;
        case 0xFF14: channel1.write_nrx4(data); break;

        case 0xFF16: channel2.write_nrx1(data); break;
        case 0xFF17: channel2.write_nrx2(data); break;
        case 0xFF18: channel2.write_nrx3(data); break;
        case 0xFF19: channel2.write_nrx4(data); break;

        case 0xFF1A: channel3.write_nr30(data); break;
        case 0xFF1B: channel3.write_nr31(data); break;
        case 0xFF1C: channel3.write_nr32(data); break;
        case 0xFF1D: channel3.write_nr33(data); break;
        case 0xFF1E: channel3.write_nr34(data); break;

        case 0xFF20: channel4.write_nr41(data); break;
        case 0xFF21: channel4.write_nr42(data); break;
        case 0xFF22: channel4.write_nr43(data); break;
        case 0xFF23: channel4.write_nr44(data); break;

        case 0xFF26: nr52 = data; break;
        case 0xFF25: nr51 = data; break;
        case 0xFF24: nr50 = data; break;
        default:
    }

    if (addr  >= 0xFF30 && addr <= 0xFF3F) {
        channel3.write_WRAM(addr, data);
    }
}

bool APU::check_master_enable() const {
    return (nr52 & 0x80) != 0;
}

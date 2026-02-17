#pragma once
#include <cstdint>
#include <algorithm>

//https://gbdev.io/pandocs/Rendering.html#rendering-overview
enum Mode {
    VBLANK, HBLANK, DRAW, OAM_SCAN
};

class PPU {
    public:
        PPU() = default;
        void tick(int clock_cycles);
        void write_vram(uint16_t addr, uint8_t data);
        uint8_t read_vram(uint16_t addr);
        void write_oam(uint16_t addr, uint8_t data);
        uint8_t read_oam(uint16_t addr);
        Mode mode{HBLANK};
    private:
        uint8_t VRAM[8192] = {0};
        uint8_t OAM[0x9F] = {0};
};
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
        void writeVRAM(uint16_t addr, uint8_t data);
        uint8_t readVRAM(uint16_t addr);
        Mode mode{HBLANK};
    private:
        uint8_t VRAM[8192] = {0};
};
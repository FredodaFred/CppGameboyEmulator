#include "ppu.hpp"


void PPU::writeVRAM(uint16_t addr, uint8_t data) {
    if (this->mode == DRAW) {
        // ignore writes while drawing to LCD screen
        return;
    }
    VRAM[addr - 0x8000] = data;
}

uint8_t PPU::readVRAM(uint16_t addr) {
        if (this->mode == DRAW) {
        // return garbage data if drawing to ignore
        return 0xFF;
    }
    return VRAM[addr - 0x8000];
}
#include "ppu.hpp"

void PPU::tick(int clock_cycles) {

}

void PPU::write_vram(uint16_t addr, uint8_t data) {
    if (mode == DRAW) {
        // ignore writes while drawing to LCD screen
        return;
    }
    VRAM[addr - 0x8000] = data;
}

uint8_t PPU::read_vram(uint16_t addr) {
        if (mode == DRAW) {
        // return garbage data if drawing to ignore
        return 0xFF;
    }
    return VRAM[addr - 0x8000];
}

void PPU::write_oam(uint16_t addr, uint8_t data) {
    OAM[addr - 0xFE00] = data;
}


uint8_t PPU::read_oam(uint16_t addr) {
    return OAM[addr - 0xFE00];
}


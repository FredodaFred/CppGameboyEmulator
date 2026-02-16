#pragma once
#include <cstdint>
#include "cart.hpp"
#include "ppu.hpp"

class Bus {
    public:
        Bus(Cart& cart, PPU& ppu);
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);

    private:
        Cart cart;
        PPU ppu;

        // Flags 
        // https://gbdev.io/pandocs/Interrupts.html#ffff--ie-interrupt-enable
        uint8_t ie{0};
        uint8_t if_reg{0};
        uint8_t serial_data[2];

        //RAM
        uint8_t WRAM[0x2000]{0};
        uint8_t HRAM[0x80]{0};
        uint8_t wram_read(uint16_t addr);
        void wram_write(uint16_t addr, uint8_t data);
        uint8_t hram_read(uint16_t addr);
        void hram_write(uint16_t addr, uint8_t data);

        // IO
        void write_io(uint16_t addr);
        uint8_t read_io(uint16_t addr, uint8_t data);

};
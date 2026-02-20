#include "bus.hpp"

Bus::Bus(Cart& cart, PPU& ppu, Timer& timer)
    : cart(cart), ppu(ppu), timer(timer)
{}

// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM / VRAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page
uint8_t Bus::read(uint16_t addr){
    if(addr < 0x8000) {
        return cart.read(addr); 
    } else if (addr < 0xA000) {
        return ppu.read_vram(addr);
    } else if (addr < 0xC000) {
        return cart.read(addr);
    }  else if (addr < 0xE000) {
        return wram_read(addr);
    } else if (addr < 0xFE00) {
        // echo ram
    } else if (addr < 0xFEA0) {
        // OAM
    }  else if (addr < 0xFF00) {
        // unuseable
    } else if (addr < 0xFF80) {  // IO registers
       read_io(addr);

    } else if (addr < 0xFFFF) {
        return hram_read(addr);
    } else if (addr == 0xFFFF) {
        return ie;
    } else {
        throw std::runtime_error("Invalid Memory Address");
    }
}

void Bus::write(uint16_t addr, uint8_t data){

    if(addr < 0x8000) {
        cart.write(addr, data); 
    } else if (addr < 0xA000) {
        ppu.write_vram(addr, data);
    } else if (addr < 0xC000) {
        cart.write(addr,data );
    }  else if (addr < 0xE000) {
        wram_write(addr, data);
    } else if (addr < 0xFE00) {
        // echo ram
    } else if (addr < 0xFEA0) {
        // OAM
    }  else if (addr < 0xFF00) {
        // unuseable
    } else if (addr < 0xFF80) {
        write_io(addr, data);
    } else if (addr < 0xFFFF) {
        hram_write(addr, data);
    } else if (addr == 0xFFFF) {
        ie = data;
    } else {
        throw std::runtime_error("Invalid Memory Address");
    }
}

uint8_t Bus::wram_read(uint16_t addr){
    addr -= 0xC000;
    return WRAM[addr];
}

void Bus::wram_write(uint16_t addr, uint8_t data){
    addr -= 0xC000;
    WRAM[addr] = data;
}

uint8_t Bus::hram_read(uint16_t addr){
    addr -= 0xFF80;
    return HRAM[addr];
}

void Bus::hram_write(uint16_t addr, uint8_t data){
    addr -= 0xFF80;
    HRAM[addr] = data;
}

void Bus::write_io(uint16_t addr, uint8_t data) {
    if (addr == 0xFF0F) {
        if_reg = data;
    } else if (addr == 0xFF01) {
        serial_data[0] = data;
        std::cout << data; //Prints for debugging / BLAARG testts
    } else if (addr == 0xFF02) {
        serial_data[1] = data;
    } else if (addr >= 0xFF04 && addr < 0xFF08) { //TIMER
        timer.write_timer(addr, data);
    } else if (addr >= 0xFF40 && addr <= 0xFF4B) {
        ppu.ppu_io_registers_write(addr, data);
    }
}

uint8_t Bus::read_io(uint16_t addr) {
    if (addr == 0xFF0F) {
        return this->if_reg;
    } else if (addr == 0xFF01) {
        return serial_data[0];
    } else if (addr == 0xFF02) {
        return serial_data[1];
    } else if (addr >= 0xFF04 && addr < 0xFF08) { //TIMER
        return this->timer.read_timer(addr);
    } else if (addr >= 0xFF40 && addr <= 0xFF4B) {
        return this->ppu.ppu_io_read(addr);
    }
}
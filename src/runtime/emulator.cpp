#include "emulator.hpp"

Emulator::Emulator(CPU& cpu, Bus& bus, Timer& timer, PPU& ppu, Screen& screen, APU& apu)
    : cpu(cpu), bus(bus), timer(timer), ppu(ppu), screen(screen), apu(apu)
{}

void Emulator::tick() {
    uint8_t cycles = cpu.step();
        
    // all other components sync
    ppu.tick(cycles);
    screen.tick();
    timer.tick(cycles);
    apu.tick(cycles);

    //Handle interrupt requests from components
    if (ppu.vblank_interrupt) {
        uint8_t if_val = bus.read(0xFF0F);
        bus.write(0xFF0F, if_val | Interrupt::VBLANK);
        ppu.vblank_interrupt = false;
    }
    if (ppu.lcd_stat_interrupt) {
        uint8_t if_val = bus.read(0xFF0F);
        bus.write(0xFF0F, if_val | Interrupt::LCD_STAT);
        ppu.lcd_stat_interrupt = false;
    }
    if (timer.interrupt) {
        uint8_t if_val = bus.read(0xFF0F);
        bus.write(0xFF0F, if_val | Interrupt::TIMER);
        timer.interrupt = false;
    }

    if (Joypad::interrupt) {
        uint8_t if_val = bus.read(0xFF0F);
        bus.write(0xFF0F, if_val | Interrupt::JOYPAD);
        Joypad::interrupt = false;
    }
}

void Emulator::run() {
    while (true) {
        this->tick();
    }
}
 
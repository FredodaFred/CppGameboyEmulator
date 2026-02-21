#include "emulator.hpp"

Emulator::Emulator(CPU& cpu, Bus& bus, Timer& timer, PPU& ppu, Screen& screen, DbgWindow& dbg_window)
    : cpu(cpu), bus(bus), timer(timer), ppu(ppu), screen(screen), dbg_window(dbg_window)
{}

void Emulator::tick() {
    uint8_t cycles = cpu.step();
        
    // all other components sync
    timer.tick(cycles);
    ppu.tick(cycles);
    screen.tick();

    //Handle interrupt requests from components
    if (ppu.vblank_interrupt) {
        uint8_t if_val = bus.read(0xFF0F);
        bus.write(0xFF0F, if_val | Interrupt::VBLANK);
    } else if (ppu.lcd_stat_interrupt != ppu.prev_lcd_stat_interrupt && ppu.lcd_stat_interrupt) {
        uint8_t if_val = bus.read(0xFF0F);
        bus.write(0xFF0F, if_val | Interrupt::LCD_STAT);
    } else if (timer.interrupt) {
        timer.interrupt = false;
        uint8_t if_val = bus.read(0xFF0F);
        bus.write(0xFF0F, if_val | Interrupt::TIMER);
    }
}

void Emulator::run() {
    while (true) {
        this->tick();
    }
}
 
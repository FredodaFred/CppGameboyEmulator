#include "emulator.hpp"

Emulator::Emulator(CPU& cpu, Bus& bus, Timer& timer, PPU& ppu, Screen& screen, APU& apu)
    : cpu(cpu), bus(bus), timer(timer), ppu(ppu), screen(screen), apu(apu)
{}

void Emulator::run() {
    while (!glfwWindowShouldClose(screen.window)) {
        int cycles_this_frame = 0;
        while (cycles_this_frame < M_CYCLES_PER_FRAME) {
            this->tick();
            cycles_this_frame++;
        }
        screen.tick();
    }
}

void Emulator::tick() {
    uint8_t m_cycles = cpu.step();
        
    // all other components sync
    bool apu_div_tick = timer.tick(m_cycles);
    ppu.tick(m_cycles);
    apu.tick(m_cycles, apu_div_tick);

    handle_isr();
}

/**
 * Interrupt Handler
 */
void Emulator::handle_isr() {
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
 
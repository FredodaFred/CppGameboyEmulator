#include "emulator.hpp"

Emulator::Emulator(CPU& cpu, Bus& bus, Timer& timer, PPU& ppu, Screen& screen)
    : cpu(cpu), bus(bus), timer(timer), ppu(ppu), screen(screen)
{}

void Emulator::tick() {
    uint8_t cycles = cpu.step();
        
    // all other components sync
    timer.tick(cycles);
    ppu.tick(cycles);

    //Handle interrupt requests from components
    if (timer.interrupt) {
        timer.interrupt = false;
        uint8_t if_val = bus.read(0xFF0F);
        bus.write(0xFF0f, if_val | Interrupt::TIMER);
    }


}

void Emulator::run() {
    screen.init();
    while (true) {
        this->tick();
    }
}
 
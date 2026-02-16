#include "emulator.hpp"

Emulator::Emulator(CPU& cpu, Bus& bus)
    : cpu(cpu), bus(bus)
{}

void Emulator::tick() {
    uint8_t cycles = cpu.step();
        
    // all other components sync
}

void Emulator::run() {
    while (true) {
        this->tick();
    }
}
 
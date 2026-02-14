#pragma once
#include "../core/bus.hpp"
#include "../core/cpu.hpp"

class Emulator {
    public:
        Emulator(CPU& cpu, Bus& bus);
        void run();
    private:
        CPU cpu;
        Bus bus;

        void tick();

};
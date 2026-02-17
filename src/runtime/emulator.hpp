#pragma once
#include "../core/bus.hpp"
#include "../core/cpu.hpp"
#include "../io/timer.hpp"
#include "../core/ppu.hpp"
#include "graphics/screen.hpp"

class Emulator {
    public:
        Emulator(CPU& cpu, Bus& bus, Timer& timer, PPU& ppu, Screen& screen);
        void run();
    private:
        CPU cpu;
        Bus bus;
        Timer timer;
        PPU ppu;
        Screen screen;

        void tick();

};

#pragma once
#include "../core/bus.hpp"
#include "../core/cpu.hpp"
#include "../core/timer.hpp"
#include "../graphics/ppu.hpp"
#include "../graphics/screen.hpp"
#include "../joypad/joypad.hpp"

class Emulator {
    public:
        Emulator(CPU& cpu, Bus& bus, Timer& timer, PPU& ppu, Screen& screen, APU& apu);
        void run();
    private:
        CPU& cpu;
        Bus& bus;
        Timer& timer;
        PPU& ppu;
        Screen& screen;
        APU& apu;

        void tick();
};

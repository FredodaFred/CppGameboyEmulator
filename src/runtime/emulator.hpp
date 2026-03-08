#pragma once
#include "../core/bus.hpp"
#include "../core/cpu.hpp"
#include "../core/timer.hpp"
#include "../graphics/ppu.hpp"
#include "../graphics/screen.hpp"
#include <chrono>
#include <thread>

#define M_CYCLES_PER_FRAME 2


class Emulator {
    public:
        Emulator(CPU& cpu, Bus& bus, Timer& timer, PPU& ppu, Screen& screen, APU& apu);
        void run();

        void handle_isr();

    private:
        CPU& cpu;
        Bus& bus;
        Timer& timer;
        PPU& ppu;
        Screen& screen;
        APU& apu;

        void tick();
};

#pragma once
#include <cstdint>

class Bus;
class Registers;

class CPU {
    public:
        CPU(Bus& bus, Registers& registers);
        void step();
    private:
        Bus& bus;
        Registers& registers;
        
        void fetch();
        void execute(uint8_t opcode);
};
#pragma once
#include <cstdint>
#include "cart.hpp"

class Bus {
    public:
        Bus(Cart& cart);
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);

    private:
        Cart cart;

};
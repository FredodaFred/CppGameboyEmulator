#include <iostream>
#include "../core/bus.hpp"
#include "../core/cart.hpp"
#include "../core/cpu.hpp"
#include "../core/registers.hpp"
#include "emulator.hpp";

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./emulator <rom_path>" << std::endl;
        return 1; 
    }
    std::string romPath = argv[1];
    // Load game
    Cart cart = loadCart(romPath);

    //Setup classes
    Registers registers;
    Bus bus(cart);
    CPU cpu(bus, registers);
    Emulator emulator(cpu, bus);

    emulator.run();

}

Cart loadCart(std::string romPath) {
    Cart cart;
    cart.loadFromFile(romPath);
    if(!cart.cart_loaded){
        std::cerr << "Cart couldn't laod" << std::endl;
        exit;
    } 
    return cart;
}


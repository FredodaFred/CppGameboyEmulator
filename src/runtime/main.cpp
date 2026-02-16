#include <iostream>
#include "../core/bus.hpp"
#include "../core/cart.hpp"
#include "../core/cpu.hpp"
#include "../core/registers.hpp"
#include "emulator.hpp"

Cart loadCart(std::string romPath) {
    Cart cart;
    cart.loadFromFile(romPath);
    if(!cart.cart_loaded){
        std::cerr << "Cart couldn't load" << std::endl;
        exit;
    } 
    return cart;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./emulator <rom_path>" << std::endl;
        return 1; 
    }
    std::string romPath = argv[1];

    if (argc >= 3 && std::string_view(argv[2]) == "--log") {
        Logger::open("cpu_trace.log");
        Logger::set_enabled(true);
    }

    // Load game
    Cart cart = loadCart(romPath);
    Logger::log_cart_header(cart);

    //Setup classes
    Registers registers;
    PPU ppu;
    Bus bus(cart, ppu);
    CPU cpu(bus, registers);
    Emulator emulator(cpu, bus);
    
    try {
        emulator.run();
    } catch (const std::runtime_error& e) {
        Logger::close();
        std::cout << e.what() << std::endl;
    }
}



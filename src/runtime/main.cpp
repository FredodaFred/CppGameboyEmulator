#include <iostream>
#include "../core/bus.hpp"
#include "../core/cart.hpp"
#include "../core/cpu.hpp"
#include "../core/registers.hpp"
#include "emulator.hpp"
#include "graphics/dbg_window.hpp"

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
    bool enable_logging = std::find(argv, argv + argc, std::string_view("--log")) != (argv + argc);
    bool enable_dbg_window = std::find(argv, argv + argc, std::string_view("--dbg")) != (argv + argc);

    if (enable_logging) {
        Logger::open("cpu_trace.log");
        Logger::set_enabled(true);
    }


    // Load game
    Cart cart = loadCart(romPath);
    Logger::log_cart_header(cart);

    //Setup classes
    Registers registers;
    Screen screen;
    screen.init();
    PPU ppu (screen);
    Timer timer;
    Bus bus(cart, ppu, timer);
    CPU cpu(bus, registers);

    DbgWindow dbgWindow(bus);
    dbgWindow.init(enable_dbg_window);


    Emulator emulator(cpu, bus, timer, ppu, screen, dbgWindow);
    
    try {
        emulator.run();
    } catch (const std::runtime_error& e) {
        Logger::close();
        std::cout << e.what() << std::endl;
    }
}



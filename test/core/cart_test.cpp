#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "cart.hpp"

void test_load_file() {
    Cart cart;
    cart.loadFromFile("./roms/tetris.gb");
    std::cout << cart.title << std::endl; 
    std::cout << cart.getSize() << std::endl; 
}

int main() {
    test_load_file();
    return 0;
}
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "cart.hpp"

void test_load_file() {
    Cart cart;
    cart.loadFromFile("./roms/Pokemon_Red.gb");
}

int main() {
    std::cout << "----------------Running Cart Tests----------------" << std::endl;
    std::cout << "* test_load_file" << std::endl;
    test_load_file();
    return 0;
}
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <format>

class Cart {
    public:
        bool cart_loaded = false;
        void loadFromFile(std::string file_path);
        void printHeader();
        void write(uint16_t addr, uint8_t data);
        uint8_t read(uint16_t addr);
    private:
        std::vector<uint8_t> rom;

        // Cartridge Header metadata
        std::string title;
        int destinationCode;
        int licenseeCode;
        int version;
        size_t rom_size{0};
        size_t ram_size{0};

        void parse(const std::vector<uint8_t>& data);
        void parse_header(const std::vector<uint8_t>& data);
};
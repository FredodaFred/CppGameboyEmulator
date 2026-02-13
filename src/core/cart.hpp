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
        std::string title;

        void loadFromFile(std::string file_path);
        std::string getSize();
        
    private:
        size_t rom_size{0};
        std::vector<uint8_t> rom;
        static const std::unordered_map<std::string, std::string> licenseMap;
        void parse(const std::vector<uint8_t>& data);
};
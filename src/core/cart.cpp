#include "cart.hpp"

using std::string;
using std::vector;
using std::unordered_map;
using std::fstream;
using std::ifstream;
using std::ios;

void Cart::loadFromFile(const string path) {
    ifstream file(path, ios::binary | ios::ate);
    if (!file.is_open()){
        throw std::runtime_error("File not found");
    }

    std::streamsize size = file.tellg();
    this->rom_size = size_t(size);
    file.seekg(0, std::ios::beg);

    // Buffer will be size of the file, not what the cart tells us it is.
    vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    parse(buffer);
}

uint8_t Cart::read(uint16_t addr) {
    return this->rom.at(addr);
}

void Cart::write(uint16_t addr, uint8_t data) {
    this->rom[addr] = data;
}


void Cart::parse(const vector<uint8_t>& data) {
    parse_header(data);
    this->rom = data;
    this->cart_loaded = true;
}

void Cart::parse_header(const vector<uint8_t>& data) {
    this->title = string(data.begin() + 0x0134, data.begin() + 0x0143);
    this->rom_size = data.at(0x0148);
    this->ram_size = data.at(0x0149);
    this->licenseeCode = data.at(0x014B);
    this->version = data.at(0x014C);
}

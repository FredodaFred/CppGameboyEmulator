#pragma once
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <format>

#define SAVE_FOLDER "./saves/"

class Cart {
    public:
        bool cart_loaded = false;
        void loadFromFile(const std::string& file_path);
        void write(uint16_t addr, uint8_t data);
        uint8_t read(uint16_t addr);
        void create_save_file();

        void load_ram();

        // Cartridge Header metadata
        std::string title;
        int destinationCode;
        int licenseeCode;
        int version;
        int rom_size{0};
        size_t ram_size{0};
        uint8_t cart_type;

    private:
        std::vector<uint8_t> rom;
        std::vector<uint8_t> ram;
        bool save_ram{false};
        std::string file_name;
        int bank_mode = 0;
        uint8_t rom_bank_reg = 0x00;
        uint8_t ram_bank_reg = 0x00;
        uint8_t rtc_reg = 0x00;
        int max_banks{2};
        bool ram_enable{false};
        bool rtc_enable{false};
        void parse(const std::vector<uint8_t>& data);
        void parse_header(const std::vector<uint8_t>& data);

        void write_MBC(uint8_t mbc_mode, uint16_t addr, uint8_t data);
        uint8_t read_MBC(uint8_t mbc_mode, uint16_t addr);

        uint8_t get_rom_bank();

        uint8_t get_ram_bank();
};
#include "cart.hpp"

using std::string;
using std::vector;
using std::unordered_map;
using std::fstream;
using std::ifstream;
using std::ios;

void Cart::loadFromFile(const string& path) {
    ifstream file(path, ios::binary | ios::ate);
    if (!file.is_open()){
        throw std::runtime_error("File not found");
    }

    file_name = path.substr(path.find_last_of('/') + 1);
    file_name = file_name.substr(0, file_name.find_last_of('.'));
    file_path = path.substr(0, path.find_last_of('/')) + "/";

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Buffer will be size of the file, not what the cart tells us it is.
    vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    parse(buffer);
    file.close();
}

void Cart::create_save_file() {
    if (!save_ram) return;
    string save_path = file_path + file_name + ".sav";
    std::ofstream file(save_path, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(ram.data()), ram.size());
    }
}

void Cart::load_ram() {
    string save_path = file_path + file_name + ".sav";
    ifstream file(save_path, std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(ram.data()), ram.size());
    }
}


/**
 * Memory map
 *  MBC 1
 *  0000–3FFF — ROM Bank X0 [read-only]
 *  4000–7FFF — ROM Bank 01-7F [read-only]
 *  A000–BFFF — RAM Bank 00–03, if any
 *
 * @param addr
 * @return
 */
uint8_t Cart::read(uint16_t addr) {
    return read_MBC(cart_type, addr);
}

void Cart::write(uint16_t addr, uint8_t data) {
    write_MBC(cart_type, addr, data);
}

void Cart::parse(const vector<uint8_t>& data) {
    parse_header(data);
    rom = data;
    switch (ram_size) {
        case 0x00: break;                              // no RAM
        case 0x02: ram.resize(0x2000, 0); break;      // 8KB
        case 0x03: ram.resize(0x8000, 0); break;      // 32KB
        case 0x04: ram.resize(0x20000, 0); break;     // 128KB
        case 0x05: ram.resize(0x10000, 0); break;     // 64KB
    }
    switch (rom_size) {
        case 0x00: max_banks = 2; break; // 32 KB
        case 0x01: max_banks = 4; break; // 64 KB
        case 0x02: max_banks = 8; break; // 128 KB
        case 0x03: max_banks = 16; break; // 256 KB
        case 0x04: max_banks = 32; break; // 512 KB
        case 0x05: max_banks = 64; break; // 1 MB
        case 0x06: max_banks = 128; break; // 2 MB
        case 0x07: max_banks = 256; break; // 3 MB
        case 0x08: max_banks = 512; break; // 8 MB
    }

    // All battery rams use save files
    if (
        cart_type == 0x03 ||
        cart_type == 0x06 ||
        cart_type == 0x09 ||
        cart_type == 0x0D ||
        cart_type == 0x0F ||
        cart_type == 0x13 ||
        cart_type == 0x1B ||
        cart_type == 0x1E ||
        cart_type == 0x22 ||
        cart_type == 0xFF
    ) {
        save_ram = true;
        load_ram();
    }
    cart_loaded = true;
}

void Cart::parse_header(const vector<uint8_t>& data) {
    this->title = string(data.begin() + 0x0134, data.begin() + 0x0143);
    this->cart_type = data.at(0x0147);
    this->rom_size = data.at(0x0148);
    this->ram_size = data.at(0x0149);
    this->licenseeCode = data.at(0x014B);
    this->version = data.at(0x014C);
}


//https://gbdev.io/pandocs/The_Cartridge_Header.html#0147--cartridge-type
void Cart::write_MBC(uint8_t mbc_mode, uint16_t addr, uint8_t data) {
    switch (mbc_mode) {
        case 0x00: break; // Rom Only
        case 0x01: // MBC1
        case 0x02: // MBC1 + RAM
        case 0x03: // MBC1 + RAM + BATTERY
            if (addr < 0x2000) { // RAM Enable (Write Only)
                ram_enable = (data & 0x0F) == 0x0A;
            } else if (addr < 0x4000) {
                // This 5-bit register (range $01-$1F) selects the ROM bank number for the 4000–7FFF region.
                rom_bank_reg = data & 0x1F;
                if (rom_bank_reg == 0x00) rom_bank_reg = 1; // If this register is set to $00, it behaves as if it is set to $01.
            } else if (addr < 0x6000) { // RAM Bank Number — or — Upper Bits of ROM Bank Number (Write Only)
                ram_bank_reg = (data & 0x03);
            } else if ( addr < 0x8000 ) {
                // If the cart is not large enough to use the 2-bit register (≤ 8 KiB RAM and ≤ 512 KiB ROM) this mode select has no observable effect.
                if (rom_size > 0x04 || ram_size > 0x02 ) bank_mode = data & 0x01;
            } else if (addr >= 0xA000 && addr <= 0xBFFF) {
                if (ram_enable) {
                    uint32_t banked_addr = (ram_bank_reg * 0x2000) + (addr - 0xA000);
                    ram[banked_addr] = data;
                }
            }
            break;
        case 0x05: // MBC 2
        case 0x06:
            if (addr <= 0x3FFF) {
                // check LSB of upper byte
                if ( (addr >> 8) & 0x0001)  {
                    rom_bank_reg = (addr >> 8) & 0x0F;
                    if (rom_bank_reg == 0x00) rom_bank_reg = 1;
                } else {
                    // when this bit is clear if lower nibble of upper byte is 0xA
                    if ((addr & 0x0F00 )== 0x0A00) ram_enable = true;
                    else ram_enable = false;
                }
            } else if (addr >= 0xA000 && addr <= 0xBFFF && ram_enable) {
                ram[addr & 0x01FF] = data & 0x0F;
            }

            break;
        case 0x11: // MBC3
        case 0x12: // MBC3 + RAM
        case 0x13: { // MBC3 + RAM + Battery
            if ( addr <= 0x1FFF) { // RAM and Timer Enable (Write Only)
                ram_enable = (data & 0x0F) == 0x0A;
                rtc_enable = (data & 0x0F) == 0x0A;
            } else if ( addr >= 0x2000 && addr <= 0x3FFF) {
                rom_bank_reg = data & 0x7F;
                if (rom_bank_reg == 0x00) rom_bank_reg = 1;
            } else if ( addr >= 0x4000 && addr <= 0x5FFF) { // RAM Bank Number - or - RTC Register Select (Write Only)
                if (data < 0x08) {
                    ram_bank_reg = data;
                } else if ( data <= 0x0C) {
                    rtc_reg = data;
                }
            } else if (addr >= 0x6000 && addr <= 0x7FFF) { // Latch Clock Data (Write Only)
                // TODO: for GBC support
            }  else if (addr >= 0xA000 && addr <= 0xBFFF) {
                if (ram_enable) {
                    uint32_t banked_addr = (ram_bank_reg * 0x2000) + (addr - 0xA000);
                    ram[banked_addr] = data;
                }
            }
            break;
        }
        default:
    }
}

uint8_t Cart::read_MBC(uint8_t mbc_mode, uint16_t addr) {
    switch (mbc_mode) {
        case 0x00: return rom.at(addr); // Rom Only
        case 0x01: // MBC1
        case 0x02: // MBC1 + RAM
        case 0x03: {
            // MBC1 + RAM + BATTERY
            if (addr <= 0x3FFF) {
                return rom.at(addr);
            }
            if (addr >= 0x4000 && addr <= 0x7FFF) {
                uint32_t banked_addr = (get_rom_bank() * 0x4000) + (addr - 0x4000);
                return rom.at(banked_addr);
            }
            if (addr >= 0xA000 && addr <= 0xBFFF) {
                if (ram_enable) {
                    uint16_t banked_addr = (get_ram_bank() * 0x2000) + (addr - 0xA000);
                    return ram.at(banked_addr);
                }
                return 0xFF;
            }
            return 0xFF;
            break;
        }
        case 0x05: // MBC 2
        case 0x06:
            if (addr <= 0x3FFF) {
                return rom.at(addr);
            }
            if (addr <= 0x7FFF) { //rom bank 1
                uint32_t banked_addr = (get_rom_bank() * 0x4000)  + (addr - 0x4000);
                return rom.at(banked_addr);
            }
            if (addr >= 0xA000 && addr <= 0xBFFF) { //RAM
                return ram.at((addr & 0x01FF)) & 0x0F;
            }
            break;
        case 0x11: // MBC3
        case 0x12: // MBC3
        case 0x13: {
            // MBC3 + RAM + Battery
            if ( addr <= 0x3FFF ) {
                return rom.at(addr);
            }
            if (addr >= 0x4000 && addr <= 0x7FFF) {
                uint32_t banked_addr = (get_rom_bank() * 0x4000) + (addr - 0x4000);
                return rom.at(banked_addr);
            }
            if (addr >= 0xA000 && addr <= 0xBFFF) {
                if (ram_enable) {
                    uint16_t ram_addr = (get_ram_bank() * 0x2000) + (addr - 0xA000);
                    return ram.at(ram_addr);
                }
                return 0xFF;
            }
            break;
        }
        default:
            return rom.at(addr);

    }
}

uint8_t Cart::get_rom_bank() {
    // MBC 3
    if (cart_type >= 0x11 && cart_type <= 0x13) {
        return rom_bank_reg;
    }

    if (cart_type == 0x05 || cart_type == 0x06) {
        return rom_bank_reg;
    }

    // MBC1
    if (bank_mode == 0) return rom_bank_reg;
    return (ram_bank_reg << 5) | rom_bank_reg;
}

uint8_t Cart::get_ram_bank() {
    // MBC 3
    if (cart_type >= 0x11 && cart_type <= 0x13) {
        return ram_bank_reg;
    }

    // MBC 2
    if (cart_type == 0x05 || cart_type == 0x06) {

    }

    // MBC1
    if (bank_mode == 0) return 0;
    return ram_bank_reg;
}

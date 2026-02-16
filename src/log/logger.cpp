#include "logger.hpp"

using namespace std;

// Initialize static members
ofstream Logger::file_stream;
bool Logger::enabled = false;
int Logger::lines_since_flush = 0;


// TODO: wrap execution of logging functions so that same logic is repeated before and after execution
// Yes we aren't controlling flushing outside of log_cpu_state, but functionally we don't need to 

void Logger::open(const std::string& filename) {
    file_stream.open(filename);
}

void Logger::close() {
    if (file_stream.is_open()) {
        file_stream.flush();
        file_stream.close();
    }
}

void Logger::log_msg(const string & msg){
    if (!enabled || !file_stream.is_open()) return;
    file_stream << msg;
}

void Logger::log_cpu_state(Registers& registers, uint8_t opcode) {
    if (!enabled || !file_stream.is_open()) return;

    file_stream << format("A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X} PC:{:04X} SP:{:04X} | OP:{:02X} -> {}\n",
        registers.A, registers.F, registers.B, registers.C, 
        registers.D, registers.E, registers.H, registers.L, 
        registers.PC, registers.SP, opcode, get_readable(opcode));

    lines_since_flush++;
    if(lines_since_flush >= 100){
        file_stream.flush();
        lines_since_flush = 0;
    }
}

void Logger::log_cart_header(Cart& cart) {
    if (!cart.cart_loaded) return;
    if (!enabled || !file_stream.is_open()) return;

    string cleanTitle = cart.title;
    cleanTitle.erase(find(cleanTitle.begin(), cleanTitle.end(), '\0'), cleanTitle.end());

    file_stream << "--- Cartridge Header ---\n";
    file_stream << std::format("Title:         {}\n", cleanTitle);
    
    file_stream << std::format("ROM Size:      {} ({} KB)\n", (int)cart.rom_size, 32 << cart.rom_size);
    
    file_stream << std::format("RAM Size Code: {}\n", (int)cart.ram_size);
    
    file_stream << std::format("Version:       {}\n", (int)cart.version);
    file_stream << std::format("Licensee:      0x{:02X}\n", (int)cart.licenseeCode);
}

string Logger::get_readable(uint8_t opcode) {
    switch (opcode) {
        case 0x00: return "NOP\n";
        case 0x01: return "LD  BC  n16\n";
        case 0x02: return "LD  BC  A\n";
        case 0x03: return "INC  BC\n";
        case 0x04: return "INC  B\n";
        case 0x05: return "DEC  B\n";
        case 0x06: return "LD  B  n8\n";
        case 0x07: return "RLCA\n";
        case 0x08: return "LD  a16  SP\n";
        case 0x09: return "ADD  HL  BC\n";
        case 0x0A: return "LD  A  BC\n";
        case 0x0B: return "DEC  BC\n";
        case 0x0C: return "INC  C\n";
        case 0x0D: return "DEC  C\n";
        case 0x0E: return "LD  C  n8\n";
        case 0x0F: return "RRCA\n";
        case 0x10: return "STOP  n8\n";
        case 0x11: return "LD  DE  n16\n";
        case 0x12: return "LD  DE  A\n";
        case 0x13: return "INC  DE\n";
        case 0x14: return "INC  D\n";
        case 0x15: return "DEC  D\n";
        case 0x16: return "LD  D  n8\n";
        case 0x17: return "RLA\n";
        case 0x18: return "JR  e8\n";
        case 0x19: return "ADD  HL  DE\n";
        case 0x1A: return "LD  A  DE\n";
        case 0x1B: return "DEC  DE\n";
        case 0x1C: return "INC  E\n";
        case 0x1D: return "DEC  E\n";
        case 0x1E: return "LD  E  n8\n";
        case 0x1F: return "RRA\n";
        case 0x20: return "JR  NZ  e8\n";
        case 0x21: return "LD  HL  n16\n";
        case 0x22: return "LD  HL  A\n";
        case 0x23: return "INC  HL\n";
        case 0x24: return "INC  H\n";
        case 0x25: return "DEC  H\n";
        case 0x26: return "LD  H  n8\n";
        case 0x27: return "DAA\n";
        case 0x28: return "JR  Z  e8\n";
        case 0x29: return "ADD  HL  HL\n";
        case 0x2A: return "LD  A  HL\n";
        case 0x2B: return "DEC  HL\n";
        case 0x2C: return "INC  L\n";
        case 0x2D: return "DEC  L\n";
        case 0x2E: return "LD  L  n8\n";
        case 0x2F: return "CPL\n";
        case 0x30: return "JR  NC  e8\n";
        case 0x31: return "LD  SP  n16\n";
        case 0x32: return "LD  HL  A\n";
        case 0x33: return "INC  SP\n";
        case 0x34: return "INC  HL\n";
        case 0x35: return "DEC  HL\n";
        case 0x36: return "LD  HL  n8\n";
        case 0x37: return "SCF\n";
        case 0x38: return "JR  C  e8\n";
        case 0x39: return "ADD  HL  SP\n";
        case 0x3A: return "LD  A  HL\n";
        case 0x3B: return "DEC  SP\n";
        case 0x3C: return "INC  A\n";
        case 0x3D: return "DEC  A\n";
        case 0x3E: return "LD  A  n8\n";
        case 0x3F: return "CCF\n";
        case 0x40: return "LD  B  B\n";
        case 0x41: return "LD  B  C\n";
        case 0x42: return "LD  B  D\n";
        case 0x43: return "LD  B  E\n";
        case 0x44: return "LD  B  H\n";
        case 0x45: return "LD  B  L\n";
        case 0x46: return "LD  B  HL\n";
        case 0x47: return "LD  B  A\n";
        case 0x48: return "LD  C  B\n";
        case 0x49: return "LD  C  C\n";
        case 0x4A: return "LD  C  D\n";
        case 0x4B: return "LD  C  E\n";
        case 0x4C: return "LD  C  H\n";
        case 0x4D: return "LD  C  L\n";
        case 0x4E: return "LD  C  HL\n";
        case 0x4F: return "LD  C  A\n";
        case 0x50: return "LD  D  B\n";
        case 0x51: return "LD  D  C\n";
        case 0x52: return "LD  D  D\n";
        case 0x53: return "LD  D  E\n";
        case 0x54: return "LD  D  H\n";
        case 0x55: return "LD  D  L\n";
        case 0x56: return "LD  D  HL\n";
        case 0x57: return "LD  D  A\n";
        case 0x58: return "LD  E  B\n";
        case 0x59: return "LD  E  C\n";
        case 0x5A: return "LD  E  D\n";
        case 0x5B: return "LD  E  E\n";
        case 0x5C: return "LD  E  H\n";
        case 0x5D: return "LD  E  L\n";
        case 0x5E: return "LD  E  HL\n";
        case 0x5F: return "LD  E  A\n";
        case 0x60: return "LD  H  B\n";
        case 0x61: return "LD  H  C\n";
        case 0x62: return "LD  H  D\n";
        case 0x63: return "LD  H  E\n";
        case 0x64: return "LD  H  H\n";
        case 0x65: return "LD  H  L\n";
        case 0x66: return "LD  H  HL\n";
        case 0x67: return "LD  H  A\n";
        case 0x68: return "LD  L  B\n";
        case 0x69: return "LD  L  C\n";
        case 0x6A: return "LD  L  D\n";
        case 0x6B: return "LD  L  E\n";
        case 0x6C: return "LD  L  H\n";
        case 0x6D: return "LD  L  L\n";
        case 0x6E: return "LD  L  HL\n";
        case 0x6F: return "LD  L  A\n";
        case 0x70: return "LD  HL  B\n";
        case 0x71: return "LD  HL  C\n";
        case 0x72: return "LD  HL  D\n";
        case 0x73: return "LD  HL  E\n";
        case 0x74: return "LD  HL  H\n";
        case 0x75: return "LD  HL  L\n";
        case 0x76: return "HALT\n";
        case 0x77: return "LD  HL  A\n";
        case 0x78: return "LD  A  B\n";
        case 0x79: return "LD  A  C\n";
        case 0x7A: return "LD  A  D\n";
        case 0x7B: return "LD  A  E\n";
        case 0x7C: return "LD  A  H\n";
        case 0x7D: return "LD  A  L\n";
        case 0x7E: return "LD  A  HL\n";
        case 0x7F: return "LD  A  A\n";
        case 0x80: return "ADD  A  B\n";
        case 0x81: return "ADD  A  C\n";
        case 0x82: return "ADD  A  D\n";
        case 0x83: return "ADD  A  E\n";
        case 0x84: return "ADD  A  H\n";
        case 0x85: return "ADD  A  L\n";
        case 0x86: return "ADD  A  HL\n";
        case 0x87: return "ADD  A  A\n";
        case 0x88: return "ADC  A  B\n";
        case 0x89: return "ADC  A  C\n";
        case 0x8A: return "ADC  A  D\n";
        case 0x8B: return "ADC  A  E\n";
        case 0x8C: return "ADC  A  H\n";
        case 0x8D: return "ADC  A  L\n";
        case 0x8E: return "ADC  A  HL\n";
        case 0x8F: return "ADC  A  A\n";
        case 0x90: return "SUB  B\n";
        case 0x91: return "SUB  C\n";
        case 0x92: return "SUB  D\n";
        case 0x93: return "SUB  E\n";
        case 0x94: return "SUB  H\n";
        case 0x95: return "SUB  L\n";
        case 0x96: return "SUB  HL\n";
        case 0x97: return "SUB  A\n";
        case 0x98: return "SBC  A  B\n";
        case 0x99: return "SBC  A  C\n";
        case 0x9A: return "SBC  A  D\n";
        case 0x9B: return "SBC  A  E\n";
        case 0x9C: return "SBC  A  H\n";
        case 0x9D: return "SBC  A  L\n";
        case 0x9E: return "SBC  A  HL\n";
        case 0x9F: return "SBC  A  A\n";
        case 0xA0: return "AND  B\n";
        case 0xA1: return "AND  C\n";
        case 0xA2: return "AND  D\n";
        case 0xA3: return "AND  E\n";
        case 0xA4: return "AND  H\n";
        case 0xA5: return "AND  L\n";
        case 0xA6: return "AND  HL\n";
        case 0xA7: return "AND  A\n";
        case 0xA8: return "XOR  B\n";
        case 0xA9: return "XOR  C\n";
        case 0xAA: return "XOR  D\n";
        case 0xAB: return "XOR  E\n";
        case 0xAC: return "XOR  H\n";
        case 0xAD: return "XOR  L\n";
        case 0xAE: return "XOR  HL\n";
        case 0xAF: return "XOR  A\n";
        case 0xB0: return "OR  B\n";
        case 0xB1: return "OR  C\n";
        case 0xB2: return "OR  D\n";
        case 0xB3: return "OR  E\n";
        case 0xB4: return "OR  H\n";
        case 0xB5: return "OR  L\n";
        case 0xB6: return "OR  HL\n";
        case 0xB7: return "OR  A\n";
        case 0xB8: return "CP  B\n";
        case 0xB9: return "CP  C\n";
        case 0xBA: return "CP  D\n";
        case 0xBB: return "CP  E\n";
        case 0xBC: return "CP  H\n";
        case 0xBD: return "CP  L\n";
        case 0xBE: return "CP  HL\n";
        case 0xBF: return "CP  A\n";
        case 0xC0: return "RET  NZ\n";
        case 0xC1: return "POP  BC\n";
        case 0xC2: return "JP  NZ  a16\n";
        case 0xC3: return "JP  a16\n";
        case 0xC4: return "CALL  NZ  a16\n";
        case 0xC5: return "PUSH  BC\n";
        case 0xC6: return "ADD  A  n8\n";
        case 0xC7: return "RST  00H\n";
        case 0xC8: return "RET  Z\n";
        case 0xC9: return "RET\n";
        case 0xCA: return "JP  Z  a16\n";
        case 0xCB: return "PREFIX\n";
        case 0xCC: return "CALL  Z  a16\n";
        case 0xCD: return "CALL  a16\n";
        case 0xCE: return "ADC  A  n8\n";
        case 0xCF: return "RST  08H\n";
        case 0xD0: return "RET  NC\n";
        case 0xD1: return "POP  DE\n";
        case 0xD2: return "JP  NC  a16\n";
        case 0xD3: return "ILLEGAL_D3\n";
        case 0xD4: return "CALL  NC  a16\n";
        case 0xD5: return "PUSH  DE\n";
        case 0xD6: return "SUB  n8\n";
        case 0xD7: return "RST  10H\n";
        case 0xD8: return "RET  C\n";
        case 0xD9: return "RETI\n";
        case 0xDA: return "JP  C  a16\n";
        case 0xDB: return "ILLEGAL_DB\n";
        case 0xDC: return "CALL  C  a16\n";
        case 0xDD: return "ILLEGAL_DD\n";
        case 0xDE: return "SBC  A  n8\n";
        case 0xDF: return "RST  18H\n";
        case 0xE0: return "LDH  a8  A\n";
        case 0xE1: return "POP  HL\n";
        case 0xE2: return "LD  C  A\n";
        case 0xE3: return "ILLEGAL_E3\n";
        case 0xE4: return "ILLEGAL_E4\n";
        case 0xE5: return "PUSH  HL\n";
        case 0xE6: return "AND  n8\n";
        case 0xE7: return "RST  20H\n";
        case 0xE8: return "ADD  SP  e8\n";
        case 0xE9: return "JP  HL\n";
        case 0xEA: return "LD  a16  A\n";
        case 0xEB: return "ILLEGAL_EB\n";
        case 0xEC: return "ILLEGAL_EC\n";
        case 0xED: return "ILLEGAL_ED\n";
        case 0xEE: return "XOR  n8\n";
        case 0xEF: return "RST  28H\n";
        case 0xF0: return "LDH  A  a8\n";
        case 0xF1: return "POP  AF\n";
        case 0xF2: return "LD  A  C\n";
        case 0xF3: return "DI\n";
        case 0xF4: return "ILLEGAL_F4\n";
        case 0xF5: return "PUSH  AF\n";
        case 0xF6: return "OR  n8\n";
        case 0xF7: return "RST  30H\n";
        case 0xF8: return "LD  HL  SP  e8\n";
        case 0xF9: return "LD  SP  HL\n";
        case 0xFA: return "LD  A  a16\n";
        case 0xFB: return "EI\n";
        case 0xFC: return "ILLEGAL_FC\n";
        case 0xFD: return "ILLEGAL_FD\n";
        case 0xFE: return "CP  n8\n";
        case 0xFF: return "RST  38H\n";
        default: return "UNKNOWN\n";
    }
}
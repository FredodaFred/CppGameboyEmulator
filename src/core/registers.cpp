#include "registers.hpp"

// Getters
bool Registers::getZ() const { return (F & 0x80) != 0; }
bool Registers::getN() const { return (F & 0x40) != 0; }
bool Registers::getH() const { return (F & 0x20) != 0; }
bool Registers::getC() const { return (F & 0x10) != 0; }

uint8_t Registers::getReg8(Reg8 reg) const {
    switch (reg) {
        case Reg8::A: return A;
        case Reg8::B: return B;
        case Reg8::C: return C;
        case Reg8::D: return D;
        case Reg8::E: return E;
        case Reg8::H: return H;
        case Reg8::L: return L;
        case Reg8::F: return F;
        default:      return 0; 
    }
}

uint16_t Registers::getReg16(Reg16 reg) const {
    switch (reg) {
        case Reg16::AF: return (uint16_t)((A << 8) | F);
        case Reg16::BC: return (uint16_t)((B << 8) | C);
        case Reg16::DE: return (uint16_t)((D << 8) | E);
        case Reg16::HL: return (uint16_t)((H << 8) | L);
        case Reg16::SP: return SP;
        case Reg16::PC: return PC;
    }
    return 0;
}

// Setters
void Registers::setZ(bool val) { setBit(7, val); }
void Registers::setN(bool val) { setBit(6, val); }
void Registers::setH(bool val) { setBit(5, val); }
void Registers::setC(bool val) { setBit(4, val); }

void Registers::setReg8(Reg8 reg, uint8_t value) {
    switch (reg) {
        case Reg8::A: A = value; break;
        case Reg8::B: B = value; break;
        case Reg8::C: C = value; break;
        case Reg8::D: D = value; break;
        case Reg8::E: E = value; break;
        case Reg8::H: H = value; break;
        case Reg8::L: L = value; break;
        case Reg8::F: F = value & 0xF0; break;
    }
}

void Registers::setReg16(Reg16 reg, uint16_t value) {
    switch (reg) {
        case Reg16::AF: A = (value >> 8); F = (value & 0xF0); break;
        case Reg16::BC: B = (value >> 8); C = (value & 0xFF); break;
        case Reg16::DE: D = (value >> 8); E = (value & 0xFF); break;
        case Reg16::HL: H = (value >> 8); L = (value & 0xFF); break;
        case Reg16::SP: SP = value; break;
        case Reg16::PC: PC = value; break;
    }
}

void Registers::reset() {
    A = 0x01; F = 0xB0;
    B = 0x00; C = 0x13;
    D = 0x00; E = 0xD8;
    H = 0x01; L = 0x4D;
    SP = 0xFFFE;
    PC = 0x0100;
}

// Private Helper
void Registers::setBit(uint8_t bit, bool val) {
    if (val) F |= (1 << bit);
    else     F &= ~(1 << bit);
}

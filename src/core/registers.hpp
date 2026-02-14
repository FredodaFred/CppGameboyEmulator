#pragma once
#include <cstdint>

enum class Reg8 {
    A, B, C, D, E, H, L, F
};

enum class Reg16 {
    AF, BC, DE, HL, SP, PC
};

class Registers {
public:
    /* -------- Registers -------- */
    uint8_t A = 0x01, B = 0x00, C = 0x13, D = 0x00;
    uint8_t E = 0xD8, H = 0x01, L = 0x4D, F = 0xB0;
    uint16_t SP = 0xFFFE;
    uint16_t PC = 0x0100;

    /* -------- Flag Getters -------- */
    bool getZ() const;
    bool getN() const;
    bool getH() const;
    bool getC() const;

    /* -------- Flag Setters -------- */
    void setZ(bool val);
    void setN(bool val);
    void setH(bool val);
    void setC(bool val);

    /* -------- Generic Accessors -------- */
    uint8_t getReg8(Reg8 reg) const;
    void setReg8(Reg8 reg, uint8_t value);

    uint16_t getReg16(Reg16 reg) const;
    void setReg16(Reg16 reg, uint16_t value);

    /* --------- Misc --------------- */
    void reset();

private:
    void setBit(uint8_t bit, bool val);
};
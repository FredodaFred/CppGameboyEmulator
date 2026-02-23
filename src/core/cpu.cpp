#include "cpu.hpp"

using namespace Interrupt;

// Constructor
CPU::CPU(Bus& bus, Registers& registers)
    : bus(bus), registers(registers)
{}

// Executes a single instruction
int CPU::step() {
    if (this->halted) {
        // Only when IE and IF are enabled we can "unhalt"
        if ((bus.read(0xFFFF) & bus.read(0xFF0F) & 0x1F) != 0) {
            this->halted = false;
        }

        clock_cycles++;
        return clock_cycles;
    }

    if (IME_delay == 2) { IME_delay--;}
    else if (IME_delay == 1) {
        IME_delay = 0;
        IME = true;
    }

    bool interrupt_handled = handle_interrupts();

    if (!interrupt_handled) {
        uint8_t opcode = this->fetch();
        if (Logger::is_enabled()) Logger::log_cpu_state(this->registers, opcode);
        this->registers.PC++;
        this->execute(opcode);
    }

    // Here we manage the clock cycles
    int cycles_passed = this->clock_cycles;
    this->clock_cycles = 0;
    return cycles_passed;
}

uint8_t CPU::fetch() {
    return bus.read(this->registers.PC);
}

void CPU::execute(uint8_t opcode) {
    switch (opcode) {
        case 0x00: nop(); break;
        case 0x01: ld_r16_n16(Reg16::BC); break;
        case 0x02: ld_mr_r(Reg16::BC, Reg8::A); break;
        case 0x03: inc_r16(Reg16::BC); break;
        case 0x04: inc_r8(Reg8::B); break;
        case 0x05: dec_r8(Reg8::B); break;
        case 0x06: ld_r8_n8(Reg8::B); break;
        case 0x07: rlca(); break;
        case 0x08: ld_a16_sp(); break;
        case 0x09: add_r16_r16(Reg16::HL, Reg16::BC); break;
        case 0x0A: ld_r_mr(Reg8::A, Reg16::BC); break;
        case 0x0B: dec_r16(Reg16::BC); break;
        case 0x0C: inc_r8(Reg8::C); break;
        case 0x0D: dec_r8(Reg8::C); break;
        case 0x0E: ld_r8_n8(Reg8::C); break;
        case 0x0F: rrca(); break;
        case 0x10: stop(); break;
        case 0x11: ld_r16_n16(Reg16::DE); break;
        case 0x12: ld_mr_r(Reg16::DE, Reg8::A); break;
        case 0x13: inc_r16(Reg16::DE); break;
        case 0x14: inc_r8(Reg8::D); break;
        case 0x15: dec_r8(Reg8::D); break;
        case 0x16: ld_r8_n8(Reg8::D); break;
        case 0x17: rla(); break;
        case 0x18: jr_e8(); break;
        case 0x19: add_r16_r16(Reg16::HL, Reg16::DE); break;
        case 0x1A: ld_r_mr(Reg8::A, Reg16::DE); break;
        case 0x1B: dec_r16(Reg16::DE); break;
        case 0x1C: inc_r8(Reg8::E); break;
        case 0x1D: dec_r8(Reg8::E); break;
        case 0x1E: ld_r8_n8(Reg8::E); break;
        case 0x1F: rra(); break;
        case 0x20: jr(Cond::NZ); break;
        case 0x21: ld_r16_n16(Reg16::HL); break;
        case 0x22: ld_hli_r(Reg8::A); break;
        case 0x23: inc_r16(Reg16::HL); break;
        case 0x24: inc_r8(Reg8::H); break;
        case 0x25: dec_r8(Reg8::H); break;
        case 0x26: ld_r8_n8(Reg8::H); break;
        case 0x27: daa(); break;
        case 0x28: jr(Cond::Z); break;
        case 0x29: add_r16_r16(Reg16::HL, Reg16::HL); break;
        case 0x2A: ld_r_hli(Reg8::A); break;
        case 0x2B: dec_r16(Reg16::HL); break;
        case 0x2C: inc_r8(Reg8::L); break;
        case 0x2D: dec_r8(Reg8::L); break;
        case 0x2E: ld_r8_n8(Reg8::L); break;
        case 0x2F: cpl(); break;
        case 0x30: jr(Cond::NC); break;
        case 0x31: ld_r16_n16(Reg16::SP); break;
        case 0x32: ld_hld_r(Reg8::A); break;
        case 0x33: inc_r16(Reg16::SP); break;
        case 0x34: inc_mr(Reg16::HL); break;
        case 0x35: dec_mr(Reg16::HL); break;
        case 0x36: ld_mr_n8(Reg16::HL); break;
        case 0x37: scf(); break;
        case 0x38: jr(Cond::C); break;
        case 0x39: add_r16_r16(Reg16::HL, Reg16::SP); break;
        case 0x3A: ld_r_hld(Reg8::A); break;
        case 0x3B: dec_r16(Reg16::SP); break;
        case 0x3C: inc_r8(Reg8::A); break;
        case 0x3D: dec_r8(Reg8::A); break;
        case 0x3E: ld_r8_n8(Reg8::A); break;
        case 0x3F: ccf(); break;
        case 0x40: ld_r_r(Reg8::B, Reg8::B); break;
        case 0x41: ld_r_r(Reg8::B, Reg8::C); break;
        case 0x42: ld_r_r(Reg8::B, Reg8::D); break;
        case 0x43: ld_r_r(Reg8::B, Reg8::E); break;
        case 0x44: ld_r_r(Reg8::B, Reg8::H); break;
        case 0x45: ld_r_r(Reg8::B, Reg8::L); break;
        case 0x46: ld_r_mr(Reg8::B, Reg16::HL); break;
        case 0x47: ld_r_r(Reg8::B, Reg8::A); break;
        case 0x48: ld_r_r(Reg8::C, Reg8::B); break;
        case 0x49: ld_r_r(Reg8::C, Reg8::C); break;
        case 0x4A: ld_r_r(Reg8::C, Reg8::D); break;
        case 0x4B: ld_r_r(Reg8::C, Reg8::E); break;
        case 0x4C: ld_r_r(Reg8::C, Reg8::H); break;
        case 0x4D: ld_r_r(Reg8::C, Reg8::L); break;
        case 0x4E: ld_r_mr(Reg8::C, Reg16::HL); break;
        case 0x4F: ld_r_r(Reg8::C, Reg8::A); break;
        case 0x50: ld_r_r(Reg8::D, Reg8::B); break;
        case 0x51: ld_r_r(Reg8::D, Reg8::C); break;
        case 0x52: ld_r_r(Reg8::D, Reg8::D); break;
        case 0x53: ld_r_r(Reg8::D, Reg8::E); break;
        case 0x54: ld_r_r(Reg8::D, Reg8::H); break;
        case 0x55: ld_r_r(Reg8::D, Reg8::L); break;
        case 0x56: ld_r_mr(Reg8::D, Reg16::HL); break;
        case 0x57: ld_r_r(Reg8::D, Reg8::A); break;
        case 0x58: ld_r_r(Reg8::E, Reg8::B); break;
        case 0x59: ld_r_r(Reg8::E, Reg8::C); break;
        case 0x5A: ld_r_r(Reg8::E, Reg8::D); break;
        case 0x5B: ld_r_r(Reg8::E, Reg8::E); break;
        case 0x5C: ld_r_r(Reg8::E, Reg8::H); break;
        case 0x5D: ld_r_r(Reg8::E, Reg8::L); break;
        case 0x5E: ld_r_mr(Reg8::E, Reg16::HL); break;
        case 0x5F: ld_r_r(Reg8::E, Reg8::A); break;
        case 0x60: ld_r_r(Reg8::H, Reg8::B); break;
        case 0x61: ld_r_r(Reg8::H, Reg8::C); break;
        case 0x62: ld_r_r(Reg8::H, Reg8::D); break;
        case 0x63: ld_r_r(Reg8::H, Reg8::E); break;
        case 0x64: ld_r_r(Reg8::H, Reg8::H); break;
        case 0x65: ld_r_r(Reg8::H, Reg8::L); break;
        case 0x66: ld_r_mr(Reg8::H, Reg16::HL); break;
        case 0x67: ld_r_r(Reg8::H, Reg8::A); break;
        case 0x68: ld_r_r(Reg8::L, Reg8::B); break;
        case 0x69: ld_r_r(Reg8::L, Reg8::C); break;
        case 0x6A: ld_r_r(Reg8::L, Reg8::D); break;
        case 0x6B: ld_r_r(Reg8::L, Reg8::E); break;
        case 0x6C: ld_r_r(Reg8::L, Reg8::H); break;
        case 0x6D: ld_r_r(Reg8::L, Reg8::L); break;
        case 0x6E: ld_r_mr(Reg8::L, Reg16::HL); break;
        case 0x6F: ld_r_r(Reg8::L, Reg8::A); break;
        case 0x70: ld_mr_r(Reg16::HL, Reg8::B); break;
        case 0x71: ld_mr_r(Reg16::HL, Reg8::C); break;
        case 0x72: ld_mr_r(Reg16::HL, Reg8::D); break;
        case 0x73: ld_mr_r(Reg16::HL, Reg8::E); break;
        case 0x74: ld_mr_r(Reg16::HL, Reg8::H); break;
        case 0x75: ld_mr_r(Reg16::HL, Reg8::L); break;
        case 0x76: halt(); break;
        case 0x77: ld_mr_r(Reg16::HL, Reg8::A); break;
        case 0x78: ld_r_r(Reg8::A, Reg8::B); break;
        case 0x79: ld_r_r(Reg8::A, Reg8::C); break;
        case 0x7A: ld_r_r(Reg8::A, Reg8::D); break;
        case 0x7B: ld_r_r(Reg8::A, Reg8::E); break;
        case 0x7C: ld_r_r(Reg8::A, Reg8::H); break;
        case 0x7D: ld_r_r(Reg8::A, Reg8::L); break;
        case 0x7E: ld_r_mr(Reg8::A, Reg16::HL); break;
        case 0x7F: ld_r_r(Reg8::A, Reg8::A); break;
        case 0x80: add_r8_r8(Reg8::A, Reg8::B); break;
        case 0x81: add_r8_r8(Reg8::A, Reg8::C); break;
        case 0x82: add_r8_r8(Reg8::A, Reg8::D); break;
        case 0x83: add_r8_r8(Reg8::A, Reg8::E); break;
        case 0x84: add_r8_r8(Reg8::A, Reg8::H); break;
        case 0x85: add_r8_r8(Reg8::A, Reg8::L); break;
        case 0x86: add_r8_mr(Reg8::A, Reg16::HL); break;
        case 0x87: add_r8_r8(Reg8::A, Reg8::A); break;
        case 0x88: adc_a_r8(Reg8::B); break;
        case 0x89: adc_a_r8(Reg8::C); break;
        case 0x8A: adc_a_r8(Reg8::D); break;
        case 0x8B: adc_a_r8(Reg8::E); break;
        case 0x8C: adc_a_r8(Reg8::H); break;
        case 0x8D: adc_a_r8(Reg8::L); break;
        case 0x8E: adc_a_hl(); break;
        case 0x8F: adc_a_r8(Reg8::A); break;
        case 0x90: sub(Reg8::B); break;
        case 0x91: sub(Reg8::C); break;
        case 0x92: sub(Reg8::D); break;
        case 0x93: sub(Reg8::E); break;
        case 0x94: sub(Reg8::H); break;
        case 0x95: sub(Reg8::L); break;
        case 0x96: sub_hl(); break;
        case 0x97: sub(Reg8::A); break;
        case 0x98: sbc(Reg8::B); break;
        case 0x99: sbc(Reg8::C); break;
        case 0x9A: sbc(Reg8::D); break;
        case 0x9B: sbc(Reg8::E); break;
        case 0x9C: sbc(Reg8::H); break;
        case 0x9D: sbc(Reg8::L); break;
        case 0x9E: sbc_hl(); break;
        case 0x9F: sbc(Reg8::A); break;
        case 0xA0: and_r8(Reg8::B); break;
        case 0xA1: and_r8(Reg8::C); break;
        case 0xA2: and_r8(Reg8::D); break;
        case 0xA3: and_r8(Reg8::E); break;
        case 0xA4: and_r8(Reg8::H); break;
        case 0xA5: and_r8(Reg8::L); break;
        case 0xA6: and_hl(); break;
        case 0xA7: and_r8(Reg8::A); break;
        case 0xA8: xor_r8(Reg8::B); break;
        case 0xA9: xor_r8(Reg8::C); break;
        case 0xAA: xor_r8(Reg8::D); break;
        case 0xAB: xor_r8(Reg8::E); break;
        case 0xAC: xor_r8(Reg8::H); break;
        case 0xAD: xor_r8(Reg8::L); break;
        case 0xAE: xor_hl(); break;
        case 0xAF: xor_r8(Reg8::A); break;
        case 0xB0: or_r8(Reg8::B); break;
        case 0xB1: or_r8(Reg8::C); break;
        case 0xB2: or_r8(Reg8::D); break;
        case 0xB3: or_r8(Reg8::E); break;
        case 0xB4: or_r8(Reg8::H); break;
        case 0xB5: or_r8(Reg8::L); break;
        case 0xB6: or_hl(); break;
        case 0xB7: or_r8(Reg8::A); break;
        case 0xB8: cp_r8(Reg8::B); break;
        case 0xB9: cp_r8(Reg8::C); break;
        case 0xBA: cp_r8(Reg8::D); break;
        case 0xBB: cp_r8(Reg8::E); break;
        case 0xBC: cp_r8(Reg8::H); break;
        case 0xBD: cp_r8(Reg8::L); break;
        case 0xBE: cp_hl(); break;
        case 0xBF: cp_r8(Reg8::A); break;
        case 0xC0: ret_cond(Cond::NZ); break;
        case 0xC1: pop(Reg16::BC); break;
        case 0xC2: jp_a16_cond(Cond::NZ); break;
        case 0xC3: jp_a16(); break;
        case 0xC4: call_cond_a16(Cond::NZ); break;
        case 0xC5: push(Reg16::BC); break;
        case 0xC6: add_r8_n8(Reg8::A); break;
        case 0xC7: rst(0x00); break;
        case 0xC8: ret_cond(Cond::Z); break;
        case 0xC9: ret(); break;
        case 0xCA: jp_a16_cond(Cond::Z); break;
        case 0xCB: execute_cb(); break; 
        case 0xCC: call_cond_a16(Cond::Z); break;
        case 0xCD: call_a16(); break;
        case 0xCE: adc_a_n8(); break;
        case 0xCF: rst(0x08); break;
        case 0xD0: ret_cond(Cond::NC); break;
        case 0xD1: pop(Reg16::DE); break;
        case 0xD2: jp_a16_cond(Cond::NC); break;
        case 0xD4: call_cond_a16(Cond::NC); break;
        case 0xD5: push(Reg16::DE); break;
        case 0xD6: sub_a_n8(); break;
        case 0xD7: rst(0x10); break;
        case 0xD8: ret_cond(Cond::C); break;
        case 0xD9: reti(); break;
        case 0xDA: jp_a16_cond(Cond::C); break;
        case 0xDC: call_cond_a16(Cond::C); break;
        case 0xDE: sbc_a_n8(); break;
        case 0xDF: rst(0x18); break;
        case 0xE0: ldh_a8_a(); break;
        case 0xE1: pop(Reg16::HL); break;
        case 0xE2: ld_mc_a(); break;
        case 0xE5: push(Reg16::HL); break;
        case 0xE6: and_a_n8(); break;
        case 0xE7: rst(0x20); break;
        case 0xE8: add_sp_e8(); break;
        case 0xE9: jp_hl(); break;
        case 0xEA: ld_a16_a(); break;
        case 0xEE: xor_a_n8(); break;
        case 0xEF: rst(0x28); break;
        case 0xF0: ldh_a_a8(); break;
        case 0xF1: pop(Reg16::AF); break;
        case 0xF2: ld_a_mc(); break;
        case 0xF3: di(); break;
        case 0xF5: push(Reg16::AF); break;
        case 0xF6: or_a_n8(); break;
        case 0xF7: rst(0x30); break;
        case 0xF8: ld_hl_sp_e8(); break;
        case 0xF9: ld_sp_hl(); break;
        case 0xFA: ld_a_a16(); break;
        case 0xFB: ei(); break;
        case 0xFE: cp_a_n8(); break;
        case 0xFF: rst(0x38); break;
        default:
            std::cerr << std::format("Unknown Opcode: 0x{:02X}\n", opcode);
            break;
    }
}

bool CPU::handle_interrupts() {
    if (!this->IME) return false;
    // NOTE: 0xFFFF is IE and 0xFF0F is IF
    uint8_t IE = this->bus.read(0xFFFF);
    uint8_t IF = this->bus.read(0xFF0F);
    uint8_t interrupt_pending = IE & IF;
    // uint8_t IF_2 = this->bus.read(0xFF0F);
    // std::cout << std::format("IF: 0x{:02X}\n", IF_2);

    if (interrupt_pending == 0) return false;

    // Priority check
    if (interrupt_pending & Interrupt::VBLANK) {
        service_interrupt(Interrupt::VBLANK, Interrupt::ADDR_VBLANK);
    } else if (interrupt_pending & Interrupt::LCD_STAT) {
        service_interrupt(Interrupt::LCD_STAT, Interrupt::ADDR_LCD_STAT);
    } else if (interrupt_pending & Interrupt::TIMER) {
        service_interrupt(Interrupt::TIMER, Interrupt::ADDR_TIMER);
    } else if (interrupt_pending & Interrupt::SERIAL) {
        service_interrupt(Interrupt::SERIAL, Interrupt::ADDR_SERIAL);
    } else if (interrupt_pending & Interrupt::JOYPAD) {
        service_interrupt(Interrupt::JOYPAD, Interrupt::ADDR_JOYPAD);
    }
    return true;
}

void CPU::service_interrupt(uint8_t interrupt, uint16_t addr) {
    Logger::log_msg(std::format("handling interrupt {}\n", interrupt));
    // Save PC address to stack
    uint8_t hi = static_cast<uint8_t>((this->registers.PC & 0xFF00) >> 8);
    uint8_t lo = static_cast<uint8_t>(this->registers.PC & 0x00FF);
    this->stkpush(hi);
    this->stkpush(lo);

    this->registers.PC = addr;
    // Clear flags
    uint8_t if_reg = this->bus.read(0xFF0F);
    this->bus.write(0xFF0F, if_reg & ~interrupt);
    this->IME = false;
    this->halted = false;
    clock_cycles += 5;
}

// INSTRUCTIONS

// Interrupts

void CPU::di() {
    this->IME = false;
    this->IME_delay = 0;
    this->clock_cycles += 1;
}

void CPU::ei() {
    this->IME_delay = 2;
    this->clock_cycles += 1;
}

// load instructions

void CPU::ld_r16_n16(Reg16 r) {
    uint8_t lsb = this->bus.read(this->registers.PC++);
    uint8_t msb = this->bus.read(this->registers.PC++);
    uint16_t n16 = msb << 8 | lsb;
    this->registers.setReg16(r, n16);
    this->clock_cycles += 3;

}

void CPU::ld_r_r(Reg8 r1, Reg8 r2){
    this->registers.setReg8(r1, this->registers.getReg8(r2));
    this->clock_cycles++;
}

void CPU::ld_mr_r(Reg16 mr, Reg8 r) {
    this->bus.write(this->registers.getReg16(mr), this->registers.getReg8(r));
    this->clock_cycles += 2;
}

void CPU::ld_r_mr(Reg8 r, Reg16 mr) {
    uint8_t val = this->bus.read(this->registers.getReg16(mr));
    this->registers.setReg8(r, val);
    this->clock_cycles += 2;
}


void CPU::ld_r8_n8(Reg8 r) {
    uint8_t data = this->bus.read(this->registers.PC++);
    this->registers.setReg8(r, data);
    this->clock_cycles += 2;
}

void CPU::ld_hli_r(Reg8 r) {
    uint8_t data = this->registers.getReg8(r);
    uint16_t hl = this->registers.getReg16(Reg16::HL);
    this->bus.write(hl, data);
    
    this->registers.setReg16(Reg16::HL, hl + 1);
    
    this->clock_cycles += 2;
}

void CPU::ld_hld_r(Reg8 r) {
    uint8_t data = this->registers.getReg8(r);
    uint16_t hl = this->registers.getReg16(Reg16::HL);
    this->bus.write(hl, data);
    
    this->registers.setReg16(Reg16::HL, hl - 1);
    
    this->clock_cycles += 2;
}

void CPU::ld_r_hld(Reg8 r) {
    uint16_t hl = this->registers.getReg16(Reg16::HL);
    uint8_t val = this->bus.read(hl);
    this->registers.setReg16(Reg16::HL, hl - 1);
    this->registers.setReg8(r, val);
    this->clock_cycles += 2;
}

void CPU::ld_r_hli(Reg8 r) {
    uint16_t hl = this->registers.getReg16(Reg16::HL);
    uint8_t val = this->bus.read(hl);
    this->registers.setReg16(Reg16::HL, hl + 1);
    this->registers.setReg8(r, val);
    this->clock_cycles += 2;
}

void CPU::ld_a16_sp() {
    uint8_t lo = this->bus.read(this->registers.PC++);
    uint8_t hi = this->bus.read(this->registers.PC++);
    uint16_t addr = (hi << 8) | lo;

    uint8_t lsb_sp = static_cast<uint8_t>(this->registers.SP & 0xFF);
    uint8_t msb_sp = static_cast<uint8_t>(this->registers.SP >> 8);

    this->bus.write(addr, lsb_sp);
    this->bus.write(addr + 1, msb_sp);

    this->clock_cycles += 5;
}

void CPU::ldh_a_a8() {
    uint8_t a8 = this->bus.read(this->registers.PC++);
    uint16_t addr = 0xFF00 | a8;
    this->registers.A = this->bus.read(addr);
    this->clock_cycles += 3;
}

void CPU::ld_mc_a() {
    uint8_t a8 = this->bus.read(this->registers.PC++);
    uint16_t addr = 0xFF00 | this->registers.C;
    this->bus.write(addr, this->registers.A);
    this->clock_cycles += 2;
}


void CPU::ldh_a8_a() {
    uint8_t a8 = this->bus.read(this->registers.PC++);
    uint16_t addr = 0xFF00 | a8;
    this->bus.write(addr, this->registers.A);
    this->clock_cycles += 3;
}

void CPU::ld_a16_a() {
    uint8_t lo = this->bus.read(this->registers.PC++);
    uint8_t hi = this->bus.read(this->registers.PC++);
    uint16_t a16 = (static_cast<uint16_t>(hi) << 8) | lo;
    this->bus.write(a16, this->registers.A);
    this->clock_cycles += 4;
}

void CPU::ld_a_mc() {
    this->registers.A = this->bus.read(
        0xFF00 | static_cast<uint16_t>(this->registers.C)
    );
    this->clock_cycles += 2;
}

void CPU::ld_hl_sp_e8() {
    uint16_t sp = this->registers.SP;
    uint8_t operand = this->bus.read(this->registers.PC++);
    uint16_t res = sp + static_cast<int8_t>(operand);
    this->registers.setZ(false);
    this->registers.setN(false);
    this->registers.setH(((sp & 0x0F) + (operand & 0x0F)) > 0x0F);
    this->registers.setC(((sp & 0xFF) + (operand & 0xFF)) > 0xFF);
    this->registers.setReg16(Reg16::HL, res);
    this->clock_cycles += 3;
}

void CPU::ld_sp_hl() {
    this->registers.SP = this->registers.getReg16(Reg16::HL);
    this->clock_cycles+=2;
}

void CPU::ld_a_a16() {
    uint8_t lo = this->bus.read(this->registers.PC++);
    uint8_t hi = this->bus.read(this->registers.PC++);
    uint16_t addr = static_cast<uint16_t>(hi) << 8 | static_cast<uint16_t>(lo);
    this->registers.A = this->bus.read(addr);
    this->clock_cycles += 4;
}

// arithmetics
void CPU::inc_r16(Reg16 r) {
    this->registers.setReg16(r, this->registers.getReg16(r) + 1);
    this-> clock_cycles += 2;
}

void CPU::inc_r8(Reg8 r) {
    uint8_t old_val = this->registers.getReg8(r);
    uint8_t new_val = old_val + 1;
    this->registers.setReg8(r, new_val);

    this->registers.setZ(new_val == 0);
    this->registers.setN(false);
    this->registers.setH((old_val & 0x0F) == 0x0F);
    
    this->clock_cycles += 1;
}

void CPU::inc_mr(Reg16 mr) {
    uint16_t addr = this->registers.getReg16(mr);
    uint8_t old_val = this->bus.read(addr);
    uint8_t new_val = old_val + 1;
    this->bus.write(addr, new_val);

    this->registers.setZ(new_val == 0);
    this->registers.setN(false);
    this->registers.setH((old_val & 0x0F) == 0x0F);

    this->clock_cycles += 3;
}

void CPU::dec_r8(Reg8 r) {
    uint8_t old_val = this->registers.getReg8(r);
    uint8_t new_val = old_val - 1;
    this->registers.setReg8(r, new_val);

    this->registers.setZ(new_val == 0);
    this->registers.setN(true);
    this->registers.setH((old_val & 0x0F) == 0x00);
    
    this->clock_cycles += 1;
}

void CPU::dec_r16(Reg16 r) {
    uint16_t val = this->registers.getReg16(r);
    this->registers.setReg16(r, val - 1);
    this->clock_cycles += 2;
}

void CPU::dec_mr(Reg16 mr) {
    uint16_t addr = this->registers.getReg16(mr);
    uint8_t old_val = this->bus.read(addr);
    uint8_t new_val = old_val - 1;
    this->bus.write(addr, new_val);

    this->registers.setZ(new_val == 0);
    this->registers.setN(true);
    this->registers.setH((old_val & 0x0F) == 0x00);

    this->clock_cycles += 3;
}

void CPU::add_r8_r8(Reg8 r1, Reg8 r2) {
    uint8_t val1 = this->registers.getReg8(r1);
    uint8_t val2 = this->registers.getReg8(r2);
    uint8_t sum = val1 + val2;

    this->registers.setReg8(r1, sum);

    this->registers.setZ(sum == 0);
    this->registers.setN(false);
    this->registers.setH(((val1 & 0x0F) + (val2 & 0x0F)) > 0x0F);
    this->registers.setC((static_cast<uint16_t>(val1) + static_cast<uint16_t>(val2)) > 0xFF);

    this->clock_cycles += 1;
}

void CPU::add_r8_mr(Reg8 r, Reg16 mr) {
    uint8_t r_val = this->registers.getReg8(r);
    uint8_t mr_val = this->bus.read(this->registers.getReg16(mr));
    uint8_t sum = r_val + mr_val;

    this->registers.setReg8(r, sum);

    this->registers.setZ(sum == 0);
    this->registers.setN(false);
    this->registers.setH(((r_val & 0x0F) + (mr_val & 0x0F)) > 0x0F);
    this->registers.setC((static_cast<uint16_t>(r_val) + static_cast<uint16_t>(mr_val)) > 0xFF);

    this->clock_cycles += 3;
}

void CPU::ld_mr_n8(Reg16 reg16) {
    uint8_t n8 = this->bus.read(this->registers.PC++);
    this->bus.write(this->registers.getReg16(Reg16::HL), n8);
   this->clock_cycles += 3;
}

void CPU::add_r16_r16(Reg16 r1, Reg16 r2) {
    uint16_t val1 = this->registers.getReg16(r1);
    uint16_t val2 = this->registers.getReg16(r2);
    uint32_t sum = (uint32_t)val1 + (uint32_t)val2;

    this->registers.setReg16(r1, (uint16_t)sum);

    this->registers.setN(false);
    this->registers.setH(((val1 & 0x0FFF) + (val2 & 0x0FFF)) > 0x0FFF);
    this->registers.setC(sum > 0xFFFF);

    this->clock_cycles += 2;
}

void CPU::add_r8_n8(Reg8 r) {
    uint8_t n8 = this->bus.read(this->registers.PC++);
    uint8_t val = this->registers.getReg8(r);
    uint8_t sum = val + n8;

    this->registers.setReg8(r, sum);

    this->registers.setZ(sum == 0);
    this->registers.setN(false);
    this->registers.setH(((val & 0x0F) + (n8 & 0x0F)) > 0x0F);
    this->registers.setC(((uint16_t)val + (uint16_t)n8) > 0xFF);

    this->clock_cycles += 1;
}

void CPU::add_sp_e8() {
    uint16_t operand = this->bus.read(this->registers.PC++);
    int8_t e8 = static_cast<int8_t>(operand);
    uint16_t res = this->registers.SP + e8;
    this->registers.setZ(false);
    this->registers.setN(false);
    this->registers.setH(((this->registers.SP & 0x0F) + (operand & 0x0F)) > 0x0F);
    this->registers.setC(((this->registers.SP & 0xFF) + (operand & 0xFF)) > 0xFF);
    this->registers.SP = res;
    this->clock_cycles += 3;
}

void CPU::adc_a_r8(Reg8 r) {
    uint8_t a = this->registers.A;
    uint8_t reg_val = this->registers.getReg8(r);
    uint8_t cy = this->registers.getC() ? 1 : 0;

    int result = a + reg_val + cy;

    this->registers.setH(((a & 0x0F) + (reg_val & 0x0F) + cy) > 0x0F);
    this->registers.setC(result > 0xFF);

    this->registers.A = static_cast<uint8_t>(result);

    this->registers.setZ(this->registers.A == 0);
    this->registers.setN(false);

    this->clock_cycles++;
}

void CPU::adc_a_n8() {
    uint8_t n8 = this->bus.read(this->registers.PC++);
    uint8_t c_in = this->registers.getC();
    uint16_t res = this->registers.A + n8 + c_in;

    this->registers.setZ((res & 0xFF) == 0);
    this->registers.setN(0);
    this->registers.setH((this->registers.A & 0x0F) + (n8 & 0x0F) + c_in > 0x0F);
    this->registers.setC(res  > 0xFF);
    this->registers.A = static_cast<uint8_t>(res);
    this->clock_cycles++;
}

void CPU::adc_a_hl() {
    uint8_t data = this->bus.read(this->registers.getReg16(Reg16::HL));

    uint8_t a = this->registers.A;
    uint8_t cy = this->registers.getC() ? 1 : 0;

    int result = a + data + cy;

    this->registers.setH(((a & 0x0F) + (data & 0x0F) + cy) > 0x0F);
    this->registers.setC(result > 0xFF);
    
    this->registers.A = static_cast<uint8_t>(result);

    this->registers.setZ(this->registers.A == 0);
    this->registers.setN(false);

    this->clock_cycles += 2;
}

void CPU::sub(Reg8 r) {
    uint8_t a_val = this->registers.A;
    uint8_t r_val = this->registers.getReg8(r);
    uint8_t res = a_val - r_val;

    this->registers.setZ(res == 0);
    this->registers.setN(true);
    this->registers.setH((a_val & 0x0F) < (r_val & 0x0F));
    this->registers.setC(a_val < r_val);
    this->registers.A = res;
    this->clock_cycles += 1;
}

void CPU::sub_d8() {
    uint8_t d8 = this->bus.read(this->registers.PC++);
    uint8_t a_val = this->registers.A;
    uint8_t res = a_val - d8;

    this->registers.setZ(res == 0);
    this->registers.setN(true);
    this->registers.setH((a_val & 0x0F) < (d8 & 0x0F));
    this->registers.setC(a_val < d8);

    this->registers.A = res;
    this->clock_cycles += 1;
}

void CPU::sub_hl() {
    uint8_t data = this->bus.read(this->registers.getReg16(Reg16::HL));
    uint8_t a_val = this->registers.A;
    uint8_t res = a_val - data;

    this->registers.setZ(res == 0);
    this->registers.setN(true); 
    this->registers.setH((a_val & 0x0F) < (data & 0x0F));
    this->registers.setC(a_val < data);

    this->registers.A= res;
    this->clock_cycles += 2;
}

void CPU::sbc(Reg8 r) {
    int a_val = this->registers.A;
    int r_val = this->registers.getReg8(r);
    int cf = this->registers.getC() ? 1 : 0;
    int res = a_val - r_val - cf;

    this->registers.setZ((res & 0xFF) == 0);
    this->registers.setN(true);
    this->registers.setH(((a_val & 0xF) - (r_val & 0xF) - cf) < 0);
    this->registers.setC(res < 0);

    this->registers.A = static_cast<uint8_t>(res);
    this->clock_cycles += 1;
}

void CPU::sbc_d8() {
    uint8_t d8 = this->bus.read(this->registers.PC++);
    uint8_t cf = this->registers.getC() ? 1 : 0;
    uint8_t a_val = this->registers.A;
    int res = a_val - d8 - cf;

    this->registers.setZ((res & 0xFF) == 0);
    this->registers.setN(true);
    this->registers.setH((a_val & 0x0F) < (d8 & 0x0F) + cf);
    this->registers.setC(res < 0);   

    this->registers.A = res;
    this->clock_cycles += 1;
}

void CPU::sub_a_n8() {
    uint8_t n8 = this->bus.read(this->registers.PC++);
    int res = this->registers.A - n8;

    this->registers.setZ((res & 0xFF) == 0);
    this->registers.setN(true);
    this->registers.setH((this->registers.A & 0x0F) < (n8 & 0x0F));
    this->registers.setC((this->registers.A < n8));
    this->registers.A = res;
    this->clock_cycles++;
}

void CPU::sbc_a_n8() {
    uint8_t n8 = this->bus.read(this->registers.PC++);
    int res = this->registers.A - n8 - this->registers.getC();
    uint8_t c = this->registers.getC();  
    this->registers.setZ((res & 0xFF) == 0);
    this->registers.setN(true);
    this->registers.setH((this->registers.A & 0x0F) < ((n8 & 0x0F) + c));
    this->registers.setC(res < 0);
    this->registers.A = res;
    this->clock_cycles += 2;
}

void CPU::sbc_hl() {
    uint8_t cf = registers.getC() ? 1 : 0;
    uint8_t data = bus.read(registers.getReg16(Reg16::HL));
    uint8_t a_val = registers.A;

    uint16_t result = a_val - data - cf;
    uint8_t res = result & 0xFF;

    registers.setZ((res & 0xFF) == 0);
    registers.setN(true);

    registers.setH((a_val & 0xF) < ((data & 0xF) + cf));
    registers.setC(a_val < static_cast<uint16_t>(data) + cf);

    registers.A = res;
    clock_cycles += 2;
}

// Logic

void CPU::and_r8(Reg8 r) {
    this->registers.A &= this->registers.getReg8(r);
    this->registers.setZ(this->registers.A == 0);
    this->registers.setN(false);
    this->registers.setH(true);
    this->registers.setC(false);
    this->clock_cycles += 1;
}

void CPU::and_hl() {
    uint8_t data = this->bus.read(this->registers.getReg16(Reg16::HL));
    this->registers.A &= data;
    this->registers.setZ(this->registers.A == 0);
    this->registers.setN(false);
    this->registers.setH(true);
    this->registers.setC(false);
    this->clock_cycles += 2;
}


void CPU::xor_r8(Reg8 r) {
    this->registers.A ^= this->registers.getReg8(r);
    this->registers.setZ(this->registers.A == 0);
    this->registers.setN(false);
    this->registers.setH(false); 
    this->registers.setC(false);
    this->clock_cycles += 1;
}

void CPU::xor_hl() {
    uint8_t data = this->bus.read(this->registers.getReg16(Reg16::HL));
    this->registers.A ^= data;
    this->registers.setZ(this->registers.A == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(false);
    this->clock_cycles += 2;
}

void CPU::xor_a_n8() {
    uint8_t n8 = this->bus.read(this->registers.PC++);
    this->registers.A ^= n8;
    this->registers.setZ(this->registers.A == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(false);
    this->clock_cycles += 2;
}

void CPU::or_r8(Reg8 r) {
    this->registers.A |= this->registers.getReg8(r);
    this->registers.setZ(this->registers.A == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(false);
    this->clock_cycles += 1;
}

void CPU::or_hl() {
    uint8_t data = this->bus.read(this->registers.getReg16(Reg16::HL));
    this->registers.A |= data;
    this->registers.setZ(this->registers.A == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(false);
    this->clock_cycles += 1;
}

void CPU::or_a_n8() {
    uint8_t n8 = this->bus.read(this->registers.PC++);
    this->registers.A = this->registers.A | n8;
    this->registers.setZ(this->registers.A == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(false);
    this->clock_cycles += 2;
}

void CPU::and_a_n8() {
    uint8_t n8 = this->bus.read(this->registers.PC++);
    uint8_t res = this->registers.A & n8;
    this->registers.setZ(res == 0);
    this->registers.setN(false);
    this->registers.setH(true);
    this->registers.setC(false);
    this->registers.A = res;
    this->clock_cycles += 2;
}

// Stack

void CPU::pop(Reg16 r) {
    uint8_t lo = this->stkpop(); 
    uint8_t hi = this->stkpop();
    uint16_t data = (static_cast<uint16_t>(hi) << 8) | lo;
    this->registers.setReg16(r, data);
    this->clock_cycles += 3;
}

void CPU::push(Reg16 r) {
    uint16_t val = this->registers.getReg16(r);
    uint8_t hi = static_cast<uint8_t>((val & 0xFF00) >> 8);
    uint8_t lo = static_cast<uint8_t>(val & 0x00FF);
    
    this->stkpush(hi);
    this->stkpush(lo);
    
    this->clock_cycles += 4;
}

void CPU::stkpush(uint8_t data) {
    this->registers.SP = this->registers.SP - 1;
    this->bus.write(this->registers.SP, data);
}

uint8_t CPU::stkpop() {
    uint8_t val = this->bus.read(this->registers.SP);
    this->registers.SP = this->registers.SP + 1;
    return val;
}

// Acumulator Functions

void CPU::rlca() {
    uint8_t a = this->registers.A;
    
    bool c = ((a >> 7) & 1) != 0;
    uint8_t c_val = c ? 1 : 0;
    
    this->registers.A = (a << 1) | c_val;
    this->registers.setZ(false);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(c);
    this->clock_cycles++;
}

void CPU::rrca() {
    uint8_t a = this->registers.A;

    bool c = (a & 1) != 0;
    uint8_t c_val = c ? 1 : 0;
    
    a = a >> 1;
    a |= (c_val << 7);
    
    this->registers.A = a;

    this->registers.setZ(false);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(c);

    this->clock_cycles++;
}

void CPU::rla() {
    uint8_t a = this->registers.A;
    bool msb = (a >> 7) != 0;

    uint8_t c = this->registers.getC() ? 1 : 0;
    
    a = (a << 1) + c;
    
    this->registers.A = a;
    this->registers.setZ(false);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(msb);

    this->clock_cycles++;
}

void CPU::rra() {
    uint8_t a = this->registers.A;
    
    bool b0 = (a & 0x01) != 0; 
    uint8_t old_c = this->registers.getC() ? 1 : 0;
    
    a = (a >> 1) | (old_c << 7);
    
    this->registers.A = a;

    this->registers.setZ(false); 
    this->registers.setN(false);
    this->registers.setH(false); 
    this->registers.setC(b0); 
    
    this-> clock_cycles++;
}

// https://forums.nesdev.org/viewtopic.php?t=15944
void CPU::daa() {
    uint8_t a = this->registers.A;
    uint8_t adjustment = 0;

    // Addition
    if (!this->registers.getN()) { 
        if (this->registers.getH() || (a & 0x0F) > 0x09) {
            adjustment |= 0x06;
        }
        if (this->registers.getC() || a > 0x99) {
            adjustment |= 0x60;
            this->registers.setC(true);
        }
    // Subtraction
    } else { 
        if (this->registers.getH()) {
            adjustment |= 0x06;
        }
        if (this->registers.getC()) {
            adjustment |= 0x60;
        }
    }

    // Apply adjustment
    this->registers.A += this->registers.getN() ? -adjustment : adjustment;

    // Standard DAA flag updates
    this->registers.setZ(this->registers.A == 0);
    this->registers.setH(false);
    this->clock_cycles++;
}

// MISC
void CPU::cpl() {
    this->registers.A = ~this->registers.A;
    this->registers.setN(true);
    this->registers.setH(true);
    this->clock_cycles++;
}

void CPU::scf() {
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(true);
    this->clock_cycles++;
}

void CPU::ccf() {
    this->registers.setN(false);
    this->registers.setH(false);
    
    bool current_c = this->registers.getC();
    this->registers.setC(!current_c);
    
    this->clock_cycles++;
}

// compares 
void CPU::cp_r8(Reg8 r) {
    uint8_t val = this->registers.getReg8(r);
    uint8_t a = this->registers.A;
    uint8_t res = a - val;
    
    this->registers.setZ(res == 0);
    this->registers.setN(1);
    this->registers.setH((a & 0x0F) < (val & 0x0F));
    this->registers.setC(a < val);
    this->clock_cycles++;
}

void CPU::cp_hl() {
    uint8_t data = this->bus.read(this->registers.getReg16(Reg16::HL));
    uint8_t a = this->registers.A;

    // Flags logic
    this->registers.setZ(a == data);
    this->registers.setN(true);
    this->registers.setH((a & 0x0F) < (data & 0x0F));
    this->registers.setC(a < data);

    this->clock_cycles += 2;
}

void CPU::cp_a_n8() {
    uint8_t n8 = this->bus.read(this->registers.PC++);
    uint8_t res = this->registers.A - n8;
    this->registers.setZ(res == 0);
    this->registers.setN(true);
    this->registers.setH((this->registers.A  & 0x0F) < (n8 & 0x0F));
    this->registers.setC(this->registers.A < n8);
    this->clock_cycles += 2;
}

// https://gist.github.com/SonoSooS/c0055300670d678b5ae8433e20bea595#nop-and-stop
void CPU::nop(){ this->clock_cycles++;}

void CPU::stop() {
    this->registers.PC++;
    this->clock_cycles += 2;
}

void CPU::halt() {
    this->halted = true;
    this->clock_cycles++;
}

// Jumps, returns, etc

void CPU::jr_e8() {
    int8_t e8=  static_cast<int8_t>(this->bus.read(this->registers.PC++));
    this->registers.PC += e8;
    this->clock_cycles += 3;
}

void CPU::jr(Cond cond) {
    int8_t e8 = static_cast<int8_t>(this->bus.read(this->registers.PC++));
    if (check_cond(cond)) {
        this->registers.PC += e8;
        this->clock_cycles += 3;
        return;
    }
    this->clock_cycles += 2;
}

void CPU::jp_a16_cond(Cond cond) {
    if (check_cond(cond)) {
        uint8_t lo = this->bus.read(this->registers.PC++);
        uint8_t hi = this->bus.read(this->registers.PC++);
        this->registers.PC = static_cast<uint16_t>(hi) << 8 | lo;
        clock_cycles += 4;
        return;
    }
    this->registers.PC += 2;
    this->clock_cycles += 3;
}

void CPU::jp_a16() {
    uint8_t lo = this->bus.read(this->registers.PC++);
    uint8_t hi = this->bus.read(this->registers.PC++);
    this->registers.PC = static_cast<uint16_t>(hi) << 8 | lo;
    clock_cycles += 4;
    return;
}

void CPU::jp_hl() {
    this->registers.PC = this->registers.getReg16(Reg16::HL);
    this->clock_cycles++;
}


void CPU::ret_cond(Cond cond) {
    if(!check_cond(cond)) {
        clock_cycles += 2;
        return;
    }

    uint8_t lo = this->stkpop();
    uint8_t hi = this->stkpop();
    this->registers.PC = static_cast<uint16_t>(hi) << 8 | lo;
    clock_cycles += 5;
}

void CPU::call_cond_a16(Cond cond) {
    if (check_cond(cond)) {
        call_a16();
        return;
    }
    this->registers.PC += 2;
    this->clock_cycles += 3;
}

void CPU::call_a16() {
    uint8_t lo = this->bus.read(this->registers.PC++);
    uint8_t hi = this->bus.read(this->registers.PC++);
    uint16_t addr = static_cast<uint16_t>(hi) << 8 | lo;
    this->bus.write(--this->registers.SP, (this->registers.PC >> 8) & 0xFF);
    this->bus.write(--this->registers.SP, this->registers.PC & 0xFF);
    this->registers.PC = addr;
    this->clock_cycles += 6;
}

void CPU::ret() {
    uint8_t lo = this->stkpop();
    uint8_t hi = this->stkpop();
    this->registers.PC = static_cast<uint16_t>(hi) << 8 | lo;
    clock_cycles += 4;
}

void CPU::rst(uint16_t addr) {

    this->bus.write(--this->registers.SP, (this->registers.PC >> 8) & 0xFF);
    this->bus.write(--this->registers.SP, this->registers.PC & 0xFF);

    this->registers.PC = addr;
    
    this->clock_cycles += 4;
}

void CPU::reti() {
    this->IME = true;
    uint8_t lo = this->stkpop();
    uint8_t hi = this->stkpop();
    this->registers.PC = static_cast<uint16_t>(hi) << 8 | lo;
    clock_cycles += 4;
}

// The result of the specific Finite State Machine (FSM) designed into the Game Boy's Sharp LR35902 (SM83) silicon.
bool CPU::check_cond(Cond cond) {
    bool check = false;
    switch (cond) {
        case Cond::Z:    check = this->registers.getZ(); break;
        case Cond::C:    check = this->registers.getC(); break;
        case Cond::NZ:   check = !this->registers.getZ(); break;
        case Cond::NC:   check = !this->registers.getC(); break;
        case Cond::NONE: check = true; break;
    }
    return check;
}


//CB
void CPU::execute_cb(){
    uint8_t cb_op = this->bus.read(this->registers.PC++);
    uint8_t reg_idx = cb_op & 0x07;
    uint8_t bit_idx = (cb_op >> 3) & 0x07;
    uint8_t op_type = (cb_op >> 6) & 0x03;
    switch (op_type) {
        case(0): shift_rotate(reg_idx, bit_idx); break;
        case(1): bit(reg_idx, bit_idx); break;
        case(2): res(reg_idx, bit_idx); break;
        case(3): set(reg_idx, bit_idx); break;
    }
}

void CPU::bit(uint8_t reg_idx, uint8_t bit_idx) {
    uint8_t val = get_cb_val(reg_idx);
    uint8_t bit_mask = 1 << (bit_idx);
    this->registers.setZ((val & bit_mask) == 0);
    this->registers.setN(false);
    this->registers.setH(true);
    clock_cycles += 2;
}

void CPU::res(uint8_t reg_idx, uint8_t bit_idx) {
    uint8_t bit_mask = ~(1 << (bit_idx));
    uint8_t val = get_cb_val(reg_idx);
    set_cb_val(reg_idx, val & bit_mask);
    clock_cycles +=2;
}

void CPU::set(uint8_t reg_idx, uint8_t bit_idx) {
    uint8_t bit_mask = 1 << (bit_idx);
    uint8_t val = get_cb_val(reg_idx);
    set_cb_val(reg_idx, val | bit_mask);
    clock_cycles +=2;
}

void CPU::shift_rotate(uint8_t reg_idx, uint8_t bit_idx) {
    uint8_t val = get_cb_val(reg_idx);
    uint8_t res = 0;

    switch (bit_idx) {
        case 0: res = rlc(val); break;
        case 1: res = rrc(val); break;
        case 2: res = rl(val);  break;
        case 3: res = rr(val);  break;
        case 4: res = sla(val); break;
        case 5: res = sra(val); break;
        case 6: res = swap(val); break;
        case 7: res = srl(val); break;
    }
    set_cb_val(reg_idx, res);
}

uint8_t CPU::swap(uint8_t val) {
    uint8_t lo = val & 0x0F;
    uint8_t hi = val & 0xF0;
    uint8_t res = (lo << 4) | (hi >> 4);
    this->registers.setZ(res == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(false);
    clock_cycles +=2;
    return res;
}

uint8_t CPU::rlc(uint8_t val) {
    uint8_t res = val << 1;
    bool carry = (val & 0x80) != 0;
    res |= carry;
    this->registers.setZ(res == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(carry);
    clock_cycles +=2;
    return res;
}

uint8_t CPU::rl(uint8_t val) {
    uint8_t old_carry = this->registers.getC() ? 1 : 0;
    bool new_carry = (val & 0x80) != 0;
    uint8_t res = (val << 1) | old_carry;
    this->registers.setZ(res == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(new_carry);
    clock_cycles +=2;
    return res;
}

uint8_t CPU::rrc(uint8_t val) {
    uint8_t res = val >> 1;
    bool carry = (val & 0x01) != 0;
    if (carry) res |= 0x80;
    this->registers.setZ(res == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(carry);
    clock_cycles +=2;
    return res;
}

uint8_t CPU::rr(uint8_t val) {
    uint8_t old_carry = this->registers.getC() ? 0x80 : 0;
    bool new_carry = (val & 0x01) != 0;
    uint8_t res = (val >> 1) | old_carry;
    this->registers.setZ(res == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(new_carry);
    clock_cycles +=2;
    return res;
}

uint8_t CPU::sla(uint8_t val) {
    uint8_t res = val << 1;
    bool carry = (val & 0x80) != 0;
    this->registers.setZ(res == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC(carry);
    clock_cycles +=2;
    return res;
}

uint8_t CPU::sra(uint8_t val) {
    uint8_t res = val >> 1;
    bool carry = (val & 0x80) != 0;
    if (carry) res |= 0x80; // MSB doesn't change.
    this->registers.setZ(res == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC((val & 0x01) != 0);
    clock_cycles +=2;
    return res;
}

uint8_t CPU::srl(uint8_t val) {
    uint8_t res = ( val >> 1) & ~0x80; // MSB set to 0
    this->registers.setZ(res == 0);
    this->registers.setN(false);
    this->registers.setH(false);
    this->registers.setC((val & 0x01) != 0);
    clock_cycles +=2;
    return res;
}

// 0:B, 1:C, 2:D, 3:E, 4:H, 5:L, 6:(HL), 7:A
uint8_t CPU::get_cb_val(uint8_t reg_idx) {
    switch (reg_idx) {
        case(0): return this->registers.B;
        case(1): return this->registers.C;
        case(2): return this->registers.D;
        case(3): return this->registers.E;
        case(4): return this->registers.H;
        case(5): return this->registers.L;
        case(6): clock_cycles++; return this->bus.read(this->registers.getReg16(Reg16::HL));
        case(7): return this->registers.A;
        default: throw std::runtime_error("Invalid register in CB opcode");
    }
}

void CPU::set_cb_val(uint8_t reg_idx, uint8_t val) {
    switch (reg_idx) {
        case(0): this->registers.B = val; break;
        case(1): this->registers.C = val; break;
        case(2): this->registers.D = val; break;
        case(3): this->registers.E = val; break;
        case(4): this->registers.H = val; break;
        case(5): this->registers.L = val; break;
        case(6): clock_cycles++; this->bus.write(this->registers.getReg16(Reg16::HL), val); break;
        case(7): this->registers.A = val; break;
        default: throw std::runtime_error("Invalid register in CB opcode");
    }
}

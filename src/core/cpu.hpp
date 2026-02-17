#pragma once
#include <cstdint>
#include "registers.hpp"
#include "bus.hpp"
#include "interrupts.hpp"
#include "../log/logger.hpp"

enum class Cond {
    NZ, NC, Z, C, NONE
};

enum class CB_OP {
    BIT = 1,
    RES = 2,
    SET = 3,

};

class CPU {
    public:
        CPU(Bus& bus, Registers& registers);
        int step();
        
    private:
        Bus& bus;
        Registers& registers;

        //Internal accumulator for clock cycles. Reset after every step;
        int clock_cycles{0};

        /**
         * IME is a flag internal to the CPU that controls whether any interrupt handlers are called, 
         * regardless of the contents of IE. IME cannot be read in any way, 
         * and is modified by these instructions/events only:
         */
        bool IME{false};
        bool halted{false};
        // Assists with one instruction delay for EI and DI
        uint8_t IME_delay{0};

        
        uint8_t fetch();
        void execute(uint8_t opcode);
        void handle_interrupts();
        void service_interrupt(uint8_t interrupt, uint16_t addr);

        // opcode executors

        // interrupts
        void di();
        void ei();
        
        // accumulator rotators
        void rlca();
        void rrca();
        void rla();
        void rra();
        void daa();
        void rst(uint16_t addr);
        void reti();

        //misc
        void nop();
        void stop();
        void halt();
        void cpl();
        void scf();
        void ccf();
        void cp_r8(Reg8 r);
        void cp_hl();
        void cp_d8();
        void cp_a_n8();

        // loads
        void ld_r16_n16(Reg16 r);
        void ld_mr_r(Reg16 r16, Reg8 r8);
        void ld_r_mr(Reg8 r, Reg16 mr);
        void ld_r_hli(Reg8 r);
        void ld_hli_r(Reg8 r);
        void ld_r8_n8(Reg8 r);
        void ld_a16_sp();
        void ld_mr_n8(Reg16 mr);
        void ld_hld_r(Reg8 r);
        void ld_r_r(Reg8 r1, Reg8 r2);
        void ld_r_hld(Reg8 r);
        void ldh_a_a8();
        void ld_a_c();
        void ld_c_a();
        void ldh_a8_a();
        void ld_mc_a();
        void ld_a16_a();
        void ld_a_mc();
        void ld_hl_sp_e8();
        void ld_sp_hl();
        void ld_a_a16();

        // arithmetic 
        void inc_r16(Reg16);
        void inc_mr(Reg16 mr);
        void inc_r8(Reg8 r);
        void dec_r8(Reg8 r);
        void dec_r16(Reg16 r);
        void dec_mr(Reg16 mr);
        void add_r8_r8(Reg8 r1, Reg8 r2);
        void add_r8_mr(Reg8 r, Reg16 mr);
        void add_r16_r16(Reg16 r1, Reg16 r2);
        void add_r8_n8(Reg8 r);
        void add_sp_e8();
        void adc_a_r8(Reg8 r);
        void adc_a_n8();
        void adc_a_hl();
        void sub(Reg8 r);
        void sub_d8();
        void sub_hl();
        void sbc(Reg8 r);
        void sbc_d8();
        void sbc_hl();
        void sub_a_n8();
        void sbc_a_n8();

        // logic
        void and_r8(Reg8 r);
        void and_hl();
        void and_d8();
        void and_a_n8();
        void xor_r8(Reg8 r);
        void xor_a_n8();
        void xor_hl();
        void or_r8(Reg8 r);
        void or_hl();
        void or_d8();
        void or_a_n8();

        //stack
        void pop(Reg16 r);
        void push(Reg16 r);
        void stkpush(uint8_t data);
        uint8_t stkpop();

        //jumps & calls
        void jr_e8();
        void jr(Cond cond);
        void ret_cond(Cond cond);
        void ret();
        void jp_a16_cond(Cond cond);
        void jp_a16();
        void jp_hl();
        void call_cond_a16(Cond cond);
        void call_a16();

        //cb instructions
        void execute_cb();
        void bit(uint8_t reg_idx, uint8_t bit_idx);
        void res(uint8_t reg_idx, uint8_t bit_idx);
        void set(uint8_t reg_idx, uint8_t bit_idx);
        void shift_rotate(uint8_t reg_idx, uint8_t bit_idx);
        uint8_t rlc(uint8_t val);
        uint8_t rrc(uint8_t val);
        uint8_t rl(uint8_t val);
        uint8_t rr(uint8_t val);
        uint8_t sla(uint8_t val);
        uint8_t sra(uint8_t val);
        uint8_t swap(uint8_t val);
        uint8_t srl(uint8_t val);
        uint8_t get_cb_val(uint8_t reg_idx);
        void set_cb_val(uint8_t reg_idx, uint8_t val);

        //helpers
        bool check_cond(Cond cond);

};
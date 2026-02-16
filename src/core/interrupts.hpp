#pragma once
#include <cstdint>

//https://gbdev.io/pandocs/Interrupts.html#interrupt-handling
namespace Interrupt {

    // Bitmask for the IE ($FFFF) and IF ($FF0F) registers
    static constexpr uint8_t VBLANK   = 0x01;
    static constexpr uint8_t LCD_STAT = 0x02;
    static constexpr uint8_t TIMER    = 0x04;
    static constexpr uint8_t SERIAL   = 0x08;
    static constexpr uint8_t JOYPAD   = 0x10;

    // The jump addresses for the CPU
    static constexpr uint16_t ADDR_VBLANK   = 0x0040;
    static constexpr uint16_t ADDR_LCD_STAT = 0x0048;
    static constexpr uint16_t ADDR_TIMER    = 0x0050;
    static constexpr uint16_t ADDR_SERIAL   = 0x0058;
    static constexpr uint16_t ADDR_JOYPAD   = 0x0060;

}
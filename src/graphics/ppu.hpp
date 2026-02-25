#pragma once
#include <cstdint>
#include <algorithm>
#include <vector>
#include "screen.hpp"

//https://gbdev.io/pandocs/Rendering.html#rendering-overview
enum Mode {
    HBLANK   = 0,
    VBLANK   = 1,
    OAM_SCAN = 2,
    DRAW     = 3
};

class PPU {
    public:
        PPU(Screen& screen);
        void tick(int clock_cycles);

        void tick_dot();

        void write_vram(uint16_t addr, uint8_t data);
        uint8_t read_vram(uint16_t addr);

        void write_oam(uint16_t addr, uint8_t data, bool dma = false);
        uint8_t read_oam(uint16_t addr, bool dma = false);
        Mode mode{OAM_SCAN};
        bool vblank_interrupt{false};

        void ppu_io_registers_write(uint16_t addr, uint8_t data);
        uint8_t ppu_io_read(uint16_t addr);
        bool lcd_stat_interrupt{false};
        bool prev_lcd_stat_interrupt{false};
        static constexpr int FRAME_BUFFER_SIZE{160*144};
    private:
        Screen& screen;

        // This is the time unit. 1 clock cycle = 4 dots;
        int dots{0};
        int window_internal_line_counter{0};

        // Mode executors
        void oam_scan();

        uint16_t draw_buffer();
        uint8_t read_vram_internal(uint16_t addr);

        void increment_LY();

        void requestStatInterrupt();

        void draw_scanline();

        bool is_window_tile(uint8_t pixels_pushed) const;

        bool oam_scanned{false};
        bool scanline_drawn{false};
        bool hblank_happened{false};
        std::vector<uint32_t> sprite_buffer;

        void handle_stat_interrupt();

        /* ----- RAM ----- */
        uint8_t OAM[0x9F] = {};
        uint8_t VRAM[8192] = {};

        /* ----- PPU Registers ----- */
        // We initialize them to what's after the powerup sequence
        uint8_t LCDC{0x91};
        uint8_t LY{0x91};
        uint8_t LYC{0};
        uint8_t STAT{0x81};
        uint8_t SCY{0};
        uint8_t SCX{0};
        uint8_t WY{0};
        uint8_t WX{0};
        uint8_t BGP{0xFC};
        uint8_t DMA{0xFF};
        uint8_t OBP0{0};
        uint8_t OBP1{0};

        void setSTATBit(uint8_t bit, bool val);
        inline bool window_enabled() const {return (LCDC & 0x20) != 0;}
        // LCDC 6
        inline bool get_window_tile_map() const {return (LCDC & 0x40) != 0;}
        inline bool get_bg_tile_map() const {return (LCDC & 0x08) != 0;}

        /* ------- Pixel FIFO ---- */
        uint8_t frame_buffer[FRAME_BUFFER_SIZE]; // 20 tiles by # of rows
        int pixels_pushed = 0;
        int window_pixels_pushed{0};
        bool wy_cond{false};

        uint8_t get_tile_map_address(bool window_rendering, uint8_t window_pixels_pushed);
        uint16_t get_tile_data(bool window_rendering, uint8_t tile_id);

        void tile_data_to_pixels(bool window_rendering, uint16_t tile_data);

        inline void set_mode(Mode new_mode) {
            mode = new_mode;
            if (new_mode == 0 && (STAT & 0x08)) lcd_stat_interrupt = true; // HBlank
            if (new_mode == 1 && (STAT & 0x10)) lcd_stat_interrupt = true; // VBlank
            if (new_mode == 2 && (STAT & 0x20)) lcd_stat_interrupt = true; // OAM

            // STAT bits 0–1 store current PPU mode
            STAT = (STAT & 0xFC) | static_cast<uint8_t>(new_mode);
        }
};
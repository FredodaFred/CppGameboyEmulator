#pragma once
#include <cstdint>
#include <algorithm>
#include <vector>
#include "screen.hpp"

//https://gbdev.io/pandocs/Rendering.html#rendering-overview
enum Mode {
    VBLANK, HBLANK, DRAW, OAM_SCAN
};

class PPU {
    public:
        PPU(Screen& screen);
        void tick(int clock_cycles);
        void write_vram(uint16_t addr, uint8_t data);
        uint8_t read_vram(uint16_t addr);
        void write_oam(uint16_t addr, uint8_t data, bool dma = false);
        uint8_t read_oam(uint16_t addr, bool dma = false);
        Mode mode{OAM_SCAN};

        void ppu_io_registers_write(uint16_t addr, uint8_t data);
        uint8_t ppu_io_read(uint16_t addr);
        bool lcd_stat_interrupt{false};
        bool prev_lcd_stat_interrupt{false};
        static constexpr int FRAME_BUFFER_SIZE{(160/8)* 144};


    private:
        Screen& screen;

        // This is the time unit. 1 clock cycle = 4 dots;
        int dots{0};
        int window_internal_line_counter{0};

        // Mode executors
        void oam_scan();

        uint16_t draw_buffer();

        std::vector<unsigned short> draw_scanline();

        bool oam_scanned{false};
        bool scanline_drawn{false};
        bool hblank_happened{false};
        uint8_t sprite_buffer[40]{};

        void handle_stat_interrupt();

        /* ----- RAM ----- */
        uint8_t VRAM[8192] = {};
        uint8_t OAM[0x9F] = {};

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

        /* ------- Pixel FIFO ---- */
        uint16_t frame_buffer[FRAME_BUFFER_SIZE];             // 20 tiles by # of rows
        //std::vector<uint8_t> bg_fifo;
        std::vector<uint8_t> oam_fifo;
        bool wy_cond{false};

        uint8_t get_tile_map_address(bool window_rendering, int pixels_pushed);
        uint16_t get_tile_data(bool window_rendering, uint8_t tile_id);
        uint16_t tile_data_to_pixels(uint16_t tile_data);
};
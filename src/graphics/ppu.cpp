#include "ppu.hpp"

using namespace std;

PPU::PPU(Screen& screen) : screen(screen) {}

// one frame 70224 dots
void PPU::tick(int cycles) {
    for (int i = 0; i < cycles * 4; i++) {
        tick_dot();
    }
}

void PPU::tick_dot() {
    dots++;

    if (LY >= 144) {
        if (dots == 1 && LY == 144) {
            set_mode(VBLANK);
            vblank_interrupt = true;
            window_internal_line_counter = 0;
        }

        if (dots >= 456) {
            dots = 0;
            increment_LY();

            if (LY == 153) {
                LY = 0;
                set_mode(OAM_SCAN);
                vblank_interrupt = false;
                screen.render(frame_buffer, FRAME_BUFFER_SIZE);
            }
        }
        return;
    }

    if (dots == 1) {
        set_mode(OAM_SCAN);
    } else if (dots == 81) {
        set_mode(DRAW);
        pixels_pushed = 0;
        draw_scanline();
    } else if (dots == 253) {
        set_mode(HBLANK);
    }

    if (dots >= 456) {
        dots = 0;
        increment_LY();
        oam_scanned = false;
        scanline_drawn = false;
        hblank_happened = false;
    }
}

void PPU::oam_scan() {
    wy_cond = (WY == LY) && window_enabled();
    ranges::fill(sprite_buffer, 0);
    int obj_idx = 0;
    for (int byte = 0; byte < 160; byte += 4) {
        uint8_t y_pos = OAM[byte];
        uint8_t x_pos = OAM[byte+1];
        uint8_t tile_idx = OAM[byte+2];
        uint8_t attr_flags = OAM[byte+3];
        int size = (LCDC & 0x04) ? 2 : 1; // 0 = 8x8, 1= 8x16
        bool in_range = LY + 16 >= y_pos && LY + 16 < y_pos  + 8 * size;
        if (in_range && obj_idx != 10) {

            int offset = obj_idx * 4;
            sprite_buffer[offset] = y_pos;
            sprite_buffer[offset+1] = x_pos;
            sprite_buffer[offset+2] = tile_idx;
            sprite_buffer[offset+3] = attr_flags;
            obj_idx++;
        }
    }
    oam_scanned = true;
}


// #define LCDS_LYC (BIT(lcd_get_context()->lcds, 2))
// #define LCDS_LYC_SET(b) (BIT_SET(lcd_get_context()->lcds, 2, b))
void PPU::increment_LY() {
    LY++;
    if (LY == LYC) {
        setSTATBit(2, true);
        uint8_t SS_LYC = (1 << 6);
        if (STAT & SS_LYC) {
            requestStatInterrupt();
        }
    } else {
        setSTATBit(2, false);
    }
}

void PPU::requestStatInterrupt() {
    lcd_stat_interrupt = true;
}

void PPU::draw_scanline() {
    // Note: Scanline (horizontal) is 160 pixels long. So each pixel_pushed increases x_pos by 1
    while (pixels_pushed < 160) {
        bool wx_cond = WX == (pixels_pushed - 7);
        bool window_rendering = window_enabled() && wx_cond && wy_cond;
        uint8_t tile_id = get_tile_map_address(window_rendering);
        uint16_t tile_data = get_tile_data(window_rendering, tile_id);
        tile_data_to_pixels(tile_data);
    }
    scanline_drawn = true;
}

/**
 * This returns tileId. By finding the correct tileID, we can use this id to get to the correct tile data/
 */
uint8_t PPU::get_tile_map_address(bool window_rendering) {
    bool window_tile_map = (LCDC & 0x40) != 0; // LCDC 6
    bool bg_tile_map = ((LCDC & 0x08) != 0) ; // LCDC 3
    uint8_t x_pos;
    uint8_t y_pos;
    uint16_t tile_map_addr;

    if (window_rendering) {
        int window_x = pixels_pushed - (WX - 7);
        if (window_x < 0) x_pos = 0; else x_pos = pixels_pushed - (WX - 7);
        y_pos = window_internal_line_counter;
        tile_map_addr = window_tile_map ? 0x9C00 : 0x9800;
    } else {
        tile_map_addr = bg_tile_map ? 0x9C00 : 0x9800;
        x_pos = ((SCX + pixels_pushed) / 8) & 0x1F;
        y_pos = (LY + SCY) & 255;
    }
    uint16_t offset = ((y_pos / 8) * 32 ) + x_pos;
    uint16_t addr = tile_map_addr + offset;
    return read_vram_internal(addr);
}

//https://gbdev.io/pandocs/Tile_Data.html#vram-tile-data
uint16_t PPU::get_tile_data(bool window_rendering, uint8_t tile_id) {
    /* 0 = 8800–97FF; 1 = 8000–8FFF
     * We are drawing at a y_pos, so we only need one row (2 bytes) from the entire tile.
     * Therefore, we will grab the correct "index" for the row in the tile data ( think row 1 2bytes | row 2 2 byes) / sequential memory */

    uint8_t row = window_rendering ? window_internal_line_counter % 8 : (LY + SCY) % 8;
    uint16_t base;
    if (LCDC & 0x10) {
        base = 0x8000 + (tile_id * 16);
    } else {
        int8_t signed_tile_id = static_cast<int8_t>(tile_id);
        base = 0x9000 +  (signed_tile_id * 16);
    }
    uint16_t low_tile_data = read_vram_internal(base + row*2 ); //row*2 because its 2 bytes per index.
    uint16_t high_tile_data = read_vram_internal(base + 1 + row*2 );
    uint16_t tile_data = high_tile_data << 8 | low_tile_data;
    return tile_data;
}

/**
 * We will convert from 2BPP form into pixel | pixel | pixel | ... form.
 * Each pixel is 2 bits.
 * Once transformed, we can evaluate each pixel and push it directly to the frame_buffer
 *
 * first byte specifies the least significant bit of the color ID
 * second byte specifies the most significant bit
 *
 *
 * @param tile_data
 * @return transformed_data
 */
void PPU::tile_data_to_pixels(uint16_t tile_data) {
    uint8_t low = static_cast<uint8_t>(tile_data & 0x00FF); // Low is at the bottom
    uint8_t high = static_cast<uint8_t>((tile_data & 0xFF00) >> 8); // High is at the top
    for (int i = 0; i < 8; i++) {
        uint8_t bit_idx = 7 - i;
        uint8_t b0 = (low >> bit_idx) & 1;
        uint8_t b1 = (high >> bit_idx) & 1;
        uint8_t pixel = (b1 << 1) | b0;

        int screen_x = pixels_pushed - (SCX % 8);
        // Only write to the buffer if the pixel is actually visible on screen
        if (screen_x >= 0 && screen_x < 160) {
            int buffer_index = (LY * 160) + screen_x;
            frame_buffer[buffer_index] = pixel;
        }
        pixels_pushed++;
    }
}


void PPU::setSTATBit(uint8_t bit, bool val) {
    if (val) STAT |= (1 << bit);
    else     STAT &= ~(1 << bit);
}

// https://nnarain.github.io/2016/09/09/Gameboy-LCD-Controller.html
// Remember, only the CPU changes bits 3,4,5,6 of STAT. Same idea as the IF register.
void PPU::handle_stat_interrupt() {
    // 2 flag must mirror LY == LYC, this is constantly being checked (according to pan docs)
    setSTATBit(2, LY == LYC);
    // Collects any valid stat interrupts
    bool current_stat_line = ((LY == LYC) && (STAT & 0x40)) ||         // LYC == LY interrupt
                             ((mode == OAM_SCAN) && (STAT & 0x20)) ||  // OAM Interrupt
                             ((mode == VBLANK) && (STAT & 0x10)) ||   // VBLANK interrupt
                             ((mode == HBLANK) && (STAT & 0x08));    // HBLANK interrupt

    if (current_stat_line && !prev_lcd_stat_interrupt) {
        lcd_stat_interrupt = true;
    } else if (!current_stat_line && prev_lcd_stat_interrupt) {
        lcd_stat_interrupt = false;
    }

    prev_lcd_stat_interrupt = current_stat_line;
}

void PPU::ppu_io_registers_write(uint16_t addr, uint8_t data) {
    switch (addr) {
        // LCD Control and Status
        case 0xFF40: LCDC = data; break;
        case 0xFF41: STAT = (data & 0x78) | (STAT & 0x07);break;

        // LCD Position and Scrolling
        case 0xFF42: SCY  = data; break;
        case 0xFF43: SCX  = data; break;
        case 0xFF45: LYC  = data; break;

        // Window Position
        case 0xFF4A: WY   = data; break;
        case 0xFF4B: WX   = data; break;

        // LCD Palettes
        case 0xFF47: BGP  = data; break;
        case 0xFF48: OBP0 = data; break;
        case 0xFF49: OBP1 = data; break;

        default: break;
    }
}
uint8_t PPU::ppu_io_read(uint16_t addr) {
    switch (addr) {
        // LCD Control and Status
        case 0xFF40: return LCDC;
        case 0xFF41: return STAT;

        // LCD Position and Scrolling
        case 0xFF42: return SCY;
        case 0xFF43: return SCX;
        case 0xFF44: return LY;
        case 0xFF45: return LYC;

        // Window Position
        case 0xFF4A: return WY;
        case 0xFF4B: return WX;

        // LCD Palettes
        case 0xFF47: return BGP;
        case 0xFF48: return OBP0;
        case 0xFF49: return OBP1;

        default: return 0xFF;
    }
}

/*-------- RAM -------- */
void PPU::write_vram(uint16_t addr, uint8_t data) {
    VRAM[addr - 0x8000] = data;
}

uint8_t PPU::read_vram(uint16_t addr) {
        if (mode == DRAW) {
        // return garbage data if drawing to ignore
        return 0xFF;
    }
    return VRAM[addr - 0x8000];
}

uint8_t PPU::read_vram_internal(uint16_t addr) {
    return VRAM[addr - 0x8000];
}

void PPU::write_oam(uint16_t addr, uint8_t data, bool dma) {
    if ((mode == DRAW | mode == OAM_SCAN) && !dma) {
        return;
    }
    OAM[addr - 0xFE00] = data;
}


uint8_t PPU::read_oam(uint16_t addr, bool dma) {
    if ((mode == DRAW | mode == OAM_SCAN) && !dma)  {
        return 0xFF;
    }
    return OAM[addr - 0xFE00];
}

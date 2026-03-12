#pragma once
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <format>
#include <iomanip>
#include "../core/registers.hpp"
#include "../core/cart.hpp"

class Logger {
    public:
        static void log_cpu_state(Registers& registers, uint8_t opcode);

        static void vram_dump(const uint8_t *VRAM);

        static void log_cart_header(Cart& cart);
        static void open(const std::string& filename);
        static void log_msg(const std::string & msg);
        static void close();
        static void set_enabled(bool state) { enabled = state; }
        static bool is_enabled() {return enabled;}

    private:
        static std::ofstream file_stream;
        static bool enabled;
        static std::string get_readable(uint8_t opcode);
        static int lines_since_flush;

};

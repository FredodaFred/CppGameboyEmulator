#pragma once

#include <cstdint>
#include <GLFW/glfw3.h>
#include <iostream>

class Joypad {
    public:
        static bool interrupt;
        static bool UP_PRESSED;
        static bool DOWN_PRESSED;
        static bool LEFT_PRESSED;
        static bool RIGHT_PRESSED;
        static bool A_PRESSED;
        static bool B_PRESSED;
        static bool START_PRESSED;
        static bool SELECT_PRESSED;
        static bool D_PAD;
        static bool KEYS;
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static uint8_t get_joypad_reg();
};
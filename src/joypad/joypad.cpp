#include "joypad.hpp"

using namespace std;

bool Joypad::KEYS = false;
bool Joypad::D_PAD  = true;
bool Joypad::interrupt = false;
bool Joypad::UP_PRESSED = false;
bool Joypad::DOWN_PRESSED = false;
bool Joypad::LEFT_PRESSED = false;
bool Joypad::RIGHT_PRESSED = false;
bool Joypad::A_PRESSED = false;
bool Joypad::B_PRESSED = false;
bool Joypad::START_PRESSED = false;
bool Joypad::SELECT_PRESSED = false;

void Joypad::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (D_PAD) { // Select D-Pad
            switch (key) {
                case (GLFW_KEY_RIGHT):
                    Joypad::RIGHT_PRESSED = true;
                    interrupt = true;
                    break;
                case (GLFW_KEY_LEFT):
                    Joypad::LEFT_PRESSED = true;
                    interrupt = true;
                    break;
                case (GLFW_KEY_UP):
                    Joypad::UP_PRESSED = true;
                    interrupt = true;
                    break;
                case (GLFW_KEY_DOWN):
                    Joypad::DOWN_PRESSED = true;
                    interrupt = true;
                    break;
            }
        }
        if (KEYS) {
            switch (key) {
                case (GLFW_KEY_A):  // Let's map A to A
                    Joypad::A_PRESSED = true;
                    interrupt = true;
                    break;
                case (GLFW_KEY_S): // Let's map S to B
                    Joypad::B_PRESSED = true;
                    interrupt = true;
                    break;
                case (GLFW_KEY_D): // D to Select
                    Joypad::SELECT_PRESSED = true;
                    interrupt = true;
                    break;
                case (GLFW_KEY_F): // F to Start
                    Joypad::START_PRESSED = true;
                    interrupt = true;
                    break;
            }
        }

    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case (GLFW_KEY_A):
                Joypad::A_PRESSED = false;
                break;
            case (GLFW_KEY_RIGHT):
                Joypad::RIGHT_PRESSED = false;
                break;
            case (GLFW_KEY_S):
                Joypad::B_PRESSED = false;
                break;
            case (GLFW_KEY_LEFT):
                Joypad::LEFT_PRESSED = false;
                break;
            case (GLFW_KEY_D):
                Joypad::SELECT_PRESSED = false;
                break;
            case (GLFW_KEY_UP):
                Joypad::UP_PRESSED = false;
                break;
            case (GLFW_KEY_F):
                Joypad::START_PRESSED = false;
                break;
            case (GLFW_KEY_DOWN):
                Joypad::DOWN_PRESSED = false;
                break;
        }
    }
}
uint8_t Joypad::get_joypad_reg() {
    uint8_t reg = 0xCF;
    if (D_PAD) {
        if (LEFT_PRESSED) {
            reg &= ~(1 << 1);
        }
        if (RIGHT_PRESSED) {
            reg &= ~(1 << 0);
        }
        if (UP_PRESSED) {
            reg &= ~(1 << 2);
        }
        if (DOWN_PRESSED) {
            reg &= ~(1 << 3);
        }
    }

    if (KEYS) {
        if (START_PRESSED) {
            reg &= ~(1 << 3);
        }
        if (SELECT_PRESSED) {
            reg &= ~(1 << 2);
        }
        if (A_PRESSED) {
            reg &= ~(1 << 0);
        }
        if (B_PRESSED) {
            reg &= ~(1 << 1);
        }
    }
    return reg;
}

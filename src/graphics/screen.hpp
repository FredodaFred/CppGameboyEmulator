#pragma once

#include <OpenGL/OpenGL.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <format>
#include "../log/logger.hpp"

class Screen {
    public:
        Screen() = default;
        void init();
        void close();
        void render(const uint8_t *frame_buffer, int size);
        void tick();

    private:
        GLFWwindow* window;
        GLuint texture_id{0}; // The handle for the 160x144 image
        GLuint pbo_id{0};     // The handle for the Pixel Buffer Object
        GLuint vao;           // the handle for VAO
        GLuint shader_program;  // shader for openGL (we don't actually use this, the library forces us to make it)

        struct RGB {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };

        static constexpr int windowWidth = 160;
        static constexpr int windowHeight = 144;
        void draw_pixel_to_buffer(uint8_t *curr_pixels, int idx, uint8_t pixel);
        RGB value_to_rgb(uint8_t pixel);
        void compile_shaders();
};
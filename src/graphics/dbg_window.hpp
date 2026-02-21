#pragma once
#include "core/bus.hpp"

class DbgWindow {
    public:
        DbgWindow(Bus& bus);
        void tick();



        void init(bool enabled);
    private:
        Bus& bus;
        int BUFFER_SIZE = 16*384*8; //384 tile * 16 byte/tile * 8 pixels/byte
        bool enabled = false;
        int frame_skip = 0;
        const int windowHeight = 480;
        const int windowWidth = 640;
        GLFWwindow* window;
        GLuint texture_id{2}; // The handle for the 160x144 image
        GLuint pbo_id{2};     // The handle for the Pixel Buffer Object
        GLuint vao{2};           // the handle for VAO
        GLuint shader_program;  // shader for openGL (we don't actually use this, the library forces us to make it)

        struct RGB {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };

        void render(const uint8_t *frame_buffer, int size);

        void draw_pixel_to_buffer(uint8_t *curr_pixels, int idx, uint8_t pixel);

        DbgWindow::RGB value_to_rgb(uint8_t pixel);

        void compile_shaders();
};


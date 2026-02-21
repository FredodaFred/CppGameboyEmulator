//
// Created by Frederick Sion on 2/21/26.
//

#include "dbg_window.hpp"

DbgWindow::DbgWindow(Bus& bus)
    : bus(bus)
{}


void DbgWindow::tick() {
    if (!enabled) {
        return;
    }
    if (frame_skip != 1000) {
        frame_skip++;
        return;
    } else {
        frame_skip = 0;
    }
    glfwPollEvents();
    uint8_t frame_buffer[BUFFER_SIZE];
    uint16_t base = 0x8000;

    // We want to display 384 tiles in a grid (16 tiles wide, 24 tiles high)
    for (int tile_idx = 0; tile_idx < 384; tile_idx++) {
        for (int row = 0; row < 8; row++) {
            // Each row of a tile is 2 bytes
            uint8_t low = bus.read(base + (tile_idx * 16) + (row * 2));
            uint8_t high = bus.read(base + (tile_idx * 16) + (row * 2) + 1);

            for (int col = 0; col < 8; col++) {
                // Calculate pixel value (0-3)
                uint8_t bit = 7 - col;
                uint8_t pixel = ((high >> bit) & 1) << 1 | ((low >> bit) & 1);

                // Calculate where this pixel goes in a 16-tile wide grid
                int tile_x = tile_idx % 16;
                int tile_y = tile_idx / 16;

                int x = (tile_x * 8) + col;
                int y = (tile_y * 8) + row;

                int final_idx = x + (y * (16 * 8)); // 16 tiles * 8 pixels wide

                if (final_idx < BUFFER_SIZE) {
                    frame_buffer[final_idx] = pixel;
                }
            }
        }
    }
    render(frame_buffer, BUFFER_SIZE);
}

void DbgWindow::render(const uint8_t* frame_buffer, const int size) {
    glfwMakeContextCurrent(window);
    // "summon" the PBO and it's stored pixels
    if (pbo_id == 0) {
        std::cerr << "Error: PBO ID is 0. Did init() run?" << std::endl;
        return;
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id);
    uint8_t* curr_pixels = (uint8_t*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    if (!curr_pixels) {
        std::cerr << "PBO Mapping failed! OpenGL Error: " << glGetError() << std::endl;
        return;
    }
    int curr_pixels_index = 0;

    // i is 8 pixels,
    for (int i = 0; i < size; i++) {
        uint8_t pixel = frame_buffer[i];
        draw_pixel_to_buffer(curr_pixels, curr_pixels_index, pixel);
        curr_pixels_index += 3;
    }

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

    // 1. Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 2. Setup the "Draw"
    glUseProgram(shader_program);
    glBindVertexArray(vao); // Use the quad we built in init
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // 3. Move PBO data to Texture and Draw
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 4. Show the frame
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void DbgWindow::draw_pixel_to_buffer(uint8_t* curr_pixels, int idx, uint8_t pixel) {
    //     loc in normal buff times 3 bc each pixel is 3 bytes
    RGB color = value_to_rgb(pixel);
    curr_pixels[idx + 0] = color.r; // R
    curr_pixels[idx + 1] = color.g; // G
    curr_pixels[idx + 2] = color.b; // B
}

DbgWindow::RGB DbgWindow::value_to_rgb(uint8_t pixel) {
    switch (pixel) {
        case 0: return RGB(255, 255, 255); /// white
        case 1: return RGB(211, 211, 211); // light gray
        case 2: return RGB(169, 169, 169); // dark gray
        case 3: return RGB(0, 0, 0); // dark
        default: std::runtime_error("Unknown pixel");
    }
}

void DbgWindow::init(bool enabled) {
    this->enabled = enabled;
    if (!enabled) {
        return;
    }
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "DBGWindow", nullptr, nullptr);
    if (!window) throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) throw std::runtime_error("GLAD failed");

    // 1. TEXTURE - MUST BE 160x144
    glGenTextures(2, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 2. PBO
    glGenBuffers(2, &pbo_id);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, BUFFER_SIZE * 3, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    // 3. GEOMETRY (VAO/VBO)
    float vertices[] = {
        -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 0.0f
    };

    glGenVertexArrays(2, &vao);
    GLuint vbo;
    glGenBuffers(2, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    compile_shaders();
}

void DbgWindow::compile_shaders() {
    const char* vSrc = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* fSrc = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D screenTexture;
        void main() {
            FragColor = texture(screenTexture, TexCoord);
        }
    )";

    auto compile = [](GLenum type, const char* src) {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);

        int success;
        glGetShaderiv(s, GL_COMPILE_STATUS, &success);
        if (!success) {
            char info[512];
            glGetShaderInfoLog(s, 512, nullptr, info);
            std::cerr << "Shader Error: " << info << std::endl;
        }
        return s;
    };

    GLuint vs = compile(GL_VERTEX_SHADER, vSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fSrc);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);

    glDeleteShader(vs);
    glDeleteShader(fs);
}
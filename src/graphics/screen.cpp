#include "screen.hpp"

void Screen::tick() {
    glfwPollEvents();
}


/**
 *  left -> right in an uint16_t. this draws 8 pixels to screen, each 2 bit value being a color
 * 0b 11 10 01 01 11 10 01 10
 *
 */

void Screen::render(const uint16_t* frame_buffer) {
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
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++) {
        uint16_t pixels8 = frame_buffer[i];
        // unpack a single 8bit sequence
        for (int j = 0; j < 8; j++) {
            uint8_t pixel = pixels8 >> 14;
            pixels8 <<= 2; // so the previous line works, we make the next "pixel" the MSB
            draw_pixel_to_buffer(curr_pixels, curr_pixels_index, pixel);
            curr_pixels_index += 3; // 1 pixel is 3 bytes

        }

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
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 160, 144, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 4. Show the frame
    glfwSwapBuffers(window);
    glfwPollEvents();
}

// Each pixel is RGB = 3 bytes
void Screen::draw_pixel_to_buffer(uint8_t* curr_pixels, int idx, uint8_t pixel) {
    //     loc in normal buff times 3 bc each pixel is 3 bytes
    RGB color = value_to_rgb(pixel);
    curr_pixels[idx + 0] = color.r; // R
    curr_pixels[idx + 1] = color.g; // G
    curr_pixels[idx + 2] = color.b; // B
}

void Screen::init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "Gameboy", nullptr, nullptr);
    if (!window) throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) throw std::runtime_error("GLAD failed");

    // 1. TEXTURE - MUST BE 160x144
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 160, 144, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 2. PBO
    glGenBuffers(1, &pbo_id);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, 160 * 144 * 3, nullptr, GL_DYNAMIC_DRAW);
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

    glGenVertexArrays(1, &vao);
    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    compile_shaders();
}

void Screen::close() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

Screen::RGB Screen::value_to_rgb(uint8_t pixel) {
    switch (pixel) {
        case 0: return RGB(255, 255, 255); /// white
        case 1: return RGB(211, 211, 211); // light gray
        case 2: return RGB(169, 169, 169); // dark gray
        case 3: return RGB(0, 0, 0); // dark
        default: std::runtime_error("Unknown pixel");
    }
}

// Thanks gemini...
void Screen::compile_shaders() {
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
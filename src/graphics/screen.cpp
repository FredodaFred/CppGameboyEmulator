#include "screen.hpp"

#include "joypad/joypad.hpp"

void Screen::tick() {
    glfwPollEvents();
}

/**
 *  left -> right in an uint16_t. this draws 8 pixels to screen, each 2 bit value being a color
 * 0b 11 10 01 01 11 10 01 10
 *
 */
void Screen::render(const uint8_t* frame_buffer, const int size) {
    glfwMakeContextCurrent(window);
    // "summon" the PBO and it's stored pixels
    if (pbo_id == 0) {
        std::cerr << "Error: PBO ID is 0. Did init() run?" << std::endl;
        return;
    }

    uint8_t pixels[160 * 144 * 3];
    for (int i = 0; i < size; i++) {
        int idx = i * 3;
        uint8_t color = frame_buffer[i];
        pixels[idx + 0] = rgb_palette[color][0];
        pixels[idx + 1] = rgb_palette[color][1];
        pixels[idx + 2] = rgb_palette[color][2];
    }

    // ===== FAST UPLOAD (NO MAPPING) =====
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id);
    glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, 160 * 144 * 3, pixels);

    // ===== REST IS SAME =====
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 160, 144, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

GLFWwindow* Screen::init() {
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
    glfwSetKeyCallback(window, Joypad::key_callback);
    return window;
}

void Screen::close() {
    glfwDestroyWindow(window);
    glfwTerminate();
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
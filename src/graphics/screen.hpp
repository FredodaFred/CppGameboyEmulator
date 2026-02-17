#include <OpenGL/OpenGL.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Screen {
    public:
        Screen() = default;
        void init();
    private:
        GLFWwindow* window;
        static constexpr int windowWidth = 144;
        static constexpr int windowHeight = 160;
};
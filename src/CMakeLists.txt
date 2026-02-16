add_library(Core STATIC
    core/cart.cpp
    core/cart.hpp
    core/cpu.cpp
    core/cpu.hpp
    core/bus.cpp
    core/bus.hpp
    core/registers.cpp
    core/registers.hpp
    core/ppu.cpp
    core/ppu.hpp
    
    log/logger.hpp
    log/logger.cpp
)

target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(GameBoyCpp

    runtime/emulator.hpp
    runtime/emulator.cpp
    runtime/main.cpp
)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(GameBoyCpp
    PRIVATE
        Core
        OpenGL::GL
        glfw
)
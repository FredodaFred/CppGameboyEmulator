set(GLAD_SRC ${CMAKE_SOURCE_DIR}/external/src/glad.c)
set_source_files_properties(${GLAD_SRC} PROPERTIES LANGUAGE C)

add_library(Core STATIC
    core/cart.cpp
    core/cart.hpp
    core/cpu.cpp
    core/cpu.hpp
    core/bus.cpp
    core/bus.hpp
    core/registers.cpp
    core/registers.hpp

    log/logger.hpp
    log/logger.cpp

    io/timer.cpp
    io/timer.hpp

    ${GLAD_SRC}

    graphics/screen.cpp
    graphics/screen.hpp
    graphics/ppu.cpp
    graphics/ppu.hpp
        graphics/dbg_window.cpp
        graphics/dbg_window.hpp
)

target_include_directories(Core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/external/include
        /opt/homebrew/include
)

add_executable(GameBoyCpp
    runtime/emulator.hpp
    runtime/emulator.cpp
    runtime/main.cpp
)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(Core PUBLIC OpenGL::GL glfw)

target_link_libraries(GameBoyCpp
    PRIVATE
        Core
)

if(APPLE)
    target_link_libraries(Core PUBLIC "-framework OpenGL" "-framework Cocoa" "-framework IOKit")
endif()
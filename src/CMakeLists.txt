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
    core/timer.cpp
    core/timer.hpp

    joypad/joypad.cpp
    joypad/joypad.hpp

    log/logger.hpp
    log/logger.cpp

    ${GLAD_SRC}

    graphics/screen.cpp
    graphics/screen.hpp
    graphics/ppu.cpp
    graphics/ppu.hpp

    audio/apu.cpp
    audio/apu.hpp
    audio/speaker.cpp
    audio/speaker.hpp
    audio/channel1.cpp
    audio/channel1.hpp
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
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
target_include_directories(Core PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_directories(Core PUBLIC ${SDL2_LIBRARY_DIRS})
target_link_libraries(Core PUBLIC OpenGL::GL glfw ${SDL2_LIBRARIES})
target_link_libraries(GameBoyCpp PRIVATE Core)

if(APPLE)
    target_link_libraries(Core PUBLIC
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
    )

    set_target_properties(GameBoyCpp PROPERTIES
            BUILD_RPATH "/opt/homebrew/lib"
            INSTALL_RPATH "/opt/homebrew/lib"
    )
endif()

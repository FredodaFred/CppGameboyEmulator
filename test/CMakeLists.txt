add_executable(RunTests
    core/cart_test.cpp
)

target_link_libraries(RunTests PRIVATE Core)

target_include_directories(RunTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/core)
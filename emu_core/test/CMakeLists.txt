add_executable(RunTests
        core/cart_test.cpp
        core/cpu_test.cpp
)

target_link_libraries(RunTests PRIVATE Core)

target_include_directories(RunTests PRIVATE ../src/core)
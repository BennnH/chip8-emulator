cmake_minimum_required(VERSION 3.16)
project(chip8 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

execute_process(
    COMMAND brew --prefix sdl2
    OUTPUT_VARIABLE SDL2_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_executable(chip8
    src/main.cpp
)

target_include_directories(chip8 PRIVATE
    include
    ${SDL2_PREFIX}/include
)

target_link_libraries(chip8 PRIVATE
    ${SDL2_PREFIX}/lib/libSDL2.dylib
)

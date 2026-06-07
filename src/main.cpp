#include <chrono>
#include <iostream>
#include <string>

#include "Chip8.h"
#include "Platform.h"

int main(int argc, char* argv[]) {

    if (argc != 3)
    {
        std::cerr << "Usage: chip8 <cycle delay> <rom path>\n";
        return 1;
    }

    int cycleDelay  = std::stoi(argv[1]);
    const char* rom = argv[2];

    Chip8 chip8;
    chip8.LoadROM(rom);
    Platform platform("CHIP-8", 640, 320, 64, 32);

    bool quit = false;
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    while (!quit)
    {
        quit = platform.ProcessInput(chip8.keypad);
        auto currentCycleTime = std::chrono::high_resolution_clock::now();
        // How many ms have passed since the last cycle ran
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentCycleTime - lastCycleTime).count();

        if (dt > cycleDelay) {
            lastCycleTime = currentCycleTime;
            chip8.Cycle();
            platform.Update(chip8.display, sizeof(chip8.display[0]) * 64);
        }
    }

    return 0;
}

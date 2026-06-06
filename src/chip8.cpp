#include "Chip8.h"

#include <fstream>
#include <stdexcept>
#include <vector>

constexpr unsigned int START_ADDRESS = 0x200;

void Chip8::LoadROM(const std::string& filename) {
    // Opening the file and move file pointer to the end of the file
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open ROM: " + filename);
    }

    std::streamsize size = file.tellg();
    std::vector<uint8_t> buffer(size);

    // Move the file pointer back to the beginning of the file
    file.seekg(0, std::ios::beg);

    // Reads size bytes from file into buffer
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    for (std::size_t i = 0; i < buffer.size(); i++) {
        memory[START_ADDRESS + i] = buffer[i];
    }

}

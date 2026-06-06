#include "Chip8.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>

constexpr unsigned int START_ADDRESS = 0x200;
constexpr unsigned int FONTSET_SIZE = 80;
constexpr unsigned int FONTSET_START_ADDRESS = 0x50;


uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void Chip8::LoadROM(const std::string& filename)
{
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


Chip8::Chip8() noexcept
    : randGen(std::random_device{}())
    , randByte(0, 255)
{
    pc = START_ADDRESS;

    // Loading fonts into memory
    for (std::size_t i = 0; i < FONTSET_SIZE; i++) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

// CLS
void Chip8::OP_00E0() {
    std::fill(std::begin(display), std::end(display), 0);
}

// RET
void Chip8::OP_00EE() {
    sp--;
    pc = stack[sp];
}

// JP address
void Chip8::OP_1nnn() {
    uint16_t operand_address = opcode & 0x0FFFu;
    pc = operand_address;
}

// Call address
void Chip8::OP_2nnn() {
    uint16_t operand_address = opcode & 0x0FFFu;
    stack[sp] = pc;
    sp++;
    pc = operand_address;
}

// SE Vx, byte
void Chip8::OP_3xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] == byte) {
        pc += 2;
    }
}

// SNE Vx, byte
void Chip8::OP_4xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] != byte) {
        pc += 2;
    }
}

// SE Vx, Vy
void Chip8::OP_5xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t Vy = (opcode & 0x00F0u) >> 4;

    if (registers[Vx] == registers[Vy]) {
        pc += 2;
    }
}

// LD Vx, byte
void Chip8::OP_6xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] = byte;
}

// ADD Vx, byte
void Chip8::OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] += byte;
}

// LD Vx, Vy
void Chip8::OP_8xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t Vy = (opcode & 0x00F0u) >> 4;
    registers[Vx] = registers[Vy];
}

// OR Vx, Vy
void Chip8::OP_8xy1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t Vy = (opcode & 0x00F0u) >> 4;
    registers[Vx] = (registers[Vx] | registers[Vy]);
}

// AND Vx, Vy
void Chip8::OP_8xy2() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t Vy = (opcode & 0x00F0u) >> 4;
    registers[Vx] = (registers[Vx] & registers[Vy]);
}

// XOR Vx, Vy
void Chip8::OP_8xy3() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t Vy = (opcode & 0x00F0u) >> 4;
    registers[Vx] = (registers[Vx] ^ registers[Vy]);
}

// ADD Vx, Vy
void Chip8::OP_8xy4() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t Vy = (opcode & 0x00F0u) >> 4;

    uint16_t result = registers[Vx] + registers[Vy];

    registers[0xF] = (result > 255u) ? 1 : 0;
    registers[Vx] = result & 0x00FFu;
}

// SUB Vx, Vy
void Chip8::OP_8xy5() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t Vy = (opcode & 0x00F0u) >> 4;

    registers[0xF] = (registers[Vx] < registers[Vy]) ? 0 : 1;
    registers[Vx] = registers[Vx] - registers[Vy];
}

// SHR Vx {, Vy}
void Chip8::OP_8xy6() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    registers[0xF] = registers[Vx] & 0x1u;
    registers[Vx] >>= 1;

}

// SUBN Vx, Vy
void Chip8::OP_8xy7() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t Vy = (opcode & 0x00F0u) >> 4;

    registers[0xF] = (registers[Vy] < registers[Vx]) ? 0 : 1;
    registers[Vx] = registers[Vy] - registers[Vx];
}

// SHL Vx {, Vy}
void Chip8::OP_8xyE() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
    registers[Vx] <<= 1;
}

#include "Chip8.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>

constexpr unsigned int START_ADDRESS = 0x200;
constexpr unsigned int FONTSET_SIZE = 80;
constexpr unsigned int FONTSET_START_ADDRESS = 0x50;


constexpr uint8_t fontset[FONTSET_SIZE] =
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

    // Fill tables with nulls
    table0.fill(&Chip8::OP_NULL);
    table8.fill(&Chip8::OP_NULL);
    tableE.fill(&Chip8::OP_NULL);
    tableF.fill(&Chip8::OP_NULL);

    // Main table
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    // 0x0 group
    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    // 0x8 group
    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    // 0xE group
    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    // 0xF group
    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;

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

// SNE Vx, Vy
void Chip8::OP_9xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t Vy = (opcode & 0x00F0u) >> 4;
    if (registers[Vx] != registers[Vy]) {
        pc += 2;
    }
}

// LD I, addr
void Chip8::OP_Annn() {
    index_register = (opcode & 0x0FFFu);
}

// JP V0, addr
void Chip8::OP_Bnnn() {
    pc = (opcode & 0x0FFFu) + registers[0];
}

// RND Vx, byte
void Chip8::OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t kk = opcode & 0x00FFu;
    registers[Vx] = randByte(randGen) & kk;
}

// DRW Vx, Vy, nibble
void Chip8::OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t Vy = (opcode & 0x00F0u) >> 4;
    uint8_t n = opcode & 0x000Fu;

    // Modulo with display resolution to wrap around.
    uint8_t xPos = registers[Vx] % 64;
    uint8_t yPos = registers[Vy] % 32;

    registers[0xF] = 0;

    for (uint8_t row = 0; row < n; row++) {
        uint8_t rowOfSprite = memory[index_register + row];
        for (uint8_t col = 0; col < 8; col++) {
            if (rowOfSprite & (0x80u >> col)) {
                uint32_t& pixel = display[((yPos + row) % 32) * 64 + ((xPos + col) % 64)];

                // Set flag register if theres an overlap between new sprite and existing
                if (pixel == 0xFFFFFFFF) {
                    registers[0xF] = 1;
                }

                pixel ^= 0xFFFFFFFF;
            }
        }
    }
}

// SKP Vx
void Chip8::OP_Ex9E() {
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    if (keypad[registers[Vx]]) {
        pc += 2;
    }
}

// SKNP Vx
void Chip8::OP_ExA1() {
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    if (!keypad[registers[Vx]]) {
        pc += 2;
    }
}

// LD Vx, DT
void Chip8::OP_Fx07(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    registers[Vx] = delayTimer;
}

// LD Vx, K
void Chip8::OP_Fx0A() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;

    for (uint8_t i = 0; i < 16; i++) {
        if (keypad[i]) {
            registers[Vx] = i;
            return;
        }
    }

    pc -= 2;
}

// LD DT, Vx
void Chip8::OP_Fx15(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    delayTimer = registers[Vx];
}

// LD ST, Vx
void Chip8::OP_Fx18() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    soundTimer = registers[Vx];
}

// ADD I, Vx
void Chip8::OP_Fx1E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    index_register += registers[Vx];
}

// LD F, Vx
void Chip8::OP_Fx29() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    index_register = FONTSET_START_ADDRESS + (registers[Vx] * 5);
}

// LD B, Vx
void Chip8::OP_Fx33(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    uint8_t value = registers[Vx];

    // Hundreds
    memory[index_register] = value / 100;
    // Tens
    memory[index_register + 1] = (value / 10) % 10;
    // Ones
    memory[index_register + 2] = value % 10;
}

// LD [I], Vx
void Chip8::OP_Fx55() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    for (uint8_t i = 0; i <= Vx; i++) {
        memory[index_register + i] = registers[i];
    }
}

// LD Vx, [I]
void Chip8::OP_Fx65() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8;
    for (uint8_t i = 0; i <= Vx; i++) {
        registers[i] = memory[index_register + i];
    }
}

// Does nothing
void Chip8::OP_NULL() {}

void Chip8::Table0() {
    (this->*table0[opcode & 0x000Fu])();
}

void Chip8::Table8() {
    (this->*table8[opcode & 0x000Fu])();
}

void Chip8::TableE() {
    (this->*tableE[opcode & 0x000Fu])();
}

void Chip8::TableF() {
    (this->*tableF[opcode & 0x00FFu])();
}

void Chip8::Cycle() {
    // Fetch
    opcode = (memory[pc] <<8u) | memory[pc + 1];
    pc += 2;

    // Decode
    (this->*table[(opcode & 0xF000u) >> 12u])();

    // Decrement
    if (soundTimer > 0) --soundTimer;
    if (delayTimer > 0 ) -- delayTimer;
}

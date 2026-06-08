#pragma once

#include <cstdint>
#include <string>
#include <random>
#include <array>

class Chip8
{
public:
    Chip8() noexcept;
    void LoadROM(const std::string& filename);
    void Cycle();

    std::array<uint8_t, 16> keypad{};
    std::array<uint32_t, 64 * 32> display{};

private:
        std::array<uint8_t, 16> registers{};
        std::array<uint8_t, 4096> memory{};
        uint16_t index_register{};
        uint16_t pc{};
        std::array<uint16_t, 16> stack{};
        uint8_t sp{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint16_t opcode{};

        std::mt19937 randGen;
        std::uniform_int_distribution<uint8_t> randByte;

        // Opcodes
        void OP_00E0();
        void OP_00EE();
        void OP_1nnn();
        void OP_2nnn();
        void OP_3xkk();
        void OP_4xkk();
        void OP_5xy0();
        void OP_6xkk();
        void OP_7xkk();
        void OP_8xy0();
        void OP_8xy1();
        void OP_8xy2();
        void OP_8xy3();
        void OP_8xy4();
        void OP_8xy5();
        void OP_8xy6();
        void OP_8xy7();
        void OP_8xyE();
        void OP_9xy0();
        void OP_Annn();
        void OP_Bnnn();
        void OP_Cxkk();
        void OP_Dxyn();
        void OP_Ex9E();
        void OP_ExA1();
        void OP_Fx07();
        void OP_Fx0A();
        void OP_Fx15();
        void OP_Fx18();
        void OP_Fx1E();
        void OP_Fx29();
        void OP_Fx33();
        void OP_Fx55();
        void OP_Fx65();
        void OP_NULL();


        // Table functions
        void Table0();
        void Table8();
        void TableE();
        void TableF();

        // Function pointer type alias and tables
        using Chip8Func = void (Chip8::*)();
        std::array<Chip8Func, 16>  table{};
        std::array<Chip8Func, 16>  table0{};
        std::array<Chip8Func, 16>  table8{};
        std::array<Chip8Func, 16>  tableE{};
        std::array<Chip8Func, 102> tableF{};

};

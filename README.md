# CHIP-8 Emulator

A CHIP-8 emulator written in C++ using SDL2, rendering to a 64x32 display scaled to 640x320. Implements all 34 opcodes with a function pointer dispatch table.

## Build

```bash
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```

## Run

```bash
./build/chip8 <cycle_delay> <rom_path>
```

- `cycle_delay` — milliseconds between CPU cycles, controls emulation speed. A value of 1-3 works for most ROMs.
- `rom_path` — path to the `.ch8` ROM file.

Example:
```bash
./build/chip8 2 ROMs/Pong.ch8
```

## Controls

```
Chip-8 Keypad     Keyboard
-------------     --------
1  2  3  C        1  2  3  4
4  5  6  D   ->   Q  W  E  R
7  8  9  E        A  S  D  F
A  0  B  F        Z  X  C  V
```

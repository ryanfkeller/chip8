#pragma once

#include "ram.h"
#include "display.h"

#include <unordered_map>

class CPU {
public:
    CPU(RAM& ram, Display& display);

    void reset(); // Reset the CPU state
    void cycle(); // Execute a single cycle of the CPU

private:
    RAM& ram_;          // Reference to the RAM object
    Display& display_;  // Reference to the Display object

    uint16_t pc_;   // Program Counter
    uint16_t i_;    // Index Register
    uint8_t v_[16]; // General Purpose Registers

    uint8_t x_;
    uint8_t y_;
    uint8_t n_;
    uint8_t nn_;
    uint16_t nnn_;

    uint16_t fetch_instruction();
    void parse_args(uint16_t opcode);

    using OpcodeHandler = void (CPU::*)(uint16_t);
    struct OpcodeInfo {
        OpcodeHandler handler;
        bool auto_increment_pc;
    };
    std::unordered_map<uint16_t, OpcodeInfo> opcode_handlers_;

    void op_00e0(uint16_t opcode); // Clear Screen
    void op_1nnn(uint16_t opcode); // Jump
    void op_6xnn(uint16_t opcode); // Set VX
    void op_7xnn(uint16_t opcode); // Add to VX
    void op_annn(uint16_t opcode); // Set I
    void op_dxyn(uint16_t opcode); // Display

};
#pragma once

#include "ram.h"
#include "types.h"
#include "display.h"
#include "timer.h"

#include <vector>

class CPU {
public:
    CPU(RAM& ram, Display& display, Timer& delay_timer, Timer& sound_timer, input_state_t& input_state);

    void reset(); // Reset the CPU state
    void cycle(); // Execute a single cycle of the CPU

private:

    // Dependency injection objects
    RAM& ram_;                      
    Display& display_;              
    Timer& delay_timer_;
    Timer& sound_timer_;
    const input_state_t& input_state_; 
    
    std::array<uint16_t, Utils::STACK_DEPTH> stack_{}; // Internal CPU stack (not in emulated memory)

    uint16_t sp_ {};   // Stack Pointer
    uint16_t pc_;   // Program Counter
    uint16_t i_;    // Index Register
    uint8_t v_[16]{}; // General Purpose Registers

    uint8_t x_;
    uint8_t y_;
    uint8_t n_;
    uint8_t nn_;
    uint16_t nnn_;

    bool waiting_for_key_ = false;
    input_state_t prev_input_state_; 

    void push_stack(uint16_t address);
    uint16_t pop_stack();
    uint16_t fetch_instruction();
    void parse_args(uint16_t opcode);

    using OpcodeHandler = void (CPU::*)();
    struct OpcodeInfo {
        uint16_t pattern = 0;
        uint16_t mask = 0;
        OpcodeHandler handler = nullptr;
        bool auto_increment_pc = true;
    };
    std::vector<OpcodeInfo> opcode_handlers_;


    // Opcodes (most to least commonly used)

    // Register operations (most common)
    void op_6xnn(); // Set VX to NN
    void op_7xnn(); // Set VX to VX + NN
    void op_8xy0(); // Set VX to VY
    void op_8xy1(); // Set VX to VX | VY
    void op_8xy2(); // Set VX to VX & VY
    void op_8xy3(); // Set VX to VX ^ VY
    void op_8xy4(); // Set VX to VX + VY
    void op_8xy5(); // Set VX to VX - VY
    void op_8xy7(); // Set VX to VY - VX
    void op_8xy6(); // Shift VX right (NOTE: modern interpretation)
    void op_8xye(); // Shift VX left (NOTE: modern interpretation)

    // Control flow operations (very common)
    void op_1nnn(); // Jump
    void op_3xnn(); // Skip if VX == NN
    void op_4xnn(); // Skip if VX != NN
    void op_5xy0(); // Skip if VX == VY
    void op_9xy0(); // Skip if VX != VY
    void op_bnnn(); // Jump plus offset (NOTE: original interpretation)

    // Memory and display (common)
    void op_annn(); // Set I to NNN
    void op_dxyn(); // Display
    void op_cxnn(); // Set VX to Rand() & NN

    // Subroutines (moderately common)
    void op_2nnn(); // Subroutine Start
    void op_00ee(); // Subroutine Return
    
    // Input handling (moderately common)
    void op_ex9e(); // Skip if VX-key is pressed
    void op_exa1(); // Skip if VX-key is not pressed
    void op_fx0a(); // Wait for VX-key

    // Timers and utility (less common)
    void op_fx07(); // Set VX to DT (delay timer)
    void op_fx15(); // Set DT to VX
    void op_fx18(); // Set ST (sound timer) to VX
    void op_fx1e(); // Set I to I + VX
    void op_fx29(); // Set I to VX-font-character
    void op_fx33(); // BCD VX into I, I+1, and I+2
    void op_fx55(); // Store V0 through VX to addresses I to I+X (NOTE: modern)
    void op_fx65(); // Store values at I to I+X to V0 through VX (NOTE: modern)

    // System operations (least common)
    void op_00e0(); // Clear Screen

};
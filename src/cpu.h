#pragma once

#include "ram.h"
#include "peripherals.h"
#include "timer.h"

#include <vector>
#include <random>

class CPU {
public:
    CPU(Peripherals& peripherals, RAM& ram, Timer& delay_timer, Timer& sound_timer);

    void reset(); // Reset the CPU state
    void cycle(); // Execute a single cycle of the CPU

    bool draw_flag = false; // Flag indicating that display render is needed

private:

    // Dependency injection objects
    Peripherals& peripherals_;
    RAM& ram_;                      
    Timer& delay_timer_;
    Timer& sound_timer_;
    
    std::array<uint16_t, Utils::STACK_DEPTH> stack_{}; // Internal CPU stack (not in emulated memory)

    // Registers and Pointers
    uint16_t sp_ = 0;                              // Stack Pointer
    uint16_t pc_ = Utils::PROGRAM_START_ADDRESS;   // Program Counter
    uint16_t i_ = 0;                               // Index Register
    uint8_t v_[16] = {};                           // General Purpose Registers

    // Utility var parsers
    uint8_t x_;
    uint8_t y_;
    uint8_t n_;
    uint8_t nn_;
    uint16_t nnn_;

    bool waiting_for_key_ = false;

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

    std::minstd_rand rand_;


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

    const std::vector<OpcodeInfo>opcode_handlers_ = {
        /*
         Match   Mask    Handler        Auto-Inc PC
        */
        // Register operations (most common - executed constantly)
        {0x6000, 0xF000, &CPU::op_6xnn, true},   // 6xnn - Set VX
        {0x7000, 0xF000, &CPU::op_7xnn, true},   // 7xnn - Add to VX
        {0x8000, 0xF00F, &CPU::op_8xy0, true},   // 8xy0 - Set VX to VY
        {0x8004, 0xF00F, &CPU::op_8xy4, true},   // 8xy4 - Set VX to VX + VY
        {0x8005, 0xF00F, &CPU::op_8xy5, true},   // 8xy5 - Set VX to VX - VY
        {0x8001, 0xF00F, &CPU::op_8xy1, true},   // 8xy1 - Set VX to VX | VY
        {0x8002, 0xF00F, &CPU::op_8xy2, true},   // 8xy2 - Set VX to VX & VY
        {0x8003, 0xF00F, &CPU::op_8xy3, true},   // 8xy3 - Set VX to VX ^ VY
        {0x8007, 0xF00F, &CPU::op_8xy7, true},   // 8xy7 - Set VX to VY - VX
        {0x8006, 0xF00F, &CPU::op_8xy6, true},   // 8xy6 - Shift VX right
        {0x800e, 0xF00F, &CPU::op_8xye, true},   // 8xye - Shift VX left
        
        // Control flow (very common)
        {0x1000, 0xF000, &CPU::op_1nnn, false},  // 1nnn - Jump
        {0x3000, 0xF000, &CPU::op_3xnn, true},   // 3xnn - Skip if VX == NN
        {0x4000, 0xF000, &CPU::op_4xnn, true},   // 4xnn - Skip if VX != NN
        {0x5000, 0xF00F, &CPU::op_5xy0, true},   // 5xy0 - Skip if VX == VY
        {0x9000, 0xF00F, &CPU::op_9xy0, true},   // 9xy0 - Skip if VX != VY
        {0xb000, 0xF000, &CPU::op_bnnn, false},  // bnnn - Jump plus offset
        
        // Memory and display (common)
        {0xa000, 0xF000, &CPU::op_annn, true},   // annn - Set I
        {0xd000, 0xF000, &CPU::op_dxyn, true},   // dxyn - Display
        {0xc000, 0xF000, &CPU::op_cxnn, true},   // cxnn - Set VX to Rand() & NN
        
        // Subroutines (moderately common)
        {0x2000, 0xF000, &CPU::op_2nnn, false},  // 2nnn - Subroutine Start  
        {0x00ee, 0xFFFF, &CPU::op_00ee, true},   // 00ee - Subroutine Return
        
        // Input handling (moderately common)
        {0xe09e, 0xF0FF, &CPU::op_ex9e, true},   // ex9e - Skip if VX-key is pressed
        {0xe0a1, 0xF0FF, &CPU::op_exa1, true},   // exa1 - Skip if VX-key is not pressed
        {0xf00a, 0xF0FF, &CPU::op_fx0a, false},   // fx0a - Get key (blocking)
        
        // Timers and utility (less common)
        {0xf007, 0xF0FF, &CPU::op_fx07, true},   // fx07 - Set VX to DT
        {0xf015, 0xF0FF, &CPU::op_fx15, true},   // fx15 - Set DT to VX
        {0xf018, 0xF0FF, &CPU::op_fx18, true},   // fx18 - Set ST to VX
        {0xf01e, 0xF0FF, &CPU::op_fx1e, true},   // fx1e - Set I to I + VX
        {0xf029, 0xF0FF, &CPU::op_fx29, true},   // fx29 - Set I to VX-font-character
        {0xf033, 0xF0FF, &CPU::op_fx33, true},   // fx33 - BCD VX into I, I+1, I+2
        {0xf055, 0xF0FF, &CPU::op_fx55, true},   // fx55 - Store V0-VX to memory
        {0xf065, 0xF0FF, &CPU::op_fx65, true},   // fx65 - Load V0-VX from memory
        
        // System operations (least common)
        {0x00e0, 0xFFFF, &CPU::op_00e0, true},   // 00e0 - Clear Screen
    };

};
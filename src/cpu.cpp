#include "cpu.h"
#include "utilities.h"
#include "debug.h"

#include <stdexcept>
#include <string>
#include <format>
#include <iostream>
#include <cstdlib>


CPU::CPU(RAM& ram, Display& display, Timer& delay_timer, Timer& sound_timer, input_state_t& input_state)
    : ram_(ram),
      display_(display),
      delay_timer_(delay_timer),
      sound_timer_(sound_timer),
      input_state_(input_state),
      pc_(Utils::PROGRAM_START_ADDRESS)
{
    
    opcode_handlers_ = {
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
}

void CPU::cycle() {
    uint16_t opcode = fetch_instruction();
    parse_args(opcode);

    //PRINT_DEBUG("PC: %04x, Opcode: %04x\n", pc_, opcode);

    // Linear search through opcode_handlers_
    //  (pre-sorted for most common first)
    for (const auto& info : opcode_handlers_) {
        if ((opcode & info.mask) == info.pattern) {
            (this->*info.handler)();
            if (info.auto_increment_pc) {
                pc_ += 2;
            }
            return;
        }
    }

    throw std::runtime_error(std::format("Invalid Opcode {:04x}", opcode));
}

void CPU::push_stack(uint16_t address) {
    if (sp_ >= stack_.size()) {
        throw std::runtime_error("Stack overflow!");
    }
    stack_[sp_++] = address;
}

uint16_t CPU::pop_stack() {
    if (sp_ == 0 ) {
        throw std::runtime_error("Stack underflow!");
    }
    return stack_[--sp_];
}

uint16_t CPU::fetch_instruction() {
    return ram_.read(pc_) << 8 | ram_.read(pc_ + 1);
}

void CPU::parse_args(uint16_t opcode) {
    x_ = (opcode & 0x0F00) >> 8;
    y_ = (opcode & 0x00F0) >> 4;
    n_ = opcode & 0x000F;
    nn_ = opcode & 0x00FF;
    nnn_ = opcode & 0x0FFF;
}

// ===== Register operations (most common) =====
void CPU::op_6xnn() {
    v_[x_] = nn_;
}

void CPU::op_7xnn() {
    v_[x_] += nn_;
}

void CPU::op_8xy0() {
    v_[x_] = v_[y_];
}

void CPU::op_8xy1() {
    v_[x_] |= v_[y_];
    v_[0xF] = 0;
}

void CPU::op_8xy2() {
    v_[x_] &= v_[y_];
    v_[0xF] = 0;
}

void CPU::op_8xy3() {
    v_[x_] ^= v_[y_];
    v_[0xF] = 0;
}

void CPU::op_8xy4() {
    // Calculate overflow up front so VF can be an input
    bool overflow = v_[x_] > UINT8_MAX - v_[y_];

    v_[x_] = v_[x_] + v_[y_];
    if (overflow) {
        // Addition did overflow
        v_[0xF] = 1;  
    }
    else {
        v_[0xF] = 0;
    }
}

void CPU::op_8xy5() {
    // Calculate underflow up front so VF can be an input
    bool underflow = v_[x_] < v_[y_];
    
    v_[x_] = v_[x_] - v_[y_];
    if (underflow) {
        // Subtraction did underflow
        v_[0xF] = 0;  
    }
    else {
        v_[0xF] = 1;
    }
}

void CPU::op_8xy7() {
    // Calculate underflow up front so VF can be an input
    bool underflow = v_[x_] > v_[y_];

    v_[x_] = v_[y_] - v_[x_];
    if (underflow) {
        // Subtraction did underflow
        v_[0xF] = 0;  
    }
    else {
        v_[0xF] = 1;
    }
}

void CPU::op_8xy6() {
    // Shift VX right, set VF to LSB
    uint8_t lsb = v_[x_] & 0x1;
    v_[x_] = v_[x_]>>1;
    v_[0xF] = lsb;
}


void CPU::op_8xye() {
    // Shift VX left, set VF to MSB
    uint8_t msb = (v_[x_] & 0x80)>>7;
    v_[x_] = v_[x_]<<1;
    v_[0xF] = msb;
}

// ===== Control flow (very common) =====
void CPU::op_1nnn() {
    pc_ = nnn_;
}

void CPU::op_3xnn() {
    if (v_[x_] == nn_)
        pc_+=2;
}

void CPU::op_4xnn() {
    if (v_[x_] != nn_)
        pc_+=2;
}

void CPU::op_5xy0() {
    if (v_[x_] == v_[y_])
        pc_+=2;
}

void CPU::op_9xy0() {
    if (v_[x_] != v_[y_])
        pc_+=2;
}

void CPU::op_bnnn() {
    // Jump to nnn + V0
    pc_ = nnn_ + v_[0];
}

// ===== Memory and display (common) =====
void CPU::op_annn() {
    i_ = nnn_;
}

void CPU::op_cxnn() {
    // Set VX to random number & NN
    v_[x_] = rand() & nn_;
}

void CPU::op_dxyn() {
    v_[0xf] = 0;

    uint8_t draw_y = v_[y_] % Utils::PIXEL_HEIGHT;

    for (uint8_t byte_idx = 0; byte_idx < n_; byte_idx++) {
        if (draw_y >= Utils::PIXEL_HEIGHT) break;

        uint8_t sprite_byte = ram_.read(i_ + byte_idx);
        uint8_t draw_x = v_[x_] % Utils::PIXEL_WIDTH;

        for (int bit_idx = 7; bit_idx >= 0; bit_idx--) {
            if (draw_x >= Utils::PIXEL_WIDTH) break;

            if ((sprite_byte >> bit_idx) & 0x1) {
                // Sprite is turning this bit on 
                if (display_.check_pixel(draw_x, draw_y)) {
                    // This bit is already on
                    v_[0xf] = 1;
                    display_.set_pixel(draw_x, draw_y, false);
                } else {
                    display_.set_pixel(draw_x, draw_y, true);
                }
            }
            draw_x++;
        }
        draw_y++;
    }
}

// ===== Subroutines (moderately common) =====
void CPU::op_2nnn() {
    push_stack(pc_);
    pc_ = nnn_;
}

void CPU::op_00ee() {
    pc_ = pop_stack();
}

// ===== Input handling (moderately common) =====
void CPU::op_ex9e() {
    // Skip if VX-key is pressed
    if (input_state_[x_]) {
        pc_+=2;
    }
}

void CPU::op_exa1() {
    // Skip if VX-key is not pressed
    if (!input_state_[x_]) {
        pc_+=2;
    }
}

void CPU::op_fx0a() {
    // Wait for key press, store in VX
    if (!waiting_for_key_) {
        // First time through, register x value
        waiting_for_key_ = true;
        prev_input_state_ = input_state_;
        
    } else {
        // Next times through, look for change
        if (input_state_ != prev_input_state_) {
            waiting_for_key_ = false;
            pc_+=2;
        }
    }
}

// ===== Timers and utility (less common) =====
void CPU::op_fx07() {
    // Set VX to delay timer
    v_[x_] = delay_timer_.get();
}

void CPU::op_fx15() {
    // Set delay timer to VX
    delay_timer_.set(v_[x_]);
}

void CPU::op_fx18() {
    // Set sound timer to VX
    sound_timer_.set(v_[x_]);
}

void CPU::op_fx1e() {
    // Set I to I + VX
    i_ = i_ + v_[x_];
}

void CPU::op_fx29() {
    // Set I to font character X
    i_ = Utils::FONT_START_ADDRESS + x_ * 5; //Each font char is 5 bytes 
}

void CPU::op_fx33() {
    // Store BCD of VX at I, I+1, I+2

    // Note: ugly BCD algo, but there you go
    uint8_t d1 = v_[x_] % 10;
    uint8_t d10 = ((v_[x_] % 100) - d1)/10;
    uint8_t d100 = ((v_[x_] % 1000) - d1 - d10)/100;
    ram_.write(i_, d100);
    ram_.write(i_+1, d10);
    ram_.write(i_+2, d1);
}

void CPU::op_fx55() {
    // Store V0-VX to memory starting at I
    for (uint8_t iter = 0; iter <= x_; iter++) {
        ram_.write(i_++, v_[iter]);
    }
}

void CPU::op_fx65() {
    // TODO: Load V0-VX from memory starting at I
    for (uint8_t iter = 0; iter <= x_; iter++) {
        v_[iter] = ram_.read(i_++);
    }
}

// ===== System operations (least common) =====
void CPU::op_00e0() {
    display_.clear();
}
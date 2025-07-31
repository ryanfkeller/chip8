#include "cpu.h"
#include "utilities.h"
#include "print.h"

#include <string>
#include <random>


CPU::CPU(Peripherals& peripherals, RAM& ram, Timer& delay_timer, Timer& sound_timer )
    : peripherals_(peripherals),
      ram_(ram),
      delay_timer_(delay_timer),
      sound_timer_(sound_timer)
{
    rand_.seed();
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

    PRINT_ERROR("Invalid Opcode %04x", opcode);
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
    // Set vx to nn
    v_[x_] = nn_;
}

void CPU::op_7xnn() {
    // Set vx to vx += nn
    v_[x_] += nn_;
}

void CPU::op_8xy0() {
    // Set vx to vy
    v_[x_] = v_[y_];
}

void CPU::op_8xy1() {
    // Set vx to vx|=vy and clear vf
    v_[x_] |= v_[y_];
    v_[0xF] = 0;
}

void CPU::op_8xy2() {
    // Set vx to vx&=vy and clear vf
    v_[x_] &= v_[y_];
    v_[0xF] = 0;
}

void CPU::op_8xy3() {
    // Set vx to vx^=vy and clear vf
    v_[x_] ^= v_[y_];
    v_[0xF] = 0;
}

void CPU::op_8xy4() {
    // Set vx to vx += vy
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
    // Set vx to vx -= vy
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

    v_[x_] = v_[y_];
    uint8_t lsb = v_[x_] & 0x1;
    v_[x_] = v_[x_]>>1;
    v_[0xF] = lsb;
}


void CPU::op_8xye() {
    // Shift VX left, set VF to MSB

    v_[x_] = v_[y_];
    uint8_t msb = (v_[x_] & 0x80)>>7;
    v_[x_] = v_[x_]<<1;
    v_[0xF] = msb;
}

// ===== Control flow (very common) =====
void CPU::op_1nnn() {
    // Jump to nnn
    pc_ = nnn_;
}

void CPU::op_3xnn() {
    // Skip the next instruction if vx == nn
    if (v_[x_] == nn_)
        pc_+=2;
}

void CPU::op_4xnn() {
    // Skip the next instruction if vx != nn
    if (v_[x_] != nn_)
        pc_+=2;
}

void CPU::op_5xy0() {
    // Skip the next instruction if vx == vy
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
    v_[x_] = rand_() & nn_;
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
                if (peripherals_.check_pixel(draw_x, draw_y)) {
                    // This bit is already on
                    v_[0xf] = 1;
                    peripherals_.set_pixel(draw_x, draw_y, false);
                } else {
                    peripherals_.set_pixel(draw_x, draw_y, true);
                }
            }
            draw_x++;
        }
        draw_y++;
    }

    draw_flag = true;
}

// ===== Subroutines (moderately common) =====
void CPU::op_2nnn() {
    // Call subroutine at nnn
    push_stack(pc_);
    pc_ = nnn_;
}

void CPU::op_00ee() {
    // Return from subroutine
    pc_ = pop_stack();
}

// ===== Input handling (moderately common) =====
void CPU::op_ex9e() {
    // Skip if VX-key is pressed
    if (peripherals_.key_state[v_[x_]]) {
        pc_+=2;
    }
}

void CPU::op_exa1() {
    // Skip if VX-key is not pressed
    if (!peripherals_.key_state[v_[x_]]) {
        pc_+=2;
    }
}

void CPU::op_fx0a() {
    // Wait for key press, store in VX
    if (!waiting_for_key_) {
        // First time through, register x value
        waiting_for_key_ = true;
        peripherals_.input_flag = false; // Reset input flag so only current inputs update
        
    } else {
        // Next times through, look for change
        if (peripherals_.input_flag) {
            waiting_for_key_ = false;
            peripherals_.input_flag = false;
            v_[x_] = peripherals_.last_key;
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

    // Calculate overflow up front so VF can be an input
    bool overflow = i_ > UINT8_MAX - v_[x_];
    i_ = i_ + v_[x_];

    if (overflow) {
        v_[0xF] = 1;
    }
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
    peripherals_.clear_pixel_buffer();
    draw_flag = true;
}
#include "cpu.h"
#include "utilities.h"
#include "debug.h"

#include <stdexcept>
#include <string>
#include <format>
#include <iostream>


CPU::CPU(RAM& ram, Display& display)
    : ram_(ram),
      display_(display),
      pc_(Utils::PROGRAM_START_ADDRESS)
{
    opcode_handlers_ = {
        {0x00e0, {&CPU::op_00e0, true}},
        {0x1000, {&CPU::op_1nnn, false}},
        {0x6000, {&CPU::op_6xnn, true}},
        {0x7000, {&CPU::op_7xnn, true}},
        {0xa000, {&CPU::op_annn, true}},
        {0xd000, {&CPU::op_dxyn, true}},
    };
}

void CPU::cycle() {
    uint16_t opcode = fetch_instruction();
    parse_args(opcode);

    PRINT_DEBUG("ADDR: %04x OP: %04x\n", pc_, opcode);

    // Try exact match of unique opcodes
    auto it = opcode_handlers_.find(opcode);
    if (it != opcode_handlers_.end()) {
        (this->*(it->second.handler))(opcode);
        if (it->second.auto_increment_pc) {
            pc_ += 2;
        }
        return;
    }

    // Use masked match for variable opcodes
    uint16_t masked = opcode & 0xF000;
    it = opcode_handlers_.find(masked);
    if (it != opcode_handlers_.end()) {
        (this->*(it->second.handler))(opcode);
        if (it->second.auto_increment_pc) {
            pc_ += 2;
        }
        return;
    }

    throw std::runtime_error(std::format("Invalid Opcode {:04x}", opcode));
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

void CPU::op_00e0(uint16_t opcode) {
    (void) opcode;
    display_.clear();
}

void CPU::op_1nnn(uint16_t opcode) {
    (void) opcode;
    pc_ = nnn_;
}

void CPU::op_6xnn(uint16_t opcode) {
    (void) opcode;
    v_[x_] = nn_;
}
void CPU::op_7xnn(uint16_t opcode) {
    (void) opcode;
    v_[x_] += nn_;
}

void CPU::op_annn(uint16_t opcode) {
    (void) opcode;
    i_ = opcode & 0xFFF;
}

void CPU::op_dxyn(uint16_t opcode) {
    (void) opcode;
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
#pragma once

#include "utilities.h"
#include <array>
#include <cstdint>
#include <string>

class RAM {
public: 
    RAM();

    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t value);
    void erase_ram();
    void load_file(const std::string& filename, uint16_t address = 0);
    void mem_dump(uint16_t address, uint16_t length) const;
    
private:
    std::array<uint8_t, Utils::MEMORY_SIZE> memory; // Memory array of size MEMORY_SIZE
};
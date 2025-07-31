#include "ram.h"
#include "utilities.h"
#include "print.h"
#include <array>
#include <stdexcept>
#include <fstream>

RAM::RAM() {
    erase_ram();
}

uint8_t RAM::read(uint16_t address) const {
    if (address >= Utils::MEMORY_SIZE) {
        throw std::out_of_range("Address out of range");
    }
    return memory[address];
}

void RAM::write(uint16_t address, uint8_t value) {
    if (address >= Utils::MEMORY_SIZE) {
        throw std::out_of_range("Address out of range");
    }
    memory[address] = value;
}

void RAM::erase_ram() {
    memory.fill(0); // Fill the memory with zeros
}

void RAM::load_file(const std::string& filename, uint16_t address) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate );
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::streamsize filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (filesize + address - Utils::MEMORY_START_ADDRESS > Utils::MEMORY_SIZE) {
        throw std::out_of_range("File \"%s\" cannot be written -- requested write location results in writes exceeding memory size");
    }

    if (!file.read(reinterpret_cast<char*>(&memory[address]), filesize))
    {
        throw std::runtime_error("Failed to read file: " + filename);
    }

    #ifdef DEBUG
        //mem_dump(address, filesize);
    #endif

    
}

void RAM::mem_dump(uint16_t address, uint16_t length) const
{
    printf("Memdump -- Addr %04x, %d bytes\n", address, length);
    uint16_t end_address = address + length;

    while (address < end_address)
    {
        uint8_t value = memory[address - Utils::MEMORY_START_ADDRESS];
        if (value != 0)
            printf("%04X: %02X\n", address, value);
        address++;
    }
}


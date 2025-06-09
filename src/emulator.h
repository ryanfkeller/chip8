#pragma once

#include <string>
#include "ram.h"
#include "display.h"
#include "cpu.h"

class Emulator {
    public:
        Emulator();

        void load_rom(const std::string& rom_filepath);
        void run();
    
    private:
        RAM ram_;
        Display display_;
        CPU cpu_;
};
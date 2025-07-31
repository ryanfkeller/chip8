#pragma once

#include <string>
#include "peripherals.h"
#include "ram.h"
#include "cpu.h"
#include "utilities.h"
#include "timer.h"


class Emulator {
    public:
        Emulator();

        void load_rom(const std::string& rom_filepath, int start_address=Utils::PROGRAM_START_ADDRESS);
        void run();
    
    private:
        Peripherals peripherals_;
        RAM ram_;
        Timer delay_timer_;
        Timer sound_timer_;
        CPU cpu_;    
};
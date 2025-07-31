#pragma once

#include <string>
#include "sdl_manager.h"
#include "ram.h"
#include "display.h"
#include "cpu.h"
#include "utilities.h"
#include "types.h"
#include "timer.h"
#include "sound.h"


class Emulator {
    public:
        Emulator();

        void load_rom(const std::string& rom_filepath, int start_address=Utils::PROGRAM_START_ADDRESS);
        void run();
    
    private:
        SDL_Manager sdl_manager_;
        RAM ram_;
        Display display_;
        Sound sound_;
        input_state_t input_state_ = {};
        Timer delay_timer_;
        Timer sound_timer_;
        CPU cpu_;
        
        


        
};
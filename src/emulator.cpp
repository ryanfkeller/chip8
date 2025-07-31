#include "emulator.h"
#include "utilities.h"
#include "debug.h"

#include <string>
#include <algorithm>

Emulator::Emulator() 
    : display_(sdl_manager_), sound_(sdl_manager_), cpu_(ram_, display_, delay_timer_, sound_timer_, input_state_) {}


void Emulator::load_rom(const std::string& rom_filepath, int start_address) {

    ram_.load_file(rom_filepath, start_address);
}

void Emulator::run() {

    bool running = true;
    SDL_Event event;

    // Set up how often we need to service CPU and Display Cycles
    uint64_t cpu_cycle_ticks = SDL_GetPerformanceFrequency() / Utils::CPU_CYCLE_HZ;
    uint64_t frame_rate_ticks = SDL_GetPerformanceFrequency() / Utils::FRAME_RATE_HZ;

    uint64_t last_cpu_tick = SDL_GetPerformanceCounter();
    uint64_t last_frame_tick = SDL_GetPerformanceCounter(); 

    while (running) {
        // Handle CPU Instruction and Display Render Cycles
        uint64_t current_tick = SDL_GetPerformanceCounter();


        uint64_t elapsed_cpu_ticks = current_tick-last_cpu_tick;
        while (elapsed_cpu_ticks >= cpu_cycle_ticks) {
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT: 
                    {
                        running = false;
                        break;
                    }
                    case SDL_KEYDOWN:
                    {
                        // Handle key press
                        if (event.key.repeat == 0) {
                            auto key_iter = Utils::KEY_MAPPING.find(event.key.keysym.sym);
                            if (key_iter != Utils::KEY_MAPPING.end()) {
                                input_state_[key_iter->second] = true;
                                PRINT_DEBUG("Keypress: %x\n", key_iter->second);
                            }
                        }
                        break;
                    }
                    case SDL_KEYUP:
                    {
                        // Handle key release
                        auto key_iter = Utils::KEY_MAPPING.find(event.key.keysym.sym);
                        if (key_iter != Utils::KEY_MAPPING.end()) {
                            input_state_[key_iter->second] = false;
                            PRINT_DEBUG("Keyrelease: %x\n", key_iter->second);
                        }
                        break;
                    }
                    default: 
                        break;
                }
            }
            cpu_.cycle();
            last_cpu_tick += cpu_cycle_ticks;
            elapsed_cpu_ticks -= cpu_cycle_ticks;
        }

        // Poll for events

        // TODO: There is a bug here related to polling events happening async with the cycle. 
        

        

        

        uint64_t elapsed_frame_ticks = current_tick-last_frame_tick;
        while (elapsed_frame_ticks >= frame_rate_ticks) {
            display_.render();
            delay_timer_.tick();
            sound_timer_.tick();
            sound_.update(!sound_timer_.in_timeout());
            last_frame_tick += frame_rate_ticks;
            elapsed_frame_ticks -= frame_rate_ticks;
        }

        // Let the host rest if we're ahead
        if (elapsed_cpu_ticks < cpu_cycle_ticks) {

            // Check how many ms we can possibly wait by converting ticks to next cpu cyle into ms
            float ms_to_wait = (cpu_cycle_ticks - elapsed_cpu_ticks) * 1000.0f / SDL_GetPerformanceFrequency();

            // If we are able to take a break greater than our smallest rest resolution (1ms), take it
            if (ms_to_wait >= 1.0) {
                SDL_Delay(static_cast<uint32_t>(std::min(ms_to_wait, 2.0f)));
            }
        } 
    }

}
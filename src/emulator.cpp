#include "emulator.h"

#include <string>
#include <algorithm>

Emulator::Emulator() 
    : ram_(),
      display_(),
      cpu_(ram_, display_)
{}

void Emulator::load_rom(const std::string& rom_filepath) {

    ram_.load_file(rom_filepath, Utils::PROGRAM_START_ADDRESS);
}

void Emulator::run() {

    bool running = true;
    SDL_Event event;

    // Set up how often we need to service CPU and Display Cycles
    uint64_t cpu_cycle_ticks = SDL_GetPerformanceFrequency() / Utils::CPU_HZ;
    uint64_t display_cycle_ticks = SDL_GetPerformanceFrequency() / Utils::DISPLAY_HZ;

    uint64_t last_cpu_tick = SDL_GetPerformanceCounter();
    uint64_t last_display_tick = SDL_GetPerformanceCounter(); 

    while (running) {
        // Poll for events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Handle input

        // Handle CPU Instruction and Display Render Cycles
        uint64_t current_tick = SDL_GetPerformanceCounter();

        uint64_t elapsed_cpu_ticks = current_tick-last_cpu_tick;
        while (elapsed_cpu_ticks >= cpu_cycle_ticks) {
            cpu_.cycle();
            last_cpu_tick += cpu_cycle_ticks;
            elapsed_cpu_ticks -= cpu_cycle_ticks;
        }

        uint64_t elapsed_display_ticks = current_tick-last_display_tick;
        while (elapsed_display_ticks >= display_cycle_ticks) {
            display_.render();
            last_display_tick += display_cycle_ticks;
            elapsed_display_ticks -= display_cycle_ticks;
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
#include "emulator.h"
#include "utilities.h"
#include "print.h"

#include <string>
#include <algorithm>

Emulator::Emulator() 
    : cpu_(peripherals_, ram_, delay_timer_, sound_timer_) {}


void Emulator::load_rom(const std::string& rom_filepath, int start_address) {

    ram_.load_file(rom_filepath, start_address);
}


void Emulator::run() {
    PRINT_DEBUG("Emulation started!");

    // Set up how often we need to service CPU and Display Cycles
    uint64_t cpu_cycle_ticks = SDL_GetPerformanceFrequency() / Utils::CPU_CYCLE_HZ;
    uint64_t timer_cycle_ticks = SDL_GetPerformanceFrequency() / Utils::TIMER_CYCLE_HZ;
    uint64_t last_timer_tick = SDL_GetPerformanceCounter();

    while (true) {
        
        uint64_t start_tick = SDL_GetPerformanceCounter();

        // Handle events that happen at CPU cycle frequency
        if (peripherals_.process_input())
            break;
    
        cpu_.cycle();


        // Handle events that happen at timer cycle frequency
        if (start_tick - last_timer_tick >= timer_cycle_ticks) {
            PRINT_DEBUG("FPS: %f", SDL_GetPerformanceFrequency()/static_cast<float>(start_tick - last_timer_tick) );
            // Update the display
            if (cpu_.draw_flag)
            {
                peripherals_.render_display();
                cpu_.draw_flag = false;
            }
            
            // Decrement the timers
            delay_timer_.tick();
            sound_timer_.tick();

            // Make the buzzer beep if the sound timer is not timed-out
            peripherals_.beep(!sound_timer_.in_timeout());

            // Update the last timer tick
            last_timer_tick = start_tick;
        }

        uint64_t elapsed_ticks = SDL_GetPerformanceCounter() - start_tick;

        // Let the host rest if we're ahead
        if (elapsed_ticks < cpu_cycle_ticks) {

            // Check how many ms we can possibly wait by converting ticks to next cpu cyle into ms
            float ms_to_wait = (cpu_cycle_ticks - elapsed_ticks) * 1000.0f / SDL_GetPerformanceFrequency();

            // If we are able to take a break greater than our smallest rest resolution (1ms), take it
            if (ms_to_wait >= 1.0) {
                SDL_Delay(static_cast<uint32_t>(std::min(ms_to_wait, 2.0f)));
            }
        } 
    }

}
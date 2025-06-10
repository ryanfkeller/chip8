#pragma once

#include <cstddef>
#include <SDL2/SDL.h>

namespace Utils
{
    // Constants for display
    constexpr const char* WINDOW_TITLE = "Chip-8 Emulator";
    constexpr SDL_Color PIXEL_COLOR_ON = {255, 255, 255, 255}; // white
    constexpr SDL_Color PIXEL_COLOR_OFF = {0, 0, 0, 255};      // black
    
    
    constexpr int PIXEL_WIDTH = 64;     // width of window in logical pixels
    constexpr int PIXEL_HEIGHT = 32;    // height of window in logical pixels
    constexpr int DISPLAY_SCALE = 10;   // scale of logical pixels to screen pixels

    constexpr int WINDOW_WIDTH = DISPLAY_SCALE * PIXEL_WIDTH;
    constexpr int WINDOW_HEIGHT = DISPLAY_SCALE * PIXEL_HEIGHT;

    // Constants for memory addresses
    constexpr uint16_t MEMORY_SIZE = 4096; 
    constexpr int MEMORY_START_ADDRESS = 0x000; 
    constexpr int MEMORY_END_ADDRESS = 0xFFF; 
    constexpr int PROGRAM_START_ADDRESS = 0x200;

    // Constants for timing
    constexpr uint32_t CPU_HZ = 700;
    constexpr uint32_t DISPLAY_HZ = 60;    
}
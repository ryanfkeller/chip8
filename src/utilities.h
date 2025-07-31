#pragma once

#include <cstddef>
#include <SDL2/SDL.h>
#include <unordered_map>

namespace Utils
{
    // Helper funcs
    constexpr uint32_t sdlcolor_to_uint32(SDL_Color color) {
        return (color.r<<24) | (color.g<<16) | (color.b<<8) | color.a;
    }
    
    // Constants for display
    constexpr const char* WINDOW_TITLE = "Chip-8 Emulator";

    constexpr SDL_Color PIXEL_COLOR_ON = {97, 184, 174, 255}; // light
    constexpr SDL_Color PIXEL_COLOR_OFF = {19, 23, 38, 255};  // dark

    constexpr uint32_t PIXEL_ON_UINT32 = sdlcolor_to_uint32(PIXEL_COLOR_ON); // light
    constexpr uint32_t PIXEL_OFF_UINT32 = sdlcolor_to_uint32(PIXEL_COLOR_OFF);  // dark

    constexpr uint16_t PIXEL_WIDTH = 64;     // width of window in logical pixels
    constexpr uint16_t PIXEL_HEIGHT = 32;    // height of window in logical pixels
    constexpr uint16_t DISPLAY_SCALE = 10;   // scale of logical pixels to screen pixels

    constexpr uint16_t WINDOW_WIDTH = DISPLAY_SCALE * PIXEL_WIDTH;
    constexpr uint16_t WINDOW_HEIGHT = DISPLAY_SCALE * PIXEL_HEIGHT;


    // Constants for memory addresses
    constexpr uint16_t MEMORY_SIZE = 4096; 
    constexpr int MEMORY_START_ADDRESS = 0x000; 
    constexpr int MEMORY_END_ADDRESS = 0xFFF; 
    constexpr int FONT_START_ADDRESS = 0x50;
    constexpr int PROGRAM_START_ADDRESS = 0x200;
    constexpr int STACK_DEPTH = 16;

    // Constants for timing
    constexpr uint32_t CPU_CYCLE_HZ = 1000; // CPU instruction rate
    constexpr uint32_t TIMER_CYCLE_HZ = 60; // Display refresh + timer ticks

    // Settings for Audio
    constexpr uint32_t AUDIO_RATE_HZ = 44100; // Audio sample rate
    constexpr uint32_t AUDIO_BUFFER_SIZE = 1024;
    constexpr float BEEP_TONE_HZ = 440.0f;
    constexpr float BEEP_AMPLITUDE = 0.05f;

    // Key Mapping
    const std::unordered_map<SDL_Keycode, uint8_t> KEY_MAPPING = {
        {SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC},
        {SDLK_q, 0x4}, {SDLK_w, 0x5}, {SDLK_e, 0x6}, {SDLK_r, 0xD},
        {SDLK_a, 0x7}, {SDLK_s, 0x8}, {SDLK_d, 0x9}, {SDLK_f, 0xE},
        {SDLK_z, 0xA}, {SDLK_x, 0x0}, {SDLK_c, 0xB}, {SDLK_v, 0xF} 
    };

}
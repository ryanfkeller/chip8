#pragma once

#include "utilities.h"
#include <SDL2/SDL.h>
#include <array>

class Peripherals {
    public:
        Peripherals();
        ~Peripherals();
    
        // Display handling
        void clear_pixel_buffer();
        void render_display();
        void set_pixel(uint16_t x, uint16_t y, bool on);
        bool check_pixel(uint16_t x, uint16_t y);
        std::array<uint32_t,Utils::PIXEL_WIDTH*Utils::PIXEL_HEIGHT> pixel_buffer = {};

        // Audio handling
        void beep(bool enable);

        // User IO handling
        bool process_input();    // Captures user input, returns true if quit detected
        bool key_state[16] = {}; // Key state storage
        bool input_flag = false; // Input event flag
        uint8_t last_key = 0;    // Last key pressed

    private:

        // SDL objects and helpers
        SDL_Window* window_ = nullptr;
        SDL_Renderer* renderer_ = nullptr;
        SDL_Texture* texture_ = nullptr;
        SDL_AudioDeviceID audio_device_ = 0;
        
        void sdl_init();    // Initialize SDL display and audio
        void sdl_cleanup(); // De-init SDL display and audio        

        // Audio functions
        static void audio_callback(void *userdata, Uint8 *stream, int len);

        
};
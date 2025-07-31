#include "peripherals.h"
#include "utilities.h"
#include "print.h"
#include <stdexcept>
#include <format>

Peripherals::Peripherals() {
    sdl_init();
    render_display();
}

Peripherals::~Peripherals() {
    sdl_cleanup();
}

/*
    SDL Management Functions
*/

void Peripherals::sdl_init() {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        throw std::runtime_error(std::format("Failed to initialize SDL: %s\n", SDL_GetError()));

    // Create the SDL window
    window_ = SDL_CreateWindow(
        Utils::WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        Utils::WINDOW_WIDTH,
        Utils::WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (!window_)
        throw std::runtime_error(std::format("Failed to create SDL window: %s\n", SDL_GetError()));

    // Create the SDL renderer
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_)
        throw std::runtime_error(std::format("Failed to create SDL renderer: %s\n", SDL_GetError()));

    // Create the SDL texture
    texture_ = SDL_CreateTexture(
        renderer_, 
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        Utils::PIXEL_WIDTH,
        Utils::PIXEL_HEIGHT
    );
    if (!texture_)
        throw std::runtime_error(std::format("Failed to create SDL texture: %s\n", SDL_GetError()));

    // Set up the SDL Audio
    SDL_AudioSpec desired, obtained;
    desired.freq = Utils::AUDIO_RATE_HZ;
    desired.format = AUDIO_S16SYS;
    desired.channels = 1;
    desired.samples = Utils::AUDIO_BUFFER_SIZE;
    desired.callback = audio_callback;
    desired.userdata = nullptr;
    audio_device_ = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (!audio_device_)
        throw std::runtime_error(std::format("Failed to create SDL audio: %s\n", SDL_GetError()));
}

void Peripherals::sdl_cleanup() {
    if (audio_device_) {
        SDL_CloseAudioDevice(audio_device_);
        audio_device_ = 0;
    }

    if (texture_) {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    SDL_Quit();
}

/*
    Display Management Functions
*/
void Peripherals::clear_pixel_buffer() {
    pixel_buffer.fill(Utils::PIXEL_OFF_UINT32);
}

void Peripherals::render_display() {

    // Update the SDL texture with the contents of pixel_buffer
    SDL_UpdateTexture(texture_, nullptr, pixel_buffer.data(), Utils::PIXEL_WIDTH*sizeof(pixel_buffer[0]));

    // Copy the texture to the renderer and present
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}

void Peripherals::set_pixel(uint16_t x, uint16_t y, bool on) {
    // Validate pixel position
    if (x >= Utils::PIXEL_WIDTH || y >= Utils::PIXEL_HEIGHT)
        throw std::runtime_error(std::format("Invalid pixel set (x:%d, y:%d)", x, y));
    
    // Update pixel value
    pixel_buffer[y * Utils::PIXEL_WIDTH + x] = on ? Utils::PIXEL_ON_UINT32 : Utils::PIXEL_OFF_UINT32;
}

bool Peripherals::check_pixel(uint16_t x, uint16_t y) {
    // Validate pixel position
    if (x >= Utils::PIXEL_WIDTH || y >= Utils::PIXEL_HEIGHT)
        throw std::runtime_error(std::format("Invalid pixel check (x:{:}, y:{:}", x, y));

    // Check pixel
    return pixel_buffer[y * Utils::PIXEL_WIDTH + x] == Utils::PIXEL_ON_UINT32;
}

/*
    Sound Management Functions
*/
void Peripherals::beep(bool enable) {
    SDL_PauseAudioDevice(audio_device_, static_cast<int>(!enable));
}

void Peripherals::audio_callback(void *userdata, Uint8 *stream, int len) {
    (void) userdata;
    for (int i = 0; i < len; ++i) {
        float phase = 2.0f * M_PI * Utils::BEEP_TONE_HZ * i / Utils::AUDIO_RATE_HZ;
        stream[i] = static_cast<int16_t>(Utils::BEEP_AMPLITUDE * std::sin(phase) * INT16_MAX);
    }
}

/*
    User IO Functions
*/
bool Peripherals::process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: 
            {
                return true; 
            }
            case SDL_KEYDOWN:
            {
                // Handle key press
                if (event.key.repeat == 0) {
                    auto key_iter = Utils::KEY_MAPPING.find(event.key.keysym.sym);
                    if (key_iter != Utils::KEY_MAPPING.end()) {
                        key_state[key_iter->second] = true;
                        input_flag = true;
                        last_key = key_iter->second;
                    }
                }
                break;
            }
            case SDL_KEYUP:
            {
                // Handle key release
                auto key_iter = Utils::KEY_MAPPING.find(event.key.keysym.sym);
                if (key_iter != Utils::KEY_MAPPING.end()) {
                    key_state[key_iter->second] = false;
                }
                break;
            }
            default: 
                break;
        }
    }
    return false;
}


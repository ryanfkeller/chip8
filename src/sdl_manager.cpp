#include "sdl_manager.h"
#include "utilities.h"
#include <SDL2/SDL.h>
#include <stdexcept>


SDL_Manager::SDL_Manager() {
    init();
}

SDL_Manager::~SDL_Manager() {
    cleanup();
}

void SDL_Manager::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        throw std::runtime_error("Failed to initialize SDL video");
    }

    // Display Setup
    window_ = SDL_CreateWindow(
        Utils::WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        Utils::WINDOW_WIDTH,
        Utils::WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!window_) {
        throw std::runtime_error("Failed to create SDL window");
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
        throw std::runtime_error("Failed to create SDL renderer");
    }

    // Audio Setup
    SDL_AudioSpec desired, obtained;
    desired.freq = Utils::AUDIO_RATE_HZ;
    desired.format = AUDIO_S16SYS;
    desired.channels = 1;
    desired.samples = Utils::AUDIO_BUFFER_SIZE;
    desired.callback = nullptr;
    desired.userdata = nullptr;

    audio_device_ = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (audio_device_ == 0) {
        throw std::runtime_error("Failed to open audio device");
    }
    
    SDL_PauseAudioDevice(audio_device_, 0);  // Start audio
}

void SDL_Manager::cleanup() {
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
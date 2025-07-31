#pragma once
#include <SDL2/SDL.h>

class SDL_Manager {
    public: 
        SDL_Manager();
        ~SDL_Manager();

        void init();
        void cleanup();

        SDL_Window* get_windows() const { return window_; }
        SDL_Renderer* get_renderer() const { return renderer_; }
        SDL_AudioDeviceID get_audio_device() const { return audio_device_; }

    private: 
        SDL_Window* window_ = nullptr;
        SDL_Renderer* renderer_ = nullptr;
        SDL_AudioDeviceID audio_device_ = 0;
};

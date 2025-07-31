#pragma once
#include <cstdint>
#include <vector>
#include "sdl_manager.h"
#include "utilities.h"

class Sound {
    public:
        Sound(SDL_Manager& sdl_manager);

        void update(bool should_beep);

        void clear_queued_audio();

    private:
        static constexpr int SAMPLES_PER_FRAME = Utils::AUDIO_RATE_HZ / Utils::FRAME_RATE_HZ;
        int16_t beep_frame_[SAMPLES_PER_FRAME];  // Pre-generated audio for one frame
        SDL_AudioDeviceID device_id_;      // Cached from SDL_Manager
};
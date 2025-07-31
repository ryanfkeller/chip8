#include "sound.h"
#include "sdl_manager.h"
#include "utilities.h"
#include <cmath>

Sound::Sound(SDL_Manager& sdl_manager) 
    : device_id_(sdl_manager.get_audio_device()) {

    // Pre-generate one frame worth of beep samples
    for (int i = 0; i < SAMPLES_PER_FRAME; i++) {
        float phase = 2.0f * M_PI * Utils::BEEP_TONE_HZ * i / Utils::AUDIO_RATE_HZ;
        beep_frame_[i] = static_cast<int16_t>(Utils::BEEP_AMPLITUDE * std::sin(phase) * INT16_MAX);
    }
}

void Sound::update(bool should_beep) {
    if (should_beep) {
        // Queue 2-3 frames worth to create a buffer
        for (int i = 0; i < 3; i++) {
            SDL_QueueAudio(device_id_, beep_frame_, 
                          SAMPLES_PER_FRAME * sizeof(int16_t));
        }
    } else {
        clear_queued_audio();
    }
}

void Sound::clear_queued_audio() {
    // Clear any remaining queued audio
    SDL_ClearQueuedAudio(device_id_);
}
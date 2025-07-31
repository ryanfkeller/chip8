#pragma once

#include <cstdint>

class Timer {
    public: 
        Timer();

        void set(uint8_t timer_val);
        uint8_t get();
        void tick();
        bool in_timeout();

    private:
        uint8_t timer_val_ = 0;
};

#include "timer.h"
#include "debug.h"


Timer::Timer() {}

void Timer::set(uint8_t timer_val) {
    timer_val_ = timer_val;
}

uint8_t Timer::get() {
    return timer_val_;
}

void Timer::tick() {
    if (timer_val_ > 0)
        timer_val_--;
}

bool Timer::in_timeout() {
    return timer_val_ == 0;
}
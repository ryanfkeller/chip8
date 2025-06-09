#pragma once

#include "ram.h"
#include "display.h"

class CPU {
public:
    CPU(RAM& ram, Display& display);

    void reset(); // Reset the CPU state
    void cycle(); // Execute a single cycle of the CPU

private:
    RAM& ram_; // Reference to the RAM object
    Display& display_; // Reference to the Display object
};
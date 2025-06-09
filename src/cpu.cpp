#include "cpu.h"
#include "utilities.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>


CPU::CPU(RAM& ram, Display& display)
    : ram_(ram), display_(display)
{}

void CPU::cycle() {}

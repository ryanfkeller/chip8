#pragma once

#include <iostream>

#define PRINT_DEBUG(fmt, ...) \
    do { if (DEBUG) printf("[DEBUG] " #fmt "\n", ##__VA_ARGS__); } while (0)

#define PRINT_ERROR(fmt, ...) \
    do { printf("[ERROR] " #fmt "\n", ##__VA_ARGS__); exit(1); } while (0)
    
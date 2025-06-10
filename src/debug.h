#pragma once

#include <iostream>

#ifdef DEBUG
    #include <iostream>
    #define PRINT_DEBUG(...) printf(__VA_ARGS__)
#else
    #define PRINT_DEBUG(...) do {} while (0)
#endif 
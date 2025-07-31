
#include "emulator.h"
#include "utilities.h"
#include "print.h"
#include <filesystem>
#include <random>

int main(int argc, char* argv[]) {

    // Parse and load input rom file
    if (argc < 2){
        PRINT_ERROR("Please enter the .ch8 file as an argument");
    }

    std::filesystem::path rompath(argv[1]);

    if (rompath.extension() !=  ".ch8" ) {
        PRINT_ERROR("Please ensure the first argument is a .ch8 file");
    }

    Emulator emulator;
    emulator.load_rom("roms/builtin/font.ch8", Utils::FONT_START_ADDRESS);
    emulator.load_rom(rompath.string());

    emulator.run();

    return 0;
}
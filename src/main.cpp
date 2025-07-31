
#include "emulator.h"
#include "utilities.h"
#include "debug.h"

int main(/**int argc, char* argv[]**/) {

    Emulator emulator;
    emulator.load_rom("roms/builtin/font.ch8", Utils::FONT_START_ADDRESS);
    //emulator.load_rom("roms/test/ibm-logo.ch8"); //Hardcoded for now
    //emulator.load_rom("roms/test/3-corax+.ch8"); //Hardcoded for now
    //emulator.load_rom("roms/test/4-flags.ch8"); //Hardcoded for now
    emulator.load_rom("roms/test/5-quirks.ch8"); //Hardcoded for now


    //emulator.load_rom("roms/test/BC_test.ch8"); //Hardcoded for now

    emulator.run();

    return 0;
}
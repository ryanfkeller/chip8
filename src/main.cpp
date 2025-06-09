
#include "emulator.h"

int main(/**int argc, char* argv[]**/) {

    Emulator emulator;
    emulator.load_rom("../roms/test/ibm-logo.ch8"); //Hardcoded for now
    emulator.run();

    return 0;
}
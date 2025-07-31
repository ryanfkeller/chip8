# CHIP-8 Emulator

A fun little CHIP-8 interpreter written in C++ using SDL2. Built this to learn more about emulation.

## About this project

- Runs classic CHIP-8 games and programs
- 64x32 pixel display that scales up nicely
- Original Chip8 opcodes, including audio, work!
- Uses your keyboard as the CHIP-8's hex keypad
- Reasonably good timing accuracy

## Controls

The CHIP-8 had a 16-key hexadecimal keypad (0-F). This implementation uses the standard mapping to QWERT keyboards: 

```
CHIP-8 Keypad    QWERTY Keyboard
1 2 3 C          1 2 3 4
4 5 6 D    -->   Q W E R
7 8 9 E          A S D F
A 0 B F          Z X C V
```

## Building

You'll need SDL2 installed and a C++23 compiler. Then just:

```bash
mkdir build && cd build
cmake ..
make
./bin/chip8 path/to/your/rom.ch8
```

For debug builds with extra info:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

## How it works

The code is split into a few main parts:

- **CPU** - Fetches and executes CHIP-8 instructions
- **RAM** - 4KB of accessible memory (not including the call stack)
- **Peripherals** - Handles the screen, keyboard, and beeper
- **Timers** - The delay and sound timers that count down at 60Hz
- **Emulator** - Ties everything together and runs the main loop

I tried to keep it simple but still accurate to how CHIP-8 actually worked. The CPU runs at about 1000Hz and the display refreshes at 60Hz.

## ROMs

The emulator needs CHIP-8 ROM files (usually .ch8 files) to run. There are tons of public domain games and demos available online. 

- **builtin** - Currently, only the default font ROM is included. Still TODO is to add a fun Chip8 splash before going to the program ROM. 
- **test** - I included and used several tests from the excellent Timendus [chip8-test-suite](https://github.com/Timendus/chip8-test-suite) to get this build working, and highly recommend them.
- **games** - Only one sample game is included -- jackiekircher's [glitchGhost](https://github.com/jackiekircher/glitch-ghost), a surprisingly fun cemetery puzzler. This emulator should work with most other .ch8 games, though.

## Technical notes

- Uses modern CHIP-8 quirks (shift operations copy from VY, memory operations don't increment I)
- Font data gets loaded at address 0x50
- Programs start at 0x200
- The beeper plays a 440Hz tone when the sound timer is active
- Display scaling is 10x (so 640x320 window for the 64x32 display)
# Gameboy (DMG) Emulator

## Controls
### Arrow Keys => D-Pad
### A => A
### S => B
### D => Select
### F => Start

## Architectural Overview

### Emulator Core (emu_core)
The emulator itself is in the emu_core folder. It is an M-Cycle based DMG emulator that can run ROM files and manage
saves. It utilizes OpenGL and GLFW for graphics, and SDL2 for sound.

### Emulator Launcher (emu_launcher)
This code handles the bundling and distribution of my emulator. 
It provides a small launcher app that executes the emulator binary so users dont have to.
It is uses  the popular desktop framework framework [Tauri 2.0](https://v2.tauri.app/)

### Platform Support
So far this emulator Apple (both intel and Mx chips). 

## Build Instructions

### Emulator Binary Only 
Use the `build.sh` script.
Then use the command ``./build/src/GameBoyCpp ROMPATH`` (rom path is the path to the gb file)

### Bundling With Tauri
Use the `deploy.sh` script. This copies the binary over to the correct place in the emu_launcher folder.
Use `pnpm tauri dev` for dev builds and testing
Use `pnpm tauri build` for creating the executable



## Further Roadmap?
- Expand to GBC

## References
- https://gbdev.io/gb-opcodes/optables/
- https://gbdev.io/pandocs/
- https://gekkio.fi/files/gb-docs/gbctr.pdf
- https://forums.nesdev.org/viewtopic.php?t=15944
# Game Boy Emulator

A optimized C++ implementation of a Game Boy emulator with SDL2 graphics, featuring extensive CPU instruction support and efficient rendering.

## Features

- **Complete CPU Emulation**: LR35902 processor with ~200+ instructions implemented
- **Optimized GPU Rendering**: SDL2 texture-based rendering for smooth 60fps performance
- **Memory Management**: Full 64KB address space with ROM loading
- **Input Handling**: Keyboard controls for Game Boy buttons
- **Timing**: Accurate frame timing and cycle counting

## Performance Optimizations

- **GPU**: Uses SDL textures instead of per-pixel drawing for 10x+ rendering speed
- **CPU**: Efficient switch-based instruction decoding
- **Memory**: Fast array-based memory access
- **Timing**: Proper frame synchronization for consistent performance

## Building

### Option 1: MSYS2 + MinGW (Easiest)
1. Install MSYS2: `winget install --id MSYS2.MSYS2`
2. Open MSYS2 MinGW64 terminal
3. Update: `pacman -Syu`
4. Install SDL3: `pacman -S mingw-w64-x86_64-SDL3`
5. Navigate to project: `cd "/c/Users/PopSo/Desktop/My-portfolio/gameboy-emulator"`
6. Compile: `g++ -O3 main.cpp cpu.cpp memory.cpp gpu.cpp input.cpp -o gameboy.exe -lSDL3`
7. Copy DLL: `cp /mingw64/bin/SDL3.dll .`
8. Run: `./gameboy.exe [rom.gb]`

### Option 2: Visual Studio (MSVC)
1. Install Visual Studio with C++ workload
2. Download SDL3 VC development libraries from https://github.com/libsdl-org/SDL/releases/download/release-3.4.8/SDL3-devel-3.4.8-VC.zip
3. Extract to a folder (e.g., C:\SDL3-VC)
4. Open Developer Command Prompt for VS
5. Set environment: `set SDL3_DIR=C:\SDL3-VC` and update INCLUDE/LIB paths
6. Compile: `cl /EHsc /O2 main.cpp cpu.cpp memory.cpp gpu.cpp input.cpp /Fe:gameboy.exe SDL3.lib`
7. Copy DLL and run

See `build.txt` for detailed instructions and troubleshooting.

## Controls

- **Arrow Keys**: D-pad
- **Z**: A button
- **X**: B button
- **Enter**: Start
- **Space**: Select

## Compatibility

This emulator now supports most basic Game Boy instructions and can run simple games. For full compatibility with commercial titles, additional features like MBC support, sound emulation, and advanced GPU modes would be needed.

## Architecture

- **CPU**: Interprets Game Boy machine code with accurate flag handling
- **GPU**: Renders to 160x144 pixel buffer using SDL2 textures
- **Memory**: Maps ROM, RAM, and I/O registers
- **Input**: Translates keyboard input to Game Boy button states

## Educational Value

This project demonstrates:
- Low-level CPU emulation techniques
- Memory-mapped I/O handling
- Real-time graphics rendering
- Cross-platform development with SDL2
- Performance optimization in C++
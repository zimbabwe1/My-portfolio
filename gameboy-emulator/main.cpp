// Game Boy Emulator in C++
// Requires SDL3 library for graphics and input
// Compile with: g++ main.cpp cpu.cpp memory.cpp gpu.cpp input.cpp -o gameboy.exe -lSDL3 -I/path/to/SDL3/include

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>

#if defined(__has_include)
#  if __has_include(<SDL3/SDL.h>)
#    include <SDL3/SDL.h>
#  elif __has_include(<SDL2/SDL.h>)
#    include <SDL2/SDL.h>
#  elif __has_include(<SDL.h>)
#    include <SDL.h>
#  else
#    error "SDL header not found"
#  endif
#else
#  include <SDL3/SDL.h>
#endif

#include "cpu.h"
#include "memory.h"
#include "gpu.h"
#include "input.h"

class GameBoy {
public:
    GameBoy();
    ~GameBoy();
    void loadROM(const std::string& filename);
    void run();
    void reset();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    CPU* cpu;
    Memory* memory;
    GPU* gpu;
    Input* input;
    bool running;
    int cycles;
};

GameBoy::GameBoy() : running(false), cycles(0) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        // Handle error
        std::cerr << "SDL_Init failed" << std::endl;
        return;
    }
    window = SDL_CreateWindow("Game Boy Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 4, 144 * 4, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed" << std::endl;
        SDL_Quit();
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    SDL_RenderSetScale(renderer, 4, 4);

    memory = new Memory();
    gpu = new GPU(renderer);
    input = new Input();
    cpu = new CPU(memory, gpu, input);
}

GameBoy::~GameBoy() {
    delete cpu;
    delete memory;
    delete gpu;
    delete input;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GameBoy::loadROM(const std::string& filename) {
    memory->loadROM(filename);
    reset();
}

void GameBoy::run() {
    running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        input->update();

        // Run CPU for one frame
        int frameCycles = 70224;
        while (cycles < frameCycles) {
            int cpuCycles = cpu->step();
            cycles += cpuCycles;
            gpu->step(cpuCycles);
        }
        cycles = 0;

        gpu->render();
    }
}

void GameBoy::reset() {
    cpu->reset();
    gpu->reset();
    cycles = 0;
}

int main(int argc, char* argv[]) {
    GameBoy gb;

    if (argc > 1) {
        gb.loadROM(argv[1]);
    }

    gb.run();

    return 0;
}
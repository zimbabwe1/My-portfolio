#ifndef GPU_H
#define GPU_H

#include <vector>
#include <cstdint>
#include <SDL3/SDL.h>

class GPU {
public:
    GPU(SDL_Renderer* renderer);
    ~GPU();
    void step(int cycles);
    void render();
    void reset();
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t* pixels;
    std::vector<uint8_t> vram;
    int mode, line, cycles;
    std::vector<uint8_t> palette;
};

#endif
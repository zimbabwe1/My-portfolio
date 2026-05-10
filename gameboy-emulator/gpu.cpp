#include "gpu.h"
#include <SDL3/SDL.h>
#include <cstring> // for memcpy

GPU::GPU(SDL_Renderer* r) : renderer(r), vram(0x2000, 0), mode(0), line(0), cycles(0),
    palette({255, 192, 96, 0}) {
    // Create texture for faster rendering
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
    pixels = new uint32_t[160 * 144];
    memset(pixels, 0, 160 * 144 * sizeof(uint32_t));
}

GPU::~GPU() {
    SDL_DestroyTexture(texture);
    delete[] pixels;
}

void GPU::step(int c) {
    cycles += c;
    if (cycles >= 456) {
        cycles -= 456;
        line++;
        if (line == 144) {
            // VBlank
        } else if (line > 153) {
            line = 0;
        }
    }
}

void GPU::render() {
    // Update texture with pixel data for faster rendering
    SDL_UpdateTexture(texture, NULL, pixels, 160 * sizeof(uint32_t));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void GPU::reset() {
    line = cycles = 0;
    mode = 0;
    memset(pixels, 0, 160 * 144 * sizeof(uint32_t));
}

// Helper to set pixel (for demo)
void GPU::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (x >= 0 && x < 160 && y >= 0 && y < 144) {
        pixels[y * 160 + x] = (r << 24) | (g << 16) | (b << 8) | 255;
    }
}
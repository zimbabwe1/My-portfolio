#include "memory.h"
#include <fstream>

Memory::Memory() : ram(0x10000, 0) {}

void Memory::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (file) {
        rom.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }
}

uint8_t Memory::read(uint16_t addr) {
    if (addr < 0x8000 && addr < rom.size()) return rom[addr];
    return ram[addr];
}

void Memory::write(uint16_t addr, uint8_t val) {
    if (addr >= 0x8000) ram[addr] = val;
}

uint16_t Memory::read16(uint16_t addr) {
    return read(addr) | (read(addr + 1) << 8);
}

void Memory::write16(uint16_t addr, uint16_t val) {
    write(addr, val & 0xFF);
    write(addr + 1, val >> 8);
}
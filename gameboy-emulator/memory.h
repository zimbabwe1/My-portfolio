#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <string>
#include <cstdint>

class Memory {
public:
    Memory();
    void loadROM(const std::string& filename);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    uint16_t read16(uint16_t addr);
    void write16(uint16_t addr, uint16_t val);

private:
    std::vector<uint8_t> ram;
    std::vector<uint8_t> rom;
};

#endif
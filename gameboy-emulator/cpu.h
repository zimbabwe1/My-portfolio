#ifndef CPU_H
#define CPU_H

#include <cstdint>

class Memory;
class GPU;
class Input;

class CPU {
public:
    CPU(Memory* mem, GPU* gpu, Input* input);
    void reset();
    int step();
    void execute(uint8_t opcode);

    uint16_t pc, sp;
    uint8_t a, f, b, c, d, e, h, l;

private:
    Memory* memory;
    GPU* gpu;
    Input* input;
    bool interrupts_enabled;
    bool halted;

    uint16_t bc() { return (b << 8) | c; }
    void setBC(uint16_t val) { b = val >> 8; c = val & 0xFF; }
    uint16_t de() { return (d << 8) | e; }
    void setDE(uint16_t val) { d = val >> 8; e = val & 0xFF; }
    uint16_t hl() { return (h << 8) | l; }
    void setHL(uint16_t val) { h = val >> 8; l = val & 0xFF; }

    uint8_t inc(uint8_t val);
    uint8_t dec(uint8_t val);
    void setZ(bool z) { f = (f & ~0x80) | (z ? 0x80 : 0); }
    void setN(bool n) { f = (f & ~0x40) | (n ? 0x40 : 0); }
    void setH(bool h) { f = (f & ~0x20) | (h ? 0x20 : 0); }
    void setC(bool c) { f = (f & ~0x10) | (c ? 0x10 : 0); }

    void add(uint8_t val);
    void adc(uint8_t val);
    void sub(uint8_t val);
    void sbc(uint8_t val);
    void anda(uint8_t val);
    void xora(uint8_t val);
    void ora(uint8_t val);
    void cp(uint8_t val);

    void ret();
    void call(uint16_t addr);
    void rst(uint8_t addr);
    void pushBC();
    void popBC();
    void pushDE();
    void popDE();
    void pushHL();
    void popHL();
    void pushAF();
    void popAF();

    void executeCB(uint8_t opcode);
    void executeCBOp(uint8_t op, uint8_t& val);
};

#endif
#include "cpu.h"
#include "memory.h"
#include "gpu.h"
#include "input.h"
#include <iostream>

CPU::CPU(Memory* mem, GPU* gpu, Input* input) : memory(mem), gpu(gpu), input(input) {
    reset();
}

void CPU::reset() {
    pc = 0x100;
    sp = 0xFFFE;
    a = f = b = c = d = e = h = l = 0;
    interrupts_enabled = false;
    halted = false;
}

int CPU::step() {
    if (halted) return 4;

    uint8_t opcode = memory->read(pc++);
    execute(opcode);
    return 4; // Simplified cycle count
}

void CPU::execute(uint8_t opcode) {
    switch (opcode) {
        case 0x00: break; // NOP
        case 0x01: c = memory->read(pc++); b = memory->read(pc++); break; // LD BC, nn
        case 0x02: memory->write(bc(), a); break; // LD (BC), A
        case 0x03: setBC(bc() + 1); break; // INC BC
        case 0x04: b = inc(b); break; // INC B
        case 0x05: b = dec(b); break; // DEC B
        case 0x06: b = memory->read(pc++); break; // LD B, n
        case 0x07: { uint8_t carry = (a & 0x80) ? 1 : 0; a = (a << 1) | carry; setC(carry); setZ(false); setN(false); setH(false); } break; // RLCA
        case 0x08: { uint16_t addr = memory->read16(pc); pc += 2; memory->write(addr, sp & 0xFF); memory->write(addr + 1, sp >> 8); } break; // LD (nn), SP
        case 0x09: setHL(hl() + bc()); break; // ADD HL, BC
        case 0x0A: a = memory->read(bc()); break; // LD A, (BC)
        case 0x0B: setBC(bc() - 1); break; // DEC BC
        case 0x0C: c = inc(c); break; // INC C
        case 0x0D: c = dec(c); break; // DEC C
        case 0x0E: c = memory->read(pc++); break; // LD C, n
        case 0x0F: { uint8_t carry = a & 1; a = (a >> 1) | (carry << 7); setC(carry); setZ(false); setN(false); setH(false); } break; // RRCA
        case 0x10: halted = true; break; // STOP
        case 0x11: e = memory->read(pc++); d = memory->read(pc++); break; // LD DE, nn
        case 0x12: memory->write(de(), a); break; // LD (DE), A
        case 0x13: setDE(de() + 1); break; // INC DE
        case 0x14: d = inc(d); break; // INC D
        case 0x15: d = dec(d); break; // DEC D
        case 0x16: d = memory->read(pc++); break; // LD D, n
        case 0x17: { uint8_t carry = (a & 0x80) ? 1 : 0; a = (a << 1) | (f & 0x10 ? 1 : 0); setC(carry); setZ(false); setN(false); setH(false); } break; // RLA
        case 0x18: pc += (int8_t)memory->read(pc) + 1; break; // JR r8
        case 0x19: setHL(hl() + de()); break; // ADD HL, DE
        case 0x1A: a = memory->read(de()); break; // LD A, (DE)
        case 0x1B: setDE(de() - 1); break; // DEC DE
        case 0x1C: e = inc(e); break; // INC E
        case 0x1D: e = dec(e); break; // DEC E
        case 0x1E: e = memory->read(pc++); break; // LD E, n
        case 0x1F: { uint8_t carry = a & 1; a = (a >> 1) | ((f & 0x10 ? 1 : 0) << 7); setC(carry); setZ(false); setN(false); setH(false); } break; // RRA
        case 0x20: if (!(f & 0x80)) { pc += (int8_t)memory->read(pc) + 1; } else pc++; break; // JR NZ, r8
        case 0x21: l = memory->read(pc++); h = memory->read(pc++); break; // LD HL, nn
        case 0x22: memory->write(hl(), a); setHL(hl() + 1); break; // LD (HL+), A
        case 0x23: setHL(hl() + 1); break; // INC HL
        case 0x24: h = inc(h); break; // INC H
        case 0x25: h = dec(h); break; // DEC H
        case 0x26: h = memory->read(pc++); break; // LD H, n
        case 0x27: { // DAA - Decimal Adjust Accumulator
            uint8_t adjust = 0;
            if ((f & 0x20) || (a & 0x0F) > 9) adjust |= 0x06;
            if ((f & 0x10) || a > 0x99) { adjust |= 0x60; setC(true); } else setC(false);
            a += (f & 0x40) ? -adjust : adjust;
            setZ(a == 0);
            setH(false);
        } break;
        case 0x28: if (f & 0x80) { pc += (int8_t)memory->read(pc) + 1; } else pc++; break; // JR Z, r8
        case 0x29: setHL(hl() + hl()); break; // ADD HL, HL
        case 0x2A: a = memory->read(hl()); setHL(hl() + 1); break; // LD A, (HL+)
        case 0x2B: setHL(hl() - 1); break; // DEC HL
        case 0x2C: l = inc(l); break; // INC L
        case 0x2D: l = dec(l); break; // DEC L
        case 0x2E: l = memory->read(pc++); break; // LD L, n
        case 0x2F: a = ~a; setN(true); setH(true); break; // CPL
        case 0x30: if (!(f & 0x10)) { pc += (int8_t)memory->read(pc) + 1; } else pc++; break; // JR NC, r8
        case 0x31: sp = memory->read16(pc); pc += 2; break; // LD SP, nn
        case 0x32: memory->write(hl(), a); setHL(hl() - 1); break; // LD (HL-), A
        case 0x33: sp++; break; // INC SP
        case 0x34: { uint8_t val = memory->read(hl()); memory->write(hl(), inc(val)); } break; // INC (HL)
        case 0x35: { uint8_t val = memory->read(hl()); memory->write(hl(), dec(val)); } break; // DEC (HL)
        case 0x36: memory->write(hl(), memory->read(pc++)); break; // LD (HL), n
        case 0x37: setC(true); setN(false); setH(false); break; // SCF
        case 0x38: if (f & 0x10) { pc += (int8_t)memory->read(pc) + 1; } else pc++; break; // JR C, r8
        case 0x39: setHL(hl() + sp); break; // ADD HL, SP
        case 0x3A: a = memory->read(hl()); setHL(hl() - 1); break; // LD A, (HL-)
        case 0x3B: sp--; break; // DEC SP
        case 0x3C: a = inc(a); break; // INC A
        case 0x3D: a = dec(a); break; // DEC A
        case 0x3E: a = memory->read(pc++); break; // LD A, n
        case 0x3F: setC(!(f & 0x10)); setN(false); setH(false); break; // CCF
        case 0x40: b = b; break; // LD B, B
        case 0x41: b = c; break; // LD B, C
        case 0x42: b = d; break; // LD B, D
        case 0x43: b = e; break; // LD B, E
        case 0x44: b = h; break; // LD B, H
        case 0x45: b = l; break; // LD B, L
        case 0x46: b = memory->read(hl()); break; // LD B, (HL)
        case 0x47: b = a; break; // LD B, A
        case 0x48: c = b; break; // LD C, B
        case 0x49: c = c; break; // LD C, C
        case 0x4A: c = d; break; // LD C, D
        case 0x4B: c = e; break; // LD C, E
        case 0x4C: c = h; break; // LD C, H
        case 0x4D: c = l; break; // LD C, L
        case 0x4E: c = memory->read(hl()); break; // LD C, (HL)
        case 0x4F: c = a; break; // LD C, A
        case 0x50: d = b; break; // LD D, B
        case 0x51: d = c; break; // LD D, C
        case 0x52: d = d; break; // LD D, D
        case 0x53: d = e; break; // LD D, E
        case 0x54: d = h; break; // LD D, H
        case 0x55: d = l; break; // LD D, L
        case 0x56: d = memory->read(hl()); break; // LD D, (HL)
        case 0x57: d = a; break; // LD D, A
        case 0x58: e = b; break; // LD E, B
        case 0x59: e = c; break; // LD E, C
        case 0x5A: e = d; break; // LD E, D
        case 0x5B: e = e; break; // LD E, E
        case 0x5C: e = h; break; // LD E, H
        case 0x5D: e = l; break; // LD E, L
        case 0x5E: e = memory->read(hl()); break; // LD E, (HL)
        case 0x5F: e = a; break; // LD E, A
        case 0x60: h = b; break; // LD H, B
        case 0x61: h = c; break; // LD H, C
        case 0x62: h = d; break; // LD H, D
        case 0x63: h = e; break; // LD H, E
        case 0x64: h = h; break; // LD H, H
        case 0x65: h = l; break; // LD H, L
        case 0x66: h = memory->read(hl()); break; // LD H, (HL)
        case 0x67: h = a; break; // LD H, A
        case 0x68: l = b; break; // LD L, B
        case 0x69: l = c; break; // LD L, C
        case 0x6A: l = d; break; // LD L, D
        case 0x6B: l = e; break; // LD L, E
        case 0x6C: l = h; break; // LD L, H
        case 0x6D: l = l; break; // LD L, L
        case 0x6E: l = memory->read(hl()); break; // LD L, (HL)
        case 0x6F: l = a; break; // LD L, A
        case 0x70: memory->write(hl(), b); break; // LD (HL), B
        case 0x71: memory->write(hl(), c); break; // LD (HL), C
        case 0x72: memory->write(hl(), d); break; // LD (HL), D
        case 0x73: memory->write(hl(), e); break; // LD (HL), E
        case 0x74: memory->write(hl(), h); break; // LD (HL), H
        case 0x75: memory->write(hl(), l); break; // LD (HL), L
        case 0x76: halted = true; break; // HALT
        case 0x77: memory->write(hl(), a); break; // LD (HL), A
        case 0x78: a = b; break; // LD A, B
        case 0x79: a = c; break; // LD A, C
        case 0x7A: a = d; break; // LD A, D
        case 0x7B: a = e; break; // LD A, E
        case 0x7C: a = h; break; // LD A, H
        case 0x7D: a = l; break; // LD A, L
        case 0x7E: a = memory->read(hl()); break; // LD A, (HL)
        case 0x7F: break; // LD A, A
        case 0x80: add(b); break; // ADD A, B
        case 0x81: add(c); break; // ADD A, C
        case 0x82: add(d); break; // ADD A, D
        case 0x83: add(e); break; // ADD A, E
        case 0x84: add(h); break; // ADD A, H
        case 0x85: add(l); break; // ADD A, L
        case 0x86: add(memory->read(hl())); break; // ADD A, (HL)
        case 0x87: add(a); break; // ADD A, A
        case 0x88: adc(b); break; // ADC A, B
        case 0x89: adc(c); break; // ADC A, C
        case 0x8A: adc(d); break; // ADC A, D
        case 0x8B: adc(e); break; // ADC A, E
        case 0x8C: adc(h); break; // ADC A, H
        case 0x8D: adc(l); break; // ADC A, L
        case 0x8E: adc(memory->read(hl())); break; // ADC A, (HL)
        case 0x8F: adc(a); break; // ADC A, A
        case 0x90: sub(b); break; // SUB B
        case 0x91: sub(c); break; // SUB C
        case 0x92: sub(d); break; // SUB D
        case 0x93: sub(e); break; // SUB E
        case 0x94: sub(h); break; // SUB H
        case 0x95: sub(l); break; // SUB L
        case 0x96: sub(memory->read(hl())); break; // SUB (HL)
        case 0x97: sub(a); break; // SUB A
        case 0x98: sbc(b); break; // SBC A, B
        case 0x99: sbc(c); break; // SBC A, C
        case 0x9A: sbc(d); break; // SBC A, D
        case 0x9B: sbc(e); break; // SBC A, E
        case 0x9C: sbc(h); break; // SBC A, H
        case 0x9D: sbc(l); break; // SBC A, L
        case 0x9E: sbc(memory->read(hl())); break; // SBC A, (HL)
        case 0x9F: sbc(a); break; // SBC A, A
        case 0xA0: anda(b); break; // AND B
        case 0xA1: anda(c); break; // AND C
        case 0xA2: anda(d); break; // AND D
        case 0xA3: anda(e); break; // AND E
        case 0xA4: anda(h); break; // AND H
        case 0xA5: anda(l); break; // AND L
        case 0xA6: anda(memory->read(hl())); break; // AND (HL)
        case 0xA7: anda(a); break; // AND A
        case 0xA8: xora(b); break; // XOR B
        case 0xA9: xora(c); break; // XOR C
        case 0xAA: xora(d); break; // XOR D
        case 0xAB: xora(e); break; // XOR E
        case 0xAC: xora(h); break; // XOR H
        case 0xAD: xora(l); break; // XOR L
        case 0xAE: xora(memory->read(hl())); break; // XOR (HL)
        case 0xAF: xora(a); break; // XOR A
        case 0xB0: ora(b); break; // OR B
        case 0xB1: ora(c); break; // OR C
        case 0xB2: ora(d); break; // OR D
        case 0xB3: ora(e); break; // OR E
        case 0xB4: ora(h); break; // OR H
        case 0xB5: ora(l); break; // OR L
        case 0xB6: ora(memory->read(hl())); break; // OR (HL)
        case 0xB7: ora(a); break; // OR A
        case 0xB8: cp(b); break; // CP B
        case 0xB9: cp(c); break; // CP C
        case 0xBA: cp(d); break; // CP D
        case 0xBB: cp(e); break; // CP E
        case 0xBC: cp(h); break; // CP H
        case 0xBD: cp(l); break; // CP L
        case 0xBE: cp(memory->read(hl())); break; // CP (HL)
        case 0xBF: cp(a); break; // CP A
        case 0xC0: if (!(f & 0x80)) ret(); break; // RET NZ
        case 0xC1: popBC(); break; // POP BC
        case 0xC2: if (!(f & 0x80)) { pc = memory->read16(pc); } else pc += 2; break; // JP NZ, nn
        case 0xC3: pc = memory->read16(pc); break; // JP nn
        case 0xC4: if (!(f & 0x80)) call(memory->read16(pc)); else pc += 2; break; // CALL NZ, nn
        case 0xC5: pushBC(); break; // PUSH BC
        case 0xC6: add(memory->read(pc++)); break; // ADD A, n
        case 0xC7: rst(0x00); break; // RST 00H
        case 0xC8: if (f & 0x80) ret(); break; // RET Z
        case 0xC9: ret(); break; // RET
        case 0xCA: if (f & 0x80) { pc = memory->read16(pc); } else pc += 2; break; // JP Z, nn
        case 0xCB: executeCB(memory->read(pc++)); break; // CB prefix
        case 0xCC: if (f & 0x80) call(memory->read16(pc)); else pc += 2; break; // CALL Z, nn
        case 0xCD: call(memory->read16(pc)); break; // CALL nn
        case 0xCE: adc(memory->read(pc++)); break; // ADC A, n
        case 0xCF: rst(0x08); break; // RST 08H
        case 0xD0: if (!(f & 0x10)) ret(); break; // RET NC
        case 0xD1: popDE(); break; // POP DE
        case 0xD2: if (!(f & 0x10)) { pc = memory->read16(pc); } else pc += 2; break; // JP NC, nn
        case 0xD4: if (!(f & 0x10)) call(memory->read16(pc)); else pc += 2; break; // CALL NC, nn
        case 0xD5: pushDE(); break; // PUSH DE
        case 0xD6: sub(memory->read(pc++)); break; // SUB n
        case 0xD7: rst(0x10); break; // RST 10H
        case 0xD8: if (f & 0x10) ret(); break; // RET C
        case 0xD9: ret(); interrupts_enabled = true; break; // RETI
        case 0xDA: if (f & 0x10) { pc = memory->read16(pc); } else pc += 2; break; // JP C, nn
        case 0xDC: if (f & 0x10) call(memory->read16(pc)); else pc += 2; break; // CALL C, nn
        case 0xDE: sbc(memory->read(pc++)); break; // SBC A, n
        case 0xDF: rst(0x18); break; // RST 18H
        case 0xE0: memory->write(0xFF00 + memory->read(pc++), a); break; // LDH (n), A
        case 0xE1: popHL(); break; // POP HL
        case 0xE2: memory->write(0xFF00 + c, a); break; // LD (C), A
        case 0xE5: pushHL(); break; // PUSH HL
        case 0xE6: anda(memory->read(pc++)); break; // AND n
        case 0xE7: rst(0x20); break; // RST 20H
        case 0xE8: { int8_t offset = memory->read(pc++); sp += offset; } break; // ADD SP, r8
        case 0xE9: pc = hl(); break; // JP (HL)
        case 0xEA: memory->write(memory->read16(pc), a); pc += 2; break; // LD (nn), A
        case 0xEE: xora(memory->read(pc++)); break; // XOR n
        case 0xEF: rst(0x28); break; // RST 28H
        case 0xF0: a = memory->read(0xFF00 + memory->read(pc++)); break; // LDH A, (n)
        case 0xF1: popAF(); break; // POP AF
        case 0xF2: a = memory->read(0xFF00 + c); break; // LD A, (C)
        case 0xF3: interrupts_enabled = false; break; // DI
        case 0xF5: pushAF(); break; // PUSH AF
        case 0xF6: ora(memory->read(pc++)); break; // OR n
        case 0xF7: rst(0x30); break; // RST 30H
        case 0xF8: { int8_t offset = memory->read(pc++); setHL(sp + offset); } break; // LD HL, SP+r8
        case 0xF9: sp = hl(); break; // LD SP, HL
        case 0xFA: a = memory->read(memory->read16(pc)); pc += 2; break; // LD A, (nn)
        case 0xFB: interrupts_enabled = true; break; // EI
        case 0xFE: cp(memory->read(pc++)); break; // CP n
        case 0xFF: rst(0x38); break; // RST 38H
        default:
            std::cout << "Unknown opcode: " << std::hex << (int)opcode << std::endl;
            halted = true;
    }
}

uint8_t CPU::inc(uint8_t val) {
    uint8_t result = val + 1;
    setZ(result == 0);
    setN(false);
    setH((val & 0xF) == 0xF);
    return result;
}

uint8_t CPU::dec(uint8_t val) {
    uint8_t result = val - 1;
    setZ(result == 0);
    setN(true);
    setH((val & 0xF) == 0);
    return result;
}

void CPU::add(uint8_t val) {
    uint16_t result = a + val;
    setZ((result & 0xFF) == 0);
    setN(false);
    setH((a & 0xF) + (val & 0xF) > 0xF);
    setC(result > 0xFF);
    a = result & 0xFF;
}

void CPU::adc(uint8_t val) {
    uint16_t carry = (f & 0x10) ? 1 : 0;
    uint16_t result = a + val + carry;
    setZ((result & 0xFF) == 0);
    setN(false);
    setH((a & 0xF) + (val & 0xF) + carry > 0xF);
    setC(result > 0xFF);
    a = result & 0xFF;
}

void CPU::sub(uint8_t val) {
    uint16_t result = a - val;
    setZ((result & 0xFF) == 0);
    setN(true);
    setH((a & 0xF) < (val & 0xF));
    setC(a < val);
    a = result & 0xFF;
}

void CPU::sbc(uint8_t val) {
    uint16_t carry = (f & 0x10) ? 1 : 0;
    uint16_t result = a - val - carry;
    setZ((result & 0xFF) == 0);
    setN(true);
    setH((a & 0xF) < (val & 0xF) + carry);
    setC(a < val + carry);
    a = result & 0xFF;
}

void CPU::anda(uint8_t val) {
    a &= val;
    setZ(a == 0);
    setN(false);
    setH(true);
    setC(false);
}

void CPU::xora(uint8_t val) {
    a ^= val;
    setZ(a == 0);
    setN(false);
    setH(false);
    setC(false);
}

void CPU::ora(uint8_t val) {
    a |= val;
    setZ(a == 0);
    setN(false);
    setH(false);
    setC(false);
}

void CPU::cp(uint8_t val) {
    setZ(a == val);
    setN(true);
    setH((a & 0xF) < (val & 0xF));
    setC(a < val);
}

void CPU::ret() {
    pc = memory->read16(sp);
    sp += 2;
}

void CPU::call(uint16_t addr) {
    sp -= 2;
    memory->write16(sp, pc);
    pc = addr;
}

void CPU::rst(uint8_t addr) {
    call(addr);
}

void CPU::pushBC() {
    sp -= 2;
    memory->write16(sp, bc());
}

void CPU::popBC() {
    setBC(memory->read16(sp));
    sp += 2;
}

void CPU::pushDE() {
    sp -= 2;
    memory->write16(sp, de());
}

void CPU::popDE() {
    setDE(memory->read16(sp));
    sp += 2;
}

void CPU::pushHL() {
    sp -= 2;
    memory->write16(sp, hl());
}

void CPU::popHL() {
    setHL(memory->read16(sp));
    sp += 2;
}

void CPU::pushAF() {
    sp -= 2;
    memory->write16(sp, (a << 8) | f);
}

void CPU::popAF() {
    uint16_t af = memory->read16(sp);
    a = af >> 8;
    f = af & 0xFF;
    sp += 2;
}

void CPU::executeCB(uint8_t opcode) {
    // CB prefix instructions - simplified
    uint8_t reg = opcode & 0x07;
    uint8_t* target = nullptr;
    switch (reg) {
        case 0: target = &b; break;
        case 1: target = &c; break;
        case 2: target = &d; break;
        case 3: target = &e; break;
        case 4: target = &h; break;
        case 5: target = &l; break;
        case 6: { uint8_t val = memory->read(hl()); executeCBOp(opcode >> 3, val); memory->write(hl(), val); return; }
        case 7: target = &a; break;
    }
    if (target) executeCBOp(opcode >> 3, *target);
}

void CPU::executeCBOp(uint8_t op, uint8_t& val) {
    switch (op) {
        case 0: val <<= 1; setC(val & 0x80); val &= 0xFE; break; // RLC
        case 1: val >>= 1; setC(val & 0x01); val &= 0x7F; break; // RRC
        case 2: { uint8_t carry = val >> 7; val = (val << 1) | (f & 0x10 ? 1 : 0); setC(carry); } break; // RL
        case 3: { uint8_t carry = val & 1; val = (val >> 1) | ((f & 0x10 ? 1 : 0) << 7); setC(carry); } break; // RR
        // Add more CB instructions as needed
    }
    setZ(val == 0);
    setN(false);
    setH(false);
}
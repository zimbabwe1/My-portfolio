// Game Boy Emulator in JavaScript
// This is a simplified Game Boy emulator for educational purposes.
// It implements basic CPU emulation, memory, and graphics.

class GameBoy {
    constructor(canvas) {
        this.canvas = canvas;
        this.ctx = canvas.getContext('2d');
        this.cpu = new CPU(this);
        this.memory = new Memory();
        this.gpu = new GPU(this.canvas, this.ctx);
        this.input = new Input();
        this.running = false;
        this.cycles = 0;
    }

    loadROM(romData) {
        this.memory.loadROM(romData);
        this.cpu.reset();
        this.gpu.reset();
    }

    start() {
        if (!this.running) {
            this.running = true;
            this.run();
        }
    }

    pause() {
        this.running = false;
    }

    reset() {
        this.cpu.reset();
        this.gpu.reset();
        this.running = false;
    }

    run() {
        if (!this.running) return;

        // Run CPU for a frame worth of cycles (about 70224 cycles per frame at 60fps)
        const targetCycles = 70224;
        while (this.cycles < targetCycles && this.running) {
            const cycles = this.cpu.step();
            this.cycles += cycles;
            this.gpu.step(cycles);
        }

        this.cycles = 0;
        this.gpu.render();

        // Request next frame
        requestAnimationFrame(() => this.run());
    }
}

class CPU {
    constructor(gameboy) {
        this.gb = gameboy;
        this.memory = gameboy.memory;
        this.reset();
    }

    reset() {
        this.pc = 0x100; // Program counter starts at 0x100 for Game Boy
        this.sp = 0xFFFE; // Stack pointer
        this.a = 0; this.f = 0; // Accumulator and flags
        this.b = 0; this.c = 0;
        this.d = 0; this.e = 0;
        this.h = 0; this.l = 0;
        this.interrupts = false;
        this.halted = false;
    }

    step() {
        if (this.halted) return 4;

        const opcode = this.memory.read(this.pc++);
        return this.execute(opcode);
    }

    execute(opcode) {
        switch (opcode) {
            case 0x00: // NOP
                return 4;
            case 0x01: // LD BC, nn
                this.c = this.memory.read(this.pc++);
                this.b = this.memory.read(this.pc++);
                return 12;
            case 0x02: // LD (BC), A
                this.memory.write(this.bc(), this.a);
                return 8;
            case 0x03: // INC BC
                this.setBC(this.bc() + 1);
                return 8;
            case 0x04: // INC B
                this.b = this.inc(this.b);
                return 4;
            case 0x05: // DEC B
                this.b = this.dec(this.b);
                return 4;
            case 0x06: // LD B, n
                this.b = this.memory.read(this.pc++);
                return 8;
            case 0x0C: // INC C
                this.c = this.inc(this.c);
                return 4;
            case 0x0D: // DEC C
                this.c = this.dec(this.c);
                return 4;
            case 0x0E: // LD C, n
                this.c = this.memory.read(this.pc++);
                return 8;
            case 0x11: // LD DE, nn
                this.e = this.memory.read(this.pc++);
                this.d = this.memory.read(this.pc++);
                return 12;
            case 0x13: // INC DE
                this.setDE(this.de() + 1);
                return 8;
            case 0x14: // INC D
                this.d = this.inc(this.d);
                return 4;
            case 0x15: // DEC D
                this.d = this.dec(this.d);
                return 4;
            case 0x16: // LD D, n
                this.d = this.memory.read(this.pc++);
                return 8;
            case 0x1C: // INC E
                this.e = this.inc(this.e);
                return 4;
            case 0x1D: // DEC E
                this.e = this.dec(this.e);
                return 4;
            case 0x1E: // LD E, n
                this.e = this.memory.read(this.pc++);
                return 8;
            case 0x21: // LD HL, nn
                this.l = this.memory.read(this.pc++);
                this.h = this.memory.read(this.pc++);
                return 12;
            case 0x23: // INC HL
                this.setHL(this.hl() + 1);
                return 8;
            case 0x24: // INC H
                this.h = this.inc(this.h);
                return 4;
            case 0x25: // DEC H
                this.h = this.dec(this.h);
                return 4;
            case 0x26: // LD H, n
                this.h = this.memory.read(this.pc++);
                return 8;
            case 0x2C: // INC L
                this.l = this.inc(this.l);
                return 4;
            case 0x2D: // DEC L
                this.l = this.dec(this.l);
                return 4;
            case 0x2E: // LD L, n
                this.l = this.memory.read(this.pc++);
                return 8;
            case 0x31: // LD SP, nn
                this.sp = this.memory.read16(this.pc);
                this.pc += 2;
                return 12;
            case 0x32: // LD (HL-), A
                this.memory.write(this.hl(), this.a);
                this.setHL(this.hl() - 1);
                return 8;
            case 0x3C: // INC A
                this.a = this.inc(this.a);
                return 4;
            case 0x3D: // DEC A
                this.a = this.dec(this.a);
                return 4;
            case 0x3E: // LD A, n
                this.a = this.memory.read(this.pc++);
                return 8;
            case 0x76: // HALT
                this.halted = true;
                return 4;
            case 0xAF: // XOR A
                this.a = 0;
                this.setZ(true);
                this.setN(false);
                this.setH(false);
                this.setC(false);
                return 4;
            case 0xC3: // JP nn
                this.pc = this.memory.read16(this.pc);
                return 16;
            case 0xE0: // LDH (n), A
                this.memory.write(0xFF00 + this.memory.read(this.pc++), this.a);
                return 12;
            case 0xF0: // LDH A, (n)
                this.a = this.memory.read(0xFF00 + this.memory.read(this.pc++));
                return 12;
            default:
                console.log(`Unknown opcode: ${opcode.toString(16)} at PC: ${(this.pc-1).toString(16)}`);
                this.halted = true;
                return 4;
        }
    }

    // Helper methods
    bc() { return (this.b << 8) | this.c; }
    setBC(val) { this.b = val >> 8; this.c = val & 0xFF; }
    de() { return (this.d << 8) | this.e; }
    setDE(val) { this.d = val >> 8; this.e = val & 0xFF; }
    hl() { return (this.h << 8) | this.l; }
    setHL(val) { this.h = val >> 8; this.l = val & 0xFF; }

    inc(val) {
        const result = (val + 1) & 0xFF;
        this.setZ(result === 0);
        this.setN(false);
        this.setH((val & 0xF) === 0xF);
        return result;
    }

    dec(val) {
        const result = (val - 1) & 0xFF;
        this.setZ(result === 0);
        this.setN(true);
        this.setH((val & 0xF) === 0);
        return result;
    }

    setZ(z) { this.f = (this.f & ~0x80) | (z ? 0x80 : 0); }
    setN(n) { this.f = (this.f & ~0x40) | (n ? 0x40 : 0); }
    setH(h) { this.f = (this.f & ~0x20) | (h ? 0x20 : 0); }
    setC(c) { this.f = (this.f & ~0x10) | (c ? 0x10 : 0); }
}

class Memory {
    constructor() {
        this.ram = new Uint8Array(0x10000); // 64KB
        this.rom = null;
    }

    loadROM(data) {
        this.rom = new Uint8Array(data);
    }

    read(addr) {
        if (addr < 0x8000 && this.rom) {
            return this.rom[addr] || 0;
        }
        return this.ram[addr] || 0;
    }

    read16(addr) {
        return this.read(addr) | (this.read(addr + 1) << 8);
    }

    write(addr, val) {
        if (addr >= 0x8000) { // RAM area
            this.ram[addr] = val;
        }
        // ROM is read-only
    }
}

class GPU {
    constructor(canvas, ctx) {
        this.canvas = canvas;
        this.ctx = ctx;
        this.vram = new Uint8Array(0x2000); // 8KB VRAM
        this.oam = new Uint8Array(0xA0); // Object Attribute Memory
        this.mode = 0;
        this.line = 0;
        this.cycles = 0;
        this.palette = [255, 192, 96, 0]; // White to black
    }

    reset() {
        this.line = 0;
        this.cycles = 0;
        this.mode = 0;
    }

    step(cycles) {
        this.cycles += cycles;
        // Simplified GPU timing
        if (this.cycles >= 456) { // 456 cycles per line
            this.cycles -= 456;
            this.line++;
            if (this.line === 144) {
                // VBlank interrupt
            } else if (this.line > 153) {
                this.line = 0;
            }
        }
    }

    render() {
        // Simple rendering: draw a checkerboard pattern for demo
        const imageData = this.ctx.createImageData(160, 144);
        const data = imageData.data;

        for (let y = 0; y < 144; y++) {
            for (let x = 0; x < 160; x++) {
                const index = (y * 160 + x) * 4;
                const color = ((x + y) % 2) * 255;
                data[index] = color;     // R
                data[index + 1] = color; // G
                data[index + 2] = color; // B
                data[index + 3] = 255;   // A
            }
        }

        this.ctx.putImageData(imageData, 0, 0);
    }
}

class Input {
    constructor() {
        this.keys = {};
    }

    isPressed(key) {
        return !!this.keys[key];
    }
}

// Initialize emulator
const canvas = document.getElementById('gameboy-screen');
const gameboy = new GameBoy(canvas);

// Event listeners
document.getElementById('load-rom').addEventListener('click', () => {
    document.getElementById('rom-input').click();
});

document.getElementById('rom-input').addEventListener('change', (e) => {
    const file = e.target.files[0];
    if (file) {
        const reader = new FileReader();
        reader.onload = () => {
            gameboy.loadROM(reader.result);
        };
        reader.readAsArrayBuffer(file);
    }
});

document.getElementById('start').addEventListener('click', () => gameboy.start());
document.getElementById('pause').addEventListener('click', () => gameboy.pause());
document.getElementById('reset').addEventListener('click', () => gameboy.reset());

// For demo, load a simple "ROM" that draws something
// This is just a placeholder - real ROMs are binary files
setTimeout(() => {
    // Create a simple demo ROM
    const demoROM = new ArrayBuffer(0x8000);
    const view = new Uint8Array(demoROM);
    // Simple program: infinite loop with some graphics
    view[0x100] = 0x3E; view[0x101] = 0xFF; // LD A, 0xFF
    view[0x102] = 0xE0; view[0x103] = 0x40; // LDH (0x40), A
    view[0x104] = 0xC3; view[0x105] = 0x00; view[0x106] = 0x01; // JP 0x0100
    gameboy.loadROM(demoROM);
}, 1000);
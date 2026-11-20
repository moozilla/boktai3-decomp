"""Small SM83 + DMG PPU oracle for this cartridge, starting after the boot ROM.

Only opcodes and peripherals used by the cartridge are implemented. Unsupported
instructions, bus accesses during DMA, LCD-off outside VBlank, and mode-3
VRAM writes raise exceptions. PPU output reads only DMG memory and registers.
"""
import struct

import numpy as np
from PIL import Image


class DMG:
    Z, N, H, C = 128, 64, 32, 16

    def __init__(self, rom):
        if sum(rom[0x134:0x14E]) + 25 & 255:
            raise ValueError("header checksum")
        if (sum(rom[:0x14E]) + sum(rom[0x150:])) & 65535 != int.from_bytes(rom[0x14E:0x150], "big"):
            raise ValueError("global checksum")
        if len(rom) != 32768 << rom[0x148] or rom[0x147] != 0x19 or rom[0x143] != 0:
            raise ValueError("expected a DMG-only MBC5 ROM")
        self.rom = rom
        self.bank = 1
        self.mem = bytearray(65536)
        self.mem[0xFF40] = 0x91
        self.pc, self.sp = 0x100, 0xFFFE
        self.r = [0, 0x13, 0, 0xD8, 1, 0x4D, 0, 1]  # B C D E H L (HL) A
        self.f = 0xB0
        self.keys = 0
        self.cycles = self.dot = self.frames = self.dma_until = 0
        self.lowest_sp = self.sp
        self.vblank_writes = 0
        self.peak_line = self.peak_oam = 0

    def advance(self, cycles):
        self.cycles += cycles
        if self.mem[0xFF40] & 128:
            # Crossing line 144 requests VBlank, including across frame wrap.
            if (self.dot + cycles - 65664) // 70224 > (self.dot - 65664) // 70224:
                self.mem[0xFF0F] |= 1
                self.frames += 1
            self.dot = (self.dot + cycles) % 70224

    def read(self, addr):
        if self.cycles < self.dma_until and not 0xFF80 <= addr <= 0xFFFE:
            raise RuntimeError(f"DMG bus read during OAM DMA at {addr:04x}")
        if addr < 0x4000:
            return self.rom[addr]
        if addr < 0x8000:
            return self.rom[self.bank * 0x4000 + addr - 0x4000]
        if addr == 0xFF44:
            return self.dot // 456
        if addr == 0xFF00:
            value = self.mem[addr]
            keys = (self.keys >> 4 if not value & 0x10 else 0) | (self.keys & 15 if not value & 0x20 else 0)
            return (value & 0x30) | 0xC0 | (~keys & 15)
        return self.mem[addr]

    def write(self, addr, value):
        if self.cycles < self.dma_until and not 0xFF80 <= addr <= 0xFFFE:
            raise RuntimeError(f"DMG bus write during DMA at {addr:04x}")
        if 0x2000 <= addr < 0x3000:
            if value * 0x4000 >= len(self.rom):
                raise ValueError(f"missing ROM bank {value}")
            self.bank = value
            return
        if addr < 0x8000:
            raise RuntimeError(f"unsupported mapper write {addr:04x}")
        lcd = self.mem[0xFF40] & 128
        if 0x8000 <= addr < 0xA000 and lcd:
            if self.dot < 65664 and 80 <= self.dot % 456 < 252:
                raise RuntimeError("VRAM write during mode 3")
            # Cartridge promises all writes here are in VBlank.
            if self.dot < 65664:
                raise RuntimeError("foreground VRAM write outside VBlank")
            self.vblank_writes += 1
        if addr == 0xFF40 and value & 128 != lcd:
            if lcd and self.dot < 65664:
                raise RuntimeError("LCD disabled during active scan")
            self.dot = 0
        if addr == 0xFF46:
            if lcd and self.dot >= 70224 - 640:
                raise RuntimeError("OAM DMA would run past VBlank")
            if lcd and self.dot < 65664:
                raise RuntimeError("OAM DMA would run during visible lines")
            base = value << 8
            self.mem[0xFE00:0xFEA0] = self.mem[base:base + 160]
            self.dma_until = self.cycles + 640
        self.mem[addr] = value & 255

    def pair(self, i):
        return self.sp if i == 3 else (self.r[i * 2] << 8) | self.r[i * 2 + 1]

    def sp_to(self, value):
        self.sp = value & 65535
        self.lowest_sp = min(self.lowest_sp, self.sp)

    def set_pair(self, i, value):
        if i == 3:
            self.sp_to(value)
        else:
            self.r[2 * i:2 * i + 2] = [(value >> 8) & 255, value & 255]

    def reg(self, n):
        return self.read(self.pair(2)) if n == 6 else self.r[n]

    def set_reg(self, n, value):
        if n == 6:
            self.write(self.pair(2), value)
        else:
            self.r[n] = value & 255

    def byte(self):
        b = self.read(self.pc)
        self.pc = (self.pc + 1) & 65535
        return b

    def word(self):
        lo = self.byte()
        return lo | self.byte() << 8

    def push(self, value):
        self.sp_to(self.sp - 2)
        self.write(self.sp, value & 255)
        self.write(self.sp + 1, value >> 8)

    def pop(self):
        n = self.read(self.sp) | self.read(self.sp + 1) << 8
        self.sp_to(self.sp + 2)
        return n

    def flags(self, val, n=False, h=False, c=False):
        self.f = (self.Z if val & 255 == 0 else 0) | (self.N if n else 0) | (self.H if h else 0) | (self.C if c else 0)

    def condition(self, n):
        return [not self.f & self.Z, self.f & self.Z, not self.f & self.C, self.f & self.C][n]

    def alu(self, op, arg):
        a = self.r[7]
        carry = int(bool(self.f & self.C))
        if op in (0, 1):
            addend = carry if op == 1 else 0
            v = a + arg + addend
            self.flags(v, h=(a & 15) + (arg & 15) + addend > 15, c=v > 255)
        elif op in (2, 3, 7):
            borrow = carry if op == 3 else 0
            v = a - arg - borrow
            self.flags(v, n=True, h=(a & 15) < (arg & 15) + borrow, c=v < 0)
        else:
            v = (a & arg, a ^ arg, a | arg)[op - 4]
            self.flags(v, h=op == 4)
        if op != 7:
            self.r[7] = v & 255

    def step(self):
        opcode = self.byte()
        hi, lo = opcode >> 3 & 7, opcode & 7
        group = opcode >> 4 & 3
        t = 4
        if opcode in (0, 0xF3):
            pass
        elif opcode == 0x76:
            if not self.mem[0xFFFF] & 1 or not self.mem[0xFF40] & 128:
                raise RuntimeError("HALT without a clocked interrupt source")
            if not self.mem[0xFF0F] & self.mem[0xFFFF] & 1:
                self.advance((65664 - self.dot) % 70224)
        elif 0x40 <= opcode <= 0x7F:
            self.set_reg(hi, self.reg(lo))
            t = 8 if 6 in (hi, lo) else 4
        elif 0x80 <= opcode <= 0xBF:
            self.alu(hi, self.reg(lo))
            t = 8 if lo == 6 else 4
        elif opcode & 0xC7 == 0xC6:
            self.alu(hi, self.byte())
            t = 8
        elif opcode & 0xC7 == 6:
            self.set_reg(hi, self.byte())
            t = 12 if hi == 6 else 8
        elif opcode & 0xCF == 1:
            self.set_pair(group, self.word())
            t = 12
        elif opcode < 0x40 and lo in (4, 5):
            old = self.reg(hi)
            decrement = lo == 5
            value = old - 1 if decrement else old + 1
            self.set_reg(hi, value)
            self.flags(value, decrement, (old & 15) == (0 if decrement else 15), self.f & self.C)
            t = 12 if hi == 6 else 4
        elif opcode & 0xCF in (3, 0x0B):
            self.set_pair(group, self.pair(group) + (-1 if opcode & 8 else 1))
            t = 8
        elif opcode & 0xCF == 9:
            hl, rr = self.pair(2), self.pair(group)
            self.f = (self.f & self.Z) | (self.H if (hl & 4095) + (rr & 4095) > 4095 else 0) | (self.C if hl + rr > 65535 else 0)
            self.set_pair(2, hl + rr)
            t = 8
        elif opcode & 0xC7 == 2:
            addr = self.pair(min(2, group))
            if opcode & 8:
                self.r[7] = self.read(addr)
            else:
                self.write(addr, self.r[7])
            if group >= 2:
                self.set_pair(2, addr + (1 if group == 2 else -1))
            t = 8
        elif opcode in (0xFA, 0xEA, 0xF0, 0xE0):
            addr = (0xFF00 + self.byte()) if lo == 0 else self.word()
            if opcode & 0x10:
                self.r[7] = self.read(addr)
            else:
                self.write(addr, self.r[7])
            t = 12 if lo == 0 else 16
        elif opcode in (0xC3, 0xCD) or opcode & 0xE7 in (0xC2, 0xC4):
            dest = self.word()
            if opcode in (0xC3, 0xCD) or self.condition(hi & 3):
                call = opcode == 0xCD or lo == 4
                if call:
                    self.push(self.pc)
                self.pc = dest
                t = 24 if call else 16
            else:
                t = 12
        elif opcode == 0x18 or opcode & 0xE7 == 0x20:
            offset = self.byte()
            if opcode == 0x18 or self.condition(hi & 3):
                self.pc += offset if offset < 128 else offset - 256
                t = 12
            else:
                t = 8
        elif opcode == 0xC9 or opcode & 0xE7 == 0xC0:
            if opcode == 0xC9 or self.condition(hi & 3):
                self.pc = self.pop()
                t = 16 if opcode == 0xC9 else 20
            else:
                t = 8
        elif opcode & 0xCF in (0xC5, 0xC1):
            if lo == 5:
                self.push(self.pair(group) if group < 3 else self.r[7] << 8 | self.f)
                t = 16
            else:
                v = self.pop()
                if group == 3:
                    self.r[7], self.f = v >> 8, v & 0xF0
                else:
                    self.set_pair(group, v)
                t = 12
        elif opcode == 0x2F:
            self.r[7] ^= 255
            self.f |= self.N | self.H
        elif opcode == 0x37:
            self.f = (self.f & self.Z) | self.C
        elif opcode == 0x0F:
            val = self.r[7]
            self.r[7] = (val >> 1) | (val & 1) << 7
            self.f = self.C if val & 1 else 0
        elif opcode == 0xCB:
            op = self.byte()
            i, bit = op & 7, (op >> 3) & 7
            value = self.reg(i)
            if 0x40 <= op < 0x80:
                self.f = self.H | (self.f & self.C) | (0 if value & (1 << bit) else self.Z)
            elif op >> 3 in (4, 7):
                v = value << 1 if bit == 4 else value >> 1
                self.set_reg(i, v)
                self.flags(v, c=(value & 128) if bit == 4 else value & 1)
            else:
                raise RuntimeError(f"unimplemented CB {op:02x}")
            t = (12 if op >> 6 == 1 else 16) if i == 6 else 8
        else:
            raise RuntimeError(f"unimplemented {opcode:02x} at {self.pc-1:04x}")
        self.advance(t)

    def until(self, pc, limit=200000):
        for _ in range(limit):
            self.step()
            if self.pc == pc:
                return
        raise RuntimeError(f"frame timeout at {self.pc:04x}")

    def render(self):
        mem = np.frombuffer(self.mem, dtype=np.uint8)
        ctrl = mem[0xFF40].item()
        if not ctrl & 128:
            return Image.new("L", (160, 144), 255)
        raw = np.unpackbits(mem[0x8000:0x9800].reshape(-1, 8, 2, 1), axis=-1)
        tiles = raw[:, :, 0, :] + raw[:, :, 1, :] * 2
        yy, xx = np.indices((144, 160))

        def layer(addr, x, y):
            ids = mem[addr + ((y // 8) % 32) * 32 + (x // 8) % 32].astype(int)
            if not ctrl & 16:
                ids = 256 + ((ids + 128) % 256 - 128)
            return tiles[ids, y % 8, x % 8]

        bg = layer(0x9C00 if ctrl & 8 else 0x9800, xx + int(mem[0xFF43]), yy + int(mem[0xFF42]))
        if ctrl & 32:
            wx, wy = int(mem[0xFF4B]) - 7, int(mem[0xFF4A])
            window = layer(0x9C00 if ctrl & 64 else 0x9800, xx - wx, yy - wy)
            bg = np.where((xx >= wx) & (yy >= wy), window, bg)
        if not ctrl & 1:
            bg.fill(0)
        bgp = int(mem[0xFF47])
        out = ((bgp >> (2 * bg.astype(int))) & 3).astype(np.uint8)
        height = 16 if ctrl & 4 else 8
        oam = [(y - 16, x - 8, tile, attr) for y, x, tile, attr in
               struct.iter_unpack("BBBB", self.mem[0xFE00:0xFEA0])]
        self.peak_oam = max(self.peak_oam, sum(-height < y < 144 for y, *_ in oam))
        if ctrl & 2:
            for sy in range(144):
                eligible = [(x, i, y, t, a) for i, (y, x, t, a) in enumerate(oam) if y <= sy < y + height]
                self.peak_line = max(self.peak_line, len(eligible))
                # First ten in OAM order are evaluated. Smaller screen X wins;
                # at equal X, smaller OAM index wins. Transparent ink passes through.
                claimed = set()
                for x, i, y, tile, attr in sorted(eligible[:10]):
                    row = sy - y
                    if attr & 64:
                        row = height - 1 - row
                    if height == 16:
                        tile = (tile & 0xFE) + row // 8
                    palette = int(mem[0xFF49 if attr & 16 else 0xFF48])
                    for sx in range(max(0, x), min(160, x + 8)):
                        u = sx - x
                        ink = int(tiles[tile, row % 8, 7-u if attr & 32 else u])
                        if not ink or sx in claimed:
                            continue
                        claimed.add(sx)
                        if not (attr & 128 and bg[sy, sx]):
                            out[sy, sx] = (palette >> (2 * ink)) & 3
        return Image.fromarray(255 - 85 * out)

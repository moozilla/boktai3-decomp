"""Shared helpers: locate and verify the base ROM, read primitives."""
import hashlib
import os
import re
import struct
import sys

BASEROM_SHA1 = "2651c5e6875ac60abff734510d152166d211c87c"  # BOKTAI3 / U33J (Japan)
ROM_BASE = 0x08000000

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)


def baserom_path():
    p = os.environ.get("BOKTAI3_ROM", os.path.join(ROOT, "baserom.gba"))
    if not os.path.exists(p):
        sys.exit(f"Base ROM not found at {p}. Put the Japanese ROM there or set BOKTAI3_ROM.")
    return p


def load_rom(verify=True):
    data = open(baserom_path(), "rb").read()
    if verify:
        h = hashlib.sha1(data).hexdigest()
        if h != BASEROM_SHA1:
            sys.exit(f"Base ROM SHA-1 mismatch: {h} (expected {BASEROM_SHA1})")
    return data


def u8(b, o): return b[o]
def u16(b, o): return struct.unpack_from("<H", b, o)[0]
def u32(b, o): return struct.unpack_from("<I", b, o)[0]


def is_rom_ptr(v, rom_len=0x1000000):
    return ROM_BASE <= v < ROM_BASE + rom_len


def off(addr):
    """GBA bus address -> ROM file offset."""
    return addr - ROM_BASE if addr >= ROM_BASE else addr


def rom_data_ranges(func, addr):
    """Data ranges of the ROM function, from its generated assembly (labels carry addresses)."""
    path = os.path.join(ROOT, "build", "asm", "nonmatching", func + ".s")
    if not os.path.exists(path):
        return None
    data, cur = [], addr
    blobs = []  # (start, end) of .incbin runs: raw bytes the pool scan must classify
    for ln in open(path):
        s = ln.split("@")[0].strip()
        m = re.match(r"(\w+):\s*(.*)", s)
        if m:
            lab = re.search(r"_([0-9A-F]{8})$", m.group(1))
            if lab:
                cur = int(lab.group(1), 16)
            s = m.group(2).strip()
        if not s or s.startswith((".global", ".syntax", "thumb_func", "arm_func", ".thumb", ".type", ".size")):
            continue
        if s.startswith(".4byte") or s.startswith(".word"):
            data.append((cur - addr, cur - addr + 4)); cur += 4
        elif s.startswith(".2byte") or s.startswith(".hword"):
            data.append((cur - addr, cur - addr + 2)); cur += 2
        elif s.startswith(".byte"):
            data.append((cur - addr, cur - addr + 1)); cur += 1
        elif s.startswith(".align"):
            n = 1 << int(s.split()[1].rstrip(","))
            pad = (-cur) % n
            if pad:
                data.append((cur - addr, cur - addr + pad))
            cur += pad
        elif s.startswith(".incbin"):
            m = re.search(r",\s*(0x[0-9A-Fa-f]+|\d+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)", s)
            if m:
                n = int(m.group(2), 0)
                blobs.append((cur - addr, cur - addr + n)); cur += n
        elif s.startswith("."):
            continue
        else:
            cur += 4 if re.match(r"bl\b", s) else 2
    return data, blobs

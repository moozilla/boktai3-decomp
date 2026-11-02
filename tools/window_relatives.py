#!/usr/bin/env python3
"""Find clean C donors for a short Thumb instruction window.

Usage: build/venv/bin/python tools/window_relatives.py 0810D43A 0810D44C

The end address is exclusive. Candidates come from the concession-free functions
in PROGRESS.json. The linked ELF's mapping symbols exclude data pools and ARM
code. The score is a search heuristic; verify any proposed C with the stock compiler.
"""
import argparse
import array
import subprocess
from bisect import bisect_right
from dataclasses import dataclass
import json
from pathlib import Path
import re
import sys

from romlib import ROOT, ROM_BASE, load_rom


def opcode(h):
    """Thumb-1 operation with local registers and most immediates abstracted."""
    if h < 0x1800:
        return h & 0xf800
    if h < 0x2000:
        return h & 0xfe00
    if h < 0x4000:
        return h & 0xf800
    if h < 0x4400:
        return h & 0xffc0
    if h < 0x4800:
        return h & 0xff00
    if 0x5000 <= h < 0x6000:
        return h & 0xfe00
    if h < 0xb000:
        return h & 0xf800
    if 0xc000 <= h < 0xd000 or h >= 0xe000:
        return h & 0xf800
    return h & 0xff00


@dataclass(frozen=True)
class Function:
    address: int
    end: int
    name: str
    file: str
    clean: bool


def read_functions(report):
    funcs = []
    for category in report["categories"]:
        for unit in category["units"]:
            for name, address, size, score, pinned, file_index, *_ in unit["functions"]:
                start = int(address, 16)
                file = report["files"][file_index] if file_index >= 0 else f"src/fn/sub_{address}.c"
                funcs.append(Function(start, start + size, name, file, score == 100 and not pinned))
    return sorted(funcs, key=lambda f: f.address)


def thumb_intervals(elf_path, end):
    listing = subprocess.check_output(
        ["arm-none-eabi-nm", "--special-syms", "-an", str(elf_path)], text=True
    )
    markers = []
    for line in listing.splitlines():
        marker = re.fullmatch(r"([0-9a-fA-F]+) [tT] \$([tda])(?:\..*)?", line)
        if marker:
            addr = int(marker[1], 16)
            if ROM_BASE <= addr < end:
                markers.append((addr, marker[2]))
    intervals = []
    for j, (addr, mode) in enumerate(markers):
        stop = markers[j + 1][0] if j + 1 < len(markers) else end
        if mode == "t" and addr < stop:
            if intervals and intervals[-1][1] == addr:
                intervals[-1] = (intervals[-1][0], stop)
            else:
                intervals.append((addr, stop))
    return intervals


class Windows:
    def __init__(self, rom, funcs, intervals):
        self.funcs = funcs
        self.starts = [f.address for f in funcs]
        self.intervals = intervals
        self.code_starts = [lo for lo, _ in intervals]
        size = max(f.end for f in funcs) - ROM_BASE
        self.raw = array.array("H", rom[:size - size % 2])
        if sys.byteorder == "big":
            self.raw.byteswap()
        self.ops = array.array("H", map(opcode, self.raw))
        self.searchable = self.ops.tobytes()

    def bounds(self, address, end):
        i = bisect_right(self.starts, address) - 1
        j = bisect_right(self.code_starts, address) - 1
        if i < 0 or j < 0:
            return None
        f = self.funcs[i]
        lo, hi = self.intervals[j]
        if end > min(f.end, hi):
            return None
        return f, (max(f.address, lo) - ROM_BASE) // 2, (min(f.end, hi) - ROM_BASE) // 2

    def rank(self, start, end):
        if start & 1 or end & 1 or end - start < 8:
            raise ValueError("use halfword-aligned addresses covering at least four halfwords")
        target = self.bounds(start, end)
        if target is None:
            raise ValueError("the target must lie inside one Thumb code interval and function")
        src, lo, hi = target
        first = (start - ROM_BASE) // 2
        length = (end - start) // 2
        seed = min(7, length)
        if self.raw[first] >> 11 == 31 and first > lo and self.raw[first - 1] >> 11 == 30:
            raise ValueError("start at the first halfword of a BL")
        if self.raw[first + length - 1] >> 11 == 30:
            raise ValueError("include both halfwords of the final BL")
        candidates = set()
        for shift in range(length - seed + 1):
            prefix = self.ops[first + shift:first + shift + seed].tobytes()
            offset = self.searchable.find(prefix)
            while offset >= 0:
                if offset % 2 == 0:
                    candidates.add(offset // 2 - shift)
                offset = self.searchable.find(prefix, offset + 1)
        best = {}
        for p in candidates:
            address = ROM_BASE + 2 * p
            dest = self.bounds(address, address + 2 * length)
            if dest is None:
                continue
            f, low, high = dest
            if not f.clean or f.address == src.address:
                continue
            # A two-halfword BL can only start at its prefix.
            if self.raw[p] >> 11 == 31 and p > low and self.raw[p - 1] >> 11 == 30:
                continue
            ops = sum(a == b for a, b in zip(self.ops[first:first+length], self.ops[p:p+length]))
            raw = sum(a == b for a, b in zip(self.raw[first:first+length], self.raw[p:p+length]))
            flank = 0
            for delta, step in ((-1, -1), (length, 1)):
                for _ in range(24):
                    if not (lo <= first + delta < hi and low <= p + delta < high):
                        break
                    if self.ops[first + delta] != self.ops[p + delta]:
                        break
                    flank += 1
                    delta += step
            quality = (2 * ops + raw) / (3 * length)
            score = quality + 0.007 * flank
            hit = (score, quality, flank, -address, f)
            if f.address not in best or hit[:4] > best[f.address][:4]:
                best[f.address] = hit
        return sorted(best.values(), key=lambda hit: hit[:4], reverse=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    for arg in ("start", "end"):
        ap.add_argument(arg, type=lambda v: int(v, 16))
    ap.add_argument("-n", type=int, default=5)
    ap.add_argument("--elf", type=Path, default=Path(ROOT, "build/boktai3.elf"))
    ap.add_argument("--report", type=Path, default=Path(ROOT, "PROGRESS.json"))
    args = ap.parse_args()
    try:
        funcs = read_functions(json.loads(args.report.read_text()))
        matches = Windows(load_rom(), funcs, thumb_intervals(args.elf, max(f.end for f in funcs))).rank(args.start, args.end)
    except (OSError, ValueError, subprocess.CalledProcessError) as exc:
        ap.error(str(exc))
    for _, quality, flank, negative_addr, f in matches[:args.n]:
        print(f"{-negative_addr:08X}  {quality:6.1%}  {flank:2} flank  {f.name}  {f.file}")


if __name__ == "__main__":
    main()

"""Check an entire matched translation unit after formatting it in place."""
import os
import subprocess
import tempfile
from pathlib import Path

import build
import check
from romlib import ROOT, off


def unit_matches(code, start, offsets, expected, rom):
    """Check the ROM extent and each entry point, including adjacent identical bodies."""
    if not expected or any(name not in offsets or start + offsets[name] != address
                           for name, (address, size) in expected.items()):
        return False
    end = max(address + size for address, size in expected.values())
    return len(code) == end - start and code == rom[off(start):off(end)]


def stock_match(src, expected, symbols, rom):
    """Link the stock compiler's object once. Fall back to per-function diagnosis on failure."""
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as work:
        obj = os.path.join(work, "unit.o")
        elf = os.path.join(work, "unit.elf")
        binary = os.path.join(work, "unit.bin")
        try:
            build.build_c(src, obj)
            start, _ = check.link_object(obj, elf, syms=symbols)
            offsets = {}
            for line in build.sh(["arm-none-eabi-nm", "--defined-only", obj]).splitlines():
                fields = line.split()
                if len(fields) == 3 and fields[1] in ("T", "t"):
                    offsets[fields[2]] = int(fields[0], 16) & ~1
            build.sh(["arm-none-eabi-objcopy", "-O", "binary", "-j", ".text", elf, binary])
            return unit_matches(Path(binary).read_bytes(), start, offsets, expected, rom)
        except (OSError, SystemExit, subprocess.SubprocessError):
            return False

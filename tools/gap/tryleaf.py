#!/usr/bin/env python3
"""tryleaf.py FILE.c...: each file defines sub_ADDR (a push-less gap leaf). Compute the boundary
(code through first bx lr + its literal pool), try old_agbcc then agbcc, keep matches in src/fn/."""
import re, struct, subprocess, sys, os, shutil
rom = open('baserom.gba', 'rb').read()
def u16(o): return struct.unpack('<H', rom[o:o + 2])[0]
for f in sys.argv[1:]:
    a = int(re.search(r'sub_([0-9A-F]{8})', f).group(1), 16)
    o = a - 0x08000000
    k = 0
    pools = []
    while u16(o + k) != 0x4770:
        w = u16(o + k)
        if (w & 0xF800) == 0x4800:
            pools.append(((o + k + 4) & ~3) + (w & 0xFF) * 4)
        k += 2
    end = o + k + 2
    if pools: end = max(end, max(pools) + 4)
    if end % 4 and u16(end) == 0: end += 2
    end += 0x08000000
    body = open(f).read()
    dst = 'src/fn/sub_%08X.c' % a
    ok = None
    for comp in ('// COMPILER: old_agbcc\n', ''):
        open(dst, 'w').write(comp + body)
        r = subprocess.run(['build/venv/bin/python', 'tools/check.py', dst], capture_output=True, text=True)
        if 'MISMATCH' not in r.stdout and 'MATCH (' in r.stdout:
            ok = comp; break
    if ok is None:
        os.remove(dst); print('%08X fail' % a); continue
    subprocess.run(['tools/gap/addgap.py', '%08X' % a, '%08X' % end, 'Independent %d-byte leaf left as incbin by gbadisasm' % (end - a)], capture_output=True)
    print('%08X ok %s end=%08X' % (a, ok.strip() or 'agbcc', end))

#!/usr/bin/env python3
"""autoleaf.py: generate C for trivial push-less gap leaves, keep exact matches, register boundaries."""
import re, struct, subprocess, sys
lines = open('gen/code_sym.s').read().split('\n')
rom = open('baserom.gba', 'rb').read()
def known():
    s = set()
    for f in ('symbols/build_extra_functions.csv', 'symbols/progress_extra_functions.csv'):
        s |= set(l.split(',')[0] for l in open(f).read().splitlines()[1:])
    return s
K = known()
def u16(o): return struct.unpack('<H', rom[o:o + 2])[0]
def u32(o): return struct.unpack('<I', rom[o:o + 4])[0]
results = []
for i, l in enumerate(lines[:-1]):
    m = re.match(r'^_(08[0-9A-F]{6}):$', l)
    if not (m and lines[i + 1].strip().startswith('.incbin "baserom.gba"') and m.group(1) not in K):
        continue
    a = int(m.group(1), 16); o = a - 0x08000000
    gap_sz = int(lines[i + 1].split(',')[2], 16)
    gap_end = a + gap_sz
    c = None; end = None
    w0 = u16(o)
    if w0 == 0x4770:  # bx lr
        end = a + 4 if a % 4 == 0 and u16(o + 2) == 0 and gap_sz >= 4 else a + 2
        c = '#include "global.h"\n\nvoid sub_%08X(void)\n{\n}\n' % a
        comps = ['']
    elif (w0 & 0xF800) == 0x4800 and ((w0 >> 8) & 7) == 1 and (u16(o + 2) & 0xFF00) == 0x2000 and u16(o + 4) == 0x6008 and u16(o + 6) == 0x4770:
        pool = ((o + 4) & ~3) + (w0 & 0xFF) * 4
        g = u32(pool); n = u16(o + 2) & 0xFF
        if not (0x02000000 <= g < 0x04000000): continue
        end = 0x08000000 + pool + 4
        c = '#include "global.h"\n\nextern u32 gUnk_%08X;\n\nvoid sub_%08X(void)\n{\n    gUnk_%08X = %d;\n}\n' % (g, a, g, n)
        comps = ['// COMPILER: old_agbcc\n', '']
    elif (w0 & 0xFF00) == 0x2000 and u16(o + 2) == 0x4770:
        end = a + 4
        c = '#include "global.h"\n\ns32 sub_%08X(void)\n{\n    return %d;\n}\n' % (a, w0 & 0xFF)
        comps = ['// COMPILER: old_agbcc\n', '']
    if c is None or end > gap_end:
        continue
    ok = False
    for comp in comps:
        path = 'src/fn/sub_%08X.c' % a
        open(path, 'w').write(comp + c)
        r = subprocess.run(['build/venv/bin/python', 'tools/check.py', path], capture_output=True, text=True)
        if 'MISMATCH' not in r.stdout and 'MATCH (' in r.stdout:
            ok = True; break
    if ok:
        subprocess.run(['tools/gap/addgap.py', '%08X' % a, '%08X' % end, 'Independent %d-byte leaf left as incbin by gbadisasm' % (end - a)], capture_output=True)
        results.append(('%08X' % a, end - a, 'ok', comp.strip()))
    else:
        import os; os.remove(path)
        results.append(('%08X' % a, end - a, 'fail', ''))
for r in results: print(*r)

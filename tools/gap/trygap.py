#!/usr/bin/env python3
"""trygap.py FILE.c...: push-style gap functions; boundary = gap end or next push after bx; try agbcc; keep matches."""
import re, struct, subprocess, sys, os
rom = open('baserom.gba', 'rb').read()
lines = open('gen/code_sym.s').read().split('\n')
ends = {}
for i, l in enumerate(lines[:-1]):
    m = re.match(r'^_(08[0-9A-F]{6}):$', l)
    if m and lines[i + 1].strip().startswith('.incbin'):
        j = i + 1
        while j < len(lines) and 'func_start' not in lines[j]: j += 1
        nm = re.search(r'(08[0-9A-F]{6})', lines[j + 1])
        ends[int(m.group(1), 16)] = int(nm.group(1), 16)
def u16(o): return struct.unpack('<H', rom[o:o + 2])[0]
for f in sys.argv[1:]:
    a = int(re.search(r'sub_([0-9A-F]{8})', f).group(1), 16)
    gend = None
    for s, e in ends.items():
        if s <= a < e: gend = e
    if gend is None: print('%08X no gap' % a); continue
    # function end: next push (b5xx/b4xx at 4-aligned... any halfword) after the first return, else gap end
    end = gend
    o = a - 0x08000000 + 2
    seen_ret = False
    while o < gend - 0x08000000:
        w = u16(o)
        if w in (0x4700, 0x4708, 0x4710, 0x4718, 0x4770): seen_ret = True
        elif seen_ret and (w & 0xFF00) in (0xB500, 0xB400) and o % 4 == 0:
            end = o + 0x08000000; break
        o += 2
    dst = 'src/fn/sub_%08X.c' % a
    open(dst, 'w').write(open(f).read())
    r = subprocess.run(['build/venv/bin/python', 'tools/check.py', dst], capture_output=True, text=True)
    res = (r.stdout + r.stderr).strip().splitlines()
    if 'MISMATCH' not in r.stdout and 'MATCH (' in r.stdout:
        subprocess.run(['tools/gap/addgap.py', '%08X' % a, '%08X' % end, 'Independent function (own push/pop-bx) left as incbin by gbadisasm'], capture_output=True)
        print('%08X ok end=%08X' % (a, end))
    else:
        os.remove(dst)
        fz = [l for l in res if 'fuzzy' in l or 'rror' in l]
        print('%08X fail %s' % (a, fz[-1][:80] if fz else res[-1][:80]))

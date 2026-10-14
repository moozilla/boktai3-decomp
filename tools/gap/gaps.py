#!/usr/bin/env python3
"""gaps.py list | gaps.py dis ADDR [END]: incbin gap functions (label + incbin, starts with push)."""
import re, sys, subprocess
lines = open('gen/code_sym.s').read().split('\n')
rom = open('baserom.gba', 'rb').read()
def known():
    s = set()
    for f in ('symbols/build_extra_functions.csv', 'symbols/progress_extra_functions.csv'):
        s |= set(l.split(',')[0] for l in open(f).read().splitlines()[1:])
    return s
def gaps():
    k = known()
    out = []
    for i, l in enumerate(lines[:-1]):
        m = re.match(r'^_(08[0-9A-F]{6}):$', l)
        if m and lines[i + 1].strip().startswith('.incbin "baserom.gba"') and m.group(1) not in k:
            a = int(m.group(1), 16)
            if rom[a - 0x08000000 + 1] in (0xb5, 0xb4):
                j = i + 1
                while j < len(lines) and 'func_start' not in lines[j]:
                    j += 1
                nxt = re.search(r'(08[0-9A-F]{6})', lines[j + 1]) if j + 1 < len(lines) else None
                out.append((a, int(nxt.group(1), 16) if nxt else a + 4))
    return out
if sys.argv[1] == 'list':
    for a, e in gaps():
        print('%08X %d' % (a, e - a))
else:
    a = int(sys.argv[2], 16)
    e = int(sys.argv[3], 16) if len(sys.argv) > 3 else dict(gaps()).get(a, a + 0x100)
    open('build/gap.bin', 'wb').write(rom[a - 0x08000000:e - 0x08000000])
    out = subprocess.run(['arm-none-eabi-objdump', '-D', '-b', 'binary', '-m', 'arm', '-M', 'force-thumb',
                          '--adjust-vma=0x%08X' % a, 'build/gap.bin'], capture_output=True, text=True).stdout
    print('\n'.join(out.split('\n')[7:]))

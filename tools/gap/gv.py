#!/usr/bin/env python3
"""gv.py ADDR...: compact disassembly (to gap end) with pool words resolved."""
import re, sys, struct, subprocess
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
for arg in sys.argv[1:]:
    a = int(arg, 16); e = ends.get(a, a + 0x80)
    open('build/gv.bin', 'wb').write(rom[a - 0x08000000:e - 0x08000000])
    dis = subprocess.run(['arm-none-eabi-objdump', '-D', '-b', 'binary', '-m', 'arm', '-M', 'force-thumb',
                          '--adjust-vma=0x%08X' % a, 'build/gv.bin'], capture_output=True, text=True).stdout
    out = []
    for d in dis.split('\n')[7:]:
        mm = re.match(r'\s*([0-9a-f]+):\t[0-9a-f ]+\t(.*)', d)
        if not mm: continue
        t = mm.group(2).replace('\t', ' ')
        pm = re.search(r'@ \(0x([0-9a-f]+)\)', t)
        if pm:
            pa = int(pm.group(1), 16) - 0x08000000
            t = t.split('@')[0].strip() + ' =0x%08X' % struct.unpack('<I', rom[pa:pa + 4])[0]
        t = re.sub(r'\s*@ 0x[0-9a-f]+$', '', t)
        out.append(mm.group(1)[-4:] + ' ' + t)
    print('=== %s (%d)' % (arg, e - a)); print('\n'.join(out))

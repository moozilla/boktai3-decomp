#!/usr/bin/env python3
"""Register boundaries for bytes left over after reviewed gap functions (keeps C units the right size)."""
import re, csv
lines = open('gen/code_sym.s').read().split('\n')
gaps = {}
for i, l in enumerate(lines[:-1]):
    m = re.match(r'^_(08[0-9A-F]{6}):$', l)
    if m and lines[i + 1].strip().startswith('.incbin "baserom.gba"'):
        j = i + 1
        while j < len(lines) and 'func_start' not in lines[j]:
            j += 1
        nm = re.search(r'(08[0-9A-F]{6})', lines[j + 1]) if j + 1 < len(lines) else None
        if nm: gaps[int(m.group(1), 16)] = int(nm.group(1), 16)
def add(path, row, a):
    ls = open(path).read().splitlines()
    if any(l.split(',')[0] == a for l in ls[1:]): return
    body = ls[1:] + [row]; body.sort(key=lambda l: l.split(',')[0])
    open(path, 'w').write('\n'.join([ls[0]] + body) + '\n')
changed = True
while changed:
    changed = False
    rows = list(csv.DictReader(open('symbols/build_extra_functions.csv')))
    starts = set(int(r['address'], 16) for r in rows)
    for r in rows:
        if not r['replacement_end']: continue
        a = int(r['address'], 16); e = int(r['replacement_end'], 16)
        g = [s for s in gaps if s <= a < gaps[s]]
        if not g: continue
        gend = gaps[max(g)]
        nxt = min([s for s in starts if s > a] + [gend])
        if e < nxt and e not in starts:
            ea = '%08X' % e; en = '%08X' % nxt
            add('symbols/build_extra_functions.csv', f'{ea},thumb,Code following a reviewed gap function (own boundary so the function can be replaced); retained assembly,{en}', ea)
            add('symbols/progress_extra_functions.csv', f'{ea},Code following a reviewed gap function; retained assembly', ea)
            print('registered', ea, en); changed = True
            break

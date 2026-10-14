#!/usr/bin/env python3
"""addgap.py ADDR END "description": register a reviewed gap function boundary in both csv files."""
import sys
a, e, desc = sys.argv[1].upper(), sys.argv[2].upper(), sys.argv[3]
def ins(path, row):
    lines = open(path).read().splitlines()
    hdr, body = lines[0], lines[1:]
    body = [l for l in body if l.split(',')[0] != a]
    body.append(row)
    body.sort(key=lambda l: l.split(',')[0])
    open(path, 'w').write('\n'.join([hdr] + body) + '\n')
ins('symbols/build_extra_functions.csv', f'{a},thumb,{desc}; exact standalone C,{e}')
ins('symbols/progress_extra_functions.csv', f'{a},{desc}; exact standalone C')

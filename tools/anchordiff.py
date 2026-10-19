#!/usr/bin/env python3
"""Realign a big function's ROM/candidate listing and print only the hunks that differ.

  anchordiff.py [RUN_DIR] [CONTEXT] [-s] [--range ROM_LO,ROM_HI,YOU_LO,YOU_HI]

RUN_DIR is a `tools/check.py FILE --full` artifact directory (default: the newest
build/check/run-*). check.py's side-by-side listing pairs instructions by position,
so after the first inserted or deleted instruction in a several-KB function every
later line reads as different. This script re-pairs the two columns:

* the listing is cut at calls that both sides make in the same order (matched by
  callee address with difflib), so a function made of near-identical inlined
  copies cannot be aligned copy-against-the-wrong-copy;
* each piece is aligned with difflib on normalised instructions (branch targets
  and literal-pool offsets dropped);
* `-s` also masks registers and stack offsets, so only structural differences
  (instruction choice, order, count) are reported and allocation is ignored.

The last line is the number of differing instructions. Use it as a score when
trying C variants: it is stable where check.py's byte count and fuzzy score jump.
"""
import difflib
import glob
import os
import re
import sys


def read_columns(run_dir):
    rom, you = [], []
    for line in open(os.path.join(run_dir, 'full.diff')):
        line = line.rstrip('\n')
        if '|' not in line or line.startswith('@@'):
            continue
        a, _, b = line[2:].partition('|')
        a, b = a.strip(), b.strip()
        if a:
            rom.append(a)
        if b:
            you.append(b)
    return rom, you


def normalise(insn, mask):
    x = insn.split(' ', 1)[1] if ' ' in insn else insn
    x = re.sub(r'^(b\w*|bl) #0x\w+', r'\1', x)
    x = re.sub(r'\[pc, #0x\w+\]', '[pc]', x)
    if mask:
        x = re.sub(r'\b(r\d+|sb|sl|fp|ip|lr)\b', 'R', x)
        x = re.sub(r'\[sp, #0x\w+\]', '[sp]', x)
        x = re.sub(r'sp, #0x\w+', 'sp, #N', x)
    return x


def call_targets(listing, lo, hi):
    """(index, target) of calls leaving the function's address range."""
    out = []
    for k, insn in enumerate(listing):
        m = re.match(r'\w+ bl #0x(\w+)', insn)
        if m and not lo <= int(m.group(1), 16) < hi:
            out.append((k, m.group(1)))
    return out


def anchors(rom, you):
    lo = int(rom[0].split()[0], 16) if rom else 0
    hi = int(rom[-1].split()[0], 16) + 2 if rom else 0
    a, b = call_targets(rom, lo, hi), call_targets(you, lo, hi)
    sm = difflib.SequenceMatcher(None, [t for _, t in a], [t for _, t in b], autojunk=False)
    pairs = []
    for blk in sm.get_matching_blocks():
        pairs += [(a[blk.a + k][0], b[blk.b + k][0]) for k in range(blk.size)]
    return pairs


def main(argv):
    mask = '-s' in argv
    rng = None
    rest = []
    it = iter(argv)
    for arg in it:
        if arg == '-s':
            continue
        if arg == '--range':
            rng = [int(v, 16) for v in next(it).split(',')]
            continue
        rest.append(arg)
    run_dir = rest[0] if rest else max(glob.glob('build/check/run-*'), key=os.path.getmtime)
    ctx = int(rest[1]) if len(rest) > 1 else 2
    rom, you = read_columns(run_dir)
    if rng:
        rom = [x for x in rom if rng[0] <= int(x.split()[0], 16) < rng[1]]
        you = [x for x in you if rng[2] <= int(x.split()[0], 16) < rng[3]]
    nrom = [normalise(x, mask) for x in rom]
    nyou = [normalise(x, mask) for x in you]
    cuts = anchors(rom, you)
    starts = [(0, 0)] + cuts
    ends = cuts + [(len(rom), len(you))]
    total = 0
    for (a0, b0), (a1, b1) in zip(starts, ends):
        sm = difflib.SequenceMatcher(None, nrom[a0:a1], nyou[b0:b1], autojunk=False)
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            if tag == 'equal':
                continue
            i1, i2, j1, j2 = i1 + a0, i2 + a0, j1 + b0, j2 + b0
            total += max(i2 - i1, j2 - j1)
            where = rom[i1].split()[0] if i1 < len(rom) else 'end'
            print(f'--- ROM {where}  ({tag} {i2 - i1}/{j2 - j1})')
            for k in range(max(i1 - ctx, 0), i1):
                print('   ', rom[k])
            for k in range(i1, i2):
                print(' R ', rom[k])
            for k in range(j1, j2):
                print(' Y ', you[k])
    print('TOTAL differing insns', total)


if __name__ == '__main__':
    main(sys.argv[1:])

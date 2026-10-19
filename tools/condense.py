#!/usr/bin/env python3
"""Fold the repeated instruction runs of a big function into one line each.

  condense.py ADDR [--min-len 6] [--min-count 3] [--max-len 120] [--no-defs]

A big function is usually a few routines inlined many times (08123194 holds 152
copies of one terrain-cell lookup). Reading it is far easier once each copy is
one line: this prints the function's listing (as tools/asmat.py shows it) with
every run that recurs, registers and branch targets abstracted, replaced by
`RUN_n  r2,r5 -> r0` (the run's registers in order of first appearance), and
the definition of each run (its first occurrence) on top. Runs are chosen
greedily by instructions saved, and runs of runs fold too, so a loop body that
inlines a lookup becomes one line of its own.

docs/BIG_FUNCTIONS.md describes the method this belongs to. A run is a hint,
not proof that the copies came from one inline: compare the definitions with
matched code (`tools/relatives.py ADDR --c`, `tools/idioms.py --shape ...`).
"""
import argparse
import re
import subprocess
import sys
from collections import defaultdict

from romlib import ROOT

REG = re.compile(r'\b(r\d+|sb|sl|fp|ip)\b')
INSN = re.compile(r'^\t(\w[\w.]*)\s*(.*?)\s*(?:@.*)?$')


def listing(addr):
    out = subprocess.run([sys.executable, f'{ROOT}/tools/asmat.py', addr, '100000'],
                         capture_output=True, text=True, check=True).stdout.split('\n')
    lines = []
    starts = 0
    for ln in out:
        if re.match(r'^\t(?:thumb|arm|non_word_aligned_thumb)_func_start', ln):
            starts += 1
            if starts == 2:
                break
        lines.append(ln)
    return lines


def abstract(ln):
    """Token for one listing line, or None for lines that are not code."""
    if re.match(r'^\w+:', ln) or re.match(r'^_[0-9A-F]{8}:', ln):
        return 'LABEL'
    m = INSN.match(ln)
    if not m or m.group(1).startswith('.') or m.group(1).endswith('_func_start') or m.group(1) == 'thumb_func_end':
        return None
    op, args = m.group(1), m.group(2)
    if op in ('b', 'bl') or (op.startswith('b') and len(op) == 3 and op not in ('bic', 'bkpt')):
        if op == 'bl':
            return f'bl {args}'
        return op + ' L'
    if op == 'ldr' and re.search(r', (?:_|=|\w+\b(?<!sp)$)', args) and '[' not in args:
        args = REG.sub('R', args.split(',')[0]) + ', POOL'
    return op + ' ' + REG.sub('R', args)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('addr')
    ap.add_argument('--min-len', type=int, default=6)
    ap.add_argument('--min-count', type=int, default=3)
    ap.add_argument('--max-len', type=int, default=120)
    ap.add_argument('--no-defs', action='store_true')
    a = ap.parse_args()

    lines = listing(a.addr)
    # items: (token, [original lines])
    items = []
    for ln in lines:
        t = abstract(ln)
        if t is None:
            continue
        items.append((t, [ln]))
    total = sum(1 for t, _ in items if t != 'LABEL')
    runs = []
    while True:
        toks = [t for t, _ in items]
        best = None
        for n in range(a.min_len, min(a.max_len, len(toks)) + 1):
            where = defaultdict(list)
            for i in range(len(toks) - n + 1):
                win = tuple(toks[i:i + n])
                if win[0] == 'LABEL' or win[-1] == 'LABEL':
                    continue
                w = where[win]
                if not w or w[-1] + n <= i:
                    w.append(i)
            found = False
            for win, pos in where.items():
                if len(pos) < a.min_count:
                    continue
                found = True
                gain = (n - 1) * len(pos)
                if not best or gain > best[0]:
                    best = (gain, n, pos)
            if not found:
                break
        if not best:
            break
        gain, n, pos = best
        name = f'RUN_{len(runs)}'
        runs.append((name, n, len(pos), [ln for _, ls in items[pos[0]:pos[0] + n] for ln in ls]))
        for i in reversed(pos):
            body = [ln for _, ls in items[i:i + n] for ln in ls]
            items[i:i + n] = [(name, body)]

    def regs(body):
        seen = []
        for ln in body:
            m = INSN.match(ln)
            if m:
                for r in REG.findall(m.group(2)):
                    if r not in seen:
                        seen.append(r)
        return ','.join(seen)

    if not a.no_defs:
        for name, n, count, body in runs:
            print(f'{name}: {n} items, {count} copies')
            for ln in body:
                print('   ', ln)
            print()
    left = 0
    for t, body in items:
        if t.startswith('RUN_'):
            print(f'\t{t}  {regs(body)}')
        else:
            left += t != 'LABEL'
            print(body[0])
    print(f'@ {total} instructions; {len(items)} lines after folding {len(runs)} runs ({left} instructions left unfolded)')


if __name__ == '__main__':
    main()

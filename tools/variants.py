#!/usr/bin/env python3
"""Score a batch of hand-written C edits against the ROM, ranked by realigned diff.

  variants.py FILE.c VARIANTS.txt [--nonmatching] [-j N] [--greedy] [--write]
                                  [--range ROM_LO,ROM_HI,YOU_LO,YOU_HI]
  variants.py FILE.c --decl-swaps [...]

The hypothesis-driven loop that took sub_08123194 from 4444 differing
instructions to 19 (docs/PERMUTER.md, "Big functions"): write each idea as a
small textual edit, compile them all, and keep what lowers the score. Every
variant is the file with its edits applied, checked with tools/check.py and
scored with tools/anchordiff.py: `struct` counts differing instructions with
registers and stack offsets masked (instruction choice, order, count), `total`
counts every differing instruction. Lower is better; the byte count is shown
last because it jumps on big functions.

VARIANTS.txt holds blocks separated by a line `---`. A block is one or more
edits separated by a line `+++`; an edit is OLD text, a line `===`, NEW text.
OLD must occur exactly once in the file (the edits of one block apply in
order). An optional first line `## NAME` names the block.

    ## hoist the mask
    u8 flags = *p;
    ===
    u32 flags = *p;
    ---
    if (!(gUnk_030053F4 & 0x800)) {
    ===
    if (gUnk_030053F4 & 0x800) {

--decl-swaps generates the variants instead: every pair of adjacent local
declarations swapped (declaration order steers register allocation).
--greedy then applies the improving variants one at a time, best first,
keeping each that still improves the combination; --write saves the result
to FILE.c, otherwise to build/variants/<stem>/best.c.
Variant sources and check.py artifacts stay under build/variants/<stem>/.
"""
import argparse
import concurrent.futures
import json
import os
import re
import subprocess
import sys

import anchordiff
from romlib import ROOT

DECL = re.compile(r'^\s+(?:(?:const|volatile|register|static|signed|unsigned|struct|union|enum)\s+)*'
                  r'[A-Za-z_]\w*(?:\s+\w+)?[\s*]+\w+(?:\[[^\]]*\])?(?:\s*=[^;]*)?;\s*(?://.*)?$')


def parse(text):
    out = []
    for k, block in enumerate(re.split(r'^---[ \t]*$\n?', text, flags=re.M)):
        if not block.strip():
            continue
        name = f'v{k}'
        m = re.match(r'##[ \t]*(.*)\n', block)
        if m:
            name, block = m.group(1).strip(), block[m.end():]
        edits = []
        for edit in re.split(r'^\+\+\+[ \t]*$\n?', block, flags=re.M):
            parts = re.split(r'^===[ \t]*$\n?', edit, maxsplit=1, flags=re.M)
            if len(parts) != 2:
                if edit.strip():
                    sys.exit(f'{name}: an edit without a === line')
                continue
            edits.append((parts[0].rstrip('\n'), parts[1].rstrip('\n')))
        out.append((name, edits))
    return out


def decl_swaps(src):
    lines = src.split('\n')
    out = []
    for i in range(len(lines) - 1):
        a, b = lines[i], lines[i + 1]
        if DECL.match(a) and DECL.match(b) and not a.lstrip().startswith('return') and '=' not in a + b:
            out.append((f'swap line {i + 1}/{i + 2}: {b.strip()} first', [(a + '\n' + b, b + '\n' + a)]))
    return out


def apply(src, edits):
    for old, new in edits:
        n = src.count(old)
        if n != 1:
            raise ValueError(f'OLD occurs {n} times: {old.splitlines()[0][:60]!r}')
        src = src.replace(old, new)
    return src


def check(path, nonmatching, rng):
    cmd = [sys.executable, os.path.join(ROOT, 'tools', 'check.py'), path, '--json', '--quiet']
    if nonmatching:
        cmd.append('--nonmatching')
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    lines = [ln for ln in r.stdout.splitlines() if ln.startswith('{')]
    if not lines:
        return None, (r.stderr.strip().splitlines() or ['compile failed'])[-1]
    res = json.loads(lines[-1])
    if res['status'] == 'match':
        return (0, 0, 0), 'MATCH'
    s, t = anchordiff.score(os.path.join(ROOT, res['artifacts']), rng)
    return (s, t, res['different_byte_count']), f"fuzzy {res['fuzzy_match_percent']:.1f}%"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('src')
    ap.add_argument('variants', nargs='?')
    ap.add_argument('--decl-swaps', action='store_true')
    ap.add_argument('--nonmatching', action='store_true')
    ap.add_argument('-j', type=int, default=os.cpu_count() or 4)
    ap.add_argument('--greedy', action='store_true')
    ap.add_argument('--write', action='store_true')
    ap.add_argument('--range', help='score only these ROM/your address ranges (anchordiff --range)')
    a = ap.parse_args()
    if not a.variants and not a.decl_swaps:
        ap.error('give VARIANTS.txt or --decl-swaps')
    rng = [int(v, 16) for v in a.range.split(',')] if a.range else None
    src_path = os.path.join(ROOT, a.src)
    base = open(src_path).read()
    variants = parse(open(a.variants).read()) if a.variants else []
    if a.decl_swaps:
        variants += decl_swaps(base)
    work = os.path.join(ROOT, 'build', 'variants', os.path.basename(a.src)[:-2])
    os.makedirs(work, exist_ok=True)

    def run(tag, text):
        path = os.path.join(work, f'{tag}.c')
        open(path, 'w').write(text)
        return check(os.path.relpath(path, ROOT), a.nonmatching, rng)

    base_score, note = run('base', base)
    if base_score is None:
        sys.exit(f'base does not compile: {note}')
    print(f'{"base":<40} struct={base_score[0]:<5} total={base_score[1]:<5} bytes={base_score[2]:<5} {note}')
    jobs = {}
    with concurrent.futures.ThreadPoolExecutor(a.j) as pool:
        for k, (name, edits) in enumerate(variants):
            try:
                jobs[k] = pool.submit(run, f'v{k}', apply(base, edits))
            except ValueError as e:
                print(f'{name[:40]:<40} skipped: {e}')
        results = []
        for k, fut in jobs.items():
            score, note = fut.result()
            results.append((score, k, note))
    results.sort(key=lambda r: (r[0] is None, r[0] or ()))
    for score, k, note in results:
        name = variants[k][0]
        if score is None:
            print(f'{name[:40]:<40} error: {note}')
            continue
        d = [x - y for x, y in zip(score, base_score)]
        print(f'{name[:40]:<40} struct={score[0]:<5} total={score[1]:<5} bytes={score[2]:<5} '
              f'({d[0]:+d}/{d[1]:+d}) {note}')
    if not a.greedy:
        return
    best, best_score = base, base_score
    for score, k, _ in results:
        if score is None or score >= base_score:
            continue
        try:
            text = apply(best, variants[k][1])
        except ValueError:
            continue
        s, note = run('greedy', text)
        if s is not None and s < best_score:
            best, best_score = text, s
            print(f'greedy + {variants[k][0][:40]}: struct={s[0]} total={s[1]} bytes={s[2]} {note}')
    out = src_path if a.write else os.path.join(work, 'best.c')
    open(out, 'w').write(best)
    print(f'greedy result struct={best_score[0]} total={best_score[1]} bytes={best_score[2]} -> '
          f'{os.path.relpath(out, ROOT)}')


if __name__ == '__main__':
    main()

#!/usr/bin/env python3
"""Find disassembler function starts that are really labels inside another function.

  split_audit.py [--write]

gbadisasm sometimes starts a "function" in the middle of a real one: at the far
branch target agbcc uses for a long jump (`bl _label` inside a function), after a
`bl` it wrongly took for a no-return call, or at the second push of a prologue
that saves high registers (`push {r4-r7,lr}; mov r7,sl; ...; push {r5-r7}`).
Matching such a "function" is impossible: it has no prologue, it reads its
parent's stack frame, and the parent has no epilogue of its own.

A start in gen/code_sym.s is reported when
  * it begins with a second prologue push (`push {r5, r6, r7}` and similar) right
    after a function that ends in `mov rN, r8/sb/sl`, or
  * it has no `push` in its first instructions and the previous function does not
    end in a return (`bx`, `pop {..pc}`, `mov pc`, a tail `b sub_`),
and nothing outside the parent (the run of such starts back to the last real
one) refers to it or to any label inside it. Starts already listed in
symbols/function_nonentries.csv are skipped.

--write appends the reported starts to symbols/function_nonentries.csv with an
evidence line, drops them from symbols/progress_functions.csv and removes their
INCLUDE_ASM lines from src/ units, so the parent's asm (and later its C) covers
the whole body. Run `make` afterwards (gen/ and the split are regenerated).
"""
import argparse
import collections
import csv
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402

START = re.compile(r'^\t(?:thumb|arm|non_word_aligned_thumb)_func_start (\S+)')
LABEL = re.compile(r'^(\w+): @ 0x([0-9A-F]{8})')
SECOND_PUSH = re.compile(r'push \{r5, r6, r7\}|push \{r5, r6\}|push \{r6, r7\}|push \{r7\}')
RETURN = re.compile(r'(bx |pop \{[^}]*pc\}|mov pc|b\s+sub_|ldr pc)')
NONENTRIES = os.path.join(ROOT, 'symbols', 'function_nonentries.csv')
PROGRESS = os.path.join(ROOT, 'symbols', 'progress_functions.csv')


def load(path):
    lines = open(path).read().split('\n')
    funcs = []
    for i, line in enumerate(lines):
        m = START.match(line)
        if m:
            label = LABEL.match(lines[i + 1])
            funcs.append({'line': i, 'name': m.group(1), 'addr': label.group(2) if label else '?',
                          'insns': []})
        elif funcs and re.match(r'^\t[a-z]', line) and not line.startswith('\t.'):
            funcs[-1]['insns'].append(line.strip())
    return lines, funcs


def suspects(funcs, known):
    out = []
    for k, f in enumerate(funcs):
        if k == 0 or not f['insns'] or f['addr'] in known:
            continue
        prev = funcs[k - 1]
        last = prev['insns'][-1] if prev['insns'] else ''
        if SECOND_PUSH.match(f['insns'][0]) and last.startswith('mov r'):
            out.append((k, f'second prologue push after {prev["name"]}'))
        elif (not any(x.startswith('push') for x in f['insns'][:6]) and last and not RETURN.match(last)
              and last != f"b {f['name']}"):  # a tail jump to it by name: a real entry (crt0_Init)
            out.append((k, f'no prologue; {prev["name"]} does not return before it (last: {last})'))
    return out


def outside_refs(lines, funcs, found):
    starts = {funcs[k]['addr'] for k, _ in found}
    refs = collections.defaultdict(list)
    tok = re.compile(r'[A-Za-z_]\w*')
    for i, line in enumerate(lines):
        if line.startswith('\t') and not line.startswith(('\t.global', '\tthumb_func', '\tarm_func', '\tnon_word')):
            for t in set(tok.findall(line)):
                refs[t].append(i)
    result = {}
    for k, _ in found:
        p = k - 1
        while funcs[p]['addr'] in starts:
            p -= 1
        q = k + 1
        while q < len(funcs) and funcs[q]['addr'] in starts:
            q += 1
        lo, hi = funcs[p]['line'], funcs[q]['line'] if q < len(funcs) else len(lines)
        end = funcs[k + 1]['line'] if k + 1 < len(funcs) else len(lines)
        labels = {funcs[k]['name']} | {m.group(1) for line in lines[funcs[k]['line']:end]
                                        for m in [re.match(r'^(\w+):', line)] if m}
        result[k] = (funcs[p]['name'], [i for lab in labels for i in refs.get(lab, []) if not lo <= i < hi])
    return result


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--write', action='store_true')
    ap.add_argument('--asm', default=os.path.join(ROOT, 'gen', 'code_sym.s'))
    a = ap.parse_args()
    known = {r['address'] for r in csv.DictReader(open(NONENTRIES))}
    lines, funcs = load(a.asm)
    found = suspects(funcs, known)
    refs = outside_refs(lines, funcs, found)
    rows = []
    for k, why in found:
        parent, outside = refs[k]
        f = funcs[k]
        # data words that merely look like pointers into the body do not make it an entry
        outside = [i for i in outside if not lines[i].strip().startswith('.4byte')]
        status = 'ok' if not outside else f'kept: {len(outside)} outside reference(s)'
        print(f"{f['addr']} {f['name']:<34} parent {parent:<34} {why} [{status}]")
        if not outside:
            rows.append((f['addr'], f['name'], f"Interior label of {parent} ({why}); "
                                               f"referenced only from inside {parent} (tools/split_audit.py)"))
    if not a.write or not rows:
        print(f"{len(rows)} interior starts to record" + ('' if a.write else ' (--write to record them)'))
        return
    with open(NONENTRIES, 'a', newline='') as out:
        w = csv.writer(out, lineterminator='\n')
        for addr, _, evidence in rows:
            w.writerow([addr, evidence])
    addrs = {r[0] for r in rows}
    prog = open(PROGRESS).read().split('\n')
    open(PROGRESS, 'w').write('\n'.join(x for x in prog if x not in addrs))
    names = {r[1] for r in rows}
    src = os.path.join(ROOT, 'src')
    for dirpath, _, files in os.walk(src):
        for fn in files:
            if not fn.endswith('.c'):
                continue
            path = os.path.join(dirpath, fn)
            text = open(path).read()
            new = re.sub(r'^INCLUDE_ASM\("asm/nonmatching", (\w+)\);\n\n?',
                         lambda m: '' if m.group(1) in names else m.group(0), text, flags=re.M)
            if new != text:
                open(path, 'w').write(new)
                print('removed INCLUDE_ASM lines in', os.path.relpath(path, ROOT))
    print(f"recorded {len(rows)} interior starts; run make")


if __name__ == '__main__':
    main()

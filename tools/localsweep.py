#!/usr/bin/env python3
"""Try deleting pointer locals that only name a field address, and keep the deletions that help.

  localsweep.py FILE.c [FUNC] [--write OUT.c] [--all]

When the ROM computes `&s->field` into a scratch register and copies it to its
final register only at the end of a block (`adds r7, r0, #0`, `mov sb, r1`),
the copy is a GCSE/PRE reaching register. That means the original wrote the
field at each use, with no `T *p = &s->field;` local. A local makes agbcc compute
the address straight into its final register, and the copies come out wrong
(notes/cloud-08122DF4.md, sub_08125528).

The script finds the locals of FUNC that are assigned exactly once from an
address expression (`p = &x->y;`, `T *p = &x[i];`). For each one it rewrites
the uses (`*p` -> `x->y`, `p->f` -> `(x->y).f`, `p` -> `&x->y`), compiles, and
scores the result. Changes are kept greedily, best first, while the score
improves. The score is (exact match, fewest differing instructions by
tools/anchordiff.py, highest fuzzy %).

FILE may be a draft under `#ifdef NONMATCHING`; it is compiled as if NONMATCHING were
defined. Without --write it only reports what it would change.
"""
import argparse
import glob
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PY = sys.executable


def enable_nonmatching(src):
    out = src.replace('#ifdef NONMATCHING', '#if 1', 1)
    return out


def func_span(src, func):
    m = re.search(r'^[\w\s\*]*\b%s\s*\([^;{]*\)\s*\{' % re.escape(func), src, re.M)
    if not m:
        return None
    depth = 0
    for i in range(m.end() - 1, len(src)):
        if src[i] == '{':
            depth += 1
        elif src[i] == '}':
            depth -= 1
            if depth == 0:
                return m.end(), i
    return None


def candidates(body):
    """[(name, expr, kind)] for pointer locals assigned once from an address expression."""
    out = []
    # T *a, *b;   or   T *a = &expr;
    decls = re.findall(r'^\s*(?:const\s+)?(?:struct\s+\w+|\w+)\s*((?:\*\s*\w+\s*(?:=[^,;]+)?\s*,\s*)*\*\s*\w+\s*(?:=[^;]+)?);', body, re.M)
    names = []
    for d in decls:
        for part in d.split(','):
            m = re.match(r'\s*\*\s*(\w+)\s*(?:=\s*(.+))?$', part.strip())
            if m:
                names.append((m.group(1), m.group(2)))
    for name, init in names:
        assigns = re.findall(r'(?<![\w.>*])%s\s*=(?!=)\s*([^;]+);' % name, body)
        exprs = ([init.strip()] if init else []) + [a.strip() for a in assigns]
        if len(exprs) != 1 or not exprs[0].startswith('&'):
            continue
        if re.search(r'(\+\+|--)\s*%s\b|(?<![*\w])\s*\b%s\s*(\+\+|--|[+\-]=)' % (name, name), body):
            continue
        out.append((name, exprs[0]))
    return out


def substitute(body, name, expr):
    inner = expr[1:].strip()
    paren = inner if re.match(r'^[\w\->.\[\]]+$', inner) else '(' + inner + ')'
    # drop the declarator and the assignment
    decl = re.compile(r'^([ \t]*(?:const[ \t]+)?(?:struct[ \t]+\w+|(?!return\b|else\b)\w+)[ \t]*)(\*[^;]*);\n', re.M)

    def drop(m):
        parts = [x for x in m.group(2).split(',')
                 if not re.match(r'\s*\*\s*%s\s*(=|$)' % name, x)]
        if len(parts) == len(m.group(2).split(',')):
            return m.group(0)
        if not parts:
            return ''
        return m.group(1) + ','.join(parts).strip() + ';\n'
    body = decl.sub(drop, body)
    body = re.sub(r'^\s*%s\s*=(?!=)\s*[^;]+;\n' % name, '', body, flags=re.M)
    body = re.sub(r'\*\s*%s\b' % name, paren, body)
    body = re.sub(r'\b%s->' % name, paren + '.', body)
    body = re.sub(r'(?<![\w.>])%s\b' % name, '&' + paren, body)
    return body


def score(src, func, tmpdir):
    path = os.path.join(tmpdir, 'sweep.c')
    open(path, 'w').write(src)
    r = subprocess.run([PY, os.path.join(ROOT, 'tools/check.py'), path, '--full'],
                       capture_output=True, text=True, cwd=ROOT)
    out = r.stdout + r.stderr
    if re.search(r'\bMATCH \(', out) and 'MISMATCH' not in out:
        return (1, 0, 100.0)
    fz = re.search(r'fuzzy ([\d.]+)%', out)
    if not fz:
        return None
    art = re.search(r'artifacts: (\S+)', out)
    run = os.path.join(ROOT, art.group(1)) if art else max(
        glob.glob(os.path.join(ROOT, 'build/check/run-*')), key=os.path.getmtime)
    a = subprocess.run([PY, os.path.join(ROOT, 'tools/anchordiff.py'), run],
                       capture_output=True, text=True, cwd=ROOT).stdout.strip().splitlines()
    n = int(a[-1].split()[-1]) if a and a[-1].startswith('TOTAL') else 10 ** 6
    return (0, -n, float(fz.group(1)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('file')
    ap.add_argument('func', nargs='?')
    ap.add_argument('--write')
    args = ap.parse_args()
    src = enable_nonmatching(open(args.file).read())
    func = args.func
    if not func:
        tail = src[src.index('#if 1'):] if '#if 1' in src else src
        m = re.search(r'^\w[\w \t\*]*?\b(\w+)\s*\([^;{]*\)\s*\{', tail, re.M)
        func = m.group(1)
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, 'build')) as tmp:
        best = score(src, func, tmp)
        print('base', best)
        if best is None:
            sys.exit('base does not compile')
        changed = True
        while changed and best[0] == 0:
            changed = False
            span = func_span(src, func)
            if not span:
                sys.exit('function %s not found' % func)
            body = src[span[0]:span[1]]
            trials = []
            for name, expr in candidates(body):
                variant = src[:span[0]] + substitute(body, name, expr) + src[span[1]:]
                sc = score(variant, func, tmp)
                print('  try %-12s = %-28s %s' % (name, expr, sc))
                if sc and sc > best:
                    trials.append((sc, name, variant))
            if trials:
                trials.sort(reverse=True)
                best, name, src = trials[0]
                print('keep: drop %s -> %s' % (name, best))
                changed = True
        print('final', best)
        if args.write:
            open(args.write, 'w').write(src)
            print('wrote', args.write)


if __name__ == '__main__':
    main()

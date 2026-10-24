#!/usr/bin/env python3
"""Compile one C file with agbcc's pass dumps and answer questions from them.

  rtlprobe.py dump FILE.c [--nonmatching] [--flags=-da]
  rtlprobe.py loop FILE.c [--nonmatching] [--insn N ...] [--regno R ...]
  rtlprobe.py gcse FILE.c [--nonmatching] [--func NAME] [--scan LO:HI] [--expr I ...]
  rtlprobe.py try-t FILE.c T|LO:HI ... [--nonmatching] [-j N]
  rtlprobe.py try-env FILE.c 'VAR=V [VAR=V ...]' ... [--nonmatching]
  rtlprobe.py perline FILE.c [--nonmatching] [--func NAME] [--top N]
  rtlprobe.py canon FILE.c [--nonmatching] [-j N]
  rtlprobe.py follow FILE.c [--nonmatching] [-j N]
  rtlprobe.py pre FILE.c [--nonmatching] [-j N]
  rtlprobe.py alias FILE.c [--nonmatching] [-j N]

Each command compiles FILE.c as build.py does (preprocessed the same way),
with `-dp` and the pass dumps, into build/rtlprobe/<stem>/, in under a second
even for a 4000-instruction function. Use it when the instructions are right
but a constant, a copy or a register differs and you need to know which pass
decided it (docs/PERMUTER.md, "Big functions").

dump  writes every pass (-da: .rtl .jump .cse .addressof .gcse .loop .cse2
      .flow .combine .regmove .lreg .greg .mach) and lists the files. Grep an
      insn number (the `@ N` after each instruction in u.s, from -dp) through
      them to see where it changed. --flags=-dX (with "=") picks other dumps: -dL loop,
      -dG gcse, -dr rtl, -dc combine, -dN regmove, -dl lreg, -dg greg.

loop  summarises loop.c's invariant-motion decisions: each movable with its
      register, lifetime, savings, and whether it was moved (hoisted out of
      the loop) or "not desirable". loop.c moves a movable only when
      threshold * savings * lifetime >= insns in the loop, threshold =
      (loop has a call ? 1 : 2) * (1 + non-fixed regs) = 13 on Thumb, minus 3
      per movable already moved. A constant that the ROM keeps in a register
      across the loop but you rematerialise (or the reverse) is decided here.

gcse  lists PRE's redundant expressions and the reaching registers it makes
      for them. pre_delete walks the expression hash table bucket by bucket,
      so reaching registers are numbered (and later allocated, or spilled to
      stack slots) in bucket order: bucket = hash(expr) % T, with
      T = (insns at GCSE / 2) | 1. When only those registers or their stack
      slots differ, the cause is the bucket order, which moves with T (the
      function's insn count before GCSE) and with the pseudo numbers inside
      the expressions. --scan LO:HI recomputes the order for every odd T in
      the range (the hash is reimplemented from gcse.c and checked against
      every entry of the dump), so you can see how many insns you would have
      to add or remove; --expr limits the order to some expression indices.
      --need E1 E2 ... lists the T windows (nearest first) that put those
      expressions in exactly that bucket order, e.g. the ROM's stack-slot order.

try-t  checks FILE.c with the instrumented compiler (tools/agbcc-instr.sh, which
      takes about 40 s to build) forcing the GCSE table size to each T, and
      prints check.py's counts. This splits a residual diff: whatever is still
      different at the T that gives the ROM's order has another cause.
      LO:HI sweeps every odd T in the range and prints windows of equal
      results: the exact set of T values that match.

try-env  the same for any of the instrumented compiler's knobs, one quoted
      set of assignments per run, e.g. 'CSE_KEEP=2093' 'AGBCC_GCSE_T=2301
      CSE_KEEP=2093'. CSE_KEEP=r1,r2 never replaces those pseudos as a CSE
      class's canonical register, CSE_NOCANON=N keeps pseudo N from becoming
      canonical, CSE_DEBUG=N traces make_regs_eqv (printed as stderr). If
      keeping the ROM-side pseudo canonical gives 0 diffs, write C that makes
      that pseudo live longer (sub_08123194: one shared `target` variable for
      both Seek calls of a path).
      AGBCC_NOINHERIT=UID[,UID] turns off reload inheritance at those insns,
      AGBCC_NORCSE=1 skips reload_cse_regs and AGBCC_NOEQUIVMOVE=all keeps the
      moves jump.c would delete as copies of a value already there: all three
      together on the use's insn match a ROM that copies a high register
      straight back (agbccsrc.py reload-mode).
      AGBCC_WHYREG=N prints what global.c find_reg saw for pseudo N: its
      conflicts, the registers used so far, the registers lower-priority
      conflicting pseudos prefer (and which pseudo prefers each), its own
      preferences and its choice. AGBCC_PREF=N:R[,N:R] adds hard register R
      to pseudo N's preferences, as a copy would. When AGBCC_FORCE matches
      but AGBCC_PRI cannot, try AGBCC_PREF on the pseudo and on the pseudos
      it is copied to or computed into (sub_08137E28: PREF=cell:4 matched,
      and the C was the cell index computed into x itself: fix.py
      sp_result_into_arg).

canon  finds the CSE canonical-register decision behind a difference, with no
      pseudo number to guess: one traced compile lists every pseudo that
      make_regs_eqv made a class's canonical register (it lives past the
      extended basic block and longer than the old head), then each is
      checked with CSE_NOCANON=N. A pseudo whose demotion matches (or gets
      closer) is the decision: its sets and uses in the .cse dump say which
      C value it is. Typical case (Player_EmitMovementEffects): an inline
      helper's return register set on a `return 0` path and on the main path
      becomes canonical, so `if (helper())` keeps testing it, its `= 0` set is
      not trivially dead, and the jump around it is threaded one pass too
      late (a null-check branch targets the next null check, not the shared
      path). Fix in C: make the ROM-side value the longer-lived one, or keep
      the copy inside one block.

follow  finds a CSE path decision behind a difference: cse extends a block into
      the target of a conditional jump (cse.c cse_end_of_basic_block) when the
      label is used once and a BARRIER precedes it, carrying what it knows
      (constants in registers, equal values) into the target. One traced
      compile (CSE_PATH=1) lists each such jump with its source line, then
      each is refused in turn (CSE_NOFOLLOW=K). A refusal that matches names
      the jump. Typical case (Ghost_TurnTowardHeading): the ROM loads a
      constant again after `if (a == b) { ...; return; }` and the draft
      reuses it. The C fix need not refuse the jump; it only has to keep the
      value from being known there, e.g. tools/spellings.py sp_narrow_mask:
      a QImode `(u8)(x & 7)` puts 7 in a QImode register, which an int
      `& 7` cannot reuse, since cse shares a constant only within one mode.

pre   keeps, one at a time, each computation that gcse's PRE replaces with a
      copy of a reaching register (AGBCC_NOPRE=K; AGBCC_PRE_TRACE=1 lists
      them). A keep that matches says the ROM's expression was not redundant
      there: spell it differently (another base or offset form) so PRE does
      not see the same expression.

perline  insns per source line in the RTL after CSE (what GCSE sees, and so
      what sets T) against after local-alloc (what survives), largest
      surplus first. Lines whose RTL shrinks a lot between the two are where
      to cut insns at GCSE time without changing the final code.
"""
import argparse
import concurrent.futures
import json
import os
import re
import subprocess
import sys

import build
from romlib import ROOT

GCC = os.path.join(build.TOOLS, 'agbcc', 'gcc')


def compile_dumps(src, flags, nonmatching, lines=False):
    if nonmatching and '-DNONMATCHING' not in build.DEFINES:
        build.DEFINES.append('-DNONMATCHING')
    work = os.path.join(ROOT, 'build', 'rtlprobe', os.path.basename(src)[:-2])
    os.makedirs(work, exist_ok=True)
    for f in os.listdir(work):
        if f.startswith('u.'):
            os.remove(os.path.join(work, f))
    i_path = os.path.join(work, 'u.i')
    with open(i_path, 'w') as f:
        if lines:  # keep line markers so -g line notes name the source file and line
            f.write(build.sh(build.CPP + ['-nostdinc', '-undef'] + build.DEFINES +
                             ['-I', 'include', '-iquote', '.', src]))
        else:
            f.write(build.preprocess(src))
    r = subprocess.run([build.compiler_for(src)] + build.cflags_for(src) + ['-dp'] + flags + ['u.i', '-o', 'u.s'],
                       cwd=work, capture_output=True, text=True)
    if r.returncode:
        sys.exit(r.stderr)
    return work


# --- gcse.c hash_expr_1, on printed RTL -------------------------------------

def rtl_tables():
    text = re.sub(r'/\*.*?\*/', '', open(os.path.join(GCC, 'rtl.def')).read(), flags=re.S)
    codes = {}
    for k, m in enumerate(re.finditer(r'DEF_RTL_EXPR\s*\(\s*(\w+)\s*,\s*"([^"]*)"\s*,\s*"([^"]*)"', text)):
        codes[m.group(2)] = (k, m.group(3))
    text = re.sub(r'/\*.*?\*/', '', open(os.path.join(GCC, 'machmode.def')).read(), flags=re.S)
    modes = {m.group(1): k for k, m in enumerate(re.finditer(r'DEF_MACHMODE\s*\(\s*\w+\s*,\s*"(\w+)"', text))}
    return codes, modes


def parse_rtl(text):
    toks = re.findall(r'\(|\)|\[|\]|"[^"]*"|[^\s()\[\]"]+', text)
    pos = 0

    def node():
        nonlocal pos
        tok = toks[pos]
        pos += 1
        if tok != '(':
            return tok
        out = []
        while toks[pos] != ')':
            out.append(node())
        pos += 1
        return out
    return node()


class Hasher:
    def __init__(self):
        self.codes, self.modes = rtl_tables()

    def head(self, x):
        name, _, mode = x[0].partition(':')
        return name.split('/')[0], self.modes.get(mode, 0)

    def __call__(self, x, mode=0):
        h = 0
        while True:
            name, xmode = self.head(x)
            code, fmt = self.codes[name]
            if name == 'reg':
                return h + (code << 7) + int(x[1])
            if name == 'const_int':
                return h + (code << 7) + mode + int(x[1], 0)
            if name == 'symbol_ref':
                s = x[1][0] if isinstance(x[1], list) else x[1]
                s, v = s.strip('"'), 0
                for c in s.encode():
                    v = (v + (v << 7) + c) & 0xFFFFFFFF
                return h + (code << 7) + v
            if name == 'label_ref':
                return h + (code << 7) + int(x[1])
            if name == 'mem':
                h += code
                x = x[1]
                continue
            h += code + xmode
            ops = x[1:]
            for i in range(len(fmt) - 1, -1, -1):
                f, op = fmt[i], ops[i] if i < len(ops) else None
                if f == 'e':
                    if i == 0:
                        x = op
                        break
                    h += self(op)
                elif f == 'E':
                    h += sum(self(e) for e in op if e not in ('[', ']'))
                elif f == 's':
                    h += sum(op.strip('"').encode())
                elif f == 'i':
                    h += int(op)
            else:
                return h & 0xFFFFFFFF


def function_dumps(path, func):
    text = open(path).read()
    parts = re.split(r'^;; Function (\w+)\n', text, flags=re.M)
    funcs = dict(zip(parts[1::2], parts[2::2]))
    if func:
        return func, funcs[func]
    return max(funcs.items(), key=lambda kv: len(kv[1]))


def gcse(a):
    work = compile_dumps(a.src, ['-dG'], a.nonmatching)
    func, text = function_dumps(os.path.join(work, 'u.i.gcse'), a.func)
    pre = text.index('PRE GCSE of') if 'PRE GCSE of' in text else len(text)
    # the expression table that PRE used: the last one printed before its summary
    starts = [m.start() for m in re.finditer(r'^Expression hash table', text[:pre], flags=re.M)]
    if not starts:
        sys.exit(f'{func}: no expression hash table (no PRE ran)')
    table = text[starts[-1]:pre]
    T = int(re.match(r'Expression hash table \((\d+) buckets', table).group(1))
    exprs = {}
    for m in re.finditer(r'^Index (\d+) \(hash value (\d+)\)\n((?:  .*\n)+)', table, flags=re.M):
        exprs[int(m.group(1))] = (int(m.group(2)), ' '.join(m.group(3).split()))
    hasher = Hasher()
    raw, bad = {}, 0
    for i, (bucket, rtl) in exprs.items():
        try:
            raw[i] = hasher(parse_rtl(rtl))
            bad += raw[i] % T != bucket
        except (KeyError, IndexError, ValueError, TypeError, AttributeError):
            pass
    reds = [tuple(map(int, m.groups())) for m in re.finditer(
        r'PRE: redundant insn (\d+) \(expression (\d+)\) in bb \d+, reaching reg is (\d+)', table + text[pre:])]
    print(f'{func}: T = {T} buckets ({len(exprs)} expressions; hash reproduced for '
          f'{len(raw) - bad} of {len(exprs)}), {len(reds)} PRE replacements')
    print('reaching reg  expr  bucket  insn   expression')
    seen = set()
    for insn, e, reg in sorted(reds, key=lambda r: r[2]):
        mark = '' if e not in seen else ' (same expr)'
        seen.add(e)
        print(f'{reg:>12}  {e:>4}  {exprs[e][0]:>6}  {insn:>5}  {exprs[e][1][:70]}{mark}')
    if a.need:
        a.expr = a.need
        a.scan = a.scan or f'{max(11, T // 2)}:{T * 2}'
    if not a.scan:
        return
    want = [e for e in dict.fromkeys(e for _, e, _ in sorted(reds, key=lambda r: r[2]))
            if not a.expr or e in a.expr]
    if a.need:
        want = [e for e in a.need if e in want]
        if missing := [e for e in want if e not in raw]:
            sys.exit(f'cannot recompute the hash of expressions {missing}')
        lo, hi = (int(v) for v in a.scan.split(':'))
        hits = [t for t in range(lo | 1, hi + 1, 2)
                if [raw[e] % t for e in want] == sorted(raw[e] % t for e in want)]
        windows = []
        for t in hits:
            if windows and windows[-1][1] == t - 2:
                windows[-1][1] = t
            else:
                windows.append([t, t])
        print(f'\nT values in {lo}..{hi} that put expressions {"-".join(map(str, want))} in that bucket order'
              f' (now T = {T}):')
        for w0, w1 in sorted(windows, key=lambda w: min(abs(w[0] - T), abs(w[1] - T))):
            print(f'  T {w0}..{w1}  ({w0 - T:+d}..{w1 - T:+d}, about {2 * (w0 - T):+d}..{2 * (w1 - T):+d} insns at GCSE)')
        print('raw hashes (bucket = raw % T; a pseudo number inside an expression adds to its raw hash 1:1):')
        for e in want:
            print(f'  {e:>5}  raw {raw[e]:>6}  {exprs[e][1][:70]}')
        return
    missing = [e for e in want if e not in raw]
    if missing:
        sys.exit(f'cannot recompute the hash of expressions {missing}')
    lo, hi = (int(v) for v in a.scan.split(':'))
    print(f'\nreaching-register order by T (expressions {", ".join(map(str, want))}; '
          f'current order is {"-".join(map(str, want))})')
    prev, first = None, None
    for t in list(range(lo | 1, hi + 1, 2)) + [None]:
        order = None if t is None else tuple(sorted(want, key=lambda e: (raw[e] % t, e)))
        if order != prev:
            if prev is not None:
                print(f'  T {first}..{last}: {"-".join(map(str, prev))}{"  <- now" if first <= T <= last else ""}')
            prev, first = order, t
        last = t


def loop(a):
    work = compile_dumps(a.src, ['-dL'], a.nonmatching)
    path = os.path.join(work, 'u.i.loop')
    n = 0
    for ln in open(path):
        m = re.match(r'Insn (\d+): regno (\d+) \(life (\d+)\)', ln)
        if m and (not a.insn or int(m.group(1)) in a.insn) and (not a.regno or int(m.group(2)) in a.regno):
            print(ln.rstrip())
            n += 1
        elif re.match(r'Loop from|^;; Function', ln) and not (a.insn or a.regno):
            print(ln.rstrip())
    print(f'{n} movables; dump {os.path.relpath(path, ROOT)} (insn numbers: {os.path.relpath(work, ROOT)}/u.s)')


def try_t(a):
    ts, sweep = [], False
    for arg in a.t:
        if ':' in arg:
            lo, hi = (int(v) for v in arg.split(':'))
            ts += range(lo | 1, hi + 1, 2)
            sweep = True
        else:
            ts.append(int(arg))
    a.envs = [f'AGBCC_GCSE_T={t}' for t in ts]
    results = try_env(a, quiet=sweep)
    if not sweep:
        return
    # windows of consecutive T with the same result
    print(f'{"default":>20}: {results[0][1]}')
    prev, first, last = None, None, None
    for (spec, text), t in list(zip(results[1:], ts)) + [((None, None), None)]:
        if text != prev:
            if prev is not None:
                print(f'  T {first}..{last}: {prev}')
            prev, first = text, t
        last = t


def try_env(a, quiet=False):
    cc = os.path.join(ROOT, 'build', 'agbcc-instr', 'gcc', 'agbcc')
    if not os.path.exists(cc):
        sys.exit('build the instrumented compiler first: sh tools/agbcc-instr.sh')

    def run(spec):
        env = dict(os.environ, AGBCC=cc, **dict(kv.split('=', 1) for kv in spec.split()))
        cmd = [sys.executable, os.path.join(ROOT, 'tools', 'check.py'), a.src, '--json', '--quiet']
        r = subprocess.run(cmd + ['--nonmatching'] * a.nonmatching, cwd=ROOT, env=env, capture_output=True, text=True)
        res = json.loads([ln for ln in r.stdout.splitlines() if ln.startswith('{')][-1])
        ad = res.get('anchordiff', {'structural': 0, 'total': 0})
        return (f"{res['different_byte_count']:>5} differing bytes, realigned {ad['total']} "
                f"({ad['structural']} ignoring registers){'  MATCH' if res['status'] == 'match' else ''}")
    specs = [''] + a.envs
    with concurrent.futures.ThreadPoolExecutor(a.j) as pool:
        texts = list(pool.map(run, specs))
    for spec, text in zip(specs, texts):
        if 'CSE_DEBUG' in spec or 'AGBCC_WHYREG' in spec:  # check.py drops stderr: compile again for the trace
            work = compile_dumps(a.src, [], a.nonmatching)
            env = dict(os.environ, **dict(kv.split('=', 1) for kv in spec.split()))
            tr = subprocess.run([cc] + build.cflags_for(a.src) + ['u.i', '-o', '/dev/null'],
                                cwd=work, env=env, capture_output=True, text=True)
            print(tr.stderr.rstrip())
        if not quiet:
            print(f"{spec or 'default':>20}: {text}")
    return list(zip(specs, texts))


def canon(a):
    cc = os.path.join(ROOT, 'build', 'agbcc-instr', 'gcc', 'agbcc')
    work = compile_dumps(a.src, [], a.nonmatching)
    tr = subprocess.run([cc] + build.cflags_for(a.src) + ['u.i', '-o', '/dev/null'], cwd=work,
                        env=dict(os.environ, CSE_DEBUG='1'), capture_output=True, text=True).stderr
    heads = {}
    for m in re.finditer(r'eqv new=(\d+) old=(\d+) first=(\d+) newlast=(\d+) newfirst=(\d+) '
                         r'firstlast=(-?\d+) bb=(\d+)\.\.(\d+)', tr):
        new, old, first, nl, nf, fl, b0, b1 = map(int, m.groups())
        if fl >= 0 and (nl > b1 or nf < b0) and nl > fl:
            heads[new] = heads.get(new, 0) + 1
    print(f'{len(heads)} pseudos become a CSE class head; trying CSE_NOCANON on each')
    a.envs = [f'CSE_NOCANON={n}' for n in sorted(heads)]
    base = None
    for spec, text in try_env(a, quiet=True):
        if not spec:
            base = text
            print(f'{"default":>20}: {text}')
        elif text != base:
            print(f'{spec:>20}: {text}')


def follow(a):
    cc = os.path.join(ROOT, 'build', 'agbcc-instr', 'gcc', 'agbcc')
    work = compile_dumps(a.src, [], a.nonmatching, lines=True)  # -g only for the line notes
    tr = subprocess.run([cc] + build.cflags_for(a.src) + ['-g', 'u.i', '-o', '/dev/null'], cwd=work,
                        env=dict(os.environ, CSE_PATH='1'), capture_output=True, text=True).stderr
    where = {}
    for m in re.finditer(r'follow k=(\d+) pass=1 jump=\d+ label=\d+ line=(\d+)', tr):
        where[int(m.group(1))] = int(m.group(2))
    lines = open(os.path.join(ROOT, a.src)).read().split('\n')
    print(f'cse follows {len(where)} jumps into their targets; refusing each in turn (CSE_NOFOLLOW=K)')
    a.envs = [f'CSE_NOFOLLOW={k}' for k in sorted(where)]
    base = None
    for spec, text in try_env(a, quiet=True):
        if not spec:
            base = text
            print(f'{"default":>16}: {text}')
        elif text != base:
            ln = where[int(spec.split('=')[1])]
            src = lines[ln - 1].strip()[:60] if 0 < ln <= len(lines) else ''
            print(f'{spec:>16}: {text}\n{"":>18}jump at line {ln}: {src}')


def pre(a):
    cc = os.path.join(ROOT, 'build', 'agbcc-instr', 'gcc', 'agbcc')
    work = compile_dumps(a.src, [], a.nonmatching)
    tr = subprocess.run([cc] + build.cflags_for(a.src) + ['u.i', '-o', '/dev/null'], cwd=work,
                        env=dict(os.environ, AGBCC_PRE_TRACE='1'), capture_output=True, text=True).stderr
    ks = [int(m.group(1)) for m in re.finditer(r'pre k=(\d+) insn=\d+ expr=\d+', tr)]
    print(f'gcse replaces {len(ks)} redundant computations with a copy; keeping each in turn (AGBCC_NOPRE=K)')
    a.envs = [f'AGBCC_NOPRE={k}' for k in ks]
    base = None
    for spec, text in try_env(a, quiet=True):
        if not spec:
            base = text
            print(f'{"default":>16}: {text}')
        elif text != base:
            print(f'{spec:>16}: {text}')


def alias(a):
    """Is the difference a type-based alias decision? One compile with AGBCC_NOSTRICT=1."""
    a.envs = ['AGBCC_NOSTRICT=1']
    (_, base), (_, off) = try_env(a, quiet=True)
    print(f'{"default":>16}: {base}')
    print(f'{"no alias sets":>16}: {off}')
    realigned = lambda s: int(re.search(r'realigned (\d+)', s).group(1))
    if 'MATCH' in off or realigned(off) < realigned(base):
        print('the difference shrinks without type-based alias sets: a store and a read the original keeps '
              'ordered (or a store it keeps alive) have the same type there and different types in yours '
              '(flow dead stores, cse reuse across a store). Read or write that field with the type of the '
              'other access: spellings.sp_alias_read `*(T *)&x->f`, or the field declared that type.')


def perline(a):
    work = compile_dumps(a.src, ['-g', '-ds', '-dl'], a.nonmatching, lines=True)

    def count(path):
        func, text = function_dumps(path, a.func)
        out, where = {}, None
        for m in re.finditer(r'^\((insn|jump_insn|call_insn|note)(?:/\w+)? \d+ \d+ \d+ (?:\(?"([^"]+)"\)? (\d+))?', text, re.M):
            if m.group(1) == 'note':
                if m.group(2):
                    where = (os.path.basename(m.group(2)), int(m.group(3)))
            elif where:
                out[where] = out.get(where, 0) + 1
        return func, out
    func, cse = count(os.path.join(work, 'u.i.cse'))
    _, lreg = count(os.path.join(work, 'u.i.lreg'))
    rows = sorted(((cse.get(k, 0) - lreg.get(k, 0), k) for k in set(cse) | set(lreg)), reverse=True)
    print(f'{func}: {sum(cse.values())} insns after CSE, {sum(lreg.values())} after local-alloc')
    print(' surplus   cse  lreg  line')
    srcs = {}
    for d, (f, ln) in rows[:a.top]:
        if f not in srcs:
            path = next((p for p in (a.src, os.path.join('include', f), f) if os.path.basename(p) == f
                         and os.path.exists(os.path.join(ROOT, p))), None)
            srcs[f] = open(os.path.join(ROOT, path)).read().split('\n') if path else []
        text = srcs[f][ln - 1].strip() if ln <= len(srcs[f]) else ''
        print(f'{d:>8} {cse.get((f, ln), 0):>5} {lreg.get((f, ln), 0):>5}  {f}:{ln}  {text[:70]}')


def dump(a):
    work = compile_dumps(a.src, a.flags.split(), a.nonmatching)
    for f in sorted(os.listdir(work)):
        print(os.path.relpath(os.path.join(work, f), ROOT))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    for name in ('dump', 'loop', 'gcse', 'try-t', 'try-env', 'perline', 'canon', 'follow', 'pre', 'alias'):
        p = sub.add_parser(name)
        p.add_argument('src')
        p.add_argument('--nonmatching', action='store_true')
    sub.choices['dump'].add_argument('--flags', default='-da')
    sub.choices['loop'].add_argument('--insn', type=int, nargs='*')
    sub.choices['loop'].add_argument('--regno', type=int, nargs='*')
    sub.choices['gcse'].add_argument('--func')
    sub.choices['gcse'].add_argument('--scan', help='LO:HI, table sizes to try')
    sub.choices['gcse'].add_argument('--expr', type=int, nargs='*', help='expression indices to order')
    sub.choices['gcse'].add_argument('--need', type=int, nargs='+',
                                     help='expression indices in the bucket order wanted: print the T windows')
    sub.choices['try-t'].add_argument('t', nargs='+', help='table sizes, or LO:HI to sweep the odd sizes')
    for name in ('try-t', 'try-env', 'canon', 'follow', 'pre', 'alias'):
        sub.choices[name].add_argument('-j', type=int, default=os.cpu_count() or 4)
    sub.choices['try-env'].add_argument('envs', nargs='+')
    sub.choices['perline'].add_argument('--func')
    sub.choices['perline'].add_argument('--top', type=int, default=25)
    a = ap.parse_args()
    a.src = os.path.relpath(os.path.abspath(a.src), ROOT)
    {'dump': dump, 'loop': loop, 'gcse': gcse, 'try-t': try_t, 'try-env': try_env,
     'perline': perline, 'canon': canon, 'follow': follow, 'pre': pre, 'alias': alias}[a.cmd](a)


if __name__ == '__main__':
    main()

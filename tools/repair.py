#!/usr/bin/env python3
"""Fix the constants and symbols of a near-miss from the ROM's own bytes.

  repair.py FILE.c FUNC [--write] [--rounds N]

A draft ported from a similar function, or written from a decompiler, is
often right in shape but wrong in a few values: a field offset, an id, an
argument, a callee, a callback in the literal pool. This lines the
candidate's instructions up with the ROM's, collects the pairs of values
that differ where the code otherwise agrees (immediates, mov/lsl and mov/neg
constants, literal-pool words, call targets and pool symbols), and tries
each ROM value in place of each matching C literal or identifier. An edit is
kept when it brings the code closer to the ROM; it repeats until nothing
helps. The C is never rewritten beyond those tokens, so whatever remains is a
real difference in shape, which the report lists (calls or stores the ROM
has and the C lacks, and the reverse) as hints.

FILE.c must hold FUNC alone (tools/isolate.py). Without --write the result
goes to build/repair/FUNC.c (or --out-dir).
"""
import argparse
import difflib
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import check  # noqa: E402
import permute  # noqa: E402
import rasolve  # noqa: E402
from romlib import ROOT  # noqa: E402

REG = re.compile(r"\b(r\d+|sb|sl|fp|ip|sp|lr|pc)\b")
IMM = re.compile(r"#(-?0x[0-9a-f]+|-?\d+)")
LIT = re.compile(r"(?<![\w.])(0[xX][0-9a-fA-F]+|\d+)([uUlL]*)(?![\w.])")
M32 = 0xFFFFFFFF
PAIRS = 24  # single edits (least harmful first) combined in pairs when none helps alone
WINDOW = 16  # instructions either side of an unequal block whose values are tried


def insns(code, addr):
    """[(address, text, values, symbol)] with the values each instruction
    materializes and the symbol a call or pool load refers to."""
    out = []
    for a, t in check.disasm(code, addr):
        vals = [int(v, 0) & M32 for v in IMM.findall(t)]
        sym = None
        op = t.split()[0]
        m = re.match(r"ldr \w+, \[pc, #(0x[0-9a-f]+|\d+)\]", t)
        if m:
            p = ((a + 4) & ~3) + int(m.group(1), 0) - addr
            if 0 <= p <= len(code) - 4:
                vals = [int.from_bytes(code[p:p + 4], "little")]
                sym = ("pool", vals[0])
        elif op == "bl":
            sym = ("call", vals[0])
            vals = []
        out.append([a, t, vals, sym])
    # constants a register holds, followed through straight-line code, so a
    # value built as movs/lsls/rsbs or stepped with adds/subs (a run of field
    # offsets) shows up as the absolute value it is
    known = {}
    for x in out:
        t = x[1]
        op, args = (t.split(None, 1) + [""])[:2]
        regs = [r.strip() for r in args.split(",")]
        d = regs[0] if regs else ""
        if op in ("bl", "bx", "pop", "push", "svc") or op.startswith("b"):
            known.clear()
            continue
        val = None
        if op == "movs" and len(regs) == 2 and regs[1].startswith("#"):
            val = x[2][0]
        elif op == "ldr" and "pc" in args and x[2]:
            val = x[2][0]
        elif op == "lsls" and len(regs) == 3 and regs[1] in known and regs[2].startswith("#"):
            val = (known[regs[1]] << (x[2][0] & 31)) & M32
        elif op == "rsbs" and len(regs) == 3 and regs[1] in known and regs[2] == "#0":
            val = (-known[regs[1]]) & M32
        elif op in ("adds", "subs") and len(regs) == 2 and d in known and regs[1].startswith("#"):
            k = x[2][0]
            val = (known[d] + k if op == "adds" else known[d] - k) & M32
        elif op in ("adds", "subs") and len(regs) == 3 and regs[1] in known and regs[2].startswith("#"):
            k = x[2][0]
            val = (known[regs[1]] + k if op == "adds" else known[regs[1]] - k) & M32
        if val is not None:
            if val not in x[2]:
                x[2] = x[2] + [val]
            known[d] = val
        elif d in known:
            del known[d]
    return out


def shape(t):
    t = REG.sub("R", t)
    return IMM.sub("#K", t).split(";")[0]


class Repair:
    def __init__(self, path, func):
        self.path = os.path.abspath(path)
        self.func = func
        self.cc = rasolve.Compiler(self.path, func, lines=False)
        self.by_addr = {}
        for name, v in self.cc.symbols.items():
            self.by_addr.setdefault(v[0], name)
            self.by_addr.setdefault(v[0] & ~1, name)
        self.work = os.path.join(ROOT, "build", "repair")
        os.makedirs(self.work, exist_ok=True)

    def compile(self, text):
        tmp = os.path.join(self.work, f"{self.func}.try.c")
        open(tmp, "w").write(text)
        try:
            self.cc.pre, _ = permute.prepare_source(os.path.relpath(tmp, ROOT), self.func)
        except Exception:
            return None, None
        self.cc.cache.clear()  # a cache hit would leave last_asm stale
        d, _ = self.cc.run()
        if d is None or not self.cc.last_asm:
            return None, None
        obj = self.cc.resolver.assemble(self.cc.last_asm, self.cc.work)
        mine, _ = self.cc.resolver.bytes_of(obj, self.func)
        return d, mine

    def rom(self, n):
        a = rasolve.off(self.cc.address)
        return self.cc.rom[a:a + n]

    def pairs(self, mine):
        """Value and symbol substitutions (C side, ROM side) suggested by the
        aligned code, plus the unpaired ROM and C instructions."""
        orig = self.rom(self.cc.size or len(mine))
        A, B = insns(orig, self.cc.address), insns(mine, self.cc.address)
        sm = difflib.SequenceMatcher(None, [shape(x[1]) for x in A], [shape(x[1]) for x in B], autojunk=False)
        vals, syms, extra_rom, extra_c = set(), set(), [], []

        def pair(x, y):
            for u, v in zip(y[2], x[2]):
                if u != v:
                    vals.add((u, v))
            if x[3] and y[3] and x[3] != y[3]:
                s, t = self.by_addr.get(y[3][1]), self.by_addr.get(x[3][1])
                if s and t and s != t:
                    syms.add((s, t))
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            if tag == "equal" or (tag == "replace" and i2 - i1 == j2 - j1):
                for x, y in zip(A[i1:i2], B[j1:j2]):
                    if x[1:] != y[1:]:
                        pair(x, y)
            else:
                # unequal blocks: the ROM may reuse a value it built nearby, so
                # any C value here may want any ROM value close to the block
                near = {v for x in A[max(0, i1 - WINDOW):i2 + WINDOW] for v in x[2]}
                for y in B[j1:j2]:
                    for u in y[2]:
                        vals.update((u, v) for v in near if v != u)
                    for x in A[i1:i2]:
                        if x[3] and y[3] and x[3][0] == y[3][0]:
                            pair(x, y)
                extra_rom += [x for x in A[i1:i2]]
                extra_c += [y for y in B[j1:j2]]
        return vals, syms, extra_rom, extra_c

    def edits(self, body, vals, syms):
        """Candidate edits, each [(start, end, replacement)] over the source:
        one token, or every copy of one value or symbol."""
        out = []
        lits = [(m.start(1), m.end(1), int(m.group(1), 0), m.group(1)) for m in LIT.finditer(body)
                if not body[:m.start()].rsplit("\n", 1)[-1].lstrip().startswith("#")]
        for u, v in vals:
            spans = []
            for s, e, val, raw in lits:
                pre = body[:s].rstrip()
                neg = pre.endswith("-") and not pre.endswith("--")
                cv = (-val if neg else val) & M32
                if cv not in (u, u & 0xFFFF, u & 0xFF) or val == 0 and u != 0:
                    continue
                nv = v if cv == u else (v & 0xFFFF if cv == u & 0xFFFF else v & 0xFF)
                if neg:
                    nv = (-nv) & M32
                    if nv > 0x7FFFFFFF:  # -x is now positive: drop the minus sign
                        nv = (-nv) & M32
                        s = len(pre) - 1
                        neg = False
                new = f"0x{nv:X}" if raw.lower().startswith("0x") else str(nv)
                spans.append((s, e, new))
                out.append((f"{'-' if neg else ''}{raw} -> {new}", [spans[-1]]))
            if len(spans) > 1:
                out.append((f"every {u:#x} -> {v:#x}", spans))
        # a block of fields moved by the same amount: every pair with one delta at once
        by_delta = {}
        for desc, spans in out:
            if len(spans) == 1 and not desc.startswith("every"):
                u, v = desc.split(" -> ")
                if u.startswith("-") or v.startswith("-"):
                    continue
                by_delta.setdefault(int(v, 0) - int(u, 0), {})[spans[0][:2]] = (spans[0], int(u, 0))
        for delta, group in by_delta.items():
            if abs(delta) >= 0x1000:
                continue
            if len(group) > 1:
                out.append((f"every pair moved by {delta:+#x}", [sp for sp, _ in group.values()]))
            # fields addressed relative to their neighbours never show up in
            # the code, so also move every literal inside the pairs' range
            us = [u for _, u in group.values()]
            lo, hi = min(us), max(us)
            if hi > lo:
                inside = [(a, b, (f"0x{val + delta:X}" if raw.lower().startswith("0x") else str(val + delta)))
                          for a, b, val, raw in lits if lo <= val <= hi]
                if len(inside) > len(group):
                    out.append((f"every literal in {lo:#x}..{hi:#x} moved by {delta:+#x}", inside))
        for s, t in syms:
            hits = [(m.start(), m.end(), t) for m in re.finditer(rf"\b{re.escape(s)}\b", body)]
            if hits:
                out.append((f"{s} -> {t}", hits))
        return out

    @staticmethod
    def apply(body, spans):
        for s, e, new in sorted(spans, reverse=True):
            body = body[:s] + new + body[e:]
        return body

    def step(self, text, mine, best):
        """The best improving edit (one, else a pair) as (distance, desc, text, bytes)."""
        vals, syms, _, _ = self.pairs(mine)
        cands = self.edits(text, vals, syms)
        tried, found = [], None
        for desc, spans in cands:
            d, m = self.compile(self.apply(text, spans))
            if d is None:
                continue
            tried.append((d, desc, spans))
            if d < best and (found is None or d < found[0]):
                found = (d, desc, self.apply(text, spans), m)
        if found:
            return found
        # values that only help together (fields swapped, a constant the
        # compiler shares): pairs of the edits that hurt least
        tried.sort(key=lambda x: x[0])
        top = tried[:PAIRS]
        for i in range(len(top)):
            for j in range(i + 1, len(top)):
                (_, d1, s1), (_, d2, s2) = top[i], top[j]
                r1, r2 = {(s, e) for s, e, _ in s1}, {(s, e) for s, e, _ in s2}
                if any(a < f and c < b for a, b in r1 for c, f in r2):
                    continue  # overlapping spans
                cand = self.apply(text, s1 + s2)
                d, m = self.compile(cand)
                if d is not None and d < best and (found is None or d < found[0]):
                    found = (d, f"{d1}, {d2}", cand, m)
        return found

    def run(self, rounds=20):
        text = open(self.path).read()
        best, mine = self.compile(text)
        if best is None:
            sys.exit(f"{self.func}: does not compile")
        start, log = best, []
        for _ in range(rounds):
            if best == 0:
                break
            found = self.step(text, mine, best)
            if not found:
                break
            best, desc, text, mine = found
            log.append((desc, best))
            print(f"  {desc}: {best} halfwords off", flush=True)
        if self.cc.server:
            self.cc.server.close()
        _, _, er, ec = self.pairs(mine) if best else (None, None, [], [])
        return start, best, text, log, er, ec


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("file")
    ap.add_argument("func")
    ap.add_argument("--write", action="store_true", help="rewrite FILE.c")
    ap.add_argument("--rounds", type=int, default=20)
    ap.add_argument("--out-dir", help="write FUNC.c here (default build/repair)")
    a = ap.parse_args()
    r = Repair(a.file, a.func)
    start, best, text, log, er, ec = r.run(a.rounds)
    dst = a.file if a.write else os.path.join(a.out_dir or r.work, f"{a.func}.c")
    if log:
        open(dst, "w").write(text)
    print(f"{a.func}: {start} -> {best} halfwords off" + (f", written to {os.path.relpath(dst, ROOT)}" if log else ""))
    if best:
        def brief(xs):
            return "; ".join(x[1] + (f" ({r.by_addr.get(x[3][1], hex(x[3][1]))})" if x[3] else "") for x in xs[:8])
        if er:
            print(f"  ROM only: {brief(er)}")
        if ec:
            print(f"  C only:   {brief(ec)}")


if __name__ == "__main__":
    main()

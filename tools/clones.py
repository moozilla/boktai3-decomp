#!/usr/bin/env python3
"""Find clone families and port matched C to unmatched siblings.

  clones.py --stats                      family statistics
  clones.py --port [--range START END]   write candidate C for unmatched clones of
                                         matched functions, check each, keep MATCHes

Two functions are clones when their instructions are identical after symbols
(callees, data labels, literal-pool addresses) are abstracted away. A matched
member's C is copied to each unmatched sibling with every referenced symbol and
address renamed in order of appearance; tools/check.py decides. Candidates are
written to wip/clones/ and copied to src/fn/ only on MATCH.
"""
import argparse
import collections
import os
import re
import shutil
import subprocess
import sys

from romlib import ROOT
from worklist import done_names

REGS = {f"r{i}" for i in range(13)} | {"sp", "lr", "pc", "ip", "sl", "sb", "fp"}
TOKEN = re.compile(r"\b(0x[0-9A-Fa-f]{8}|[A-Za-z_]\w*)\b")


def parse():
    """name -> (addr, [normalized lines], [symbol keys in order])"""
    funcs = {}
    cur = None
    for ln in open(os.path.join(ROOT, "gen", "code_sym.s")):
        m = re.match(r"^\t(?:thumb|arm|non_word_aligned_thumb)_func_start (\S+)", ln)
        if m:
            cur = [m.group(1), None, [], []]
            funcs[cur[0]] = cur
            continue
        if cur is None:
            continue
        s = ln.split("@")[0].strip()
        if not s:
            continue
        m = re.match(r"^(\w+):$", s) or re.match(r"^(\w+):\s*(.*)$", s)
        if m and cur[1] is None and m.group(1) == cur[0]:
            a = re.search(r"0x([0-9A-F]{8})", ln)
            cur[1] = int(a.group(1), 16) if a else None
            continue
        if re.match(r"^_0[89][0-9A-F]{6}:$", s):
            cur[2].append("L:")
            continue
        if s.startswith(".incbin") or s.startswith(".byte"):
            cur[2].append("DATA")  # differing trailing data: not a safe clone
            cur[2].append(s)
            continue
        m = re.match(r"^_0[89][0-9A-F]{6}:\s*(.*)$", s)
        if m:
            s = m.group(1)
        op, _, args = s.partition(" ")

        def sub(mt):
            t = mt.group(1)
            if t in REGS or t.startswith("_0"):
                return "L" if t.startswith("_0") else t
            if t.startswith("0x") and op != ".4byte":
                return t
            cur[3].append(t)
            return "S"
        cur[2].append(op + " " + TOKEN.sub(sub, args))
    return {n: (f[1], f[2], f[3]) for n, f in funcs.items() if f[1] is not None}


def key_of(sym):
    """The string to rename in C for a referenced symbol."""
    m = re.match(r"^0x([0-9A-Fa-f]{8})$", sym)
    return m.group(1).upper() if m else sym


def families(funcs):
    fam = collections.defaultdict(list)
    for n, (a, lines, _) in funcs.items():
        if len(lines) >= 3 and not any(l == "DATA" for l in lines):
            fam["\n".join(lines)].append(n)
    return [v for v in fam.values() if len(v) > 1]


def port(src_c, src_name, dst_name, funcs):
    a_syms = [src_name] + funcs[src_name][2]
    b_syms = [dst_name] + funcs[dst_name][2]
    mapping = {}
    for x, y in zip(a_syms, b_syms):
        kx, ky = key_of(x), key_of(y)
        if mapping.get(kx, ky) != ky:
            return None  # inconsistent renaming
        mapping[kx] = ky
    text = open(src_c).read()
    # rename identifiers and 8-digit hex addresses in one pass
    hexmap = {}
    for k, v in mapping.items():
        for pat in (k, k[-8:] if re.search(r"[0-9A-F]{8}$", k) else None):
            if pat and re.fullmatch(r"[0-9A-F]{8}", pat):
                tgt = v[-8:] if re.search(r"[0-9A-F]{8}$", v) else None
                if tgt:
                    if hexmap.get(pat, tgt) != tgt:
                        return None
                    hexmap[pat] = tgt
    names = {k: v for k, v in mapping.items() if not re.fullmatch(r"[0-9A-F]{8}", k)}

    def rep(m):
        t = m.group(0)
        if t in names:
            return names[t]
        h = re.search(r"([0-9A-Fa-f]{8})$", t)
        if h and h.group(1).upper() in hexmap:
            return t[:h.start(1)] + hexmap[h.group(1).upper()]
        return t
    return re.sub(r"\b\w+\b", rep, text)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--stats", action="store_true")
    ap.add_argument("--port", action="store_true")
    ap.add_argument("--range", nargs=2)
    a = ap.parse_args()
    funcs = parse()
    done = done_names()
    fams = families(funcs)
    if a.stats or not a.port:
        n_un = sum(1 for f in fams for n in f if n not in done)
        port_ok = sum(1 for f in fams if any(n in done for n in f) for n in f if n not in done)
        print(f"{len(fams)} clone families covering {sum(map(len, fams))} functions; "
              f"{n_un} unmatched members, {port_ok} in families with a matched member")
        return
    lo, hi = (int(a.range[0], 16), int(a.range[1], 16)) if a.range else (0, 1 << 32)
    srcs = {os.path.basename(p)[:-2]: os.path.join(ROOT, "src", "fn", p)
            for p in os.listdir(os.path.join(ROOT, "src", "fn")) if p.endswith(".c")}
    wip = os.path.join(ROOT, "wip", "clones")
    os.makedirs(wip, exist_ok=True)
    ok = 0
    for f in fams:
        tmpl = [n for n in f if n in srcs]
        if not tmpl:
            continue
        for n in f:
            if n in done or not (lo <= funcs[n][0] < hi):
                continue
            for t in tmpl:
                c = port(srcs[t], t, n, funcs)
                if c is None:
                    continue
                out = os.path.join(wip, n + ".c")
                open(out, "w").write(c)
                r = subprocess.run([sys.executable, "tools/check.py", out, "--quiet"], cwd=ROOT,
                                   capture_output=True, text=True)
                if r.returncode == 0:
                    shutil.copy(out, os.path.join(ROOT, "src", "fn", n + ".c"))
                    ok += 1
                    print(f"MATCH {n} (from {t})")
                    break
    print(f"{ok} functions ported")


if __name__ == "__main__":
    main()

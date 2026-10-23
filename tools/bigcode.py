"""Corpus retrieval for depinning: which C spellings in this codebase produce a given instruction shape.

  python3 tools/bigcode.py mine            # compile every unit with -g (about 10 min on 4 cores)
  python3 tools/bigcode.py query FUNC FILE # FILE: a pin-free copy of FUNC (pinfarm's iso/ copies)

`mine` records, for every function, its instructions as register-masked "shape" tokens with
the C line that produced each one (from -g line info), and indexes 4-grams of the functions
that have no register pins or asm hints. `query` compiles FILE, aligns its shape against the
ROM, and for each window that differs prints the lines that produced it and clean functions
whose code has the ROM's exact shape there, with their C. See notes/depin-search-2026-10-23.md.
"""
import argparse
import collections
import difflib
import os
import pickle
import re
import subprocess
import sys
import tempfile
from multiprocessing import Pool

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
os.chdir(ROOT)
import build  # noqa: E402

INDEX = os.path.join(ROOT, "build", "bigcode", "index.pkl")
REG = re.compile(r"\b(r[0-9]|r1[0-2]|ip|fp|sl)\b")


def mask(ins):
    """objdump instruction text -> shape token (registers, branch targets and lists dropped)."""
    ins = ins.split(";")[0].split("@")[0].strip()
    if not ins:
        return None
    op, _, args = ins.partition("\t")
    op = re.sub(r"\.[nw]$", "", op.strip())
    args = args.strip()
    if re.fullmatch(r"b(l|x|lx)?(eq|ne|cs|cc|hs|lo|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al)?", op):
        return op if op != "bx" else "bx " + REG.sub("R", args)
    if "[pc" in args:
        return op + " R, lit"
    if op in ("push", "pop", "ldmia", "stmia"):
        return op
    return op + " " + REG.sub("R", args)


def objdump_lines(obj):
    out = subprocess.run(["arm-none-eabi-objdump", "-d", "-l", "--no-show-raw-insn", obj],
                         capture_output=True, text=True).stdout
    func, loc, res = None, None, []
    for ln in out.splitlines():
        m = re.match(r"^[0-9a-f]+ <(\w+)>:", ln)
        if m:
            func, loc = m.group(1), None
            continue
        m = re.match(r"^(/?[\w./-]+\.[ch]):(\d+)", ln)
        if m:
            path = m.group(1)
            loc = (os.path.relpath(path, ROOT) if path.startswith("/") else path, int(m.group(2)))
            continue
        m = re.match(r"^\s+([0-9a-f]+):\s+(.*)$", ln)
        if m and func:
            t = mask(m.group(2))
            if t and not t.startswith((".word", ".short")):
                res.append((func, loc, t))
    return res


def compile_lines(unit):
    """[(function, (file, line), token)] for one source file compiled with -g."""
    try:
        pre = build.sh(build.CPP + ["-nostdinc", "-undef"] + build.DEFINES + ["-I", "include", "-iquote", ".", unit])
        r = subprocess.run([build.AGBCC, *build.cflags_for(unit), "-g", "-o", "-"], input=pre, text=True,
                           capture_output=True, timeout=60)
        if r.returncode:
            return []
        with tempfile.TemporaryDirectory() as d:
            s, o = os.path.join(d, "x.s"), os.path.join(d, "x.o")
            open(s, "w").write(r.stdout)
            if subprocess.run(build.AS + ["-o", o, s], capture_output=True).returncode:
                return []
            return objdump_lines(o)
    except Exception:
        return []


def _mine_one(unit):
    import pinfarm
    text = open(unit).read()
    hinted = {name for name, item in pinfarm.definitions(text) if pinfarm.hinted(item)}
    return unit, compile_lines(unit), hinted


def cmd_mine(a, n=4):
    units = sorted(os.path.relpath(os.path.join(d, f), ROOT)
                   for d, _, fs in os.walk("src") for f in fs if f.endswith(".c"))
    seqs, idx = {}, collections.defaultdict(list)
    with Pool(a.jobs) as p:
        for k, (unit, rows, hinted) in enumerate(p.imap_unordered(_mine_one, units, chunksize=8)):
            if k % 500 == 0:
                print(k, len(units), flush=True)
            by = collections.OrderedDict()
            for f, loc, t in rows:
                if loc is not None:
                    by.setdefault(f, []).append((loc, t))
            for f, seq in by.items():
                if f in hinted or len(seq) < n:
                    continue
                seqs[f] = seq
                toks = [t for _, t in seq]
                for i in range(len(toks) - n + 1):
                    idx[tuple(toks[i:i + n])].append((f, i))
    os.makedirs(os.path.dirname(INDEX), exist_ok=True)
    pickle.dump((seqs, dict(idx)), open(INDEX, "wb"))
    print(f"{len(seqs)} clean functions, {len(idx)} distinct 4-grams -> {os.path.relpath(INDEX, ROOT)}")


def rom_tokens(cc):
    """Shape tokens of the ROM function, without literal pools and their alignment padding."""
    import rasolve
    off = rasolve.off(cc.address)
    out = subprocess.run(["arm-none-eabi-objdump", "-D", "-b", "binary", "-marm", "-Mforce-thumb",
                          f"--start-address={off}", f"--stop-address={off + cc.size}", "--no-show-raw-insn",
                          "baserom.gba"], capture_output=True, text=True).stdout
    ins = [(int(m.group(1), 16), m.group(2)) for m in
           (re.match(r"^\s+([0-9a-f]+):\s+(.*)$", ln) for ln in out.splitlines()) if m]
    pool = set()
    for _, txt in ins:
        m = re.search(r"\[pc, #\d+\]\s*@ \(0x([0-9a-f]+)\)", txt)
        if m:
            w = int(m.group(1), 16)
            pool |= {w, w + 2}
    toks = []
    for a, txt in ins:
        if a in pool or a + 2 in pool and re.match(r"movs\s+r0, r0", txt):
            continue
        t = mask(txt)
        if t:
            toks.append(t)
    return toks


def lookup(window, seqs, idx, n=4, limit=6):
    hits = []
    if len(window) < n:
        return hits
    for f, i in idx.get(tuple(window[:n]), []):
        toks = [t for _, t in seqs[f]]
        if toks[i:i + len(window)] == window:
            hits.append((f, seqs[f][i][0], seqs[f][i + len(window) - 1][0]))
            if len(hits) >= limit:
                break
    return hits


def src_line(loc):
    try:
        return open(os.path.join(ROOT, loc[0])).read().splitlines()[loc[1] - 1].strip()
    except Exception:
        return "?"


def cmd_query(a):
    import isolate
    import rasolve
    seqs, idx = pickle.load(open(INDEX, "rb"))
    src = isolate.ensure(os.path.relpath(os.path.abspath(a.file), ROOT), a.func)
    cc = rasolve.Compiler(os.path.join(ROOT, src), a.func, lines=False)
    cand = [(loc, t) for f, loc, t in compile_lines(src) if f == a.func]
    rom = rom_tokens(cc)[:len(cand) + 8]
    sm = difflib.SequenceMatcher(None, rom, [t for _, t in cand], autojunk=False)
    n = 0
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            continue
        n += 1
        lo, hi = max(0, i1 - 2), min(len(rom), i2 + 2)
        print(f"== window {n}: ROM wants  {rom[lo:hi]}")
        print(f"   this file gives {[t for _, t in cand[max(0, j1 - 2):min(len(cand), j2 + 2)]]}")
        for loc in sorted({cand[j][0] for j in range(max(0, j1 - 1), min(len(cand), j2 + 1)) if cand[j][0]}):
            print(f"   from line {loc[1]}: {src_line(loc)}")
        hits = lookup(rom[lo:hi], seqs, idx)
        if not hits and hi - lo > 4:  # shorter: the differing part with one instruction of context
            hits = lookup(rom[max(0, i1 - 1):min(len(rom), i2 + 1)], seqs, idx)
        for f, s, e in hits:
            same = s[0] == e[0] and 0 <= e[1] - s[1] < 8
            txt = " | ".join(src_line((s[0], k)) for k in range(s[1], e[1] + 1)) if same else src_line(s)
            print(f"   corpus {f} ({s[0]}:{s[1]}): {txt}")
        if not hits:
            print("   (no clean function has this exact shape)")
    if not n:
        print("no shape differences: only register allocation differs")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    m = sub.add_parser("mine")
    m.add_argument("-j", "--jobs", type=int, default=os.cpu_count() or 4)
    q = sub.add_parser("query")
    q.add_argument("func")
    q.add_argument("file")
    a = ap.parse_args()
    {"mine": cmd_mine, "query": cmd_query}[a.cmd](a)


if __name__ == "__main__":
    main()

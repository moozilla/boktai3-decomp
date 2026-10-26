#!/usr/bin/env python3
"""The foundry: asm to a committed match, one function at a time, measured as a funnel.

  foundry.py sample [--per-bucket 10] [--seed S] [--max-insns 1500]
                                   stratified sample of never-drafted functions
                                   -> build/foundry/sample.tsv
  foundry.py context FUNC          the worker's context pack -> build/foundry/FUNC/context.md
  foundry.py brief FUNC            the prompt for a model worker (includes the pack's path)
  foundry.py check FILE.c [FUNC]   tools/check.py, logged per attempt (the worker's only check)
  foundry.py finish FUNC           the machine stages after the worker: repair.py, then
                                   rasolve/translate when only registers differ
  foundry.py usage FUNC TOKENS SECONDS   record the worker's cost
  foundry.py report [--markdown]   the funnel per size bucket

Stages, and where each function's state lives (build/foundry/FUNC/):

  1. draft    a worker (a small model) reads context.md and writes
              wip/foundry/FUNC.c; every check goes through `foundry.py check`,
              so checks.tsv has the first draft's score and every later one
  2. repair   tools/repair.py on the worker's best file: the ROM's constants and
              symbols in place of the C's
  3. regs     when the remaining difference is registers only, tools/rasolve.py
              and tools/translate.py (tools/autosolve.py)
  4. commit   a MATCH goes to src/fn/FUNC.c through the usual batch PR

Distances are aligned halfwords (rasolve.distance); "shape" is the same with
registers and immediates abstracted, i.e. what a C change has to fix.
"""
import argparse
import difflib
import glob
import json
import os
import random
import re
import shutil
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402

PY = os.path.join(ROOT, "build", "venv", "bin", "python")
RUN = os.environ.get("FOUNDRY_RUN", "foundry")  # a separate run keeps its own state and drafts
OUT = os.path.join(ROOT, "build", RUN)
WIP = os.path.join(ROOT, "wip", RUN)
BUCKETS = [(0, 50, "<50"), (50, 150, "50-150"), (150, 400, "150-400"), (400, 10 ** 9, "400+")]


def bucket(n):
    return next(name for lo, hi, name in BUCKETS if lo <= n < hi)


def fdir(func):
    d = os.path.join(OUT, func)
    os.makedirs(d, exist_ok=True)
    return d


# ---------------------------------------------------------------- sample

def drafted():
    """Functions someone has already drafted: NONMATCHING blocks, wip/, build/drafts/."""
    names = set()
    for p in glob.glob(os.path.join(ROOT, "wip", "**", "*.c"), recursive=True) + \
            glob.glob(os.path.join(ROOT, "build", "drafts", "*.c")):
        names |= set(re.findall(r"^\w[\w\s\*]*?\b(sub_[0-9A-F]{8}|[A-Z]\w+)\s*\([^;]*$", open(p, errors="replace").read(),
                                re.M))
        names.add(os.path.splitext(os.path.basename(p))[0])
    return names


def sample(a):
    import worklist
    done = worklist.done_names()
    skip = drafted()
    fs = [f for f in worklist.functions() if f["name"] not in done and f["name"] not in skip
          and not worklist.in_library(f["addr"]) and f["addr"] >= 0x0800145C and f["insns"] <= a.max_insns]
    rng = random.Random(a.seed)
    rows = []
    for lo, hi, name in BUCKETS:
        pool = [f for f in fs if lo <= f["insns"] < hi]
        for f in rng.sample(pool, min(a.per_bucket, len(pool))):
            rows.append((f["name"], f"{f['addr']:08X}", f["insns"], name, "JT" if f["jt"] else ""))
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "sample.tsv"), "w") as fh:
        for r in rows:
            fh.write("\t".join(str(x) for x in r) + "\n")
    for r in rows:
        print(*r, sep="\t")


def sample_rows():
    p = os.path.join(OUT, "sample.tsv")
    return [ln.rstrip("\n").split("\t") for ln in open(p)] if os.path.exists(p) else []


# ---------------------------------------------------------------- context

def sh(cmd, timeout=600):
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, timeout=timeout)
    return r.stdout + r.stderr


_decls = None


def declarations():
    """name -> a C declaration of it, from include/ and matched src/."""
    global _decls
    if _decls is not None:
        return _decls
    _decls = {}
    proto = re.compile(r"^((?:extern\s+|static\s+|const\s+|volatile\s+|struct\s+\w+\s*|unsigned\s+|signed\s+)*"
                       r"\w+[\s\*]+)(\w+)\s*(\([^;{]*\)|\[[^\]]*\])\s*;", re.M)
    defn = re.compile(r"^((?:static\s+|const\s+|struct\s+\w+\s*|unsigned\s+)*\w+[\s\*]+)(\w+)\s*(\([^;{)]*\))\s*\n?\{", re.M)
    files = glob.glob(os.path.join(ROOT, "include", "**", "*.h"), recursive=True)
    for p in files:
        for m in proto.finditer(open(p, errors="replace").read()):
            _decls.setdefault(m.group(2), (m.group(0).strip(), os.path.relpath(p, ROOT)))
    for p in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        text = open(p, errors="replace").read()
        if "#ifdef NONMATCHING" in text:
            text = re.sub(r"#ifdef NONMATCHING.*?#endif", "", text, flags=re.S)
        for m in defn.finditer(text):
            if "static" in m.group(1):
                continue
            _decls.setdefault(m.group(2), (f"{m.group(1).strip()} {m.group(2)}{m.group(3)};", os.path.relpath(p, ROOT)))
        for m in proto.finditer(text):
            _decls.setdefault(m.group(2), (m.group(0).strip(), os.path.relpath(p, ROOT)))
    return _decls


def callers(func, limit=3):
    """Matched C that calls func, to show how its arguments are used."""
    out = []
    for p in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        text = open(p, errors="replace").read()
        for m in re.finditer(rf"^.*\b{re.escape(func)}\s*\(.*$", text, re.M):
            ln = m.group(0).strip()
            if not ln.endswith(";") or ln.startswith(("void", "s32", "u32", "u8", "u16", "s16", "s8", "int", "extern")):
                continue
            out.append(f"{ln}    // {os.path.relpath(p, ROOT)}")
            break
        if len(out) >= limit:
            break
    return out


def strong_relatives(text, minimum=40):
    """relatives.py output without relatives that explain under minimum% (a
    shared prologue is noise)."""
    blocks = re.split(r"\n(?=  \S)", text.strip())
    keep = [b for b in blocks[1:] if (m := re.search(r"covers \d+/\d+ \((\d+)%\)", b)) and int(m.group(1)) >= minimum]
    return "\n".join(keep)


def context(func):
    import worklist
    f = next((x for x in worklist.functions() if x["name"] == func), None)
    if not f:
        sys.exit(f"{func}: not a function")
    addr = f"{f['addr']:08X}"
    asm = sh([PY, "tools/asmat.py", addr, str(f["insns"] + 40)])
    # keep this function only (asmat runs on into the next)
    lines, keep = asm.splitlines(), []
    for i, ln in enumerate(lines):
        if i > 1 and re.match(r"\s*(thumb|arm|non_word_aligned_thumb)_func_start", ln):
            break
        keep.append(ln)
    asm = "\n".join(keep)
    names = real_names()
    asm = re.sub(r"\bsub_([0-9A-F]{8})\b", lambda m: names.get(m.group(1), m.group(0))
                 if m.group(0) != func else m.group(0), asm)
    refs = sorted(set(re.findall(r"\b(?:bl|=)\s*([A-Za-z_]\w+)", asm)) - {func})
    decls = declarations()
    known, unknown = [], []
    for r in refs:
        if r.startswith("_0"):
            continue
        (known if r in decls else unknown).append(r)
    rel = strong_relatives(sh([PY, "tools/relatives.py", addr, "-n", "3", "--c"]))
    use = callers(func)
    pack = [f"# {func} at 0x{addr}: {f['insns']} instructions, {f['calls']} calls, {f['branches']} branches"
            + (", jump table" if f["jt"] else ""),
            "",
            "## The ROM's code (tools/asmat.py)",
            "", "```", asm.strip(), "```", ""]
    pack += ["## Declarations of what it calls and loads", "",
             "Copy the ones you use into your file (keep them local), or #include the header named.", "", "```c"]
    for r in known:
        d, where = decls[r]
        pack.append(f"{d}    // {where}")
    for r in unknown:
        kind = "function, signature unknown: infer it" if re.match(r"(sub_|[A-Z])", r) and not r.startswith("gUnk") \
            else "data: declare `extern u8 NAME[];` or a better type"
        pack.append(f"// {r}: {kind}")
    pack += ["```", ""]
    if use:
        pack += ["## How matched code calls this function", "", "```c"] + use + ["```", ""]
    lo, hi, sname = subsystem_of(f["addr"])
    if hi:
        import collections
        inc = collections.Counter()
        for q in glob.glob(os.path.join(ROOT, "src", "fn", "sub_*.c")):
            a = int(os.path.basename(q)[4:12], 16)
            if lo <= a < hi:
                inc.update(re.findall(r'#include "([^"]+\.h)"', open(q, errors="replace").read()))
        inc.pop("global.h", None)
        pack += [f"## Subsystem: {sname} ({lo:08X}-{hi:08X})", "",
                 "Headers its matched functions include (shared types live here; extend them):", ""]
        pack += [f"* include/{h} (used by {n} matched functions)" for h, n in inc.most_common(6)] or ["* (none yet)"]
        pack.append("")
    pack += ["## Matched relatives (tools/relatives.py: matched functions sharing code with this one)", "",
             rel.strip() or "(none)", ""]
    path = os.path.join(fdir(func), "context.md")
    open(path, "w").write("\n".join(pack) + "\n")
    return path, f


BRIEF = """You are a decompilation worker on the Boktai 3 (GBA) decomp in {root}.
Goal: C that compiles to byte-identical code for {func} with agbcc -O2 -mthumb-interwork (gcc 2.95).

Read {context} first: the ROM's assembly, declarations for what it calls, and the C of
matched relatives (start from a relative's C when one covers most of the function).
Read docs/AGBCC_TIPS.md, including "Wrong values in the right shape"
bullet for compiler idioms.

Write your C to wip/{run}/{func}.c: `#include "global.h"`, local declarations, then the one
function {func} (keep the sub_ name). Use the names the context pack shows: the linker
does not resolve sub_XXXXXXXX for a function that has a real name, or D_XXXXXXXX labels. No data definitions, no string literals, no register
pins (`register ... asm("rN")`), no asm() statements.

Check ONLY with:  {env}build/venv/bin/python tools/foundry.py check wip/{run}/{func}.c
It prints the aligned original/candidate listing (`!` marks differences) and MATCH or a fuzzy %.
When the shape is right but values differ, run:
  build/venv/bin/python tools/repair.py wip/{run}/{func}.c {func} --write
(it takes constants, offsets and callees from the ROM; its "ROM only"/"C only" lines name what
is structurally missing). Also useful: build/venv/bin/python tools/explain.py wip/{run}/{func}.c {func}

Register copies are reachable in plain C: do not conclude that a pin is needed. When the ROM
copies a constant or value between registers (`movs r3, #1; adds r1, r3, #0`) or keeps a mask
live (0xFFFF in r5), try, in order: a local holding the constant, declared in a small scope
just before its uses (`{{ u16 one = 1; ... }}`, and its type matters: u16 vs u32 vs s32); a second
variable holding the same value; re-assigning the local before each loop to stop CSE; an
early `return` or `goto` that changes block layout; the order of declarations and of
statements that compute the same value. A value computed separately in each branch usually
means the C repeats the expression in each branch.

Each check also prints a CORE line: the distance with register allocation ignored (copies,
spills and the frame dropped, registers abstracted), and which regions of the function are
LOCKED. On anything over ~60 instructions, steer by CORE, not by the fuzzy %: allocation noise
makes the fuzzy % jump around, while CORE only moves when the C does different work. Lock regions
one at a time. Once CORE is 0, work on registers (the levers above); the machine stages after
you also try register allocation.

Budget: {checks} checks. Get the structure right first (calls, branches, loops), then the values,
then registers. Stop early when you match, or when several checks bring no improvement.
Do not edit anything outside wip/{run}/. Do not commit.
Finish with one line: RESULT {func} MATCH|fuzzy N% checks=K  and a two-sentence note on what
was left or what made it hard.
"""


def brief(func, checks=None):
    path, f = context(func)
    if checks is None:
        checks = 12 if f["insns"] < 150 else 20
    env = f"FOUNDRY_RUN={RUN} " if RUN != "foundry" else ""
    return BRIEF.format(root=ROOT, func=func, context=os.path.relpath(path, ROOT), checks=checks, run=RUN, env=env)


# ---------------------------------------------------------------- check and stages

_names = None


def real_names():
    """address -> the name the linker knows, for functions renamed in symbols/functions.csv"""
    global _names
    if _names is None:
        import csv
        _names = {r["addr"].upper(): r["name"] for r in csv.DictReader(open(os.path.join(ROOT, "symbols", "functions.csv")))}
    return _names


def link_names(path):
    """Address-style names (sub_X, D_X) that check.py resolves but the ROM link does not."""
    text = open(path).read()
    out = []
    for m in sorted(set(re.findall(r"\b(sub|D|gUnk|dword|byte)_([0-9A-Fa-f]{8})\b", text))):
        kind, a = m
        real = real_names().get(a.upper())
        name = f"{kind}_{a}"
        if kind == "sub" and real and real != name:
            out.append(f"{name} -> {real}")
        elif kind in ("D", "dword", "byte"):
            out.append(f"{name} -> gUnk_{a.upper()}")
    return out


def fuzzy_of(text):
    if re.search(r": MATCH \(", text):
        return 100.0
    m = re.search(r"fuzzy ([\d.]+)%", text)
    return float(m.group(1)) if m else None


def check(path, func=None):
    func = func or os.path.splitext(os.path.basename(path))[0]
    t0 = time.time()
    r = subprocess.run([PY, "tools/check.py", path], cwd=ROOT, capture_output=True, text=True)
    text = r.stdout + r.stderr
    fz = fuzzy_of(text)
    d = fdir(func)
    n = len(open(os.path.join(d, "checks.tsv")).readlines()) if os.path.exists(os.path.join(d, "checks.tsv")) else 0
    shutil.copy(path, os.path.join(d, f"try{n + 1:02d}.c"))
    with open(os.path.join(d, "checks.tsv"), "a") as fh:
        fh.write(f"{n + 1}\t{time.time():.0f}\t{fz if fz is not None else 'error'}\t{round(time.time() - t0, 2)}\n")
    print(text, end="")
    if fz != 100.0 and fz is not None:
        try:
            import regions
            print(regions.render(regions.Report(path, func).run(), limit=6))
        except Exception as e:  # the report is advice; never fail the check over it
            print(f"(regions report failed: {e!r})")
    bad = link_names(path)
    if bad:
        print("LINK NAMES: the ROM build will not link these; rename them: " + ", ".join(bad))
        return r.returncode or 3
    return r.returncode


def best_try(func):
    d = fdir(func)
    p = os.path.join(d, "checks.tsv")
    if not os.path.exists(p):
        return None, None
    rows = [ln.split("\t") for ln in open(p)]
    scored = [(float(r[2]), int(r[0])) for r in rows if r[2] != "error"]
    if not scored:
        return None, None
    fz, n = max(scored)
    return os.path.join(d, f"try{n:02d}.c"), fz


def distances(path, func):
    """(halfword distance, shape distance) of a candidate, or (None, None)."""
    import rasolve
    import permute
    import check as chk
    try:
        cc = rasolve.Compiler(os.path.abspath(path), func, lines=False)
        cc.pre, _ = permute.prepare_source(os.path.relpath(os.path.abspath(path), ROOT), func)
        d, _ = cc.run()
        if d is None:
            return None, None
        obj = cc.resolver.assemble(cc.last_asm, cc.work)
        mine, _ = cc.resolver.bytes_of(obj, func)
        orig = cc.rom[rasolve.off(cc.address):rasolve.off(cc.address) + (cc.size or len(mine))]
        if cc.server:
            cc.server.close()
        norm = lambda t: re.sub(r"#(-?0x[0-9a-f]+|-?\d+)", "#K", re.sub(r"\b(r\d+|sb|sl|fp|ip)\b", "R", t))  # noqa
        A = [norm(t) for _, t in chk.disasm(orig, 0)]
        B = [norm(t) for _, t in chk.disasm(mine, 0)]
        sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
        shape = sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != "equal")
        return d, shape
    except BaseException:
        return None, None


def finish(func):
    """Machine stages on the worker's best try; writes stages.json."""
    d = fdir(func)
    st = {"func": func}
    best, fz = best_try(func)
    st["worker_best_fuzzy"] = fz
    p = os.path.join(d, "checks.tsv")
    rows = [ln.split("\t") for ln in open(p)] if os.path.exists(p) else []
    st["checks"] = len(rows)
    first = next((float(r[2]) for r in rows if r[2] != "error"), None)
    st["first_fuzzy"] = first
    st["first_check_errors"] = next((i for i, r in enumerate(rows) if r[2] != "error"), len(rows))
    if not best:
        st["stage"] = "no draft compiled"
        json.dump(st, open(os.path.join(d, "stages.json"), "w"), indent=1)
        return st
    cand = os.path.join(d, "candidate.c")
    shutil.copy(best, cand)
    st["worker_dist"], st["worker_shape"] = distances(cand, func)
    if fz == 100.0:
        st["stage"] = "matched by worker"
    else:
        sh([PY, "tools/repair.py", cand, func, "--write"], timeout=600)
        st["repair_dist"], st["repair_shape"] = distances(cand, func)
        try:
            import regions
            st["core"] = regions.Report(cand, func).run()["core"]
        except Exception:
            st["core"] = None
        if st["repair_dist"] == 0:
            st["stage"] = "matched by repair"
        elif st["repair_dist"] and (st["repair_shape"] == 0 or (st["core"] is not None and st["core"] <= 4)):
            log = sh([PY, "tools/autosolve.py", os.path.relpath(cand, ROOT), func, "--out",
                      os.path.join(d, "autosolve"), "-j", "2", "--solve-seconds", "60",
                      "--translate-seconds", "150"], timeout=1800)
            m = "TRANSLATED" in log or "MATCHES" in log
            st["stage"] = "matched by register solver" if m else (
                "registers only (solver failed)" if st["core"] == 0 else f"core {st['core']} (solver failed)")
            if m:
                hit = glob.glob(os.path.join(d, "autosolve", func, "match-*.c"))
                if hit:
                    shutil.copy(hit[0], cand)
        else:
            s = st["repair_shape"]
            st["stage"] = "shape off by " + ("1-3" if s is not None and s <= 3 else "4-15" if s is not None and s <= 15
                                              else "16+" if s is not None else "?")
    if st["stage"].startswith("matched"):
        r = subprocess.run([PY, "tools/check.py", cand], cwd=ROOT, capture_output=True, text=True)
        if r.returncode != 0 or "MATCH (" not in r.stdout:
            st["stage"] += " (check.py disagrees!)"
    json.dump(st, open(os.path.join(d, "stages.json"), "w"), indent=1)
    return st


def usage(func, tokens, seconds):
    d = fdir(func)
    json.dump({"tokens": int(tokens), "seconds": int(seconds)}, open(os.path.join(d, "usage.json"), "w"))


def report(markdown):
    rows = sample_rows()
    by = {name: [] for _, _, name in BUCKETS}
    for func, addr, insns, bname, jt in rows:
        d = os.path.join(OUT, func)
        st = json.load(open(os.path.join(d, "stages.json"))) if os.path.exists(os.path.join(d, "stages.json")) else None
        us = json.load(open(os.path.join(d, "usage.json"))) if os.path.exists(os.path.join(d, "usage.json")) else {}
        by[bname].append((func, int(insns), st, us))
    hdr = ["bucket", "n", "first draft fuzzy (median)", "worker best (median)", "matched: worker / repair / regs",
           "shape 1-3", "shape 4-15", "shape 16+", "no compile", "checks (mean)", "tokens (mean)", "tokens per match"]
    out = []
    for _, _, bname in BUCKETS:
        xs = [x for x in by[bname] if x[2]]
        if not xs:
            continue
        med = lambda v: sorted(v)[len(v) // 2] if v else None  # noqa
        firsts = [x[2]["first_fuzzy"] for x in xs if x[2].get("first_fuzzy") is not None]
        bests = [x[2]["worker_best_fuzzy"] for x in xs if x[2].get("worker_best_fuzzy") is not None]
        stage = [x[2]["stage"] for x in xs]
        mw = sum(s == "matched by worker" for s in stage)
        mr = sum(s == "matched by repair" for s in stage)
        mg = sum(s == "matched by register solver" for s in stage)
        toks = [x[3].get("tokens", 0) for x in xs if x[3]]
        matched = mw + mr + mg
        out.append([bname, len(xs), med(firsts), med(bests), f"{mw} / {mr} / {mg}",
                    sum(s == "shape off by 1-3" or s.startswith("registers only") for s in stage),
                    sum(s == "shape off by 4-15" for s in stage), sum(s == "shape off by 16+" for s in stage),
                    sum(s == "no draft compiled" for s in stage),
                    round(sum(x[2]["checks"] for x in xs) / len(xs), 1),
                    round(sum(toks) / len(toks)) if toks else "",
                    round(sum(toks) / matched) if toks and matched else ""])
    if markdown:
        print("| " + " | ".join(hdr) + " |")
        print("|" + "---|" * len(hdr))
        for r in out:
            print("| " + " | ".join(str(x) for x in r) + " |")
    else:
        print("\t".join(hdr))
        for r in out:
            print("\t".join(str(x) for x in r))
        for _, _, bname in BUCKETS:
            for func, insns, st, us in by[bname]:
                print(f"  {bname:8s} {func:28s} {insns:5d}  {st['stage'] if st else 'pending'}"
                      + (f"  first {st.get('first_fuzzy')} best {st.get('worker_best_fuzzy')}"
                         f" shape {st.get('repair_shape', st.get('worker_shape'))}" if st else ""))


# ---------------------------------------------------------------- at scale

SCALE = os.path.join(OUT, "scale")


def make_queue(buckets, max_insns):
    """Every never-drafted, unmatched function in the buckets, in ROM order."""
    import worklist
    done = worklist.done_names()
    skip = drafted() | {r[0] for r in sample_rows()}
    want = set(buckets)
    fs = [f for f in worklist.functions() if f["name"] not in done and f["name"] not in skip
          and not worklist.in_library(f["addr"]) and f["addr"] >= 0x0800145C and f["insns"] <= max_insns
          and bucket(f["insns"]) in want]
    os.makedirs(SCALE, exist_ok=True)
    with open(os.path.join(SCALE, "queue.tsv"), "w") as fh:
        for f in fs:
            fh.write(f"{f['name']}\t{f['insns']}\t{bucket(f['insns'])}\n")
    print(f"{len(fs)} functions queued")


BATCH_BRIEF = """You are a decompilation worker on the Boktai 3 (GBA) decomp in {root}.
These functions are a contiguous run in the {subsystem} subsystem ({range}). Work through them
in order, one at a time; neighbours share types and callees, so carry what you learn forward:

{funcs}

For EACH function FUNC:
1. Run: build/venv/bin/python tools/foundry.py brief FUNC
   It writes the context pack and prints the per-function instructions. Follow them exactly
   (its check budget applies to that function alone).
2. Leave the match organized, not just matched:
   * NAME it. At the top of wip/foundry/FUNC.c add `#define Subsystem_Verb FUNC` (the style of
     symbols/functions.csv: e.g. ScenePanel_BeginFade) when you can say what it does; a
     provisional name is fine. Name callees you understood the same way
     (`#define Name sub_XXXXXXXX`), and globals with `#define gName gUnk_XXXXXXXX`. Keep the
     sub_ name as the definition's name (the alias renames it later). Don't invent a name
     you can't support, and never use placeholder words (Something, Stuff, Thing, Do, Handle
     alone): if you can't say what it does, write NAME none.
   * TYPES go in the shared header. If the object it works on has a struct in include/ (the
     context pack lists the headers this subsystem's matched code uses), use it, and add the
     fields you discovered there: only by replacing padding/unknown bytes, never moving an
     existing offset. After editing a header run
       build/venv/bin/python tools/foundry.py hdrcheck include/HEADER.h
     which re-checks every matched file that includes it; undo the edit if anything breaks.
     Don't edit include/global.h or headers outside this subsystem. If the object has no
     header yet, define its struct locally, named after the object (not `struct S1`).
3. Write build/foundry/FUNC/result.txt with these lines:
     RESULT FUNC MATCH checks=K            (or: RESULT FUNC fuzzy N% checks=K)
     NAME Subsystem_Verb low "what it does, in a few words"      (or: NAME none)
     FAMILY <the subsystem or object family it belongs to>
     NOTE <one sentence on what was hard or what is left>
   If FUNC is not a real function (a split error: a lone epilogue tail or prologue head of a
   neighbour), write `RESULT FUNC notafunction checks=0` and move on.
Finish with the RESULT lines, one per function.
"""


_subsys = None


def subsystems():
    global _subsys
    if _subsys is None:
        import csv
        _subsys = [(int(r["start"], 16), int(r["end"], 16), r["name"])
                   for r in csv.DictReader(open(os.path.join(ROOT, "symbols", "progress_subsystems.csv")))]
    return _subsys


def subsystem_of(addr):
    return next(((s, e, n) for s, e, n in subsystems() if s <= addr < e), (0, 0, "unassigned"))


def batch(n, bucket_filter=None, lo=0, hi=1 << 32):
    """Claim the next n queued functions of one subsystem not already being worked on, as a
    contiguous run, and print a batch brief."""
    q = os.path.join(SCALE, "queue.tsv")
    claimed_p = os.path.join(SCALE, "claimed.txt")
    active_p = os.path.join(SCALE, "active.txt")
    claimed = set(open(claimed_p).read().split()) if os.path.exists(claimed_p) else set()
    active = set(open(active_p).read().split("\n")) if os.path.exists(active_p) else set()
    import worklist
    fs = worklist.functions()
    names = {f["name"]: f["addr"] for f in fs}
    by_addr = {f["addr"]: f["name"] for f in fs}
    done = worklist.done_names()
    rows = []
    for ln in open(q):
        r = ln.rstrip("\n").split("\t")
        # names go stale when promote_names renames a callee: resolve by address
        a = names.get(r[0])
        if a is None and re.fullmatch(r"sub_[0-9A-F]{8}", r[0]):
            a = int(r[0][4:], 16)
        cur = by_addr.get(a)
        if cur is None or cur in done:
            continue
        rows.append([cur] + r[1:])
        names[cur] = a
    take, sub = [], None
    for r in rows:
        if r[0] in claimed or (bucket_filter and r[2] != bucket_filter) or not lo <= names[r[0]] < hi:
            continue
        s = subsystem_of(names[r[0]])
        if sub is None:
            if s[2] in active and s[2] != "unassigned":
                continue
            sub = s
        if s != sub:
            if take:
                break
            continue
        take.append(r[0])
        if len(take) >= n:
            break
    if not take:
        sys.exit("queue empty")
    with open(claimed_p, "a") as fh:
        fh.write("\n".join(take) + "\n")
    with open(active_p, "a") as fh:
        fh.write(sub[2] + "\n")
    # several runners claim at once: take the next free number with an exclusive create
    k = len(glob.glob(os.path.join(SCALE, "batch-*.txt"))) + 1
    while True:
        path = os.path.join(SCALE, f"batch-{k:03d}.txt")
        try:
            os.close(os.open(path, os.O_CREAT | os.O_EXCL | os.O_WRONLY))
            break
        except FileExistsError:
            k += 1
    rng = f"{sub[0]:08X}-{sub[1]:08X}" if sub[1] else "no recorded range"
    open(path, "w").write(BATCH_BRIEF.format(root=ROOT, funcs="\n".join(take), subsystem=sub[2], range=rng))
    open(path + ".subsystem", "w").write(sub[2])
    print(path)


def release(path):
    """A batch finished: its subsystem is free for another batch."""
    sp = path + ".subsystem"
    if not os.path.exists(sp):
        return
    name = open(sp).read()
    active_p = os.path.join(SCALE, "active.txt")
    lines = open(active_p).read().split("\n") if os.path.exists(active_p) else []
    if name in lines:
        lines.remove(name)
    open(active_p, "w").write("\n".join(lines))


def hdrcheck(header):
    """Re-check every matched src/ file that includes header; exit 1 if any no longer matches."""
    base = os.path.basename(header)
    files = [p for p in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True)
             if f'"{base}"' in open(p, errors="replace").read() and "INCLUDE_ASM" not in open(p).read()]
    bad = []
    for p in files:
        r = subprocess.run([PY, "tools/check.py", p], cwd=ROOT, capture_output=True, text=True)
        if r.returncode != 0 or "MATCH" not in r.stdout:
            bad.append(os.path.relpath(p, ROOT))
    print(f"{len(files)} files include {base}; " + ("all still match" if not bad else f"BROKEN: {' '.join(bad[:20])}"))
    return 1 if bad else 0


def batch_usage(k, tokens, seconds):
    with open(os.path.join(SCALE, "usage.tsv"), "a") as fh:
        fh.write(f"{k}\t{tokens}\t{seconds}\t{time.time():.0f}\n")


def scale_report():
    """Funnel over the at-scale run: per bucket, and matches per hour / tokens per match."""
    q = os.path.join(SCALE, "queue.tsv")
    rows = [ln.rstrip("\n").split("\t") for ln in open(q)] if os.path.exists(q) else []
    by = {}
    for func, insns, b in rows:
        sp = os.path.join(OUT, func, "stages.json")
        if not os.path.exists(sp):
            continue
        try:
            st = json.load(open(sp))
        except ValueError:
            continue
        x = by.setdefault(b, {"n": 0, "m": 0, "insns": 0, "minsns": 0, "named": 0})
        x["n"] += 1
        x["insns"] += int(insns)
        if st["stage"].startswith("matched") and "disagrees" not in st["stage"]:
            x["m"] += 1
            x["minsns"] += int(insns)
            nm = parse_result(func).get("NAME", "none")
            x["named"] += not nm.startswith("none") and bool(nm)
    up = os.path.join(SCALE, "usage.tsv")
    us = [ln.split("\t") for ln in open(up)] if os.path.exists(up) else []
    def batch_tokens(u):
        j = os.path.join(SCALE, f"batch-{u[0]}.json")
        if os.path.exists(j):
            try:
                d = json.loads(open(j).read().strip().split("\n")[-1])["usage"]
                return d["input_tokens"] + d["output_tokens"] + d.get("cache_creation_input_tokens", 0)
            except Exception:
                pass
        return int(u[1])
    tokens = sum(batch_tokens(u) for u in us)
    cost = sum(float(u[4]) for u in us if len(u) > 4 and u[4].strip())
    times = [float(u[3]) - float(u[2]) for u in us] + [float(u[3]) for u in us]
    hours = (max(times) - min(times)) / 3600 if us else 0
    m = sum(x["m"] for x in by.values())
    for b, x in by.items():
        print(f"{b:8s} finished {x['n']:4d}  matched {x['m']:4d} ({100 * x['m'] / max(x['n'], 1):.0f}%)  "
              f"named {x['named']}/{x['m']}  instructions matched {x['minsns']}/{x['insns']}")
    print(f"batches {len(us)}  tokens {tokens} (non-cached)  headless cost ${cost:.2f}  matches {m}  "
          f"tokens/match {tokens // max(m, 1)}  "
          f"wall {hours:.1f} h  matches/hour {m / hours if hours else 0:.1f}")


def committable():
    """Matched functions not yet in src/: (func, source file)."""
    import csv
    import worklist
    # matched elsewhere meanwhile (a unit run, another session), possibly under a new name:
    # compare addresses, not names
    by_name = {r["name"]: r["addr"].upper() for r in csv.DictReader(open(os.path.join(ROOT, "symbols", "functions.csv")))}
    defined = {f"sub_{by_name.get(n, n[4:] if n.startswith('sub_') else '').upper()}" for n in worklist.done_names()}
    out = []
    seen = set()
    for rp in glob.glob(os.path.join(OUT, "sub_*", "result.txt")):
        func = os.path.basename(os.path.dirname(rp))
        if func in defined:
            continue
        sp = os.path.join(OUT, func, "stages.json")
        try:
            st = json.load(open(sp)) if os.path.exists(sp) else None
        except ValueError:
            st = None
        if st is None:
            # the worker's own MATCH; integrate re-checks the file before committing it
            first = open(rp).readline()
            if not re.match(rf"RESULT {func} MATCH\b", first):
                continue
            st = {"stage": "matched by worker", "checks": len(open(os.path.join(OUT, func, "checks.tsv")).readlines())
                  if os.path.exists(os.path.join(OUT, func, "checks.tsv")) else None}
        if not st["stage"].startswith("matched") or "disagrees" in st["stage"]:
            continue
        if os.path.exists(os.path.join(ROOT, "src", "fn", func + ".c")):
            continue
        src = os.path.join(WIP, func + ".c") if st["stage"] == "matched by worker" else os.path.join(OUT, func, "candidate.c")
        out.append((func, src, st["stage"], st.get("checks")))
    return sorted(out)


def parse_result(func):
    p = os.path.join(OUT, func, "result.txt")
    out = {}
    if os.path.exists(p):
        for ln in open(p):
            k, _, v = ln.strip().partition(" ")
            if k in ("NAME", "FAMILY", "NOTE"):
                out[k] = v.strip()
    return out


def unit_with_placeholder(func):
    for p in glob.glob(os.path.join(ROOT, "src", "*.c")):
        if re.search(rf'INCLUDE_ASM\("asm/nonmatching", {func}\);', open(p).read()):
            return p
    return None


def renamed_refs(text, func):
    """Callees renamed in symbols/functions.csv since the worker wrote this: use the names, and
    drop aliases that the rename made redundant (the linker needs the real names)."""
    names = real_names()
    out = []
    for ln in text.split("\n"):
        m = re.match(r"#define\s+(\w+)\s+sub_([0-9A-F]{8})\s*$", ln)
        if m and f"sub_{m.group(2)}" != func and names.get(m.group(2), "").startswith(("sub_", "")) and \
                names.get(m.group(2)) and names[m.group(2)] != f"sub_{m.group(2)}":
            if names[m.group(2)] == m.group(1):
                continue
            ln = f"#define {m.group(1)} {names[m.group(2)]}"
        out.append(ln)
    text = "\n".join(out)
    return re.sub(r"\bsub_([0-9A-F]{8})\b",
                  lambda m: m.group(0) if m.group(0) == func else names.get(m.group(1), m.group(0)), text)


def drop_conflicting_decls(path):
    """Compile path; remove local prototypes that conflict with a header's; True when it matches."""
    retried = False
    for _ in range(8):
        r = subprocess.run([PY, "tools/check.py", path], cwd=ROOT, capture_output=True, text=True)
        t = r.stdout + r.stderr
        if r.returncode == 0 and "MATCH" in t:
            return True
        bad = set(re.findall(r"conflicting types for `(\w+)'", t))
        if not bad:
            if not retried:  # a check can fail transiently under load; try once more
                retried = True
                time.sleep(5)
                continue
            print(f"  {os.path.basename(path)}: {t.strip().splitlines()[-1][:160] if t.strip() else 'no output'}")
            return False
        s = open(path).read()
        n = s
        for b in bad:
            n = re.sub(rf"^[^\n;{{}}#]*\b{b}\s*\([^;{{]*\)\s*;[ \t]*\n", "", n, count=1, flags=re.M)
        if n == s:
            return False
        open(path, "w").write(n)
    return False


def integrate(dry=False, limit=10 ** 9):
    """Commit each new match, organized: named, placed, family recorded. One commit per function."""
    fam_p = os.path.join(ROOT, "symbols", "foundry_families.csv")
    if not os.path.exists(fam_p):
        open(fam_p, "w").write("addr,function,name,family,note\n")
    done = 0
    for func, src, stage, checks in committable():
        if done >= limit:
            break
        clean = os.path.join(OUT, func, "clean.c")
        if stage != "matched by worker" and os.path.exists(clean):
            src = clean
        if not os.path.exists(src):
            continue
        text = open(src).read()
        if text.lstrip().startswith("typedef"):
            print(f"{func}: {stage}, but the source is preprocessed solver output; "
                  f"carry its edit into build/foundry/{func}/clean.c first")
            continue
        r = parse_result(func)
        name = None
        m = re.match(r"(\w+)\s+(\w+)\s*(.*)", r.get("NAME", ""))
        if m and m.group(1) != "none":
            name = m.group(1)
        if name and not re.search(rf"^#define\s+\w+\s+{func}\b", text, re.M):
            text = re.sub(r'((?:#include[^\n]*\n)+)', rf"\1#define {name} {func}\n", text, count=1)
        text = renamed_refs(text, func)
        alias = re.search(rf"^#define\s+(\w+)\s+{func}\b", text, re.M)
        name = alias.group(1) if alias else None
        placed = None
        unit = unit_with_placeholder(func)
        if unit and not dry:
            old = open(unit).read()
            body = "\n".join(ln for ln in text.split("\n") if not ln.startswith("#include"))
            open(unit, "w").write(old.replace(f'INCLUDE_ASM("asm/nonmatching", {func});', body.strip()))
            ok = subprocess.run([PY, "tools/check.py", unit], cwd=ROOT, capture_output=True, text=True)
            if ok.returncode == 0:
                placed = unit
            else:
                open(unit, "w").write(old)
        if not placed:
            placed = os.path.join(ROOT, "src", "fn", func + ".c")
            if not dry:
                open(placed, "w").write(text)
                if not drop_conflicting_decls(placed):
                    os.remove(placed)
                    print(f"{func}: no longer matches as src/fn; skipped")
                    continue
        addr = func[4:]
        fam = r.get("FAMILY", "")
        note = (r.get("NAME", "").split(None, 2)[2:] or [""])[0].strip('"')
        if not dry:
            with open(fam_p, "a") as fh:
                fh.write(f'{addr},{func},{name or ""},"{fam}","{note}"\n')
        how = {"matched by worker": "a foundry worker", "matched by repair": "a foundry worker plus tools/repair.py",
               "matched by register solver": "a foundry worker plus tools/autosolve.py"}.get(stage, stage)
        msg = f"Match {func}" + (f" ({name})" if name else "")
        body = f"From the foundry: {how}, {checks} checks." + (f" Family: {fam}." if fam else "") + \
            (f" {note}." if note else "") + (f" Placed in {os.path.relpath(placed, ROOT)}." if unit and placed == unit else "")
        print(f"{msg} -> {os.path.relpath(placed, ROOT)}")
        if not dry:
            subprocess.run(["git", "add", placed, fam_p], cwd=ROOT, check=True)
            subprocess.run(["git", "commit", "-q", "-m", msg, "-m", body, "-m",
                            "Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"], cwd=ROOT, check=True)
        done += 1
    print(f"{done} integrated")


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("sample")
    s.add_argument("--per-bucket", type=int, default=10)
    s.add_argument("--seed", default="foundry-2026-10-17")
    s.add_argument("--max-insns", type=int, default=1500)
    sub.add_parser("context").add_argument("func")
    b = sub.add_parser("brief")
    b.add_argument("func")
    b.add_argument("--checks", type=int)
    c = sub.add_parser("check")
    c.add_argument("file")
    c.add_argument("func", nargs="?")
    sub.add_parser("finish").add_argument("func")
    u = sub.add_parser("usage")
    u.add_argument("func")
    u.add_argument("tokens")
    u.add_argument("seconds")
    qp = sub.add_parser("queue")
    qp.add_argument("--buckets", default="<50,50-150")
    qp.add_argument("--max-insns", type=int, default=150)
    bp = sub.add_parser("batch")
    bp.add_argument("n", type=int)
    bp.add_argument("--bucket")
    bp.add_argument("--range", nargs=2, help="START END (hex)")
    bu = sub.add_parser("batch-usage")
    bu.add_argument("k")
    bu.add_argument("tokens")
    bu.add_argument("seconds")
    sub.add_parser("scale-report")
    sub.add_parser("committable")
    ip = sub.add_parser("integrate")
    ip.add_argument("--dry", action="store_true")
    ip.add_argument("--limit", type=int, default=10 ** 9)
    sub.add_parser("hdrcheck").add_argument("header")
    r = sub.add_parser("report")
    r.add_argument("--markdown", action="store_true")
    a = ap.parse_args()
    if a.cmd == "sample":
        sample(a)
    elif a.cmd == "context":
        print(context(a.func)[0])
    elif a.cmd == "brief":
        print(brief(a.func, a.checks))
    elif a.cmd == "check":
        sys.exit(check(a.file, a.func))
    elif a.cmd == "finish":
        print(json.dumps(finish(a.func), indent=1))
    elif a.cmd == "usage":
        usage(a.func, a.tokens, a.seconds)
    elif a.cmd == "queue":
        make_queue(a.buckets.split(","), a.max_insns)
    elif a.cmd == "batch":
        batch(a.n, a.bucket, *([int(x, 16) for x in a.range] if a.range else []))
    elif a.cmd == "batch-usage":
        batch_usage(a.k, a.tokens, a.seconds)
    elif a.cmd == "scale-report":
        scale_report()
    elif a.cmd == "hdrcheck":
        sys.exit(hdrcheck(a.header))
    elif a.cmd == "integrate":
        integrate(a.dry, a.limit)
    elif a.cmd == "committable":
        for row in committable():
            print(*row, sep="\t")
    else:
        report(a.markdown)


if __name__ == "__main__":
    main()

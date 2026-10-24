#!/usr/bin/env python3
"""Try the matched corpus's spellings of each inline helper a draft calls.

  helperswap.py [--files SRC ...] [--out FILE] [-j N]

A helper's spelling decides much of the code around its call (its return
width decides the mode of the expression it feeds, its parameters which
values are copied first, its body the order of loads), and the same helper
often recurs: the random-table step, a flag take, a sign-preserving shift.
Matched C shows how the original spelled it. For each draft (tools/split.py's
NONMATCHING blocks), every static inline helper it calls is keyed by what it
refers to (globals, callees, constants other than 0 and 1); each matched
helper in src/ with the same key and the same number of parameters is put in
its place under the draft's name, compiled and scored against the ROM like
tools/fix.py does. Example: the random step returning s32 instead of u16
(sub_08239D38) matched CallbackPool_SpawnHitParticles (agbccsrc.py
reload-mode).

Results go to --out (jsonl: start, best, helper, the corpus helper used);
an improved draft is written whole to build/helperswap/FUNC.c (.closer.c if
not a match). Follow a closer one with tools/fix.py.
"""
import argparse
import glob
import json
import os
import re
import sys
import tempfile
import time
from concurrent.futures import ProcessPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fix  # noqa: E402  (puts the permuter on sys.path)
import isolate  # noqa: E402
from romlib import ROOT  # noqa: E402
from split import nonmatching_functions  # noqa: E402

OUT = os.path.join(ROOT, "build", "helperswap")
HELPER = re.compile(r"^static inline [^\n;{]*?\b(\w+)\s*\(([^)]*)\)\s*\n\{\n.*?^\}", re.M | re.S)
SYMBOL = re.compile(r"\b(?:gUnk_[0-9A-Fa-f]+|g[A-Z]\w+|sub_[0-9A-F]{8}|[A-Z][A-Za-z0-9]*_[A-Z]\w+)\b")
NUMBER = re.compile(r"\b(?:0x[0-9a-fA-F]+|\d+)\b")


def key(name, text):
    """(symbols, constants) a helper refers to, its own name left out."""
    body = text[text.index("{"):]
    syms = sorted(set(SYMBOL.findall(body)) - {name})
    consts = sorted({str(int(c, 16) if c.lower().startswith("0x") else int(c)) for c in NUMBER.findall(body)}
                    - {"0", "1"})
    return tuple(syms), tuple(consts)


def nparams(params):
    params = params.strip()
    return 0 if params in ("", "void") else params.count(",") + 1


def matched_text(text):
    """TEXT without its NONMATCHING blocks."""
    return re.sub(r"#ifdef NONMATCHING\n.*?#else", "", text, flags=re.S)


def corpus():
    """{(key, nparams): [(file, name, text)]} over the matched C in src/."""
    out = {}
    for f in sorted(glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True)):
        for m in HELPER.finditer(matched_text(open(f).read())):
            k = key(m.group(1), m.group(0))
            if k[0]:
                out.setdefault((k, nparams(m.group(2))), []).append(
                    (os.path.relpath(f, ROOT), m.group(1), m.group(0)))
    return out


def rename(text, old, new):
    return re.sub(r"\b" + re.escape(old) + r"\b", new, text)


def variants(d, table):
    """(helper, corpus file, corpus name, raw text renamed to the helper) for D's helpers."""
    seen = set()
    for name in d.helpers:
        mine = fix.ast_util.to_c(d.helpers[name])
        m = HELPER.search(mine.replace("inline static", "static inline"))
        if not m:
            continue
        k = (key(name, m.group(0)), nparams(m.group(2)))
        norm_mine = re.sub(r"\s+", " ", rename(m.group(0), name, "H"))
        for f, cname, text in table.get(k, []):
            raw = rename(text, cname, name)
            norm = re.sub(r"\s+", " ", rename(text, cname, "H"))
            if norm == norm_mine or (name, norm) in seen:
                continue
            seen.add((name, norm))
            yield name, f, cname, raw


def source_with(d, name, raw):
    """D's source with helper NAME spelled as RAW."""
    out = []
    for seg, h in d.segments:
        out.append(seg)
        out.append(raw if h == name else fix.ast_util.to_c(d.helpers[h]))
    out.append(d.tail)
    if d.need_take:
        out.append(fix.TAKE)
    out.append(fix.ast_util.to_c(d.fn))
    return "\n".join(out)


def score(d, src):
    sc = d.score_source(src)
    return sc and sc[0]


def swap_one(job):
    src, func, table = job
    t0 = time.time()
    row = {"src": src, "func": func}
    try:
        iso = isolate.ensure(src, func)
        text = open(os.path.join(ROOT, iso)).read()
        with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as wd:
            d = fix.Draft(text, func, wd)
            base = d.score()
            row["start"] = base and base[0]
            best = None
            tried = 0
            for name, f, cname, raw in variants(d, table):
                tried += 1
                sc = score(d, source_with(d, name, raw))
                if sc is not None and base and sc < base[0] and (best is None or sc < best[0]):
                    best = (sc, name, f"{f}:{cname}", raw)
                if sc == 0:
                    break
            row["tried"] = tried
            if best:
                row.update(best=best[0], helper=best[1], used=best[2])
                span = fix.find_def(text, best[1])
                if span:
                    out = text[:span[0]] + best[3] + "\n" + text[span[1]:]
                else:
                    # a header's helper: a local copy under another name, called by the draft
                    new = best[1] + "_"
                    fspan = fix.find_def(text, d.src_name)
                    body = rename(text[fspan[0]:fspan[1]], best[1], new)
                    out = (text[:fspan[0]] + rename(best[3], best[1], new) + "\n\n" + body
                           + text[fspan[1]:])
                if out:
                    name = func + (".c" if best[0] == 0 else ".closer.c")
                    open(os.path.join(OUT, name), "w").write(out)
    except (Exception, SystemExit) as e:
        row["error"] = f"{type(e).__name__}: {str(e)[:160]}"
    row["seconds"] = round(time.time() - t0, 1)
    return row


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--files", nargs="*")
    ap.add_argument("--out", default=os.path.join(OUT, "swap.jsonl"))
    ap.add_argument("-j", type=int, default=1)
    a = ap.parse_args()
    os.makedirs(OUT, exist_ok=True)
    table = corpus()
    files = a.files or sorted(glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True))
    jobs = []
    for path in files:
        text = open(path).read()
        if "#ifdef NONMATCHING" not in text:
            continue
        for name, _ in nonmatching_functions(path):
            jobs.append((os.path.relpath(path, ROOT), name, table))
    done = set()
    if os.path.exists(a.out):
        done = {json.loads(ln)["func"] for ln in open(a.out)}
    jobs = [j for j in jobs if j[1] not in done]
    print(f"{sum(len(v) for v in table.values())} matched helpers; {len(jobs)} drafts", flush=True)
    with ProcessPoolExecutor(a.j) as pool, open(a.out, "a") as fh:
        for row in pool.map(swap_one, jobs):
            fh.write(json.dumps(row) + "\n")
            fh.flush()
            if row.get("best") is not None:
                print(f"{row['func']:44} {row['start']:5} -> {row['best']:5}  {row['helper']} as {row['used']}",
                      flush=True)


if __name__ == "__main__":
    main()

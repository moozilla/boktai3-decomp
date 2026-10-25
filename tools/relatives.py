#!/usr/bin/env python3
"""Matched functions that share code with an unmatched one.

  relatives.py ADDR|NAME [-n 5]        the closest matched relatives and the runs they share
  relatives.py ADDR --c                also print each relative's C
  relatives.py --report [--min 0.5]    every unmatched function with a strong relative,
                                       best first (docs/tasks/relatives.md with --markdown)
  relatives.py --port [--min 0.9] [--keep]
                                       write each strong relative's C as a candidate for the
                                       unmatched function (wip/relatives/), check it, and with
                                       --keep copy MATCHes to src/fn/ (as clones.py does)

clones.py ports C between functions whose instructions are identical once
symbols are abstracted. Most unmatched functions have no clone, but many share
long runs with something already matched: an inline helper both expand (the
packed-command sprite stepper, flag tests, angle wraps), a state handler
copied with one field changed, the next variant of a family. Those runs are
the C a worker can start from.

Instructions are compared as tokens: the mnemonic, registers replaced by `r`,
immediates and offsets kept, branch targets dropped, call targets and
literal-pool symbols kept by name. A relative's score is the share of the
target's 6-instruction windows that also occur in it (containment, so a small
helper inside a big target scores by how much of the target it explains, and
a big function that contains the target scores high). Runs are maximal
stretches of identical tokens of at least 8 instructions, given as
instruction ranges in both functions.
"""
import argparse
import collections
import glob
import os
import re
import sys

from romlib import ROOT
from worklist import done_names as _done_by_name

_done = None


def done_names():
    """gen/code.s names (sub_XXXXXXXX) of matched functions. A C file may
    define a matched function under its real name only, which the name-based
    worklist.done_names misses; tools/progress.py decides by address."""
    global _done
    if _done is None:
        sys.path.insert(0, os.path.join(ROOT, "tools", "assets"))
        import script_targets
        _done = set(_done_by_name()) | {f"sub_{a:08X}" for a, (_, ok, _, _) in script_targets.progress().items() if ok}
    return _done

K = 6
MIN_RUN = 8
REG = re.compile(r"\b(r\d{1,2}|sb|sl|fp|ip|sp|lr|pc)\b")


def names():
    out = {}
    for ln in open(os.path.join(ROOT, "symbols", "functions.csv")):
        p = ln.split(",")
        if len(p) > 1 and re.match(r"^[0-9A-F]{8}$", p[0]):
            out["sub_" + p[0]] = p[1]
    return out


def token(line, nm):
    s = line.split("@")[0].strip()
    if not s or s.endswith(":") or s.startswith("."):
        return None
    m = re.match(r"(\w+)\s*(.*)", s)
    op, args = m.group(1), m.group(2)
    if op in ("b", "bl", "blx", "bx") or op.startswith("b") and len(op) <= 3 and op not in ("bic",):
        if op == "bl":
            return f"bl {nm.get(args, args)}"
        return op if op != "bx" else "bx"
    lit = re.search(r"@ =(\S+)", line)
    if lit:  # a literal-pool load: keep what it loads
        v = lit.group(1)
        return f"{op} r, ={nm.get(v, v)}"
    return f"{op} {REG.sub('r', args)}"


_funcs = None


def functions():
    """{name: (addr, [token], [raw line])} for every function in gen/code.s."""
    global _funcs
    if _funcs is not None:
        return _funcs
    from split import FUNC_LABEL, extra_function_starts, function_nonentries
    nm = names()
    nonentries = function_nonentries()
    extra = extra_function_starts()  # inventory starts left as plain labels
    out, cur, prev, fresh = {}, None, None, False
    for ln in open(os.path.join(ROOT, "gen", "code.s")):
        m = re.match(r"_([0-9A-F]{8}):", ln)
        if m and int(m.group(1), 16) in extra:
            prev, cur, fresh = cur, "sub_" + m.group(1), False
            out[cur] = [int(m.group(1), 16), [], []]
            continue
        m = re.match(r"\s*(?:non_word_aligned_)?(?:thumb|arm)_func_start (\w+)", ln)
        if m:
            prev, cur, fresh = cur, m.group(1), True
            out[cur] = [int(cur[4:], 16) if cur.startswith("sub_") else 0, [], []]
            continue
        if fresh:
            fresh = False
            m = FUNC_LABEL.match(ln)
            if m and prev and int(m.group(2), 16) in nonentries:
                # Interior label (not in the inventory): the body
                # continues the previous function.
                del out[cur]
                cur = prev
                continue
        if cur and ln.startswith("\t") and not ln.startswith("\t."):
            t = token(ln, nm)
            if t:
                out[cur][1].append(t)
                out[cur][2].append(ln.rstrip())
    _funcs = out
    return out


def grams(toks):
    return [hash(tuple(toks[i:i + K])) for i in range(len(toks) - K + 1)]


_index = None


def index():
    """(k-gram -> set of matched names, matched name -> gram set)."""
    global _index
    if _index is None:
        fs, done = functions(), done_names()
        inv, sets = collections.defaultdict(set), {}
        for n, (_, toks, _) in fs.items():
            if n in done and len(toks) >= K:
                g = set(grams(toks))
                sets[n] = g
                for x in g:
                    inv[x].add(n)
        _index = (inv, sets)
    return _index


def runs(a, b, min_run=MIN_RUN):
    """Maximal shared stretches [(i, j, length)] of token lists a and b."""
    at = collections.defaultdict(list)
    for j in range(len(b) - K + 1):
        at[tuple(b[j:j + K])].append(j)
    out, covered = [], set()
    for i in range(len(a) - K + 1):
        for j in at.get(tuple(a[i:i + K]), ()):
            if (i, j) in covered or (i > 0 and j > 0 and a[i - 1] == b[j - 1]):
                continue
            n = 0
            while i + n < len(a) and j + n < len(b) and a[i + n] == b[j + n]:
                covered.add((i + n, j + n))
                n += 1
            if n >= min_run:
                out.append((i, j, n))
    return sorted(out, key=lambda r: -r[2])


def relatives(name, n=5):
    """Up to n matched relatives, chosen greedily by how many more of the
    target's instructions each explains; exact clones of a chosen relative
    are listed with it instead of taking a place."""
    fs = functions()
    inv, sets = index()
    toks = fs[name][1]
    g = grams(toks)
    if not g:
        return []
    count = collections.Counter()
    for x in set(g):
        for m in inv.get(x, ()):
            if m != name:
                count[m] += 1
    total = len(set(g))
    cands, seen = [], {}
    for m, c in count.most_common(60):
        key = tuple(fs[m][1])
        if key in seen:
            seen[key]["clones"].append(m)
            continue
        rs = runs(toks, fs[m][1])
        cover = {i + k for i, _, ln in rs for k in range(ln)}
        r = {"name": m, "score": c / total, "covered": len(cover), "cover": cover, "insns": len(fs[m][1]),
             "runs": rs, "clones": []}
        seen[key] = r
        cands.append(r)
    out, have = [], set()
    while cands and len(out) < n:
        best = max(cands, key=lambda r: (len(r["cover"] - have), r["score"]))
        if not best["cover"] - have and out:
            break
        best["adds"] = len(best["cover"] - have)
        have |= best["cover"]
        out.append(best)
        cands.remove(best)
    for r in out:
        del r["cover"]
    return out


def c_file(name):
    nm = names()
    p = os.path.join(ROOT, "src", "fn", f"{name}.c")
    if os.path.exists(p):
        return p
    pretty = nm.get(name, name)
    for f in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        t = open(f).read()
        if re.search(r"\b(" + re.escape(name) + "|" + re.escape(pretty) + r")\s*\([^;{]*\)\s*\{", t):
            return f
    return None


def c_body(name):
    f = c_file(name)
    if not f:
        return None, None
    t = open(f).read()
    pretty = names().get(name, name)
    m = re.search(r"^[^\n;{}]*\b(" + re.escape(name) + "|" + re.escape(pretty) + r")\s*\([^;{]*\)\s*\{", t, re.M)
    if not m:
        return f, None
    depth, i = 0, t.index("{", m.start())
    for k in range(i, len(t)):
        depth += {"{": 1, "}": -1}.get(t[k], 0)
        if depth == 0:
            return f, t[m.start():k + 1]
    return f, None


def resolve(arg):
    fs = functions()
    if arg in fs:
        return arg
    a = arg.upper().removeprefix("0X").removeprefix("SUB_")
    if f"sub_{a}" in fs:
        return f"sub_{a}"
    rev = {v: k for k, v in names().items()}
    if arg in rev and rev[arg] in fs:
        return rev[arg]
    sys.exit(f"no function {arg}")


def show(name, n, with_c):
    fs, nm = functions(), names()
    toks = fs[name][1]
    print(f"{name} ({nm.get(name, name)}): {len(toks)} instructions")
    rs = relatives(name, n)
    if not rs:
        print("  no matched relative shares 6 instructions in a row")
    for r in rs:
        f = c_file(r["name"])
        cl = f" (and {len(r['clones'])} identical: {', '.join(r['clones'][:3])}{'...' if len(r['clones']) > 3 else ''})" if r["clones"] else ""
        print(f"\n  {r['name']} ({nm.get(r['name'], r['name'])}), {r['insns']} instructions{cl}: "
              f"covers {r['covered']}/{len(toks)} ({100 * r['covered'] // max(len(toks), 1)}%), "
              f"{r['adds']} not covered above  {os.path.relpath(f, ROOT) if f else ''}")
        for i, j, ln in r["runs"][:6]:
            print(f"    target {i}-{i + ln - 1}  =  its {j}-{j + ln - 1}  ({ln} insns): "
                  f"{' ; '.join(toks[i:i + min(ln, 4)])}{' ...' if ln > 4 else ''}")
        if with_c:
            path, body = c_body(r["name"])
            if body:
                print("\n" + "\n".join("      " + x for x in body.split("\n")))


def report(minimum, markdown):
    fs, done, nm = functions(), done_names(), names()
    rows = []
    for name, (addr, toks, _) in fs.items():
        if name in done or len(toks) < 2 * K or not name.startswith("sub_"):
            continue
        rs = relatives(name, 1)
        if rs:
            r = rs[0]
            frac = r["covered"] / len(toks)
            if frac >= minimum:
                rows.append((frac, name, len(toks), r))
    rows.sort(key=lambda x: (-x[0], -x[2]))
    if markdown:
        print("# Unmatched functions with matched relatives\n")
        print("Generated by `python3 tools/relatives.py --report --markdown`. Each row is an")
        print("unmatched function and the matched function that shares the most of its")
        print("instructions in runs of 8 or more: start from that C (`relatives.py ADDR --c`).\n")
        print("| Function | Insns | Relative | Covered | Longest run |")
        print("|---|---|---|---|---|")
    for frac, name, n, r in rows:
        longest = r["runs"][0][2] if r["runs"] else 0
        if markdown:
            print(f"| `{name[4:]}` {nm.get(name, '') if not nm.get(name, name).startswith('sub_') else ''} | {n} | "
                  f"`{nm.get(r['name'], r['name'])}` | {100 * frac:.0f}% | {longest} |")
        else:
            print(f"{name}  {n:5d} insns  {100 * frac:3.0f}% from {nm.get(r['name'], r['name'])} (longest run {longest})")
    print(f"\n{len(rows)} unmatched functions with a relative covering at least {100 * minimum:.0f}%"
          if not markdown else f"\n{len(rows)} functions.")


def candidate(name, rel):
    """The relative's C file, renamed to define name; None if the relative
    does not live alone in src/fn/."""
    path = os.path.join(ROOT, "src", "fn", f"{rel}.c")
    pretty = names().get(rel, rel)
    if not os.path.exists(path):
        f = c_file(rel)
        if not f or not os.path.basename(f).startswith("sub_"):
            return None
        path = f
    text = open(path).read()
    if text.count("INCLUDE_ASM") or "#ifdef NONMATCHING" in text:
        return None
    # the definition's name (a real name, its sub_ label, or a #define alias)
    # any `#define Alias rel` (the file's own alias may differ from functions.csv)
    aliases = set(re.findall(r"^#define\s+(\w+)\s+" + re.escape(rel) + r"\s*$", text, flags=re.M))
    text = re.sub(r"^#define\s+\w+\s+" + re.escape(rel) + r"\s*$", "", text, flags=re.M)
    for old in {pretty, rel} | aliases:
        text = re.sub(r"\b" + re.escape(old) + r"\b(?=\s*\()", name, text)
    return text


_loose = None


def loose_port(name, rel):
    """clones.port with immediates mapped, when the two functions have the same
    instructions apart from registers and immediates; else None."""
    global _loose
    import clones
    if _loose is None:
        clones.LOOSE = True
        _loose = clones.parse()
    nm = names()
    a, b = nm.get(rel, rel), name
    if a not in _loose or b not in _loose:
        return None
    norm = lambda L: [REG.sub("r", x) for x in L]  # noqa: E731
    if norm(_loose[a][1]) != norm(_loose[b][1]):
        return None
    src = c_file(rel)
    return clones.port(src, a, b, _loose) if src else None


def port(minimum, keep):
    import shutil
    import subprocess
    fs, done, nm = functions(), done_names(), names()
    wip = os.path.join(ROOT, "wip", "relatives")
    os.makedirs(wip, exist_ok=True)
    rows = []
    for name, (addr, toks, _) in sorted(fs.items()):
        if name in done or len(toks) < 2 * K or not name.startswith("sub_"):
            continue
        rs = relatives(name, 1)
        if not rs or rs[0]["covered"] / len(toks) < minimum:
            continue
        r = rs[0]
        for rel in [r["name"]] + r["clones"]:
            c = candidate(name, rel)
            if c:
                break
        if not c:
            rows.append((name, rel, "no single-function C to port"))
            continue
        out = os.path.join(wip, name + ".c")
        verdict = None
        for attempt in ("plain", "constants"):
            if attempt == "constants":
                # the same code with other constants (field offsets, ids):
                # clones.py's renaming with immediates, registers abstracted
                c = loose_port(name, rel)
                if not c:
                    break
            open(out, "w").write(c)
            res = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "check.py"), out], cwd=ROOT,
                                 capture_output=True, text=True)
            text = res.stdout + res.stderr
            if res.returncode == 0 and f"{name}: MATCH" in text:  # the target, not the relative
                verdict = "MATCH" + (" (constants renamed)" if attempt == "constants" else "")
                dest = os.path.join(ROOT, "src", "fn", name + ".c")
                if keep and not os.path.exists(dest):  # never replace existing C
                    shutil.copy(out, dest)
                break
            m = re.search(r"fuzzy ([\d.]+)%", text)
            v = f"fuzzy {m.group(1)}%" if m else "does not compile: " + (text.strip().split("\n") or [""])[-1][:80]
            verdict = v if verdict is None or (m and v > verdict) else verdict
        rows.append((name, rel, verdict))
        print(f"{name}  from {nm.get(rel, rel):40s} {verdict}", flush=True)
    n = sum(1 for r in rows if r[2].startswith("MATCH"))
    print(f"\n{n} MATCH of {len(rows)} candidates" + (" (copied to src/fn/)" if keep and n else ""))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("func", nargs="?")
    ap.add_argument("-n", type=int, default=5)
    ap.add_argument("--c", action="store_true")
    ap.add_argument("--report", action="store_true")
    ap.add_argument("--min", type=float, default=0.5)
    ap.add_argument("--markdown", action="store_true")
    ap.add_argument("--port", action="store_true")
    ap.add_argument("--keep", action="store_true")
    a = ap.parse_args()
    if a.port:
        port(a.min if a.min != 0.5 else 0.9, a.keep)
    elif a.report:
        report(a.min, a.markdown)
    elif a.func:
        show(resolve(a.func), a.n, a.c)
    else:
        ap.print_help()


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Name matched functions that still carry sub_XXXXXXXX labels, in batches, by model workers.

  namer.py batches [--size 20]        contiguous ROM-order runs of matched sub_ functions
                                      (split at subsystem edges) -> build/namer/batches.tsv
  namer.py pack N                     batch N's context pack -> build/namer/N/context.md
  namer.py run [--jobs 8] [--engine codex|claude] [--model M] [--only N,M] [--limit K]
                                      one headless worker per pending batch (default
                                      `codex exec -m gpt-6.1-sol` at low effort, read-only: no Claude usage);
                                      its answer -> build/namer/N/names.csv
  namer.py apply [--dry-run]          validate every answer and add `#define Name sub_X`
                                      aliases to the defining C files; then run
                                      tools/promote_names.py and make
  namer.py report                     batches done, names accepted and rejected

The pack gives each function its C, who calls it, what it calls, who takes its
address (callbacks), the subsystem range it sits in, named neighbours (for a
consistent prefix) and what the docs say about it (tools/docindex.py). Names
go in as confidence=low with the worker's one-line note, as CLAUDE.md asks:
useful provisional names, revised later.
"""
import argparse
import csv
import glob
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402
from split import c_functions  # noqa: E402

OUT = os.path.join(ROOT, "build", "namer")
SUB = re.compile(r"^sub_([0-9A-F]{8})$")
IDENT = re.compile(r"^[A-Z][A-Za-z0-9]*(_[A-Za-z0-9]+)+$")


def code_graph():
    """Calls and address-takes from gen/code_sym.s: {name: (addr, callees, pool refs)}."""
    funcs, cur = {}, None
    for line in open(os.path.join(ROOT, "gen", "code_sym.s")):
        m = re.match(r"\t(?:arm|thumb|non_word_aligned_thumb)_func_start (\w+)", line)
        if m:
            cur = m.group(1)
            funcs[cur] = [set(), set()]
            continue
        if cur is None:
            continue
        m = re.match(r"\tbl (\w+)", line)
        if m:
            funcs[cur][0].add(m.group(1))
            continue
        m = re.search(r"\.4byte (\w+)", line)
        if m and not m.group(1).startswith("_"):
            funcs[cur][1].add(m.group(1))
    callers, takers = {}, {}
    for f, (calls, refs) in funcs.items():
        for c in calls:
            callers.setdefault(c, set()).add(f)
        for r in refs:
            takers.setdefault(r, set()).add(f)
    return funcs, callers, takers


def addresses():
    """Every function label's address: functions.csv names plus sub_ labels."""
    out = {}
    for r in csv.DictReader(open(os.path.join(ROOT, "symbols", "functions.csv"))):
        out[r["name"]] = int(r["addr"], 16)
    return out


def definitions():
    """{function: path} for functions defined in C (not INCLUDE_ASM)."""
    out = {}
    for p in sorted(glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True)):
        if os.sep + "lib" + os.sep in p:
            continue
        for f in c_functions(p, with_asm=False):
            out.setdefault(f, p)
    return out


def body(path, name):
    """The text of NAME's definition in PATH (its comment block, signature and braces)."""
    text = open(path).read()
    m = re.search(r"^[^\n;{}]*\b" + re.escape(name) + r"\s*\([^;{]*\)\s*\{", text, re.M)
    if not m:
        return ""
    i, depth = text.index("{", m.start()), 0
    for j in range(i, len(text)):
        depth += {"{": 1, "}": -1}.get(text[j], 0)
        if depth == 0:
            break
    start = m.start()
    before = text[:start].rstrip("\n")
    cm = re.search(r"(/\*(?:[^*]|\*(?!/))*\*/\s*)$", before)
    if cm:
        start = cm.start()
    return text[start:j + 1]


def subsystems():
    rows = list(csv.DictReader(open(os.path.join(ROOT, "symbols", "progress_subsystems.csv"))))
    return [(int(r["start"], 16), int(r["end"], 16), r["name"], r["evidence"]) for r in rows]


def subsystem_of(a, subs):
    for lo, hi, name, ev in subs:
        if lo <= a < hi:
            return (lo, hi, name, ev)
    return None


def cmd_batches(args):
    defs, subs = definitions(), subsystems()
    todo = sorted((int(SUB.match(f).group(1), 16), f) for f in defs if SUB.match(f))
    batches, cur, key = [], [], None
    for a, f in todo:
        k = subsystem_of(a, subs)
        k = k[:2] if k else None
        if cur and (k != key or len(cur) >= args.size or a - int(cur[-1][2:], 16) > 0x2000):
            batches.append(cur)
            cur = []
        cur.append(f)
        key = k
    if cur:
        batches.append(cur)
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "batches.tsv"), "w") as fh:
        for i, b in enumerate(batches):
            fh.write(f"{i}\t{' '.join(b)}\n")
    print(f"{len(todo)} matched sub_ functions in {len(batches)} batches -> build/namer/batches.tsv")


def load_batches():
    return {int(i): fs.split() for i, fs in (l.rstrip("\n").split("\t") for l in open(os.path.join(OUT, "batches.tsv")))}


def label_addr(name, addr):
    m = SUB.match(name)
    return int(m.group(1), 16) if m else addr.get(name)


def pack(n, ctx):
    funcs, callers, takers, defs, addr, subs = ctx
    fs = load_batches()[n]
    a0, a1 = int(fs[0][4:], 16), int(fs[-1][4:], 16)
    sub = subsystem_of(a0, subs)
    named = sorted((a, f) for f, a in addr.items() if not SUB.match(f))
    near = [f"{a:08X} {f}" for a, f in named if a0 - 0x1800 <= a <= a1 + 0x1800]
    out = [f"# Naming batch {n}: {len(fs)} functions, {a0:08X}-{a1:08X}", ""]
    if sub:
        out += [f"Subsystem range {sub[0]:08X}-{sub[1]:08X}: **{sub[2]}** (evidence: {sub[3]})", ""]
    out += ["Named functions nearby (keep prefixes consistent with these where they fit):", ""]
    out += [f"    {x}" for x in near[:60]] or ["    (none)"]
    out.append("")

    def fmt(names):
        names = sorted(names, key=lambda x: (label_addr(x, addr) or 0))
        return ", ".join(names[:25]) + (f" (+{len(names) - 25} more)" if len(names) > 25 else "") if names else "none"
    for f in fs:
        a = int(f[4:], 16)
        calls, refs = funcs.get(f, [set(), set()])
        out += [f"## {f}", "",
                f"* called by: {fmt(callers.get(f, set()))}",
                f"* address taken by (callback/table): {fmt(takers.get(f, set()))}",
                f"* calls: {fmt(calls)}",
                f"* references: {fmt(r for r in refs if not r.startswith('sub_') or True)}"]
        try:
            d = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "docindex.py"), f"{a:08X}"],
                               capture_output=True, text=True, timeout=60).stdout.strip().split("\n")[1:9]
        except subprocess.TimeoutExpired:
            d = []
        if d:
            out += ["* docs:"] + [f"  {x.strip()[:220]}" for x in d]
        out += ["", "```c", body(defs[f], f) or "(body not found)", "```", ""]
    path = os.path.join(OUT, str(n), "context.md")
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, "w").write("\n".join(out))
    return path


PROMPT = """You are naming functions in a matching decompilation of Boktai 3 (GBA, Konami, 2005).
Read {ctx}. It lists {k} functions that are already decompiled to byte-identical C but
still carry address labels (sub_XXXXXXXX), with their callers, callees, callback users,
nearby named functions and any project documentation that mentions them.

You may also read the repository (src/, include/, docs/, symbols/functions.csv) to
understand callers and data, but do not edit any file.

Give every function a useful provisional name in the project's style:
Subsystem_VerbObject in PascalCase with one underscore after the prefix, e.g.
DisplayAnimation_UpdatePlayback, ScenePanel_SetSubjectState, Script_EvalExpression.
Reuse an existing prefix from the nearby or called functions when the function belongs
to that object. Be concrete about what it does (Update, Create, Destroy, Init, Get*, Set*,
Is*, OnMessage...); for a callback say what it is the callback of. Each name must be
unique and must not already exist in symbols/functions.csv. If you really cannot tell,
still give your best descriptive guess.

Answer with ONLY CSV lines, no header and no prose, one per function, in this form:
sub_XXXXXXXX,NewName,one-line note on what it does and why the name fits
"""


def cmd_pack(args):
    print(pack(args.n, context()))


def context():
    funcs, callers, takers = code_graph()
    return funcs, callers, takers, definitions(), addresses(), subsystems()


def run_one(n, ctx, model, engine="codex", effort="low"):
    d = os.path.join(OUT, str(n))
    if os.path.exists(os.path.join(d, "names.csv")):
        return n, "done"
    if os.path.exists(os.path.join(d, "answer.txt")):           # answered before; parse again
        st = parse_answer(n, open(os.path.join(d, "answer.txt")).read().split("\n# stderr\n")[0],
                          len(load_batches()[n]))
        if st != "no answer":
            return n, st + " (reparsed)"
    path = pack(n, ctx)
    k = len(load_batches()[n])
    prompt = PROMPT.format(ctx=os.path.relpath(path, ROOT), k=k)
    if engine == "codex":       # read-only sandbox; the answer is the last message
        last = os.path.join(d, "last.txt")
        r = subprocess.run(["codex", "exec", "-m", model, "-s", "read-only", "-C", ROOT,
                            "-c", f"model_reasoning_effort={effort}", "-o", last, prompt],
                           capture_output=True, text=True, timeout=1800, stdin=subprocess.DEVNULL)
        r.stdout = open(last).read() if os.path.exists(last) else r.stdout
    else:
        r = subprocess.run(["claude", "-p", prompt, "--model", model,
                            "--allowedTools", "Read,Grep,Glob", "--output-format", "text"],
                           cwd=ROOT, capture_output=True, text=True, timeout=1800)
    open(os.path.join(d, "answer.txt"), "w").write(r.stdout + ("\n# stderr\n" + r.stderr if r.stderr else ""))
    return n, parse_answer(n, r.stdout, k)


def parse_answer(n, text, k):
    """Answer lines -> names.csv; accepts sub_XXXXXXXX or bare XXXXXXXX labels."""
    lines = []
    for l in text.split("\n"):
        l = l.strip().strip("`")
        m = re.match(r"^(?:sub_)?([0-9A-Fa-f]{8}),", l)
        if m:
            lines.append(f"sub_{m.group(1).upper()}" + l[m.end() - 1:])
    last = {l.split(",", 1)[0]: l for l in lines}                # a revised answer wins
    lines = list(last.values())
    if not lines:
        return "no answer"
    open(os.path.join(OUT, str(n), "names.csv"), "w").write("\n".join(lines) + "\n")
    return f"{len(lines)}/{k}"


def cmd_run(args):
    ctx = context()
    todo = sorted(load_batches())
    if args.only:
        todo = [int(x) for x in args.only.split(",")]
    todo = [n for n in todo if not os.path.exists(os.path.join(OUT, str(n), "names.csv"))][:args.limit]
    print(f"{len(todo)} batches, {args.jobs} at a time", flush=True)
    with ThreadPoolExecutor(args.jobs) as ex:
        for n, status in ex.map(lambda n: run_one(n, ctx, args.model, args.engine, args.effort), todo):
            print(f"batch {n}: {status}", flush=True)


def answers():
    """(sub label, name, note, batch) for every answer line, in batch order."""
    out = []
    for n, fs in sorted(load_batches().items()):
        p = os.path.join(OUT, str(n), "names.csv")
        if not os.path.exists(p):
            continue
        for row in csv.reader(open(p)):
            if len(row) >= 2 and row[0].strip() in fs:
                out.append((row[0].strip(), row[1].strip(), ",".join(row[2:]).strip(), n))
    return out


def validate():
    taken = set(addresses())
    taken |= {f for f in definitions()}
    seen, ok, bad = set(), [], []
    for label, name, note, n in answers():
        why = None
        if not IDENT.match(name) or len(name) > 64:
            why = "not a Prefix_Name identifier"
        elif name in taken or name in seen:
            why = "name already used"
        elif label in {o[0] for o in ok}:
            why = "labelled twice"
        if why:
            bad.append((label, name, why, n))
        else:
            seen.add(name)
            ok.append((label, name, note, n))
    return ok, bad


def cmd_apply(args):
    ok, bad = validate()
    defs = definitions()
    for label, name, why, n in bad:
        print(f"reject batch {n}: {label} -> {name}: {why}")
    by_file = {}
    for label, name, note, n in ok:
        by_file.setdefault(defs[label], []).append((label, name, note))
    for path, items in by_file.items():
        text = open(path).read()
        lines = "".join(f"#define {name} {label} /* {note[:150].replace('*/', '')} */\n" for label, name, note in items)
        m = re.search(r"^(#include[^\n]*\n)+", text, re.M)
        at = m.end() if m else 0
        text = text[:at] + lines + text[at:]
        if not args.dry_run:
            open(path, "w").write(text)
    print(f"{len(ok)} names accepted in {len(by_file)} files, {len(bad)} rejected"
          + (" (dry run)" if args.dry_run else "; now run tools/promote_names.py, then make"))


def cmd_report(args):
    b = load_batches()
    done = [n for n in b if os.path.exists(os.path.join(OUT, str(n), "names.csv"))]
    ok, bad = validate()
    print(f"batches {len(done)}/{len(b)}, functions {sum(len(b[n]) for n in done)}/{sum(map(len, b.values()))}, "
          f"accepted {len(ok)}, rejected {len(bad)}")


def main():
    ap = argparse.ArgumentParser()
    sp = ap.add_subparsers(dest="cmd", required=True)
    p = sp.add_parser("batches")
    p.add_argument("--size", type=int, default=20)
    p = sp.add_parser("pack")
    p.add_argument("n", type=int)
    p = sp.add_parser("run")
    p.add_argument("--jobs", type=int, default=8)
    p.add_argument("--engine", choices=["codex", "claude"], default="codex",
                   help="codex exec (no Claude usage) or claude -p")
    p.add_argument("--model", default="gpt-6.1-sol")
    p.add_argument("--effort", default="low", help="codex reasoning effort")
    p.add_argument("--only")
    p.add_argument("--limit", type=int, default=10 ** 6)
    p = sp.add_parser("apply")
    p.add_argument("--dry-run", action="store_true")
    sp.add_parser("report")
    a = ap.parse_args()
    {"batches": cmd_batches, "pack": cmd_pack, "run": cmd_run, "apply": cmd_apply, "report": cmd_report}[a.cmd](a)


if __name__ == "__main__":
    main()

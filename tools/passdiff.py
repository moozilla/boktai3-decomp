#!/usr/bin/env python3
"""Which agbcc pass does a function's register pin (or asm hint) act on?

  passdiff.py FILE.c [FUNC] [--show N]
  passdiff.py --batch LIST.tsv OUT.tsv     (`progress.py --list-pinned` rows)

FILE.c holds a matching function that still has pins or empty asm() hints.
It is compiled twice with `agbcc -da` (RTL dumped after every pass): as is,
and with every hint stripped (tools/unpin.py). Each dump of FUNC is
normalized (insn numbers, line notes and basic-block bookkeeping dropped;
every register, hard or pseudo, renamed in order of first appearance, so a
pinned `r5` and the pseudo it replaces read the same) and the two versions
are compared pass by pass, in pipeline order:

  rtl jump cse addressof gcse loop cse2 flow combine regmove lreg greg jump2 mach

The first pass whose output differs is where the hint changes the compiler's
decision; that names the idiom family that can replace it (docs/REGALLOC.md):
cse/gcse/cse2 (which value a load or a constant is shared with), loop
(hoisting, induction variables), combine (instruction fusion), regmove,
lreg/greg (allocation proper: priorities, reload). "none" means the stripped
copy compiles to the same RTL, so the hint is unneeded or only acts at the
assembly level.
"""
import argparse
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import build  # noqa: E402
import unpin  # noqa: E402

PASSES = ["rtl", "jump", "cse", "addressof", "gcse", "loop", "cse2", "flow", "combine",
          "regmove", "lreg", "greg", "jump2", "mach"]


def dumps(text, func, work):
    """{pass: normalized insn lines of FUNC} for one source text."""
    src = os.path.join(work, "x.c")
    open(src, "w").write(text)
    pre = build.preprocess(os.path.relpath(src, ROOT))
    open(os.path.join(work, "x.i"), "w").write(pre)
    for p in PASSES:
        try:
            os.remove(os.path.join(work, "x.i." + p))
        except FileNotFoundError:
            pass
    r = subprocess.run([build.AGBCC, "-O2", "-mthumb-interwork", "-fhex-asm", "-da", "x.i", "-o", "x.s"],
                       cwd=work, capture_output=True, text=True)
    if r.returncode:
        raise RuntimeError(r.stderr[-2000:])
    out = {}
    for p in PASSES:
        path = os.path.join(work, "x.i." + p)
        if os.path.exists(path):
            out[p] = normalize(section(open(path).read(), func), hard=PASSES.index(p) >= PASSES.index("greg"))
    return out


def section(dump, func):
    m = re.search(r"^;; Function " + re.escape(func) + r"\b.*?$(.*?)(?=^;; Function |\Z)", dump, re.M | re.S)
    return m.group(1) if m else ""


def normalize(text, hard=False):
    """hard: after reload every register is a hard one, and its number is the
    allocation result; keep it."""
    regs = {}

    def reg(m):
        if hard and int(m.group(2)) < 16:
            return f"(reg{m.group(1) or ''} r{m.group(2)})"
        # a pinned variable is its hard register with the /v (user variable)
        # flag; keep it apart from the same register's other uses (arguments)
        key = m.group(2) + ("v" if int(m.group(2)) < 16 and "/v" in (m.group(1) or "") else "")
        if key not in regs:
            regs[key] = f"R{len(regs)}"
        return f"(reg{m.group(1)} {regs[key]})"

    lines = []
    for chunk in re.split(r"\n(?=\((?:insn|jump_insn|call_insn|code_label|barrier|note)[/ ])", text):
        chunk = chunk.strip()
        if not chunk.startswith("(") or chunk.startswith("(note") or chunk.startswith("(barrier"):
            continue
        chunk = re.sub(r";;[^\n]*", "", chunk)  # basic-block comments
        chunk = re.sub(r"\s+", " ", chunk).strip()
        # drop REG_* notes and LOG_LINKS: compare what the insns do
        chunk = re.sub(r" \((?:expr_list|insn_list)[: ].*$", ")", chunk)
        chunk = re.sub(r"( \(nil\))+\)*$", ")", chunk)
        if re.search(r'asm_(?:operands|input)(?:/\w+)? \(""', chunk):
            continue  # the empty asm() hint itself
        chunk = re.sub(r"^\((\w+(?:/\w+)?) \d+ \d+ \d+", r"(\1", chunk)          # uid, prev, next
        chunk = re.sub(r"\(code_label \d+ \d+ \d+ (\d+)", r"(code_label L", chunk)
        chunk = re.sub(r"\(reg([:/][\w:/]*)? (\d+)(?: \w+)?\)", reg, chunk)
        chunk = re.sub(r"\(label_ref \d+\)", "(label_ref L)", chunk)
        chunk = re.sub(r"\(code_label \d+\)", "(code_label L)", chunk)
        chunk = re.sub(r" \d+ \{[^}]*\}", "", chunk)                    # insn code + pattern name
        lines.append(chunk)
    return renumber(merge_hard_copies(lines))


def skeleton(lines):
    """The operations alone: every register is `R`, a low-part subreg is its
    register, and plain register-to-register copies are dropped. A pinned
    variable is a hard register from expand onward, so its subregs fold and
    its copies vanish where a pseudo's don't; those differences only matter
    to the allocator. What is left differs only where a pass made a
    different decision about the code itself."""
    out = []
    for ln in lines:
        ln = re.sub(r"\(subreg:\w+ (\(reg[^)]*\)) 0\)", r"\1", ln)
        ln = re.sub(r"\(reg[^ )]* R\d+\)", "(R)", ln)
        ln = re.sub(r"\(reg[^ )]* r\d+\)", "(R)", ln)
        ln = re.sub(r" -?\d+\)$", ")", ln)  # insn code: -1 until recog runs
        ln = re.sub(r"\(code_label \d+ \(\"\w+\"\)\)", "(code_label L)", ln)
        if re.fullmatch(r"\(insn(?:/\w+)? \(set \(R\) \(R\)\)(?: -?\d+)?\)", ln):
            continue
        out.append(ln)
    return out


def renumber(lines):
    names = {}
    return [re.sub(r"\bR\d+\b", lambda m: names.setdefault(m.group(0), f"R{len(names)}"), ln) for ln in lines]


def merge_hard_copies(lines):
    """Expand stores into a pinned (hard register) variable through a fresh
    pseudo: `(set (reg T) E)` then `(set (reg/v V) (reg T))`. Read both as
    `(set (reg/v V) E)`, the form an unpinned variable gets."""
    out = []
    for ln in lines:
        m = re.match(r"\(insn \(set \((reg/v[^ ]*) (R\d+)\) \((reg[^ ]*) (R\d+)\)\)", ln)
        if m and out:
            prev = re.match(r"\(insn \(set \((reg[^ /]*) " + m.group(4) + r"\) (.*)$", out[-1])
            if prev:
                out[-1] = f"(insn (set ({m.group(1)} {m.group(2)}) " + prev.group(2)
                continue
        out.append(ln)
    return out


def first_difference(pinned, stripped, skel=True):
    """First pass whose output differs. With skel, compare skeleton()s; if
    those never differ, the hint acts on allocation alone: "alloc"."""
    for p in PASSES:
        a, b = pinned.get(p), stripped.get(p)
        if a is None or b is None:
            continue
        if skel:
            a, b = skeleton(a), skeleton(b)
        if a != b:
            return p
    if skel:
        return "alloc" if first_difference(pinned, stripped, False) != "none" else "none"
    return "none"


def family(p, pinned, stripped):
    """Name what the hint changes at pass p, from the skeleton diff."""
    if p in ("alloc", "none", "error"):
        return p
    from collections import Counter
    a, b = Counter(skeleton(pinned[p])), Counter(skeleton(stripped[p]))
    only_a, only_b = a - b, b - a
    lines = list(only_a.elements()) + list(only_b.elements())
    if not lines:
        return "order"
    if any(re.search(r"\((?:jump_insn|code_label)", ln) for ln in lines):
        return "branch"
    if any("_extend" in ln for ln in lines):
        return "extend"
    if p == "greg" or any("use (R)" in ln or "clobber" in ln for ln in lines):
        return "spill"
    if p == "loop":
        return "hoist"
    if p in ("combine", "regmove"):
        return "combine"
    if sum("(mem" in ln for ln in only_a.elements()) != sum("(mem" in ln for ln in only_b.elements()):
        return "load-reuse"
    if any(re.search(r"\(set \(R\) \(const_int", ln) for ln in lines):
        return "const-in-reg"
    return "expr-shape"


def analyze(path, func, show=0):
    text = open(path).read()
    if unpin.strip_code(text) == text:
        return "header", "header", []  # the hint is in a header helper, not in this copy
    with tempfile.TemporaryDirectory(prefix="passdiff-", dir=os.path.join(ROOT, "build")) as work:
        a = dumps(text, func, work)
        b = dumps(unpin.strip_code(text), func, work)
    p = first_difference(a, b)
    detail = []
    if p in a and show:
        import difflib
        detail = list(difflib.unified_diff(skeleton(a[p]), skeleton(b[p]), "pinned", "stripped", lineterm="", n=0))[:show]
    return p, family(p, a, b) if p in a or p in ("alloc", "none") else p, detail


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("file", nargs="?")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--show", type=int, default=12, help="diff lines of the first differing pass")
    ap.add_argument("--batch", nargs=2, metavar=("LIST", "OUT"))
    ap.add_argument("-j", type=int, default=1, help="batch: functions in parallel")
    a = ap.parse_args()
    if a.batch:
        from collections import Counter
        counts = Counter()
        import isolate
        from concurrent.futures import ThreadPoolExecutor
        rows = [ln.rstrip("\n").split("\t") for ln in open(a.batch[0])]
        rows = [r for r in rows if len(r) >= 5 and r[0] != "name"]

        def one(row):
            try:
                return row, "\t".join(analyze(os.path.join(ROOT, isolate.ensure(row[4], row[0])), row[0])[:2])
            except Exception:  # noqa: BLE001
                return row, "error\terror"

        with open(a.batch[1], "w") as out, ThreadPoolExecutor(a.j) as ex:
            for row, p in ex.map(one, rows):
                counts[p] += 1
                out.write(f"{row[0]}\t{row[2]}\t{row[3]}\t{p}\n")
                out.flush()
        for p, n in counts.most_common():
            print(f"{p}\t{n}")
        return
    import isolate
    import regdiag
    func = a.func or regdiag.first_function(a.file)
    src = os.path.join(ROOT, isolate.ensure(os.path.relpath(os.path.abspath(a.file), ROOT), func))
    p, fam, detail = analyze(src, func, a.show)
    print(f"{func}: first difference after pass {p} ({fam})")
    for d in detail:
        print(d)


if __name__ == "__main__":
    main()

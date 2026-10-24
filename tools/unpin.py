#!/usr/bin/env python3
"""Try removing register-pinning concessions from C files.

  unpin.py [FILE.c ...] [--write] [-j N]

For each file carrying `// MATCHING_CONCESSION: register-pinning` (default: all
of src/), strip every `register T x asm("rN")` pin (keeping a plain
declaration) and every `asm("")` barrier, compile with tools/check.py, and
report whether the file still matches. With --write, a file that still matches
is rewritten without the hints and without the marker.

  unpin.py --drafts [FILE.c ...] [--write] [-j N]

The same for `#ifdef NONMATCHING` drafts that carry hints: compile the draft
with every hint removed, then (if that does not match) remove the hints one at
a time, keeping each removal that does not make the draft worse. Pins are often
added while other differences remain and outlive their reason: two drafts that
were 1 instruction off with pins matched with none. With --write, an improved
draft replaces the old one (still inside its #ifdef; promote a match with
promote_draft.py).
"""
import argparse
import os
import re
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MARK = "// MATCHING_CONCESSION: register-pinning"
PIN = re.compile(r'\bregister\s+([^;=]*?)\s*\basm\s*\(\s*"\w+"\s*\)')
BARRIER = re.compile(r'^[ \t]*(?:__)?asm(?:__)?\s*(?:volatile\s*|__volatile__\s*)?\(\s*""\s*(?::[^;]*)?\)\s*;[ \t]*\n', re.M)
BARRIER_INLINE = re.compile(r'(?:__)?asm(?:__)?\s*(?:volatile\s*|__volatile__\s*)?\(\s*""\s*(?::[^;]*)?\)\s*;')


DRAFT = re.compile(r"#\s*ifdef\s+NONMATCHING\b.*?#\s*else\b", re.S)


def strip_code(text):
    text = PIN.sub(lambda m: m.group(1), text)
    text = BARRIER.sub("", text)
    return BARRIER_INLINE.sub("", text)


def strip(text):
    """Remove hints outside NONMATCHING drafts (a draft's hints are left alone:
    the default build compiles its INCLUDE_ASM fallback, so a check would pass
    trivially). The marker goes only if no hints remain."""
    out, last = [], 0
    for m in DRAFT.finditer(text):
        out.append(strip_code(text[last:m.start()]))
        out.append(m.group(0))
        last = m.end()
    out.append(strip_code(text[last:]))
    text = "".join(out)
    if not any(p.search(text) for p in (PIN, BARRIER_INLINE)):
        text = re.sub(r"^" + re.escape(MARK) + r"[^\n]*\n", "", text, flags=re.M)
    return text


RAM_ARRAY = re.compile(r"^extern\s+(?:const\s+)?(\w+)\s+(gUnk_0([234])[0-9A-F]{6})\s*\[\s*\]\s*;", re.M)


def literalize(text):
    """Use literal addresses for RAM arrays (gUnk_02xxxxxx[] etc.): a constant
    address is loaded by reload, which can move later reload registers."""
    for typ, sym, _ in RAM_ARRAY.findall(text):
        addr = sym[5:]
        text = re.sub(r"(?<!extern )(?<![\w ]" + typ + r" )\b" + sym + r"\b(?!\s*\[\s*\]\s*;)",
                      f"(({typ} *)0x{addr})", text)
    return text


VARIANTS = [("strip", strip), ("strip+literal", lambda t: literalize(strip(t)))]


def check(path, text):
    # keep the file in its own directory so relative includes keep working
    fd, tmp = tempfile.mkstemp(suffix=".c", prefix=".unpin_", dir=path.parent)
    with os.fdopen(fd, "w") as f:
        f.write(text)
    try:
        r = subprocess.run([sys.executable, str(ROOT / "tools/check.py"), tmp, "--quiet"],
                           cwd=ROOT, capture_output=True, text=True)
        out = r.stdout + r.stderr
        m = re.search(r"(\d+) differing bytes; fuzzy ([\d.]+)%", out)
        if r.returncode == 0:
            return True, "match"
        if m:
            return False, f"{m.group(1)} bytes differ ({m.group(2)}%)"
        tail = [ln for ln in out.splitlines() if ln.strip()][-1:] or ["?"]
        return False, "error: " + tail[0][:100]
    finally:
        os.unlink(tmp)


def per_function(files, write):
    """Strip hints function by function, keeping each change that still matches."""
    sys.path.insert(0, str(ROOT / "tools"))
    from merge_unit import items
    total = 0
    for p in files:
        text = p.read_text()
        parts = items(text)
        # rebuild the file from items, so an item can be swapped in place
        sep_text = text
        changed = []
        drafts = [m.span() for m in re.finditer(r"#\s*ifdef\s+NONMATCHING.*?#\s*endif", text, re.S)]
        for item in parts:
            if not (PIN.search(item) or BARRIER_INLINE.search(item)):
                continue
            at = text.find(item)
            if any(a <= at < b for a, b in drafts):  # a draft: its check would pass trivially
                continue
            m = re.search(r"\b(\w+)\s*\([^;{]*\)\s*\{", item)
            name = m.group(1) if m else "?"
            for vname, fn in (("strip", strip_code), ("strip+literal", lambda t: literalize_item(t, sep_text))):
                cand_item = fn(item)
                if cand_item == item:
                    continue
                cand = sep_text.replace(item, cand_item, 1)
                ok, msg = check(p, cand)
                if ok:
                    sep_text = cand
                    changed.append(f"{name} ({vname})")
                    break
        if changed:
            if not (PIN.search(sep_text) or BARRIER_INLINE.search(sep_text)):
                sep_text = re.sub(r"^" + re.escape(MARK) + r"[^\n]*\n", "", sep_text, flags=re.M)
            if write:
                p.write_text(sep_text)
            total += len(changed)
        print(f"{p.relative_to(ROOT)}: unpinned {', '.join(changed) or 'none'}", flush=True)
    print(f"{total} functions unpinned")


HINT = re.compile(PIN.pattern + "|" + BARRIER.pattern + "|" + BARRIER_INLINE.pattern, re.M)


def check_draft(path, text):
    """(score, matched function names) for the file's drafts. The score is
    (-fuzzy %, differing bytes), lower is better, (-100, 0) a match: differing
    bytes alone count a one-instruction shift as a mismatch to the end, and
    rank a pinned 99% draft below an unpinned 94% one."""
    fd, tmp = tempfile.mkstemp(suffix=".c", prefix=".unpin_", dir=path.parent)
    with os.fdopen(fd, "w") as f:
        f.write(text)
    try:
        r = subprocess.run([sys.executable, str(ROOT / "tools/check.py"), tmp, "--nonmatching"],
                           cwd=ROOT, capture_output=True, text=True, timeout=300)
        out = r.stdout + r.stderr
    except subprocess.TimeoutExpired:
        return WORST, []
    finally:
        os.unlink(tmp)
    m = re.search(r"(\d+) differing bytes; fuzzy ([\d.]+)%", out)
    score = (-float(m.group(2)), int(m.group(1))) if m else WORST
    return score, re.findall(r"^(\w+): MATCH", out, re.M)


WORST = (0.0, 10 ** 9)
MATCH = (-100.0, 0)


def show(score):
    return "MATCH" if score == MATCH else f"{-score[0]:.1f}% ({score[1]} bytes)"


def remove_hint(m):
    # a pin keeps its declaration; a barrier goes
    return m.group(1) if m.group(1) is not None else ""


def depin_draft(p, write):
    text = p.read_text()
    blocks = [m for m in DRAFT.finditer(text) if HINT.search(m.group(0))]
    if not blocks:
        return p, None, "no pinned drafts"
    base, _ = check_draft(p, text)
    best, best_text = base, text
    # 1. every hint in every draft
    allstrip = text
    for m in reversed(blocks):
        allstrip = allstrip[:m.start()] + HINT.sub(remove_hint, m.group(0)) + allstrip[m.end():]
    d, matched = check_draft(p, allstrip)
    tried = 1
    if d < best:
        best, best_text = d, allstrip
    if d != MATCH:
        # 2. one hint at a time, keeping removals that do not make it worse
        cur, cur_score = text, base
        i = 0
        while True:
            spans = [h for m in DRAFT.finditer(cur) if HINT.search(m.group(0))
                     for h in HINT.finditer(cur, m.start(), m.end())]
            if i >= len(spans):
                break
            h = spans[i]
            cand = cur[:h.start()] + remove_hint(h) + cur[h.end():]
            dd, mm = check_draft(p, cand)
            tried += 1
            if dd <= cur_score:
                cur, cur_score = cand, dd
                if dd < best:
                    best, best_text, matched = dd, cand, mm
                if dd == MATCH:
                    break
            else:
                i += 1
        # 3. a pin together with the barriers that name only its variable (a pin alone,
        # with its barrier left, still keeps the value in some register there)
        for var in sorted({re.findall(r"\w+", h.group(1))[-1] for h in HINT.finditer(cur) if h.group(1)}):
            if best == MATCH:
                break
            group = [h for m in DRAFT.finditer(cur) for h in HINT.finditer(cur, m.start(), m.end())
                     if (h.group(1) and re.findall(r"\w+", h.group(1))[-1] == var) or
                     (not h.group(1) and set(re.findall(r'"\+?=?r"\s*\(\s*(\w+)\s*\)', h.group(0))) == {var})]
            if len(group) < 2:
                continue
            cand = cur
            for h in reversed(group):
                cand = cand[:h.start()] + remove_hint(h) + cand[h.end():]
            dd, mm = check_draft(p, cand)
            tried += 1
            if dd <= cur_score:
                cur, cur_score = cand, dd
                if dd < best:
                    best, best_text, matched = dd, cand, mm
    if write and best < base:
        p.write_text(best_text)
    left = sum(len(HINT.findall(m.group(0))) for m in DRAFT.finditer(best_text))
    return p, best == MATCH, f"{show(base)} -> {show(best)} ({left} hints left, {tried} compiles)" + \
        (f"; matches: {' '.join(matched)}" if matched else "")


def literalize_item(item, whole):
    for typ, sym, _ in RAM_ARRAY.findall(whole):
        item = re.sub(r"\b" + sym + r"\b", f"(({typ} *)0x{sym[5:]})", item)
    return strip_code(item)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("files", nargs="*")
    ap.add_argument("--write", action="store_true")
    ap.add_argument("-j", type=int, default=6)
    ap.add_argument("--per-function", action="store_true",
                    help="in multi-function units, strip one function's hints at a time")
    ap.add_argument("--drafts", action="store_true",
                    help="remove hints from #ifdef NONMATCHING drafts instead")
    a = ap.parse_args()
    if a.drafts:
        files = [Path(f).resolve() for f in a.files] or sorted(
            p for p in (ROOT / "src").rglob("*.c")
            if any(HINT.search(m.group(0)) for m in DRAFT.finditer(p.read_text())))
        n = 0
        with ThreadPoolExecutor(a.j) as ex:
            for p, ok, msg in ex.map(lambda q: depin_draft(q, a.write), files):
                n += bool(ok)
                print(f"{'OK  ' if ok else '--  '}{p.relative_to(ROOT)}: {msg}", flush=True)
        print(f"{n}/{len(files)} pinned drafts match without (some) hints")
        return
    files = [Path(f).resolve() for f in a.files] or sorted(
        p for p in (ROOT / "src").rglob("*.c") if MARK in p.read_text())

    def one(p):
        text = p.read_text()
        if strip(text) == text:
            return p, None, "no hints"
        msgs = []
        for name, fn in VARIANTS:
            new = fn(text)
            if name != "strip" and new == strip(text):
                continue
            ok, msg = check(p, new)
            msgs.append(f"{name}: {msg}")
            if ok:
                if a.write:
                    p.write_text(new)
                return p, True, "; ".join(msgs)
        return p, False, "; ".join(msgs)

    if a.per_function:
        return per_function(files, a.write)
    n = 0
    with ThreadPoolExecutor(a.j) as ex:
        for p, ok, msg in ex.map(one, files):
            n += bool(ok)
            print(f"{'OK  ' if ok else '--  '}{p.relative_to(ROOT)}: {msg}", flush=True)
    print(f"{n}/{len(files)} files match without hints")


if __name__ == "__main__":
    main()

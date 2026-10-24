"""Program-structure views across the source tree: what one function's diff cannot show.

census: every register pin and empty asm barrier in matched code, grouped by the call it serves (the
first call after it, preferably one that uses the pinned or barriered variable), with how many matched
sites spell the same call with no concession and how many drafts contain it. A large group is usually
one missing helper or macro (ZeroFill8 replaced 16 such concessions around CpuSet(_, _, 0x05000002)).
"""
import collections
import glob
import os
import re

KEYWORDS = {"if", "while", "for", "switch", "return", "sizeof", "asm", "__asm__", "do", "else", "register",
            "typeof", "volatile", "void", "defined"}
PIN = re.compile(r'\bregister\b[^;]*\basm\s*\(\s*"(?:r\d+|sl|fp|ip|sb|lr)"\s*\)')
BARRIER = re.compile(r'^\s*(?:__)?asm\s*(?:volatile\s*)?\(\s*""')
CALL = re.compile(r"\b([A-Za-z_]\w*)\s*\(")
DEF = re.compile(r"^[A-Za-z_][^;=()]*?\b(\w+)\s*\([^;{]*\)\s*\{?\s*$")


def split_drafts(text):
    """(matched lines, draft lines): lines of #ifdef NONMATCHING branches go to drafts; the
    #else branches (INCLUDE_ASM) are dropped. Each list holds (line number, text)."""
    matched, drafts = [], []
    state = None
    for n, ln in enumerate(text.split("\n"), 1):
        s = ln.strip()
        if s.startswith("#ifdef NONMATCHING"):
            state = "draft"
            continue
        if state and s.startswith("#else"):
            state = "else"
            continue
        if state and s.startswith("#endif"):
            state = None
            continue
        (drafts if state == "draft" else matched if state is None else []).append((n, ln))
    return matched, drafts


def call_key(line, start=0):
    """'Callee(_, 0x5000002)': the first call at or after START in LINE, literal arguments kept."""
    for m in CALL.finditer(line, start):
        name = m.group(1)
        if name in KEYWORDS:
            continue
        depth, i, args, cur = 1, m.end(), [], ""
        while i < len(line) and depth:
            c = line[i]
            if c == "(":
                depth += 1
            elif c == ")":
                depth -= 1
                if not depth:
                    break
            if c == "," and depth == 1:
                args.append(cur)
                cur = ""
            else:
                cur += c
            i += 1
        args.append(cur)
        norm = []
        for a in args:
            a = a.strip()
            norm.append(a.lower() if re.fullmatch(r"-?(0x[0-9a-fA-F]+|\d+)[uUlL]*", a) else "_")
        return f"{name}({', '.join(norm)})" if args != [""] else f"{name}()"
    return None


def concession_var(line):
    m = re.search(r"\bregister\b[^;]*?\*?\s*(\w+)\s+asm\s*\(", line)
    if m:
        return m.group(1)
    m = re.search(r'\(\s*(\w+)\s*\)\s*\)\s*;', line) or re.search(r'"\+?=?r"\s*\(\s*(\w+)\s*\)', line)
    return m.group(1) if m else None


def sites(root):
    """Concession sites, plain call sites and draft call sites: dicts key -> [(file, line, func, kind)]."""
    conc = collections.defaultdict(list)
    clean = collections.defaultdict(list)
    draft = collections.defaultdict(list)
    for path in sorted(glob.glob(os.path.join(root, "src", "**", "*.c"), recursive=True)):
        rel = os.path.relpath(path, root)
        text = open(path, errors="replace").read()
        matched, drafts = split_drafts(text)
        for n, ln in drafts:
            k = call_key(ln)
            if k:
                draft[k].append((rel, n, None, None))
        func = None
        recent = []  # line numbers of concessions in the last few lines
        lines = matched
        for idx, (n, ln) in enumerate(lines):
            d = DEF.match(ln)
            if d and not ln.startswith(("static inline", "#")) and d.group(1) not in KEYWORDS:
                func = d.group(1)
            if ln.lstrip().startswith(("//", "/*", "*")):
                continue
            kind = "pin" if PIN.search(ln) else "barrier" if BARRIER.match(ln) else None
            if kind:
                var = concession_var(ln)
                key = None
                # the call this concession serves: the first later call using its variable, else the first call
                window = [x for x in lines[idx + 1:idx + 9]]
                for _, w in window:
                    if var and re.search(rf"\b{re.escape(var)}\b", w) and call_key(w):
                        key = call_key(w)
                        break
                if key is None:
                    key = next((call_key(w) for _, w in window if call_key(w)), None)
                if key:
                    conc[key].append((rel, n, func or os.path.basename(rel)[:-2], kind))
                recent.append(n)
                continue
            k = call_key(ln)
            if k and not any(n - r <= 8 for r in recent[-3:]):
                clean[k].append((rel, n, func, None))
    return conc, clean, draft


def callee(key):
    return key.split("(", 1)[0]


def census(root, show=20, want=None):
    conc, clean, draft = sites(root)
    if want:  # drill down: one callee (or one exact key)
        keys = [k for k in conc if k == want or callee(k) == want]
        if not keys:
            print(f"no concession serves a call to {want}")
            return 1
        for k in sorted(keys, key=lambda k: -len(conc[k])):
            funcs = sorted({(f, fn) for f, _, fn, _ in conc[k]})
            print(f"{k}: {len(conc[k])} concessions in {len(funcs)} functions;"
                  f" {len(clean.get(k, []))} clean matched sites; {len(draft.get(k, []))} draft sites")
            for f, n, fn, kind in conc[k]:
                print(f"  {kind:7} {f}:{n}  {fn or ''}")
            ex = clean.get(k, [])[:5]
            if ex:
                print("  clean spellings, e.g.: " + "  ".join(f"{f}:{n}" for f, n, _, _ in ex))
        return 0
    by_callee = collections.defaultdict(list)
    for k, v in conc.items():
        by_callee[callee(k)].append(k)
    rows = []
    for c, keys in by_callee.items():
        n = sum(len(conc[k]) for k in keys)
        funcs = {(f, fn) for k in keys for f, _, fn, _ in conc[k]}
        top = max(keys, key=lambda k: len(conc[k]))
        rows.append((n, len(funcs), c, top, len(conc[top]), sum(len(clean.get(k, [])) for k in keys),
                     sum(len(draft.get(k, [])) for k in keys)))
    rows.sort(key=lambda r: (-r[0], r[2]))
    total = sum(r[0] for r in rows)
    print(f"{total} concessions in matched code serve calls to {len(rows)} functions; the largest groups"
          f" (clean = matched calls with no concession nearby, drafts = calls inside NONMATCHING drafts):")
    print(f"  {'conc':>4} {'funcs':>5} {'clean':>5} {'draft':>5}  call (most common argument shape)")
    for n, nf, c, top, ntop, ncl, ndr in rows[:show]:
        shape = top if ntop * 2 >= n else c + "(...)"
        print(f"  {n:4} {nf:5} {ncl:5} {ndr:5}  {shape}")
    print("\nnext: scope.py census CALLEE (every site, and clean spellings to compare)")
    return 0

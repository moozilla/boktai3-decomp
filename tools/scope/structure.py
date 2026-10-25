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


# ---------------------------------------------------------------- like: matched functions whose ROM looks alike

def norm_insn(text, names):
    """One instruction as a token: registers abstracted, calls by name, literals and immediates kept."""
    m = re.match(r"bl #0x([0-9a-f]+)$", text)
    if m:
        return "bl " + names.get(int(m.group(1), 16) & ~1, "?")
    m = re.match(r"ldr \w+, =(0x[0-9A-Fa-f]+)$", text)
    if m:
        v = int(m.group(1), 16)
        return "ldr =" + names.get(v & ~1, f"0x{v:X}")
    if re.match(r"b\w*\s+(func\+|#)", text):
        return text.split()[0]
    t = re.sub(r"\br([0-9]|1[0-2])\b", "R", text)
    return re.sub(r"\b(sb|sl|fp|ip|r8|r9|r10)\b", "H", t)


def build_index(funcs, sizes, rom, decode, pool_ranges):
    """{name: [tokens]} for every function in the inventory, from the ROM."""
    names = {a: n for n, a in funcs}
    out = {}
    for n, a in funcs:
        size = sizes[n]
        if not 0x08000000 <= a < 0x08000000 + len(rom) or size <= 0 or size > 0x4000:
            continue
        code = rom[a - 0x08000000:a - 0x08000000 + size]
        try:
            rows = decode(code, a, pool_ranges(code, a))
        except Exception:
            continue
        out[n] = [norm_insn(r[2], names) for r in rows if not r[2].startswith(".")]
    return out


def shingles(tokens, k=3):
    return {"|".join(tokens[i:i + k]) for i in range(max(0, len(tokens) - k + 1))}


def tables(index):
    """(shingle sets, document frequency, idf) over the index: what like and todo weigh windows by."""
    import math
    sh = {n: shingles(t) for n, t in index.items()}
    df = collections.Counter(s for v in sh.values() for s in v)
    return sh, df, {s: math.log(len(sh) / c) for s, c in df.items()}


def similar(target, index, status, tabs=None):
    """[(similarity, coverage, name)] of matched functions sharing rare 3-insn windows with TARGET, best first."""
    import math
    sh, df, idf = tabs or tables(index)
    mine = sh[target]
    self_w = sum(idf[s] for s in mine) or 1
    scores = []
    for n, v in sh.items():
        if n == target or status.get(n) != "matched":
            continue
        common = mine & v
        if not common:
            continue
        w = sum(idf[s] for s in common)
        other_w = sum(idf[s] for s in v) or 1
        scores.append((w / math.sqrt(self_w * other_w), w / self_w, n))
    scores.sort(reverse=True)
    return scores


def like(target, index, status, files, show=8, tabs=None):
    """Matched functions ranked by shared rare 3-insn windows with TARGET; prints the shared calls."""
    import math
    if target not in index:
        print(f"{target}: not in the ROM index")
        return 1
    scores = similar(target, index, status, tabs)
    tcalls = [t for t in index[target] if t.startswith(("bl ", "ldr ="))]
    print(f"{target} ({len(index[target])} insns, {status.get(target, '?')}): {len(tcalls)} calls/literals;"
          f" matched functions with the most shared rare 3-insn windows:")
    for sim, cover, n in scores[:show]:
        theirs = set(t for t in index[n] if t.startswith(("bl ", "ldr =")))
        shared = [t for t in dict.fromkeys(tcalls) if t in theirs]
        print(f"  {sim:.2f} (covers {cover:4.0%})  {n:40} {len(index[n]):4} insns  {files.get(n, '?')}")
        if shared:
            print("        shared: " + ", ".join(s.replace('ldr =', '=') for s in shared[:8])
                  + (f" +{len(shared) - 8}" if len(shared) > 8 else ""))
    unknown = [t for t in dict.fromkeys(tcalls) if not any(
        t in index[n] for _, _, n in scores[:show])]
    if unknown:
        print("  in no listed match: " + ", ".join(s.replace('ldr =', '=') for s in unknown[:10]))
    return 0


# ---------------------------------------------------------------- fields: which struct has fields at these offsets

def struct_members(text):
    """{struct name: [member names]} for top-level struct definitions in a header's text (members
    of nested anonymous structs and bit-fields are skipped: offsetof cannot name them simply)."""
    out = {}
    text = re.sub(r"/\*.*?\*/|//[^\n]*", "", text, flags=re.S)
    for m in re.finditer(r"^struct\s+(\w+)\s*\{", text, re.M):
        i, depth, body = m.end(), 1, []
        while i < len(text) and depth:
            c = text[i]
            depth += (c == "{") - (c == "}")
            body.append(c if depth >= 1 else "")
            i += 1
        names = []
        level, cur = 0, ""
        for c in "".join(body):
            if c == "{":
                level += 1
            elif c == "}":
                level -= 1
            if c == ";" and level == 0:
                decl = re.sub(r"\{.*\}", "{}", cur, flags=re.S).strip()
                cur = ""
                if not decl or ":" in decl or decl.startswith(("union {", "struct {")) and decl.endswith("}"):
                    continue
                fp = re.search(r"\(\s*\*\s*(\w+)\s*\)", decl)
                if fp:
                    names.append(fp.group(1))
                    continue
                parts = decl.split(",")
                first = re.sub(r"\[[^\]]*\]", "", parts[0])
                ids = re.findall(r"[A-Za-z_]\w*", first)
                if len(ids) >= 2:
                    names.append(ids[-1])
                for p in parts[1:]:
                    q = re.findall(r"[A-Za-z_]\w*", re.sub(r"\[[^\]]*\]", "", p))
                    if q:
                        names.append(q[0])
                continue
            cur += c
        out[m.group(1)] = names
    return out


def layouts(root, compile_c):
    """{struct: [(offset, size, member)]} for the structs of include/*.h, measured by agbcc: one
    `offsetof`/`sizeof` table per header. COMPILE_C(text) -> assembly text or None."""
    out = {}
    for h in sorted(glob.glob(os.path.join(root, "include", "**", "*.h"), recursive=True)):
        rel = os.path.relpath(h, os.path.join(root, "include"))
        members = struct_members(open(h, errors="replace").read())
        members = {s: m for s, m in members.items() if m}
        if not members:
            continue

        def table(items):
            src = f'#include "global.h"\n#include "{rel}"\n'
            for k, (s, ms) in enumerate(items):
                vals = ", ".join(f"(unsigned)&((struct {s} *)0)->{x}, sizeof(((struct {s} *)0)->{x})" for x in ms)
                src += f"const unsigned scope_layout{k}[] = {{ sizeof(struct {s}), {vals} }};\n"
            return compile_c(src)
        items = list(members.items())
        asm = table(items)
        groups = [items] if asm else [[it] for it in items]  # a bad member: measure struct by struct
        for g in groups:
            a = asm if asm and len(groups) == 1 else table(g)
            if not a:
                continue
            for k, (s, ms) in enumerate(g):
                blk = re.search(rf"scope_layout{k}:\n((?:\s*\.word\s+-?(?:0x[0-9a-fA-F]+|\d+)\n)+)", a)
                if not blk:
                    continue
                vals = [int(v, 0) & 0xFFFFFFFF for v in re.findall(r"\.word\s+(-?(?:0x[0-9a-fA-F]+|\d+))",
                                                                 blk.group(1))]
                rows = [(vals[1 + 2 * j], vals[2 + 2 * j], ms[j]) for j in range(len(ms))]
                out.setdefault(s, {"size": vals[0], "header": rel, "fields": rows})
    return out


def fields(offs, lay, want=None, show=10):
    """Structs with fields at OFFS: exact starts first, then offsets inside a member (arrays)."""
    def at(rows, o):
        for start, size, name in rows:
            if start == o:
                return name, True
        for start, size, name in rows:
            if start < o < start + size:
                return f"{name}+0x{o - start:X}", False
        return None, False
    if want:
        if want not in lay:
            print(f"struct {want}: not measured (not in include/, or its header did not compile)")
            return 1
        L = lay[want]
        print(f"struct {want} ({L['size']:#x} bytes, include/{L['header']}):")
        for o in offs:
            name, exact = at(L["fields"], o)
            print(f"  {o:#6x}  {name or '(beyond the struct)' if o >= L['size'] else name or '(padding)'}")
        return 0
    ranked = []
    for s, L in lay.items():
        if max(offs) >= L["size"]:
            continue
        hits = [at(L["fields"], o) for o in offs]
        exact = sum(1 for n, e in hits if e)
        inside = sum(1 for n, e in hits if n and not e)
        if exact:
            ranked.append((exact, inside, -L["size"], s, hits))
    ranked.sort(reverse=True)
    print(f"structs in include/ with members at {', '.join(f'{o:#x}' for o in offs)}"
          f" ({len(ranked)} have at least one exactly):")
    for exact, inside, negsize, s, hits in ranked[:show]:
        print(f"  {exact}/{len(offs)}  {s} ({-negsize:#x} bytes, include/{lay[s]['header']}): "
              + ", ".join(f"{o:#x} {n or '-'}" for o, (n, e) in zip(offs, hits)))
    return 0


def todo(index, status, funcs, sizes, asm_ranges, show=20, order="addr", tabs=None, arm=()):
    """Phase-1 work list: functions with no C at all (not even a draft), outside the reviewed assembly
    ranges, in ROM order (take them in turn), with how much matched prior art `like` finds for each.
    ORDER 'prior' (most covered first) or 'size' (smallest first) sorts it otherwise."""
    import math
    addr = dict(funcs)
    todo_ = [n for n, s in status.items() if s == "undrafted" and n in addr
             and not any(lo <= addr[n] < hi for lo, hi in asm_ranges)]
    sh, df, idf = tabs or tables(index)
    matched = [(n, v) for n, v in sh.items() if status.get(n) == "matched"]
    # inverted index: shingle -> matched functions that have it, so each target looks at its own windows only
    inv = collections.defaultdict(list)
    for n, v in matched:
        for s in v:
            if df[s] <= 50:  # common windows (push/pop, loads) say nothing about prior art
                inv[s].append(n)
    rows = []
    for n in todo_:
        mine = sh.get(n, set())
        self_w = sum(idf[s] for s in mine) or 1
        acc = collections.Counter()
        for s in mine:
            for m in inv.get(s, ()):
                acc[m] += idf[s]
        best, cover = max(((m, w / self_w) for m, w in acc.items()), key=lambda x: x[1], default=(None, 0.0))
        rows.append((n, addr[n], sizes.get(n, 0), cover, best))
    key = {"prior": lambda r: (-r[3], r[2]), "size": lambda r: (r[2], -r[3]), "addr": lambda r: r[1]}[order]
    rows.sort(key=key)
    total = sum(r[2] for r in rows)
    print(f"{len(rows)} functions with no C ({total} bytes), outside the reviewed assembly ranges;"
          f" by {order} (covers = share of its rare 3-insn windows found in one matched function):")
    for n, a, size, cover, best in rows[:show]:
        print(f"  {a:08X} {size:6} B  covers {cover * 100:3.0f}%  {n:34} {('like ' + best) if best else ''}"
              + ("  [ARM: no scope compile]" if n in arm else ""))
    if len(rows) > show:
        print(f"  ... {len(rows) - show} more (todo N; todo N prior|size)")
    first = next((r for r in rows if r[0] not in arm), None)
    if first:
        print(f"next: scope.py new {first[0]}")
    return 0

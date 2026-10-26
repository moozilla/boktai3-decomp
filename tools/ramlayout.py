#!/usr/bin/env python3
"""The RAM inventory, symbols/ram.csv: one row per object in EWRAM/IWRAM.

    python3 tools/ramlayout.py check            # inventory + declarations; prints the accounting
    python3 tools/ramlayout.py show 030042A0     # a row, its declarations and consumers
    python3 tools/ramlayout.py unify 030042E4 --decl 'extern u8 *gPaletteBankData;' \\
        --header include/render.h [--note '...']
    python3 tools/ramlayout.py unify --batch FILE --header include/render.h   # ADDR | DECL | NOTE lines
    python3 tools/ramlayout.py seed [--kit DIR]  # (re)build rows from the ELF and layoutmerge

Columns: addr, name, size (hex bytes, blank when unknown), kind (object, or
interior for a label inside another object), decl (the one declaration, with
the name), header (where it lives), confidence, notes.

An object is *unified* when its row has a header and decl, the header holds
that declaration, and no other file declares the object. `unify` gets there
mechanically: it renames gUnk_X to the new name everywhere, puts the
declaration in the header, deletes every identical declaration elsewhere (a
header that had one includes the new header instead), and leaves files whose
declaration differs alone, listing them. Every file that changes, or includes a
header that changes, is compiled before and after, with and without
-DNONMATCHING; the assembly must be identical apart from the name, or nothing
is written. See docs/RAM.md.
"""
import argparse
import concurrent.futures
import csv
import json
import os
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(__file__))
from romlib import ROOT  # noqa: E402

CSV = os.path.join(ROOT, "symbols", "ram.csv")
COLS = ["addr", "name", "size", "kind", "decl", "header", "confidence", "notes"]
RAM = [(0x02000000, 0x02040000), (0x03000000, 0x03008000)]


def is_ram(v):
    return any(lo <= v < hi for lo, hi in RAM)


def load():
    if not os.path.exists(CSV):
        return []
    with open(CSV, newline="") as f:
        return list(csv.DictReader(f))


def save(rows):
    rows.sort(key=lambda r: (int(r["addr"], 16), r["kind"] != "object", r["name"]))
    with open(CSV, "w", newline="") as f:
        w = csv.DictWriter(f, COLS, lineterminator="\n")
        w.writeheader()
        w.writerows({c: r.get(c, "") for c in COLS} for r in rows)


def placeholder(addr):
    return f"gUnk_{addr.upper()}"


def names_of(r):
    """Every spelling the sources may use for this row."""
    ph = placeholder(r["addr"])
    return [ph] if r["name"] == ph else [r["name"], ph]


# --- reading declarations -------------------------------------------------

def sources():
    out = []
    for d, _, fs in os.walk(os.path.join(ROOT, "src")):
        out += [os.path.relpath(os.path.join(d, f), ROOT) for f in fs if f.endswith(".c")]
    out += [os.path.join("include", f) for f in os.listdir(os.path.join(ROOT, "include")) if f.endswith(".h")]
    return sorted(out)


def read(path):
    with open(os.path.join(ROOT, path)) as f:
        return f.read()


def split_top(s, sep=","):
    parts, depth, cur = [], 0, ""
    for ch in s:
        depth += ch in "([{"
        depth -= ch in ")]}"
        if ch == sep and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += ch
    return parts + [cur]


DECL = re.compile(r"^[ \t]*extern\s+([^;(){}=]*?)\s*;[ \t]*(/\*.*?\*/|//[^\n]*)?[ \t]*\n", re.M | re.S)


def declarations(text, name):
    """(match, specifier, declarators, index) for each file-scope extern line naming `name`."""
    out = []
    for m in DECL.finditer(text):
        body = m.group(1)
        if not re.search(rf"\b{name}\b", body):
            continue
        # specifier = text up to the first declarator; declarators split on top-level commas
        dm = re.match(r"((?:const\s+|volatile\s+|unsigned\s+|signed\s+|struct\s+\w+\s*|union\s+\w+\s*|\w+\s+)+?)"
                      r"(\**\s*\w+\s*(?:\[[^\]]*\]\s*)*(?:,.*)?)$", body, re.S)
        if not dm:
            continue
        decls = [d.strip() for d in split_top(dm.group(2))]
        idx = next((i for i, d in enumerate(decls) if re.search(rf"\b{name}\b", d)), None)
        if idx is not None:
            out.append((m, " ".join(dm.group(1).split()), decls, idx))
    return out


def norm(spec, declarator, name, canon):
    d = re.sub(rf"\b{name}\b", canon, declarator)
    d = re.sub(r"\s+", "", d)
    return f"{spec} {d}"


def parse_decl(decl):
    """'extern u8 *gX; ' -> (normalized form, name)."""
    text = decl.strip()
    if not text.endswith("\n"):
        text += "\n"
    m = DECL.search(text)
    if not m:
        sys.exit(f"cannot read declaration {decl!r}")
    dm = re.match(r"(.*?)(\**\s*(\w+)\s*(?:\[[^\]]*\]\s*)*)$", m.group(1), re.S)
    spec, d, name = " ".join(dm.group(1).split()), dm.group(2), dm.group(3)
    return norm(spec, d, name, name), name


# --- check / accounting ---------------------------------------------------

def survey(rows, texts=None):
    """{addr: {'decls': {file: [normalized]}, 'uses': set(files)}} for every row."""
    texts = texts or {p: read(p) for p in sources()}
    by_name = {}
    for r in rows:
        for n in names_of(r):
            by_name[n] = r
    word = re.compile(r"\b(" + "|".join(sorted(by_name, key=len, reverse=True)) + r")\b") if by_name else None
    info = {r["addr"]: {"decls": {}, "uses": set()} for r in rows}
    for p, t in texts.items():
        if word is None:
            break
        found = set(word.findall(t))
        for n in found:
            r = by_name[n]
            i = info[r["addr"]]
            i["uses"].add(p)
            for _, spec, decls, idx in declarations(t, n):
                i["decls"].setdefault(p, []).append(norm(spec, decls[idx], n, r["name"]))
    return info


def status(r, i):
    """unified / partial / named / unnamed, plus the files with a declaration of their own."""
    canon = parse_decl(r["decl"])[0] if r.get("decl") else None
    other = sorted(p for p, ds in i["decls"].items() if p != r["header"])
    if not canon or not r["header"]:
        return ("named" if r["name"] != placeholder(r["addr"]) else "unnamed"), other
    held = canon in i["decls"].get(r["header"], [])
    if not held:
        return "broken", other
    return ("unified" if not other else "partial"), other


def measures(rows=None, info=None):
    """Byte counts for the progress report (no ROM needed)."""
    rows = load() if rows is None else rows
    info = survey(rows) if info is None else info
    total = matched = complete = 0
    for r in rows:
        if r["kind"] != "object" or not r["size"]:
            continue
        size = int(r["size"], 16)
        st, _ = status(r, info[r["addr"]])
        total += size
        matched += size if st in ("unified", "partial") else 0
        complete += size if st == "unified" else 0
    return {"total_data": total, "matched_data": matched, "complete_data": complete}


def cmd_check(a):
    rows = load()
    info = survey(rows)
    bad = 0
    names = {}
    for r in rows:
        if r["name"] in names:
            print(f"{r['addr']}: name {r['name']} also used at {names[r['name']]}")
            bad += 1
        names[r["name"]] = r["addr"]
        st, other = status(r, info[r["addr"]])
        if st == "broken":
            print(f"{r['addr']} {r['name']}: {r['header']} does not declare `{r['decl']}`")
            bad += 1
        elif st == "partial" and a.verbose:
            print(f"{r['addr']} {r['name']}: own declarations in {', '.join(other)}")
    objs = [r for r in rows if r["kind"] == "object"]
    last = None
    for r in objs:
        if last and r["size"] and last["size"] and int(last["addr"], 16) + int(last["size"], 16) > int(r["addr"], 16):
            print(f"{last['addr']} {last['name']}: size {last['size']} overlaps {r['addr']} {r['name']}")
            bad += 1
        last = r
    m = measures(rows, info)
    counts = {}
    for r in objs:
        counts[status(r, info[r["addr"]])[0]] = counts.get(status(r, info[r["addr"]])[0], 0) + 1
    print(f"{len(objs)} objects ({sum(1 for r in objs if r['size'])} sized, {m['total_data']} bytes); "
          + ", ".join(f"{v} {k}" for k, v in sorted(counts.items())))
    print(f"declared in one header: {m['matched_data']} bytes; no other declaration left: {m['complete_data']} bytes")
    sys.exit(1 if bad else 0)


def cmd_show(a):
    rows = load()
    r = find(rows, a.addr)
    i = survey([r])[r["addr"]]
    st, other = status(r, i)
    for c in COLS:
        print(f"{c:>10}: {r.get(c, '')}")
    print(f"{'status':>10}: {st}")
    forms = {}
    for p, ds in i["decls"].items():
        for d in ds:
            forms.setdefault(d, []).append(p)
    for d, ps in sorted(forms.items(), key=lambda kv: -len(kv[1])):
        print(f"  {len(ps):4}  {d}   e.g. {ps[0]}")
    print(f"  used by {len(i['uses'])} files")


def find(rows, addr):
    addr = addr.upper().removeprefix("0X").removeprefix("GUNK_")
    for r in rows:
        if r["addr"] == addr or r["name"] == addr:
            return r
    sys.exit(f"no row for {addr} in symbols/ram.csv")


# --- unify ----------------------------------------------------------------

def compile_asm(path, text, nonmatching):
    """agbcc assembly for `text` as if it were `path` (headers from the tree)."""
    sys.path.insert(0, os.path.dirname(__file__))
    import build
    flags = ["-DNONMATCHING"] if nonmatching else []
    d = os.path.dirname(path)
    with tempfile.NamedTemporaryFile("w", suffix=".c", dir=os.path.join(ROOT, d), delete=False) as f:
        f.write(text)
        tmp = f.name
    try:
        pre = subprocess.run(build.CPP + ["-P", "-nostdinc", "-undef"] + flags + ["-I", "include", "-iquote", ".", tmp],
                             cwd=ROOT, capture_output=True, text=True)
        if pre.returncode:
            return "CPP ERROR\n" + pre.stderr
        cflags = build.cflags_for(path)
        r = subprocess.run([build.compiler_for(path)] + cflags + ["-o", "-"], input=pre.stdout,
                           cwd=ROOT, capture_output=True, text=True)
        if r.returncode or "error" in r.stderr:
            return "CC ERROR\n" + r.stderr
        return r.stdout
    finally:
        os.unlink(tmp)


def includes(text):
    return set(re.findall(r'^\s*#\s*include\s+"([^"]+)"', text, re.M))


def add_include(text, header):
    inc = os.path.basename(header)
    if inc in includes(text):
        return text
    lines = text.split("\n")
    last = max((i for i, l in enumerate(lines) if re.match(r'\s*#\s*include\b', l)), default=None)
    if last is None:
        guard = next((i for i, l in enumerate(lines) if l.startswith("#define GUARD_")), -1)
        last = guard
    lines.insert(last + 1, f'#include "{inc}"')
    return "\n".join(lines)


def drop_decl(text, m, decls, idx):
    if len(decls) == 1:
        return text[:m.start()] + text[m.end():]
    rest = decls[:idx] + decls[idx + 1:]
    body = m.group(1)
    head = body[:body.index(decls[0])] if decls[0] in body else body.split(decls[0])[0]
    new = m.group(0).replace(body, head + ", ".join(rest), 1)
    return text[:m.start()] + new + text[m.end():]


def new_header(header):
    g = "GUARD_" + re.sub(r"\W", "_", os.path.basename(header)).upper()
    return (f"#ifndef {g}\n#define {g}\n\n#include \"global.h\"\n\n"
            f"/* RAM objects; each row of symbols/ram.csv with header {header}. */\n\n#endif\n")


def cmd_unify(a):
    rows = load()
    jobs = []
    if a.batch:
        for ln in open(a.batch):
            if ln.strip() and not ln.startswith("#"):
                addr, decl, *note = [x.strip() for x in ln.rstrip("\n").split("|")]
                jobs.append((find(rows, addr), decl, note[0] if note else ""))
    else:
        if not (a.addr and a.decl):
            sys.exit("unify needs ADDR --decl, or --batch FILE")
        jobs.append((find(rows, a.addr), a.decl, a.note))
    texts = {p: read(p) for p in sources()}
    created = not os.path.exists(os.path.join(ROOT, a.header))
    if created:
        texts[a.header] = new_header(a.header)
    new = dict(texts)
    renames = {}
    for r, decl, _ in jobs:
        name = parse_decl(decl)[1]
        for n in names_of(r):
            if n != name:
                renames[n] = name
    pat = re.compile(r"\b(" + "|".join(renames) + r")\b") if renames else None
    # `#define NAME gUnk_X` was a file's own name for the object: now redundant
    alias = re.compile(r"^[ \t]*#[ \t]*define[ \t]+(\w+)[ \t]+(\w+)[ \t]*(?://[^\n]*|/\*.*?\*/)?[ \t]*\n", re.M)
    aliased = set()
    for p, t in texts.items():
        if "define" in t:
            for m in alias.finditer(t):
                if renames.get(m.group(2)) == m.group(1):
                    aliased.add((p, m.group(1)))
            new[p] = alias.sub(lambda m: "" if renames.get(m.group(2)) == m.group(1) else m.group(0), t)
    taken = {n: p for p, t in new.items() for n in set(renames.values())
             if (p, n) not in aliased and re.search(rf"\b{n}\b", t)}
    taken = {n: p for n, p in taken.items() if n not in {r["name"] for r, _, _ in jobs}}
    if taken:
        sys.exit("names already used: " + ", ".join(f"{n} ({p})" for n, p in sorted(taken.items())))
    if pat:
        for p, t in new.items():
            if pat.search(t):
                new[p] = pat.sub(lambda m: renames[m.group(1)], t)
    hdr_inc = os.path.basename(a.header)
    # every declaration the header will hold: earlier rows and this batch
    canon = {r["name"]: parse_decl(r["decl"])[0] for r in rows if r["header"] == a.header and r["decl"]}
    canon.update({parse_decl(d)[1]: parse_decl(d)[0] for _, d, _ in jobs})
    differ, blocked = {}, set()
    for p in sorted(new):
        if p == a.header:
            continue
        for name, c in canon.items():
            if re.search(rf"\b{name}\b", new[p]):
                for _, spec, decls, idx in declarations(new[p], name):
                    if norm(spec, decls[idx], name, name) != c:
                        differ.setdefault(name, set()).add(p)
                        blocked.add(p)
    # a file that declares one of them differently cannot include the header
    for r, decl, note in jobs:
        c, name = parse_decl(decl)
        for p in sorted(new):
            if p == a.header or p in blocked or not re.search(rf"\b{name}\b", new[p]):
                continue
            t = new[p]
            for m, spec, decls, idx in reversed(declarations(t, name)):
                t = drop_decl(t, m, decls, idx)
                if p.endswith(".h") or hdr_inc not in includes(t):
                    t = add_include(t, a.header)
            new[p] = t
        h = new[a.header]
        if not any(norm(s, d[i], name, name) == c for _, s, d, i in declarations(h, name)):
            line = decl.strip() + (f" /* {note} */" if note else "")
            h = h[:h.rindex("#endif")].rstrip("\n") + "\n" + line + "\n\n#endif\n"
            new[a.header] = h
    changed = [p for p in new if new[p] != texts.get(p)]
    # A file needs compiling when its text changed or it includes a changed
    # header. Extern lines appended to the target header cannot change code
    # that never names them, so it counts only when new or with --all.
    hdrs = {os.path.basename(p) for p in changed if p.endswith(".h") and (p != a.header or created or a.all)}
    closure, grew = set(hdrs), True
    while grew:
        grew = False
        for p, t in new.items():
            if p.endswith(".h") and os.path.basename(p) not in closure and includes(t) & closure:
                closure.add(os.path.basename(p))
                grew = True
    verify = sorted(p for p, t in new.items() if p.endswith(".c")
                    and (new[p] != texts.get(p) or includes(t) & closure or p in blocked))
    ndiff = sum(len(v) for v in differ.values())
    print(f"{len(jobs)} objects: {len(changed)} files change, {ndiff} declarations differ and stay "
          f"({len(blocked)} files keep all their own); "
          f"compiling {len(verify)} files before and after")
    if a.dry_run:
        for n, ps in sorted(differ.items()):
            print(f"  {n} differs in: {' '.join(sorted(ps))}")
        return

    def before(p):
        return {nm: compile_asm(p, texts[p], nm) for nm in (False, True)} if "NONMATCHING" in texts[p] \
            else {False: compile_asm(p, texts[p], False)}
    with concurrent.futures.ThreadPoolExecutor(a.jobs) as ex:
        base = dict(zip(verify, ex.map(before, verify)))
    write(new, changed)
    failed, chosen = [], {}
    try:
        def same(p, t):
            for nm, asm in base[p].items():
                if "ERROR" in asm[:9]:
                    continue        # a draft that did not compile before
                mine = compile_asm(p, t, nm)
                if pat:
                    asm = pat.sub(lambda m: renames[m.group(1)], asm)
                if mine != asm:
                    return p, nm, mine
            return None

        def after(p):
            if p not in blocked:
                return same(p, new[p])
            # its own declaration differs: the shared one if the code is
            # unchanged, else its own (if the header stays out of sight),
            # else its own view as a cast at each use
            err = None
            for how, t in (("shared", adopt(new[p], canon, a.header, cast=False)), ("own", new[p]),
                           ("cast", adopt(new[p], canon, a.header, cast=True))):
                if t is None:
                    continue
                err = same(p, t)
                if err is None:
                    chosen[p] = (how, t)
                    return None
            return err
        with concurrent.futures.ThreadPoolExecutor(a.jobs) as ex:
            failed = [x for x in ex.map(after, verify) if x]
        for p, (how, t) in chosen.items():
            if t != new[p]:
                new[p] = t
                write(new, [p])
                if p not in changed:
                    changed.append(p)
    finally:
        if failed:
            write(texts, [p for p in changed if p in texts])
            for p in changed:
                if p not in texts:
                    os.unlink(os.path.join(ROOT, p))
    if failed:
        for p, nm, mine in failed[:10]:
            err = "ERROR" in mine[:9]
            print(f"  {p}{' (NONMATCHING)' if nm else ''}: {mine.splitlines()[0] if err else 'assembly differs'}")
            if err:
                print("   ", "\n    ".join(mine.splitlines()[1:6]))
        sys.exit(f"{len(failed)} files differ; nothing written")
    formatted(texts, new, changed)
    for r, decl, note in jobs:
        r.update(name=parse_decl(decl)[1], decl=decl.strip(), header=a.header)
        if note:
            r["notes"] = note
    save(rows)
    kept = {}
    for n, ps in sorted(differ.items()):
        for p in ps:
            how = chosen.get(p, ("own",))[0] if p.endswith(".c") else "own"
            kept.setdefault(how, []).append(f"{n}:{p}")
    for how in ("shared", "cast", "own"):
        if kept.get(how):
            print(f"  {how}: " + " ".join(sorted(kept[how])))
    print(f"{len(jobs)} objects unified in {a.header}; files whose declaration differed now use the shared one "
          f"({len(kept.get('shared', []))}), a cast at each use ({len(kept.get('cast', []))}), "
          f"or keep their own ({len(kept.get('own', []))})")


def adopt(t, canon, header, cast):
    """`t` with every declaration of the header's objects removed, so the
    header's declarations apply; with cast=True a different declaration
    becomes a cast at each use instead."""
    for name, c in canon.items():
        if not re.search(rf"\b{name}\b", t):
            continue
        exprs = set()
        for m, spec, decls, idx in reversed(declarations(t, name)):
            own = norm(spec, decls[idx], name, name)
            if own != c and cast:
                e = view(spec, decls[idx], name, c)
                if e is None:
                    return None
                exprs.add(e)
            t = drop_decl(t, m, decls, idx)
        if len(exprs) > 1:
            return None
        if exprs:
            e = exprs.pop()
            t = re.sub(rf"(?<![\w&.>]){name}\b", lambda _: e, t)
    return add_include(t, header)


def view(spec, declarator, name, canon):
    """A cast expression that gives `name` the type of `spec declarator`."""
    d = re.sub(r"\s+", "", declarator)
    stars = len(d) - len(d.lstrip("*"))
    dims = re.findall(r"\[[^\]]*\]", d)
    canon_array = canon.rstrip().endswith("]")
    if not dims:
        if canon_array:
            return None
        return f"(*({spec} {'*' * stars}*)&{name})"
    if not canon_array:
        return None
    rest = "".join(dims[1:])
    t = f"{spec} {'*' * stars}*" if not rest else f"{spec} {'*' * stars}(*){rest}"
    return f"(({t}){name})"


def formatted(texts, new, changed):
    """clang-format the changed files that were clang-formatted before
    (whitespace only, so the verified code stays the same)."""
    cf = os.path.join(ROOT, "build", "venv", "bin", "clang-format")
    if not os.path.exists(cf):
        return

    def fmt(text, p):
        return subprocess.run([cf, "--assume-filename=" + p], input=text, cwd=ROOT,
                              capture_output=True, text=True).stdout
    for p in changed:
        if p in texts and fmt(texts[p], p) == texts[p]:
            out = fmt(new[p], p)
            if out and out != new[p]:
                new[p] = out
                write(new, [p])


def write(texts, paths):
    for p in paths:
        with open(os.path.join(ROOT, p), "w") as f:
            f.write(texts[p])


# --- seed -----------------------------------------------------------------

def elf_ram():
    out = subprocess.run(["arm-none-eabi-readelf", "-sW", os.path.join(ROOT, "build", "boktai3.elf")],
                         capture_output=True, text=True, check=True).stdout
    syms = {}
    for ln in out.splitlines():
        p = ln.split()
        if len(p) == 8 and p[0][:-1].isdigit() and p[6] != "UND" and p[3] != "FUNC":
            v = int(p[1], 16)
            if is_ram(v) and not p[7].startswith(("$", ".")):
                syms.setdefault(v, []).append(p[7])
    return syms


def cmd_seed(a):
    keep = {r["addr"]: r for r in load()}
    lay = json.load(open(os.path.join(ROOT, "build", "layouts", "layouts.json")))
    kit = {}
    if a.kit:
        for e in json.load(open(os.path.join(a.kit, "layout", "reviewed_address_map.json")))["addresses"]:
            kit[e["address"].upper().removeprefix("0X")] = e
    syms = elf_ram()
    rows = {}
    for v, ns in sorted(syms.items()):
        addr = f"{v:08X}"
        ns = sorted(ns, key=lambda n: (n != placeholder(addr), not n.startswith("gUnk_"), n))
        name = next((n for n in ns if not n.startswith(("gUnk_", "gUnknown_"))), ns[0])
        e = lay.get(f"{name}:direct") or next((lay[f"{n}:direct"] for n in ns if f"{n}:direct" in lay), None)
        k = kit.get(addr, {})
        r = {"addr": addr, "name": name, "kind": "object", "confidence": "", "notes": "", "decl": "", "header": ""}
        size = e["extent"] if e else 0
        if k.get("span_bytes"):
            size = max(size, int(k["span_bytes"]))
        if e and e.get("inside"):
            r["kind"] = "interior"
            r["notes"] = f"{e['inside'][0]} +0x{e['inside'][1]:X}"
            size = 0
        r["size"] = f"{size:X}" if size else ""
        if k:
            r["confidence"] = k.get("confidence", "")
            r["notes"] = "; ".join(x for x in (r["notes"], k.get("meaning", "")) if x)
        old = keep.get(addr)
        if old:
            for c in ("name", "decl", "header", "confidence", "notes", "kind", "size"):
                if old.get(c):
                    r[c] = old[c]
        rows[addr] = r
    # clip each object to the next object's start
    objs = [r for r in rows.values() if r["kind"] == "object"]
    for r, nxt in zip(objs, objs[1:]):
        if r["size"]:
            room = int(nxt["addr"], 16) - int(r["addr"], 16)
            if int(r["size"], 16) > room:
                r["size"] = f"{room:X}"
    save(list(rows.values()))
    print(f"{len(rows)} rows ({len(objs)} objects, {sum(1 for r in objs if r['size'])} sized) -> symbols/ram.csv")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("check")
    s.add_argument("-v", "--verbose", action="store_true")
    s = sub.add_parser("show")
    s.add_argument("addr")
    s = sub.add_parser("seed")
    s.add_argument("--kit", help="research kit directory with layout/reviewed_address_map.json")
    s = sub.add_parser("unify")
    s.add_argument("addr", nargs="?")
    s.add_argument("--decl", help="the declaration, e.g. 'extern u8 *gPaletteBankData;'")
    s.add_argument("--batch", help="file of ADDR | DECL | NOTE lines")
    s.add_argument("--all", action="store_true", help="also compile every includer of the target header")
    s.add_argument("--header", required=True)
    s.add_argument("--note", default="")
    s.add_argument("--dry-run", action="store_true")
    s.add_argument("--jobs", type=int, default=os.cpu_count())
    a = ap.parse_args()
    {"check": cmd_check, "show": cmd_show, "seed": cmd_seed, "unify": cmd_unify}[a.cmd](a)


if __name__ == "__main__":
    main()

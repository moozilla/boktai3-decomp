#!/usr/bin/env python3
"""Group every game function into a subsystem unit, and every unit into a category.

  subsystems.py                  print the units and categories (ROM order)
  subsystems.py --tree           print the category -> unit hierarchy with sizes
  subsystems.py --regroup        recompute the generated rows of symbols/progress_subsystems.csv
                                 (needs gen/code_sym.s and gen/data.s: run `make disasm`)

The progress report (tools/progress.py, shown by decomp.dev) has three levels:
a top-level category (symbols/progress_categories.csv), a unit per object or
subsystem (symbols/progress_subsystems.csv), and the unit's functions. Reviewed
rows of progress_subsystems.csv (evidence not starting with "auto:") are kept as
they are. --regroup replaces the generated rows: the code between reviewed rows
is cut into contiguous objects by the name prefixes of its functions (Prefix_Action,
docs/NAMING.md), smoothed with a change penalty so one stray prefix does not split
an object, and unnamed functions are pulled to the object they call, are called
by, or share a pointer table or data label with. Objects under MIN_BYTES are folded
into their closest neighbour. Credit is unchanged: grouping only changes how
units are displayed.

Each unit gets one category: the `category` column of its row if set, else the
first rule in progress_categories.csv whose name regex or address range matches.
"""
import argparse
import collections
import csv
import json
import math
import os
import re
import sys
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import progress  # noqa: E402
from romlib import ROOT  # noqa: E402

CSV = Path(ROOT) / "symbols" / "progress_subsystems.csv"
FIELDS = ["name", "start", "end", "category", "name_files", "evidence"]
GAME_START = 0x0800145C
CHANGE_PENALTY = 2.0   # cost of starting a new object, in units of name evidence
MIN_BYTES = 1024       # smaller generated objects merge into a neighbour
LOCAL = 0x8000         # graph edges farther apart than this are ignored


def prefix(name):
    if re.match(r"^sub_[0-9A-F]{8}$", name) or "_" not in name:
        return None
    return name.split("_", 1)[0]


ACRONYMS = {"Hud": "HUD", "Npc": "NPC", "Rfu": "RFU", "Bg": "BG", "Oam": "OAM", "Gfx": "GFX", "Ui": "UI"}


def spaced(p):
    """ScenePanel -> Scene panel."""
    words = re.findall(r"[A-Z][a-z]+|[0-9A-F]{2,}(?![a-z])|[A-Z]+(?![a-z])|[a-z0-9]+|[0-9]+", p)
    words = [ACRONYMS.get(w, w) for w in words]
    return " ".join([words[0]] + [w if w.isupper() else w.lower() for w in words[1:]]) if words else p


def is_generated(row):
    return row["evidence"].startswith("auto:") or "(auto" in row["name"]


def read_rows(path=CSV):
    with Path(path).open(newline="") as f:
        return [{k: r.get(k) or "" for k in FIELDS} for r in csv.DictReader(f)]


def write_rows(rows, path=CSV):
    rows.sort(key=lambda r: int(r["start"], 16))
    with Path(path).open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=FIELDS, lineterminator="\n")
        w.writeheader()
        w.writerows(rows)


def graph(funcs):
    """Undirected affinity between functions: calls, function-pointer literals,
    shared pointer tables and shared (rarely used) data labels."""
    idx = {n for n, _ in funcs}
    code = Path(ROOT, "gen", "code_sym.s")
    data = Path(ROOT, "gen", "data.s")
    if not code.exists() or not data.exists():
        sys.exit("gen/code_sym.s and gen/data.s are needed: run `make disasm`")
    tables = collections.defaultdict(set)
    label = None
    for line in data.open():
        m = re.match(r"^(\w+):", line)
        if m:
            label = m.group(1)
            continue
        m = re.match(r"^\t\.4byte (\w+)", line)
        if m and label and m.group(1) in idx:
            tables[label].add(m.group(1))
    adj = collections.defaultdict(lambda: collections.defaultdict(float))

    def link(a, b, w):
        if a != b and a in idx and b in idx:
            adj[a][b] += w
            adj[b][a] += w

    users = collections.defaultdict(set)
    func = None
    for line in code.open():
        m = re.match(r"^\t(?:thumb|arm|non_word_aligned_thumb)_func_start (\S+)$", line)
        if m:
            func = m.group(1)
            continue
        s = line.strip()
        if func is None:
            continue
        if s.startswith("bl "):
            link(func, s[3:].split()[0], 1.0)
        elif ".4byte" in s:
            t = s.split(".4byte", 1)[1].strip().split()[0].split("+")[0]
            if t in idx:
                link(func, t, 1.0)
            elif re.match(r"^[A-Za-z_]\w*$", t):
                users[t].add(func)
                for f in tables.get(t, ()):
                    link(func, f, 1.0)
    for groups, limit in ((tables, 40), (users, 6)):
        for fs in groups.values():
            fs = sorted(fs)
            if 2 <= len(fs) <= limit:
                for i, a in enumerate(fs):
                    for b in fs[i + 1:]:
                        link(a, b, (2.0 if groups is tables else 1.0) / len(fs))
    return adj


def regroup(funcs, sizes, rows, libraries):
    adj = graph(funcs)
    addr = {n: a for n, a in funcs}
    starts = [a for _, a in funcs] + [progress.CODE_END]
    kept = [r for r in rows if not is_generated(r)]
    fixed = sorted([(int(r["start"], 16), int(r["end"], 16), r["name"]) for r in kept]
                   + [(lib["start"], lib["end"], None) for lib in libraries])

    def fixed_of(a):
        return next((f for f in fixed if f[0] <= a < f[1]), None)

    # A prefix used all over the ROM (SceneActor, Actor) is weaker evidence than
    # one used in a single place (Ghoul).
    spread = collections.defaultdict(set)
    for n, a in funcs:
        if prefix(n):
            spread[prefix(n)].add(a >> 15)
    weight = {p: 1.0 / len(w) ** 0.7 + 0.5 * (len(w) <= 2) for p, w in spread.items()}

    def evidence(n):
        ev = collections.Counter()
        if prefix(n):
            ev[prefix(n)] += weight[prefix(n)]
        for m, w in adj[n].items():
            if abs(addr[m] - addr[n]) > LOCAL:
                continue
            if prefix(m):
                ev[prefix(m)] += 0.3 * w * (weight[prefix(m)] - 0.5 * (len(spread[prefix(m)]) <= 2))
            f = fixed_of(addr[m])
            if f and f[2]:
                ev["@" + f[2]] += 0.3 * w
        return ev

    regions = []
    for i, (n, a) in enumerate(funcs):
        if a < GAME_START or fixed_of(a):
            continue
        if regions and regions[-1][-1] == i - 1:
            regions[-1].append(i)
        else:
            regions.append([i])

    segments = []
    for reg in regions:
        left = fixed_of(funcs[reg[0]][1] - 1)
        right = fixed_of(starts[reg[-1] + 1])
        edge = {"@" + f[2] for f in (left, right) if f and f[2]}
        evs = [collections.Counter({k: v for k, v in evidence(funcs[i][0]).items()
                                    if not k.startswith("@") or k in edge}) for i in reg]
        labels = sorted(set().union(*evs) | edge) or ["?"]
        # Viterbi over labels: reward evidence, pay CHANGE_PENALTY per change.
        cost = [0.0 if (left and f"@{left[2]}" == lab) else CHANGE_PENALTY / 2 for lab in labels]
        cost = [c - evs[0].get(lab, 0.0) for c, lab in zip(cost, labels)]
        back = []
        for e in evs[1:]:
            best = min(range(len(labels)), key=cost.__getitem__)
            row, new = [], []
            for j, lab in enumerate(labels):
                if cost[j] <= cost[best] + CHANGE_PENALTY:
                    row.append(j)
                    new.append(cost[j] - e.get(lab, 0.0))
                else:
                    row.append(best)
                    new.append(cost[best] + CHANGE_PENALTY - e.get(lab, 0.0))
            back.append(row)
            cost = new
        j = min(range(len(labels)), key=lambda j: cost[j] + (
            0 if (right and f"@{right[2]}" == labels[j]) else CHANGE_PENALTY / 2))
        path = [j]
        for row in reversed(back):
            j = row[j]
            path.append(j)
        path.reverse()
        segs = []
        for i, j in zip(reg, path):
            if segs and segs[-1]["label"] == labels[j]:
                segs[-1]["end"] = i
            else:
                segs.append({"label": labels[j], "start": i, "end": i})
        # An object is named after a prefix it actually contains.
        for s in segs:
            if s["label"].startswith("@"):
                continue
            names = [funcs[i][0] for i in range(s["start"], s["end"] + 1)]
            if not any(prefix(n) == s["label"] for n in names):
                found = collections.Counter(prefix(n) for n in names if prefix(n))
                s["label"] = found.most_common(1)[0][0] if found else "?"
        # A reviewed row can only grow into the code next to it.
        for k, s in enumerate(segs):
            if s["label"].startswith("@") and not (
                    (k == 0 and left and s["label"] == f"@{left[2]}")
                    or (k == len(segs) - 1 and right and s["label"] == f"@{right[2]}")):
                s["label"] = segs[k - 1]["label"] if k else segs[k + 1]["label"]
        merged = []
        for s in segs:
            if merged and merged[-1]["label"] == s["label"]:
                merged[-1]["end"] = s["end"]
            else:
                merged.append(s)
        segs = merged
        segments.append((segs, left, right))

    def size(s):
        return starts[s["end"] + 1] - funcs[s["start"]][1]

    def affinity(s, t):
        a = {funcs[i][0] for i in range(s["start"], s["end"] + 1)}
        b = {funcs[i][0] for i in range(t["start"], t["end"] + 1)}
        return sum(w for n in a for m, w in adj[n].items() if m in b)

    out = []
    for segs, left, right in segments:
        # Fold small objects into the neighbour they are most connected to.
        while len(segs) > 1:
            small = [k for k, s in enumerate(segs) if size(s) < MIN_BYTES and not s["label"].startswith("@")]
            if not small:
                break
            k = min(small, key=lambda k: size(segs[k]))
            nbrs = [m for m in (k - 1, k + 1) if 0 <= m < len(segs)]
            m = max(nbrs, key=lambda m: (affinity(segs[k], segs[m]), m < k))
            lo, hi = sorted((k, m))
            keep = segs[m]
            segs[lo:hi + 1] = [{"label": keep["label"], "start": segs[lo]["start"], "end": segs[hi]["end"],
                                "absorbed": keep.get("absorbed", []) + segs[k].get("absorbed", [])
                                + [segs[k]["label"]]}]
            # Two neighbours with the same label become one.
            merged = []
            for s in segs:
                if merged and merged[-1]["label"] == s["label"]:
                    merged[-1]["end"] = s["end"]
                    merged[-1]["absorbed"] = merged[-1].get("absorbed", []) + s.get("absorbed", [])
                else:
                    merged.append(s)
            segs[:] = merged
        # A lone small object between reviewed rows joins one of them.
        if len(segs) == 1 and size(segs[0]) < MIN_BYTES and segs[0]["label"] != "@":
            for f in (left, right):
                if f and f[2] and not segs[0]["label"].startswith("@"):
                    segs[0]["label"] = f"@{f[2]}"
        out.extend(segs)

    # Generated rows; segments attached to a reviewed row extend that row.
    byname = {r["name"]: r for r in kept}
    new = []
    for s in out:
        lo, hi = funcs[s["start"]][1], starts[s["end"] + 1]
        if s["label"].startswith("@"):
            r = byname[s["label"][1:]]
            r["start"] = f"{min(lo, int(r['start'], 16)):08X}"
            r["end"] = f"{max(hi, int(r['end'], 16)):08X}"
            continue
        names = [funcs[i][0] for i in range(s["start"], s["end"] + 1)]
        hits = sum(prefix(n) == s["label"] for n in names)
        others = sorted({x for x in s.get("absorbed", []) if x != s["label"] and not x.startswith("@")})
        ev = f"auto: {hits} of {len(names)} functions named {s['label']}_*"
        if others:
            ev += "; also " + " ".join(f"{o}_*" for o in others[:6]) + (" ..." if len(others) > 6 else "")
        new.append({"name": spaced(s["label"]) if s["label"] != "?" else f"Unnamed {lo:08X}",
                    "start": f"{lo:08X}", "end": f"{hi:08X}", "category": "", "name_files": "",
                    "evidence": ev})
    # Rows that share a name are one unit in the report, so an object cut
    # into several ranges keeps its name. Only a clash with a reviewed row
    # is numbered.
    reviewed = {r["name"] for r in kept}
    seen = collections.Counter()
    for r in sorted(new, key=lambda r: r["start"]):
        if r["name"] in reviewed:
            base = r["name"]
            seen[base] += 1
            r["name"] = f"{base} {seen[base]}"
            while r["name"] in reviewed:
                seen[base] += 1
                r["name"] = f"{base} {seen[base]}"
    return kept + new


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--regroup", action="store_true")
    ap.add_argument("--tree", action="store_true")
    a = ap.parse_args()
    funcs, sizes = progress.load_functions()
    libraries = progress.load_libraries(funcs, sizes)
    rows = read_rows()
    if a.regroup:
        rows = regroup(funcs, sizes, rows, libraries)
        write_rows(rows)
        print(f"wrote {CSV.relative_to(ROOT)}: {len(rows)} rows")
    rules = progress.load_categories()
    groups = progress.load_groups(funcs, sizes, CSV, "game")
    tree = collections.defaultdict(list)
    for g in groups:
        tree[progress.category_of(rules, g)].append(g)
    total = sum(sizes.values())
    covered = sum(g["end"] - g["start"] for g in groups)
    for cid, (label, _) in rules.items():
        units = tree.get(cid, [])
        if not units:
            continue
        b = sum(g["end"] - g["start"] for g in units)
        print(f"{label}: {len(units)} units, {b} bytes ({100 * b / total:.1f}%)")
        if a.tree or not a.regroup:
            for g in units:
                print(f"    {g['start']:08X} {g['end'] - g['start']:7d}  {g['name']}")
    print(f"{covered} of {total} code bytes in game units, the rest in libraries")


if __name__ == "__main__":
    main()

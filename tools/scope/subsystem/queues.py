#!/usr/bin/env python3
"""One queue per report category of every NONMATCHING draft, in ROM order.

  queues.py OUTDIR [--exclude src/iwram_code.c ...]

Reads PROGRESS.json and src/ of the checkout it runs in (run it on an up-to-date main). Writes
OUTDIR/<category-slug>.json: rows {func, addr, size, fuzzy, src, unit}, the format
tools/scope/campaign.py land reads from build/scope/campaign/queue.json.
"""
import collections, glob, json, os, re, sys

out = sys.argv[1]
exclude = set(sys.argv[sys.argv.index("--exclude") + 1:]) if "--exclude" in sys.argv else set()
prog = json.load(open("PROGRESS.json"))
drafts = {}
for f in glob.glob("src/**/*.c", recursive=True):
    t = open(f, errors="replace").read()
    for m in re.finditer(r"#ifdef NONMATCHING(.*?)#else(.*?)#endif", t, re.S):
        for n in re.findall(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)', m.group(2)):
            drafts[n] = f
by = collections.defaultdict(list)
for c in prog["categories"]:
    for u in c["units"]:
        for name, addr, size, fuzzy, *_ in u["functions"]:
            src = drafts.get(name) or drafts.get("sub_" + addr)
            if fuzzy < 100 and src and src not in exclude:
                by[c["name"]].append(dict(func=name, addr=addr, size=size, fuzzy=fuzzy, src=src, unit=u["name"]))
os.makedirs(out, exist_ok=True)
for cat, rows in sorted(by.items()):
    rows.sort(key=lambda r: int(r["addr"], 16))   # ROM order: hard functions are not deferred
    slug = re.sub(r"[^a-z]+", "-", cat.lower()).strip("-")
    json.dump(rows, open(f"{out}/{slug}.json", "w"), indent=1)
    print(f"{slug:28} {len(rows):4} drafts {sum(r['size'] for r in rows):8} B   \"{cat}\"")

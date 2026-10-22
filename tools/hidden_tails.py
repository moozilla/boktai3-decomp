#!/usr/bin/env python3
"""Find and register functions hidden in raw .byte tails.

gbadisasm leaves some code as `.byte` runs, usually after a function whose
literal pool confused it. Those runs often hold several complete functions
(script commands, getters) that no inventory lists.

  hidden_tails.py scan            list raw runs with likely function starts
  hidden_tails.py split S,E,DESC  register a start S (end E, or empty for
                                  retained assembly) in build_extra_functions.csv
                                  and progress_extra_functions.csv, then refresh
                                  the progress inventory. Repeat the argument
                                  for several functions.

A start candidate is a `push {..., lr}` halfword right after a `bx`, a
`pop {pc}` or a literal word. Check each with objdump before splitting.
"""
import csv
import io
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(__file__))
from romlib import ROOT  # noqa: E402
from split import extra_function_starts  # noqa: E402

LABEL = re.compile(r"^(?:\w+): @ 0x([0-9A-F]{8})|^_([0-9A-F]{8}):")


def raw_runs():
    runs, cur, addr = [], None, None
    for line in open(os.path.join(ROOT, "gen/code.s")).read().split("\n"):
        m = LABEL.match(line)
        if m:
            addr, cur = int(m.group(1) or m.group(2), 16), None
            continue
        if line.startswith("\t.byte"):
            data = [int(x, 16) for x in re.findall(r"0x([0-9A-Fa-f]{2})", line)]
            if cur is None:
                cur = [addr, []]
                runs.append(cur)
            cur[1] += data
        elif line.strip() and not line.startswith(("\t.align", "@")):
            cur = None
    return [(a, b) for a, b in runs if a is not None]


def starts(data):
    hits = []
    for o in range(0, len(data) - 1, 2):
        if data[o + 1] != 0xB5:
            continue
        if o == 0 or data[o - 1] in (0x47, 0xBD) or (o >= 4 and data[o - 1] in (0x00, 0x02, 0x03, 0x04, 0x06, 0x08)):
            hits.append(o)
    return hits


def scan():
    extra = extra_function_starts()
    with open(os.path.join(ROOT, "symbols/progress_functions.csv")) as f:
        extra |= {int(r["address"], 16) for r in csv.DictReader(f)}
    total = 0
    for addr, data in raw_runs():
        h = [addr + o for o in starts(data) if addr + o not in extra]
        if h:
            total += len(h)
            print(f"{addr:08X} {len(data):5d}  " + " ".join(f"{x:08X}" for x in h))
    print(f"{total} unregistered start candidates")


def upsert(path, key_rows, ncols):
    rows = list(csv.reader(io.StringIO(open(path).read())))
    hdr, body = rows[0], {r[0]: r for r in rows[1:] if r}
    for r in key_rows:
        body[r[0]] = r[:ncols]
    out = io.StringIO()
    w = csv.writer(out, lineterminator="\n")
    w.writerow(hdr)
    w.writerows(sorted(body.values(), key=lambda r: int(r[0], 16)))
    open(path, "w").write(out.getvalue())


def split(args):
    extra, prog = [], []
    for a in args:
        s, e, desc = a.split(",", 2)
        s, e = s.upper(), e.upper()
        extra.append([s, "thumb", desc, e])
        prog.append([s, desc])
    upsert(os.path.join(ROOT, "symbols/build_extra_functions.csv"), extra, 4)
    path = os.path.join(ROOT, "symbols/progress_extra_functions.csv")
    have = {r[0] for r in csv.reader(open(path)) if r}
    upsert(path, [r for r in prog if r[0] not in have], 2)
    subprocess.run([sys.executable, os.path.join(ROOT, "tools/progress.py"), "--refresh-inventory",
                    os.path.join(ROOT, "gen/code_sym.s")], check=True, stdout=subprocess.DEVNULL)


if __name__ == "__main__":
    if len(sys.argv) >= 2 and sys.argv[1] == "scan":
        scan()
    elif len(sys.argv) >= 3 and sys.argv[1] == "split":
        split(sys.argv[2:])
    else:
        sys.exit(__doc__)

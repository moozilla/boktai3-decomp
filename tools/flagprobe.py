#!/usr/bin/env python3
"""Compile pinned functions with their hints stripped under other compilers/flags.

  flagprobe.py LIST.tsv OUT.tsv [JOBS]

LIST.tsv is `progress.py --list-pinned` output (one-function src/fn files).
For each function and each variant in VARIANTS, the hints are stripped
(tools/unpin.py), a `// COMPILER:` / `// CFLAGS:` header is set, and
tools/check.py scores it. OUT.tsv gets NAME, VARIANT, differing bytes
(0 = exact match). Copies go to wip/flagtest/VARIANT/. Used on 2026-10-21 to
test whether pins are an artefact of the wrong compiler or flags (they are not;
docs/REGALLOC.md).
"""
import sys, os, re, subprocess
from concurrent.futures import ThreadPoolExecutor
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, ROOT + "/tools")
import unpin
VARIANTS = {
 "base": ("agbcc", "-O2 -mthumb-interwork"),
 "old": ("old_agbcc", "-O2 -mthumb-interwork"),
 "O1": ("agbcc", "-O1 -mthumb-interwork"),
 "O3": ("agbcc", "-O3 -mthumb-interwork"),
 "old_O1": ("old_agbcc", "-O1 -mthumb-interwork"),
 "noregmove": ("agbcc", "-O2 -mthumb-interwork -fno-regmove"),
 "noexp": ("agbcc", "-O2 -mthumb-interwork -fno-expensive-optimizations"),
 "nosr": ("agbcc", "-O2 -mthumb-interwork -fno-strength-reduce"),
 "nogcse": ("agbcc", "-O2 -mthumb-interwork -fno-gcse"),
 "nocs": ("agbcc", "-O2 -mthumb-interwork -fno-caller-saves"),
 "nororl": ("agbcc", "-O2 -mthumb-interwork -fno-rerun-loop-opt -fno-rerun-cse-after-loop"),
 "nocse_fj": ("agbcc", "-O2 -mthumb-interwork -fno-cse-follow-jumps -fno-cse-skip-blocks"),
}
rows = [l.rstrip("\n").split("\t") for l in open(sys.argv[1]) if l.strip() and not l.startswith("name\t")]
out=sys.argv[2]
def run(job):
    name, src, v = job
    comp, flags = VARIANTS[v]
    text = unpin.strip_code(open(os.path.join(ROOT, src)).read())
    text = re.sub(r"^// (COMPILER|CFLAGS):.*\n", "", text, flags=re.M)
    hdr = (f"// COMPILER: {comp}\n" if comp != "agbcc" else "") + f"// CFLAGS: {flags}\n"
    d = os.path.join(ROOT, "wip/flagtest", v); os.makedirs(d, exist_ok=True)
    p = os.path.join(d, os.path.basename(src)); open(p, "w").write(hdr + text)
    r = subprocess.run([sys.executable, ROOT+"/tools/check.py", p, "--quiet"], cwd=ROOT, capture_output=True, text=True)
    o = r.stdout + r.stderr
    m = re.search(r"(\d+) differing bytes", o)
    b = 0 if ": MATCH (" in o else (int(m.group(1)) if m else -1)
    return name, v, b
jobs=[(r[0], r[4], v) for r in rows for v in VARIANTS]
with ThreadPoolExecutor(int(sys.argv[3]) if len(sys.argv)>3 else 2) as ex, open(out,"w") as f:
    for name, v, b in ex.map(run, jobs):
        f.write(f"{name}\t{v}\t{b}\n"); f.flush()

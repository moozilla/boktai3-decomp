"""First-pass sweep: steering forms tools/concessions.py does not count. Usage: sweep.py REPO"""
import re, sys, glob, os, collections
repo = sys.argv[1]
sys.path.insert(0, os.path.join(repo, "tools"))
import concessions
K = {
 # a non-empty do { ... } while (0) outside a macro: found by brace matching below
 "zero-shift":  re.compile(r"\(\s*u16\s*\)\s*[\w.\->\[\]]+\s*>>\s*16\b|\(\s*u8\s*\)\s*[\w.\->\[\]]+\s*>>\s*(?:8|16|24)\b"),
 "zero-mask":   re.compile(r"[&]\s*0(?:x0+)?\b(?![xX.\d])|[|^+\-]\s*\(\s*[^()]*&\s*0(?:x0+)?\s*\)"),
 "identity-op": re.compile(r"(?<![\w)\]])\s[+|^]\s*0\s*[;)]|\*\s*1\s*[;)]|<<\s*0\s*[;)]|>>\s*0\s*[;)]"),
 "self-assign": re.compile(r"(?<![\w.>\]])(\b[A-Za-z_]\w*)\s*=\s*\1\s*;"),
 "if-const":    re.compile(r"\bif\s*\(\s*[01]\s*\)|\bwhile\s*\(\s*1\s*\)\s*\{[^{}]*\bbreak;\s*\}"),
 "volatile-view": re.compile(r"\(\s*(?:const\s+)?volatile\s+\w+\s*\*\s*\)\s*(?!&)[\w(]|(?:const\s+)?volatile\s+(?:const\s+)?\w+\s*\*\s*\w+\s*[=;]"),
 "local-union": re.compile(r"^\s{4,}union\b", re.M),
 "dead-read":   re.compile(r"(?m)^\s+\(void\)\s*[\w*(][^;]*;|^\s+\*?(?!break;|return;|continue;)[A-Za-z_][\w.\->\[\]]*;\s*$"),
 "comma-expr":  re.compile(r"(?m)^\s+[^;(){}\n]*\b\w+\s*=\s*\([^()]*,[^()]*\)\s*;"),
}
ADMIT = re.compile(r"alloc|regalloc|priorit|scaffold|phase[- ]?4|jump2|reload|regmove|combine|lifetime|tie|steer|dead (?:arm|call|read|store)|unreachable|impossible|never (?:taken|true|negative)|to match|for matching|fake|hack|dummy|bounded", re.I)
hits = collections.defaultdict(list)
files = sorted(glob.glob(repo + "/src/**/*.c", recursive=True))
for f in files:
    rel = os.path.relpath(f, repo)
    if rel.startswith("src/lib/"): continue
    raw = open(f, errors="replace").read()
    code = concessions.strip(raw)
    # drop macro bodies (continuation lines)
    nomac = re.sub(r"(?m)^[ \t]*#[ \t]*define(?:.*\\\n)*.*$", lambda m: re.sub(r"[^\n]", " ", m.group(0)), code)
    for k, rx in K.items():
        for m in rx.finditer(nomac):
            hits[k].append((rel, nomac.count("\n", 0, m.start()) + 1, m.group(0).strip()[:70]))
    narrow = {}
    for m in re.finditer(r"\b(?:const\s+)?(u8|s8|u16|s16|bool8)\s+(\w+)\s*[;=,)]", nomac):
        narrow.setdefault(m.group(2), set()).add(m.group(1))
    for m in re.finditer(r"(?<![\w.>\]])(\w+)\s*>>\s*(\d+)\b", nomac):
        t = narrow.get(m.group(1))
        if t and len(t) == 1:
            w = 8 if "8" in next(iter(t)) and "16" not in next(iter(t)) else 16
            if next(iter(t))[0] in "ub" and int(m.group(2)) >= w:
                hits["zero-shift"].append((rel, nomac.count("\n", 0, m.start()) + 1, m.group(0) + " (" + next(iter(t)) + ")"))
    for m in re.finditer(r"&\s*(0x[0-9A-Fa-f]+|\d+)\s*\)\s*>>\s*(\d+)\b", nomac):
        if int(m.group(1), 0) >> int(m.group(2)) == 0:
            hits["zero-shift"].append((rel, nomac.count("\n", 0, m.start()) + 1, m.group(0)))
    for m in re.finditer(r"\(\s*s32\s*\)\s*(\w+)\s*<\s*0\b|(?<![\w.>\]])(\w+)\s*<\s*0\b", nomac):
        n = m.group(1) or m.group(2); t = narrow.get(n)
        if t and len(t) == 1 and next(iter(t))[0] in "ub":
            hits["never-negative"].append((rel, nomac.count("\n", 0, m.start()) + 1, m.group(0) + " (" + next(iter(t)) + ")"))
    for m in re.finditer(r"\}\s*while\s*\(\s*0\s*\)\s*;", nomac):
        # walk back to matching do {
        d, i = 0, m.start()
        while i >= 0:
            c = nomac[i]
            if c == "}": d += 1
            elif c == "{":
                d -= 1
                if d == 0: break
            i -= 1
        body = nomac[i + 1:m.start()].strip()
        if body:
            hits["once-loop"].append((rel, nomac.count("\n", 0, m.start()) + 1, body.replace("\n", " ")[:70]))
    for m in re.finditer(r"/\*.*?\*/|//[^\n]*", raw, re.S):
        t = m.group(0)
        if "concession-ok" in t or "MATCHING_CONCESSION" in t: continue
        if ADMIT.search(t):
            hits["comment"].append((rel, raw.count("\n", 0, m.start()) + 1, " ".join(t.split())[:110]))
pass
out = sys.argv[2]
with open(out, "w") as o:
    for k in sorted(hits):
        for h in hits[k]:
            o.write("%s\t%s\t%d\t%s\n" % (k, *h))
for k in sorted(hits):
    print("%-14s sites %5d  files %4d" % (k, len(hits[k]), len({h[0] for h in hits[k]})))

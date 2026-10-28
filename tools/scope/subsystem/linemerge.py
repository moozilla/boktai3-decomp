"""Resolve diff3 conflict hunks in FILE by an order-preserving three-way merge: apply each side's
changes against the base where they touch different base lines (adjacent edits are fine); exit 1
when both sides change the same base lines differently."""
import difflib, re, sys

def changes(base, side):
    sm = difflib.SequenceMatcher(None, base, side, autojunk=False)
    return [(i1, i2, side[j1:j2]) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != "equal"]

def merge(base, ours, theirs):
    a, b = changes(base, ours), changes(base, theirs)
    allc = sorted([(i1, i2, l, 0) for i1, i2, l in a] + [(i1, i2, l, 1) for i1, i2, l in b], key=lambda c: (c[0], c[1]))
    out, pos, prev = [], 0, None
    for i1, i2, lines, side in allc:
        if prev and (i1 < prev[1] or (i1 == prev[0] and i1 == i2 == prev[1])):
            if (i1, i2, lines) == prev[:3]:
                continue  # both sides made the same change
            if not (i1 == i2 == prev[0] == prev[1]):
                raise ValueError(f"overlap at base lines {i1}-{i2}")
            # both sides insert at the same point: keep both, ours first
        out += base[pos:i1] + lines
        pos, prev = i2, (i1, i2, lines)
    return out + base[pos:]

DECL = re.compile(r"^\s*(#include\s+\S+|extern\s+[^;]*;)\s*$")

def items(lines):
    """#include lines and extern declarators of a declarations-only hunk, in order."""
    out = []
    for l in lines:
        t = l.strip()
        if not t:
            continue
        if t.startswith("#include"):
            out.append(("inc", t, ""))
            continue
        body = t[len("extern"):].rstrip(";").strip()
        parts = [x.strip() for x in body.split(",")]
        m = re.match(r"^(.*?)([*\s]*\w+(?:\[[^\]]*\])?)$", parts[0])
        typ, first = m.group(1).strip(), m.group(2).strip()
        for d in [first] + parts[1:]:
            out.append(("ext", typ, d))
    return out

def declmerge(base, ours, theirs):
    if not all(DECL.match(l) or not l.strip() for l in base + ours + theirs):
        raise ValueError("not a declarations-only hunk")
    b, o, t = items(base), items(ours), items(theirs)
    keep = [x for x in b if x in o and x in t] + [x for x in o if x not in b] + [x for x in t if x not in b and x not in o]
    incs = [x for x in keep if x[0] == "inc"]
    exts = [x for x in keep if x[0] == "ext"]
    return [x[1] + "\n" for x in incs] + [f"extern {x[1]} {x[2]};\n" for x in exts]

bad = False
for path in sys.argv[1:]:
    text = open(path).read()
    pat = re.compile(r"^<<<<<<< [^\n]*\n(.*?)^\|\|\|\|\|\|\| [^\n]*\n(.*?)^=======\n(.*?)^>>>>>>> [^\n]*\n", re.S | re.M)
    def res(m):
        ours, base, theirs = (s.splitlines(True) for s in m.groups())
        try:
            return "".join(merge(base, ours, theirs))
        except ValueError:
            pass
        try:
            tok = lambda x: re.findall(r"\w+|\s+|[^\w\s]", "".join(x))
            return "".join(merge(tok(base), tok(ours), tok(theirs)))
        except ValueError:
            return "".join(declmerge(base, ours, theirs))
    try:
        new, n = pat.subn(res, text)
    except ValueError as e:
        print(f"{path}: {e}"); bad = True; continue
    open(path, "w").write(new)
    print(f"{path}: {n} hunks")
sys.exit(1 if bad else 0)

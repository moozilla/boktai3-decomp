#!/usr/bin/env python3
"""Settle merge_unit.py's conflicts before a merge, keeping every input byte-identical.

  merge_resolve.py IN.c... [--dry-run]       (inputs in ROM order, as for merge_unit.py)

Single-function files often carry their own copy of a helper (`static inline SetControl`,
`struct SpiderRecoveryContext`) under the same name with a slightly different body, which
merge_unit.py reports as a conflict. For each conflict this tries the first file's version
in the later file: if the later file still matches (tools/check.py), the copy is dropped
and the merge keeps one helper. Otherwise the later file's helper is renamed after the
function that uses it (`SetControl` in Spider_SelectRecoveryRoute becomes
`SetControl_SelectRecoveryRoute`), so the two can sit in one unit; a later cleanup can
still unify them. A prototype that disagrees with the definition takes the header's or the
definition's signature (with forward declarations of the definition's struct tags), or a
variant; failing those, the definition takes the callers' signature and keeps its own
parameter types in locals. Each choice must leave every input matching and, with --out, the
merged unit compiling. It prints what it did for the log; UNSETTLED lines need a hand edit.
"""
import argparse
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import merge_unit as mu  # noqa: E402

from source_layout import first_function  # noqa: E402

ROOT = mu.ROOT


def suffix(path):
    """The action part of the file's first function's name (Spider_UpdateDefeat -> UpdateDefeat)."""
    name = first_function(path, ROOT) or os.path.splitext(os.path.basename(path))[0]
    return name.split("_", 1)[1] if "_" in name else name


def matches(path):
    r = subprocess.run([sys.executable, os.path.join(ROOT, "tools/check.py"), "--quiet", path],
                       cwd=ROOT, capture_output=True, text=True)
    out = r.stdout + r.stderr
    return r.returncode == 0 and "MATCH" in out and "MISMATCH" not in out


def item_of(path, k):
    for item in mu.items(open(os.path.join(ROOT, path)).read()):
        if mu.key(item) == k:
            return item
    return None


def rename(text, k, new):
    kind, name = k
    if kind == "type" and " " in name:  # struct/union/enum tag
        tag = name.split()[1]
        return re.sub(r"\b(struct|union|enum)(\s+)%s\b" % re.escape(tag), r"\1\2" + new, text)
    if kind == "static":  # a function: its definition and calls, not a member of that name
        return re.sub(r"(?<![\w.>])%s(?=\s*\()" % re.escape(name), new, text)
    return re.sub(r"(?<![\w.>])%s\b" % re.escape(name), new, text)


def signature(paths, name):
    """The definition's header as a prototype, if one of the inputs defines `name`."""
    for path in paths:
        for item in mu.items(open(os.path.join(ROOT, path)).read()):
            code = mu._NONCODE.sub(" ", item)
            if "{" in code and not item.lstrip().startswith(("struct", "union", "enum", "typedef")):
                head = code.split("{", 1)[0].strip()
                if re.search(r"\b%s\s*\(" % re.escape(name), head) and "=" not in head:
                    head = re.sub(r"^static\s+(inline\s+)?", "", head)
                    return " ".join(head.split()) + ";"
    return None


def header_declarations(name):
    """One-line prototypes of `name` in include/ (the merged unit may include that header)."""
    r = subprocess.run(["git", "grep", "-h", "-P", r"^[A-Za-z_][\w \*]*\b%s\s*\([^;{}]*\);" % re.escape(name),
                        "--", "include"], cwd=ROOT, capture_output=True, text=True)
    return list(dict.fromkeys(l.strip() for l in r.stdout.splitlines()))


def settle_declaration(paths, k, dry_run, out=None):
    """One declaration text for `name` in every input that declares it, if all still match:
    the definition's own signature first, then each variant in ROM order."""
    users, variants = {}, []
    for path in paths:
        for item in mu.items(open(os.path.join(ROOT, path)).read()):
            if mu.key(item) == k:
                users.setdefault(path, []).append(item)
                if item not in variants:
                    variants.append(item)
    own = signature(paths, k[1])
    forward = None
    if own and re.search(r"\b(struct|union)\s+\w+", own):
        # the definition's own types, declared ahead so a prototype earlier in the unit agrees
        tags = dict.fromkeys(re.findall(r"\b((?:struct|union)\s+\w+)", own))
        forward = "".join(f"{tag};\n" for tag in tags) + own
    candidates = header_declarations(k[1]) + ([own] if own else []) + ([forward] if forward else []) + variants
    candidates = list(dict.fromkeys(candidates))
    if dry_run:
        return f"{k[1]}: {len(variants)} declarations in {len(users)} files"
    saved = {p: open(os.path.join(ROOT, p)).read() for p in users}
    for cand in candidates:
        for p, text in saved.items():
            for v in set(users[p]):
                text = text.replace(v, cand)
            open(os.path.join(ROOT, p), "w").write(text)
        if all(matches(p) for p in users) and (out is None or k[1] not in probe(paths, out)[0]):
            return f"{k[1]}: one declaration in {len(users)} files: {' '.join(cand.split())}"
    for p, text in saved.items():
        open(os.path.join(ROOT, p), "w").write(text)
    for v in variants:
        line = adapt_definition(paths, k[1], v, users, saved, out)
        if line:
            return line
    return f"{k[1]}: UNSETTLED declaration ({len(variants)} variants, none matches in every file)"


def split_params(params):
    out, depth, cur = [], 0, ""
    for ch in params:
        depth += ch in "(["
        depth -= ch in ")]"
        if ch == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def adapt_definition(paths, name, variant, users, saved, out):
    """Give the definition the callers' signature: its parameters keep their names inside the
    body through a local of the old type. Kept only if the definition and every caller match."""
    m = re.match(r"\s*(.*?)\b%s\s*\((.*)\)\s*;\s*$" % re.escape(name), mu._NONCODE.sub(" ", variant), re.S)
    if not m:
        return None
    vret, vparams = m.group(1).strip(), split_params(m.group(2))
    for path in paths:
        text = open(os.path.join(ROOT, path)).read()
        dm = re.search(r"^([A-Za-z_][^;{}()]*?)\b%s\s*\(([^)]*)\)\s*\{\n" % re.escape(name), text, re.M)
        if not dm or dm.group(1).lstrip().startswith("static"):
            continue
        dparams = split_params(dm.group(2))
        if len(dparams) != len(vparams) or dparams == ["void"]:
            return None
        header, locals_ = [], []
        for dp, vp in zip(dparams, vparams):
            dn = re.search(r"(\w+)\s*$", dp).group(1)
            vtype = re.sub(r"\b\w+\s*$", "", vp).strip() if re.search(r"[\w\*]\s+\w+\s*$|\*\s*\w+\s*$", vp) else vp
            dtype = dp[: dp.rfind(dn)].strip()
            if " ".join(vtype.split()) == " ".join(dtype.split()):
                header.append(dp)
                continue
            header.append(f"{vtype}{'' if vtype.endswith('*') else ' '}{dn}In")
            locals_.append(f"    {dtype}{'' if dtype.endswith('*') else ' '}{dn} = ({dtype}){dn}In;\n")
        new_head = f"{vret} {name}({', '.join(header)})\n{{\n" + "".join(locals_)
        adapted = text[: dm.start()] + new_head + text[dm.end():]
        for p, s in saved.items():
            for v in set(users[p]):
                s = s.replace(v, variant)
            open(os.path.join(ROOT, p), "w").write(s)
        own_text = open(os.path.join(ROOT, path)).read() if path in saved else text
        open(os.path.join(ROOT, path), "w").write(own_text[: own_text.find(dm.group(0))] + new_head
                                                   + own_text[own_text.find(dm.group(0)) + len(dm.group(0)):])
        files = set(users) | {path}
        if all(matches(p) for p in files) and (out is None or name not in probe(paths, out)[0]):
            return f"{name}: definition takes the callers' signature: {' '.join(variant.split())}"
        for p, s in saved.items():
            open(os.path.join(ROOT, p), "w").write(s)
        open(os.path.join(ROOT, path), "w").write(text)
        return None
    return None


def resolve(paths, dry_run=False):
    log = []
    tried = set()
    while True:
        _, conflicts = mu.merge(paths)
        conflicts = [c for c in conflicts if (c[0], c[2]) not in tried]
        if not conflicts:
            return log
        k, first, later = conflicts[0]
        tried.add((k, later))
        if k[0] == "decl":
            log.append(settle_declaration(paths, k, dry_run))
            continue
        mine, theirs = item_of(later, k), item_of(first, k)
        full = os.path.join(ROOT, later)
        text = open(full).read()
        label = k[1]
        adopted = text.replace(mine, theirs, 1)
        if not dry_run:
            open(full, "w").write(adopted)
            if matches(later):
                log.append(f"{later}: {label} now the copy from {first}")
                continue
        tail = suffix(later)
        if k[0] == "type" and " " in label:
            new = label.split()[1] + tail
        else:
            new = f"{label}_{tail}"
        renamed = rename(text, k, new)
        if dry_run:
            log.append(f"{later}: {label} conflicts with {first}")
            continue
        open(full, "w").write(renamed)
        if matches(later):
            log.append(f"{later}: {label} renamed {new}")
        else:
            open(full, "w").write(text)
            log.append(f"{later}: {label} UNSETTLED (neither copy nor rename matches)")


def probe(paths, out):
    """Compile the merge at `out` (removed afterwards); the names agbcc says are declared twice."""
    text, _ = mu.merge(paths)
    full = os.path.join(ROOT, out)
    existed = os.path.exists(full)
    if existed:
        raise SystemExit(f"{out} exists; probe needs a fresh output path")
    open(full, "w").write(text)
    try:
        r = subprocess.run([sys.executable, os.path.join(ROOT, "tools/check.py"), "--quiet", out],
                           cwd=ROOT, capture_output=True, text=True)
    finally:
        os.remove(full)
    out_text = r.stdout + r.stderr
    clash = re.findall(r"conflicting types for `(\w+)'", out_text)
    errors = [l for l in out_text.splitlines() if re.search(r":\d+: (?!warning)", l)
              and "conflicting types" not in l and "previous declaration" not in l]
    return list(dict.fromkeys(clash)), errors


def resolve_compiled(paths, out):
    """After the textual conflicts: settle the declarations the merged unit still disagrees on."""
    log, done = [], set()
    while True:
        clash, errors = probe(paths, out)
        again = [n for n in clash if n in done]
        clash = [n for n in clash if n not in done]
        if not clash:
            return log + [f"{n}: UNSETTLED, still declared two ways in the merge" for n in again], errors
        for name in clash:
            done.add(name)
            log.append(settle_declaration(paths, ("decl", name), False, out))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("inputs", nargs="+")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--out", help="the unit to be: also settle what agbcc rejects in the merge")
    a = ap.parse_args()
    for line in resolve(a.inputs, a.dry_run):
        print(line)
    _, left = mu.merge(a.inputs)
    print(f"{len(left)} conflicts left")
    if a.out and not left and not a.dry_run:
        log, errors = resolve_compiled(a.inputs, a.out)
        for line in log + errors:
            print(line)
        print(f"{len(errors)} other compile errors in the merge")
        return 1 if errors or any("UNSETTLED" in l for l in log) else 0
    return 1 if left else 0


if __name__ == "__main__":
    sys.exit(main())

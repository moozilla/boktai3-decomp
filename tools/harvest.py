#!/usr/bin/env python3
"""Cases for the spelling library: every draft in git history that later matched.

  harvest.py scan [--rev REV] [--out FILE]   walk history, write build/harvest/cases.jsonl
  harvest.py show FUNC                       the draft-to-match diff of one case
  harvest.py stats                           how many cases, and of what size

A NONMATCHING draft that a later commit turned into matching C is a solved
problem with its answer: the edit between the two is a technique that closed
a real divergence. tools/spellings.py is meant to hold those techniques as
deterministic rewrites keyed by the agbcc decision they flip, so the cases
are both the library's source and its test (tools/replay.py: does fix.py,
with the library alone, get from each draft to its match?).

scan reads every commit on REV (default origin/main) in order. For each
function name it remembers the last draft seen (the `#ifdef NONMATCHING`
text whose `#else` INCLUDE_ASM names it, or its `#define Name sub_X` alias),
and when a commit first defines that function as plain C it records a case:
the draft's text, the matched text, both commits and files. Matches that use
register pins (`asm("rN")`) are marked, since their final C is not the
technique's natural spelling.
"""
import argparse
import difflib
import json
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402
from split import INCLUDE_ASM, split_nonmatching  # noqa: E402

OUT = os.path.join(ROOT, "build", "harvest", "cases.jsonl")
ALIAS = re.compile(r"^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$", re.M)
DEF = re.compile(r"^(?:static\s+|inline\s+)*[A-Za-z_][\w \t\*]*?\b(\w+)\s*\(([^;{)]|\([^)]*\))*\)\s*\{", re.M)
PIN = re.compile(r'\basm\s*\(\s*"r\d+"\s*\)')


def git(*args, inp=None):
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True, text=True, input=inp,
                          check=True).stdout


class Blobs:
    """git cat-file --batch: file text at a commit, fast."""

    def __init__(self):
        self.p = subprocess.Popen(["git", "cat-file", "--batch"], cwd=ROOT, stdin=subprocess.PIPE,
                                  stdout=subprocess.PIPE)

    def get(self, rev, path):
        self.p.stdin.write(f"{rev}:{path}\n".encode())
        self.p.stdin.flush()
        head = self.p.stdout.readline().split()
        if len(head) < 3 or head[1] == b"missing":
            return None
        data = self.p.stdout.read(int(head[2]))
        self.p.stdout.read(1)
        return data.decode("utf-8", "replace")


def body_span(text, start):
    """End of the definition whose header starts at START (brace matching, comments skipped)."""
    i = text.index("{", start)
    depth = 0
    while i < len(text):
        c = text[i]
        if text.startswith("//", i):
            i = text.find("\n", i)
            if i < 0:
                return len(text)
            continue
        if text.startswith("/*", i):
            i = text.find("*/", i) + 2
            continue
        if c in "\"'":
            j = i + 1
            while j < len(text) and text[j] != c:
                j += 2 if text[j] == "\\" else 1
            i = j + 1
            continue
        depth += {"{": 1, "}": -1}.get(c, 0)
        i += 1
        if depth == 0:
            return i
    return len(text)


def definitions(code):
    """{name: definition text} for the function definitions in CODE."""
    out = {}
    for m in DEF.finditer(code):
        name = m.group(1)
        if name in ("if", "while", "for", "switch", "return", "sizeof"):
            continue
        out.setdefault(name, code[m.start():body_span(code, m.start())])
    return out


def file_facts(text):
    """(drafts {key: draft text}, matched {key: definition text}, aliases) for one source file.
    A key is the function's address label when the file aliases it, else its name."""
    alias = {n: a for n, a in ALIAS.findall(text)}
    key = lambda n: alias.get(n, n)
    default, drafts = split_nonmatching(text)
    found = {}
    for draft, fallback in drafts:
        for name in INCLUDE_ASM.findall(fallback):
            found[key(name)] = draft
    matched = {}
    code = re.sub(r"^\s*INCLUDE_ASM\([^)]*\);", "", default, flags=re.M)
    for name, d in definitions(code).items():
        if d.lstrip().startswith(("static", "inline")) and "inline" in d.split("(")[0]:
            continue  # helpers are not ROM functions of their own
        matched[key(name)] = (name, d)
    return found, matched


def draft_function(dtext, names):
    """The draft's definition of one of NAMES (the draft block may hold helpers too)."""
    defs = definitions(dtext)
    for n in names:
        if n in defs:
            return n, defs[n]
    # a block holding a single non-inline definition
    plain = [(n, d) for n, d in defs.items() if "inline" not in d.split("(")[0]]
    return plain[-1] if len(plain) == 1 else (None, None)


def scan(rev, out):
    commits = git("rev-list", "--reverse", "--topo-order", rev).split()
    blobs = Blobs()
    last_draft = {}   # key -> (commit, path, draft block text)
    matched = set()   # keys already seen as plain C
    pending = set()   # keys with a draft and no match yet
    cases = []
    for k, c in enumerate(commits):
        files = [f for f in git("diff-tree", "--root", "--no-commit-id", "-r", "--name-only", c).split()
                 if f.startswith("src/") and f.endswith(".c")]
        for f in files:
            text = blobs.get(c, f)
            if text is None:
                continue
            # only files with a draft, or naming a function whose draft is pending, matter
            if "NONMATCHING" not in text and not any(k in text for k in pending):
                continue
            drafts, defs = file_facts(text)
            for key_, d in drafts.items():
                last_draft[key_] = (c, f, d)
                if key_ not in matched:
                    pending.add(key_)
            for key_, (name, d) in defs.items():
                if key_ in matched or key_ not in pending:
                    continue
                matched.add(key_)
                pending.discard(key_)
                if key_ not in drafts:
                    dc, df, block = last_draft[key_]
                    dname, dfn = draft_function(block, [name, key_])
                    if dfn is None:
                        continue
                    cases.append({"key": key_, "name": name, "draft_commit": dc, "draft_file": df,
                                  "match_commit": c, "match_file": f, "draft": dfn, "draft_block": block,
                                  "final": d, "pinned": bool(PIN.search(d)), "draft_pinned": bool(PIN.search(dfn)),
                                  "subject": git("log", "-1", "--format=%s", c).strip()})
        if k % 200 == 0:
            print(f"{k}/{len(commits)} commits, {len(cases)} cases", file=sys.stderr)
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, "w") as fh:
        for case in cases:
            fh.write(json.dumps(case) + "\n")
    print(f"{len(cases)} cases ({sum(c['pinned'] for c in cases)} with pinned matches) -> "
          f"{os.path.relpath(out, ROOT)}")


def load(path=OUT):
    return [json.loads(ln) for ln in open(path)]


def diff_lines(case):
    norm = lambda t: [ln.rstrip() for ln in re.sub(r"//[^\n]*", "", t).splitlines() if ln.strip()]
    return [ln for ln in difflib.unified_diff(norm(case["draft"]), norm(case["final"]), "draft", "match",
                                              lineterm="", n=1)]


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("scan")
    s.add_argument("--rev", default="origin/main")
    s.add_argument("--out", default=OUT)
    s = sub.add_parser("show")
    s.add_argument("func")
    sub.add_parser("stats")
    a = ap.parse_args()
    if a.cmd == "scan":
        scan(a.rev, a.out)
    elif a.cmd == "show":
        for c in load():
            if a.func in (c["key"], c["name"]):
                print(f"{c['key']} {c['name']}: draft {c['draft_commit'][:8]} {c['draft_file']} -> "
                      f"match {c['match_commit'][:8]} {c['match_file']} ({c['subject']})")
                print("\n".join(diff_lines(c)))
    else:
        cases = load()
        sizes = sorted(sum(1 for ln in diff_lines(c) if ln[:1] in "+-" and ln[:3] not in ("+++", "---"))
                       for c in cases)
        print(f"{len(cases)} cases, {sum(c['pinned'] for c in cases)} pinned matches")
        for lo, hi in ((0, 0), (1, 2), (3, 4), (5, 8), (9, 16), (17, 10 ** 9)):
            print(f"  {lo}-{hi} changed lines: {sum(lo <= n <= hi for n in sizes)}")


if __name__ == "__main__":
    main()

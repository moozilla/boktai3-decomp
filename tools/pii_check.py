#!/usr/bin/env python3
"""Refuse commits that would add personal information to the repository.

    tools/pii_check.py --staged          added lines of the staged diff (pre-commit hook)
    tools/pii_check.py --msg FILE        a commit message (commit-msg hook)
    tools/pii_check.py --range A..B      added lines and commit messages in a range
    tools/pii_check.py --all             every tracked file

Generic rules catch home directories: /Users/<name>, /home/<name> (except the
sandbox's /home/user), C:\\Users\\<name>. Write ~ or a repo-relative path instead.

Names, handles and domains that only one person knows are personal go in a private
pattern list, never in this file: one case-insensitive regex per line, read from
$PII_PATTERNS, $PII_PATTERNS_FILE,
~/.config/boktai3/pii-patterns and .git/pii-patterns. Lines starting with # are comments.

Install the hooks with `git config core.hooksPath .githooks` (tools/cloud_setup.sh and
tools/worktree.sh do it). Exit status 1 lists each hit as FILE:LINE: RULE.
"""
import argparse
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

GENERIC = [
    ("home path", re.compile(r"(?<![\w.-])/Users/(?!Shared\b)[A-Za-z0-9._-]+")),
    ("home path", re.compile(r"(?<![\w.-])/home/(?!user\b)[A-Za-z][A-Za-z0-9._-]*")),
    ("home path", re.compile(r"[A-Za-z]:\\+Users\\+[A-Za-z0-9._-]+", re.I)),
]
# Third-party text, and this checker and its test
SKIP = re.compile(r"^(?:tools/pii_check\.py|tests/tools/test_pii_check\.py|licenses/|THIRD_PARTY\.md$)")


def private_patterns():
    texts = [os.environ.get("PII_PATTERNS", "")]
    paths = [
        os.environ.get("PII_PATTERNS_FILE", ""),
        os.path.expanduser("~/.config/boktai3/pii-patterns"),
        os.path.join(ROOT, ".git", "pii-patterns"),
    ]
    for p in paths:
        if p and os.path.isfile(p):
            with open(p, encoding="utf-8") as f:
                texts.append(f.read())
    pats = []
    for text in texts:
        for line in text.splitlines():
            line = line.strip()
            if line and not line.startswith("#"):
                pats.append(("private pattern", re.compile(line, re.I)))
    return pats


def scan_line(text, rules):
    hits = []
    for name, rx in rules:
        for m in rx.finditer(text):
            # private patterns are secret: never echo what matched
            hits.append(name if name == "private pattern" else f"{name} {m.group(0)!r}")
    return hits


def git(*args):
    return subprocess.run(
        ["git", "-C", ROOT, *args], check=True, capture_output=True, text=True, errors="replace"
    ).stdout


def scan_diff(diff, rules, out):
    path, lineno = None, 0
    for line in diff.splitlines():
        if line.startswith("+++ "):
            path = line[6:] if line.startswith("+++ b/") else None
        elif line.startswith("@@"):
            m = re.search(r"\+(\d+)", line)
            lineno = int(m.group(1)) if m else 0
        elif line.startswith("+") and path and not SKIP.match(path):
            for h in scan_line(line[1:], rules):
                out.append(f"{path}:{lineno}: {h}")
            lineno += 1
        elif not line.startswith("-"):
            lineno += 1


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument("--staged", action="store_true")
    g.add_argument("--msg", metavar="FILE")
    g.add_argument("--range", metavar="A..B")
    g.add_argument("--all", action="store_true")
    a = ap.parse_args()
    rules = GENERIC + private_patterns()
    hits = []
    if a.staged:
        scan_diff(git("diff", "--cached", "-U0", "--no-color", "--no-ext-diff", "--text"), rules, hits)
    elif a.msg:
        with open(a.msg, encoding="utf-8", errors="replace") as f:
            for i, line in enumerate(f, 1):
                if not line.startswith("#"):
                    hits += [f"commit message:{i}: {h}" for h in scan_line(line, rules)]
    elif a.range:
        scan_diff(git("diff", "-U0", "--no-color", "--no-ext-diff", "--text", a.range), rules, hits)
        for c in git("rev-list", a.range).split():
            for i, line in enumerate(git("log", "-1", "--format=%B", c).splitlines(), 1):
                hits += [f"commit {c[:10]} message:{i}: {h}" for h in scan_line(line, rules)]
    else:
        for path in git("ls-files").splitlines():
            full = os.path.join(ROOT, path)
            if SKIP.match(path) or not os.path.isfile(full) or os.path.getsize(full) > 4 << 20:
                continue
            with open(full, "rb") as f:
                data = f.read()
            if b"\0" in data[:8192]:
                continue
            for i, line in enumerate(data.decode("utf-8", "replace").splitlines(), 1):
                hits += [f"{path}:{i}: {h}" for h in scan_line(line, rules)]
    if hits:
        print("pii_check: personal information found (use ~ or a neutral word instead):", file=sys.stderr)
        for h in hits:
            print("  " + h, file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""Free disk space by deleting only what the build and tools regenerate.

  clean.py                     dry run: what would go, per worktree, with sizes
  clean.py --yes               delete it
  clean.py --runs              also emulator coverage dumps (cov-*/total.* .read/.exec/...)
  clean.py --worktrees         also remove merged, idle worktrees under ../wt and ../b3-*
                               (their small build artifacts are copied to
                               build/salvage/NAME/ in the main checkout first)

Scope. Every worktree of this repository is visited (`git worktree list`),
except ones a running process is working in (any process's cwd inside it) and
Codex's own worktrees (~/.codex: the Codex app manages them). Inside each, only
these go:

  build/src/  build/asm/            compiler and assembler outputs (make)
  build/check/  build/shift_test/  build/ci-clean/
                                    tools/check.py scratch, make shifttest, CI rehearsal
  build/*.elf  build/*.gba  build/*.map  build/*.o
                                    linked ROMs (make, make mod, make shifttest)
  build/permute/FUNC/               permuter folders of functions now matched in C
  with --runs: cov-*.{read,exec,word32,ewram_exec,iwram_exec} and total.* of the
               same kinds anywhere under build/ (re-run the harness to remake them)

Never touched: build/text/ (Studio text edits), build/venv/ and build/tools/,
the tool state folders (namer, foundry, unitize, showcase, research...), every
.c/.diff/.txt/.log/.json/.md/.tsv/.csv/.py/.sh file outside the folders above,
savestates and saves, wip/, gen/ (regenerated, but slow), and anything
tracked by git. Symlinks are unlinked, never followed.

A worktree is removed (--worktrees) only if: it is under ../wt/ or ../b3-*,
nothing runs in it, `git status` is clean, every commit on it is already in
origin/main (by ancestry or as an identical patch), and wip/ is empty.
Desktop sessions' worktrees (.claude/worktrees) are left to the app.
"""
import argparse
import os
import re
import shutil
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402

REGEN_DIRS = ["src", "asm", "check", "shift_test", "ci-clean"]
REGEN_TOP = re.compile(r".*\.(elf|gba|map|o)$")
RUN_FILE = re.compile(r"^(cov-.*|total)\.(read|exec|word32|ewram_exec|iwram_exec)$")
KEEP_EXT = re.compile(r".*\.(c|h|diff|patch|txt|log|json|md|tsv|csv|py|sh|asm|s|ss|sav|png|jpg|ppm)$")
HOME = os.path.expanduser("~")


def git(*a, cwd=ROOT):
    return subprocess.run(["git", *a], cwd=cwd, capture_output=True, text=True).stdout


def worktrees():
    out, cur = [], {}
    for line in git("worktree", "list", "--porcelain").split("\n"):
        if line.startswith("worktree "):
            cur = {"path": line[9:]}
            out.append(cur)
        elif line.startswith("branch "):
            cur["branch"] = line[7:]
        elif line == "detached":
            cur["branch"] = None
    return out


IDLE = {"zsh", "bash", "sh", "claude", "node", "login", "caffeinate", "tail", "less", "Claude", "Claude Helper",
        "Code Helper", "Electron", "git", "disclaimer", "sleep", "lsof"}


def busy_dirs():
    """Current directories of running processes that do work (not idle shells or agent sessions,
    whose folders stay open while they wait)."""
    r = subprocess.run(["lsof", "-a", "-d", "cwd", "-Fpcn"], capture_output=True, text=True)
    out, cmd = [], ""
    for l in r.stdout.split("\n"):
        if l.startswith("c"):
            cmd = l[1:]
        elif l.startswith("n/") and cmd not in IDLE and not cmd.startswith("Claude"):
            out.append((cmd, l[1:]))
    return out


def in_use(path, busy):
    """The commands working inside PATH (other worktrees nested in it don't count)."""
    p = os.path.realpath(path) + os.sep
    nested = os.path.join(p, ".claude", "worktrees") + os.sep
    return sorted({c for c, b in busy if (os.path.realpath(b) + os.sep).startswith(p)
                   and not (os.path.realpath(b) + os.sep).startswith(nested)})


def size(p):
    if os.path.islink(p):
        return 0
    if os.path.isfile(p):
        return os.path.getsize(p)
    t = 0
    for dp, dn, fn in os.walk(p):
        for f in fn:
            fp = os.path.join(dp, f)
            if not os.path.islink(fp):
                try:
                    t += os.path.getsize(fp)
                except OSError:
                    pass
    return t


def matched_functions(wt):
    names = set()
    for dp, _, fn in os.walk(os.path.join(wt, "src")):
        for f in fn:
            if f.endswith(".c"):
                text = open(os.path.join(dp, f), errors="replace").read()
                names |= set(re.findall(r"^\w[\w\s\*]*?\b(\w+)\s*\([^;]*?\)\s*\{", text, re.M))
                names -= set(re.findall(r'INCLUDE_ASM\("[^"]*",\s*(\w+)\)', text))
    return names


def targets(wt, runs):
    b = os.path.join(wt, "build")
    if not os.path.isdir(b) or os.path.islink(b):
        return []
    out = [os.path.join(b, d) for d in REGEN_DIRS if os.path.isdir(os.path.join(b, d))
           and not os.path.islink(os.path.join(b, d))]
    out += [os.path.join(b, f) for f in os.listdir(b) if REGEN_TOP.match(f) and os.path.isfile(os.path.join(b, f))]
    perm = os.path.join(b, "permute")
    if os.path.isdir(perm) and not os.path.islink(perm):
        done = matched_functions(wt)
        out += [os.path.join(perm, f) for f in os.listdir(perm) if f in done]
    if runs:
        for dp, dn, fn in os.walk(b):
            dn[:] = [d for d in dn if not os.path.islink(os.path.join(dp, d)) and d not in ("venv", "tools", "text")]
            out += [os.path.join(dp, f) for f in fn if RUN_FILE.match(f)]
    return out


def remove(p):
    if os.path.islink(p) or os.path.isfile(p):
        os.unlink(p)
    else:
        shutil.rmtree(p)


def merged(wt):
    """Every commit on this worktree's HEAD is in origin/main (ancestry or identical patch)."""
    head = git("rev-parse", "HEAD", cwd=wt).strip()
    if subprocess.run(["git", "merge-base", "--is-ancestor", head, "origin/main"], cwd=wt).returncode == 0:
        return True
    cherry = git("cherry", "origin/main", head, cwd=wt).split()
    return bool(cherry) and all(c == "-" for c in cherry[::2])


def salvage(wt, dest, runs):
    """Copy the build folder's small hand-made artifacts (not regenerable outputs) to DEST."""
    b = os.path.join(wt, "build")
    skip = set(targets(wt, runs)) | {os.path.join(b, d) for d in ("venv", "tools")}
    files = []
    if os.path.isdir(b):
        for dp, dn, fn in os.walk(b):
            dn[:] = [d for d in dn if os.path.join(dp, d) not in skip and not os.path.islink(os.path.join(dp, d))]
            files += [os.path.join(dp, f) for f in fn if KEEP_EXT.match(f) and os.path.join(dp, f) not in skip
                      and not os.path.islink(os.path.join(dp, f)) and not RUN_FILE.match(f)]
    total = sum(os.path.getsize(f) for f in files)
    return files, total


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--yes", action="store_true")
    ap.add_argument("--runs", action="store_true")
    ap.add_argument("--worktrees", action="store_true")
    a = ap.parse_args()
    if a.worktrees:
        subprocess.run(["git", "fetch", "-q", "origin"], cwd=ROOT)
    busy = busy_dirs()
    freed = 0
    for w in worktrees():
        wt = w["path"]
        name = os.path.relpath(wt, os.path.dirname(ROOT))
        if wt.startswith(os.path.join(HOME, ".codex")):
            continue
        if not os.path.isdir(wt):
            continue
        using = in_use(wt, busy)
        if using:
            print(f"{name}: in use ({', '.join(using)}), skipped")
            continue
        removable = False
        if a.worktrees and os.path.realpath(wt) != os.path.realpath(ROOT) and \
                (wt.startswith(os.path.join(os.path.dirname(ROOT), "wt") + os.sep)
                 or os.path.basename(wt).startswith("b3-")):
            why = []
            if git("status", "--porcelain", cwd=wt).strip():
                why.append("uncommitted changes")
            if not merged(wt):
                why.append("commits not in origin/main")
            wip = os.path.join(wt, "wip")
            if os.path.isdir(wip) and any(os.scandir(wip)):
                why.append("wip/ not empty")
            files, total = salvage(wt, None, a.runs)
            if total > 50 << 20:
                why.append(f"{total >> 20} MB of build artifacts")
            if why:
                print(f"{name}: kept as a worktree ({', '.join(why)})")
            else:
                removable = True
                n = size(wt)
                print(f"{name}: remove worktree, {n / 1e9:.2f} GB"
                      + (f" (salvage {len(files)} artifact files, {total >> 10} KB)" if files else ""))
                if a.yes:
                    dest = os.path.join(ROOT, "build", "salvage", os.path.basename(wt))
                    for f in files:
                        to = os.path.join(dest, os.path.relpath(f, os.path.join(wt, "build")))
                        os.makedirs(os.path.dirname(to), exist_ok=True)
                        shutil.copy2(f, to)
                    subprocess.run(["git", "worktree", "remove", "--force", wt], cwd=ROOT, check=True)
                freed += n
        if removable:
            continue
        ts = targets(wt, a.runs)
        n = sum(size(t) for t in ts)
        if n:
            print(f"{name}: {len(ts)} build outputs, {n / 1e9:.2f} GB")
            if a.yes:
                for t in ts:
                    remove(t)
        freed += n
    print(f"{'freed' if a.yes else 'would free'} {freed / 1e9:.1f} GB" + ("" if a.yes else " (dry run; --yes to delete)"))


if __name__ == "__main__":
    main()

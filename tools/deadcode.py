#!/usr/bin/env python3
"""C that compiles to nothing: what the compiler proves about a unit that its author didn't write down.

    python3 tools/deadcode.py FILE.c...        report per file
    python3 tools/deadcode.py --all            every unit under src/ (cached by content hash)

scope's instrumented agbcc (tools/scope/build_agbcc.sh), run with AGBCC_DEAD, reports two things:

* `unreachable`: an instruction an optimization pass deleted as unreachable after the first jump
  pass. The first pass removes what the syntax already made dead (code after a return, `if (0)`);
  anything later is a branch whose condition the compiler had to work out, such as `x < 0` on a
  value that came from an unsigned halfword.
* `zero`: an operation an accepted combine transformation proved is always zero from the
  known-zero bits of its operands, such as `(u16)x >> 16` or `(x & 31) >> 5`, in a statement
  that shifts, masks or divides. Speculative proofs from rejected combinations are discarded,
  and so is the sign correction of a signed division by a power of two (a shift by 31 the C
  never wrote; the compiler reports a constant shift with its count, `lshiftrt:31`).

Both are found from what the compiler does, not from how the C is spelled. tools/concessions.py
turns them into the warning kinds `dead-code` and `proved-zero`. A site inside a header (an inline
helper) is reported at the header's line, for every unit that expands it.
"""
from __future__ import annotations

import hashlib
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
AGBCC = os.path.join(ROOT, "build", "scope", "agbcc", "gcc", "agbcc")
CLANG = "/Library/Developer/CommandLineTools/usr/bin/clang"
CPP = [CLANG if os.path.exists(CLANG) else "cc", "-E"] if sys.platform == "darwin" else ["cpp"]
CACHE = os.path.join(ROOT, "build", "deadcode")
KIND = {"unreachable": "dead-code", "zero": "proved-zero"}
# an unreachable register-only instruction is usually the compiler's own leftovers; these are a person's
EFFECTS = ("call", "store")
# proved-zero is only reported for a statement that itself shifts, masks or divides. The compiler
# also proves zeros in its own expansion of a packed struct local (`pos.y -= 16`), which nobody wrote.
_OPERATOR = re.compile(r">>|<<|\s[&/%]=?\s")
_DIVIDE = re.compile(r"\s[/%]=?\s")
SIGN_SHIFT = ":31"  # a shift by the word size less one
# an and with a literal zero: the mask store_bit_field makes when a store replaces a whole halfword
# bit-field (`(old & 0) | new`). C's `x & 0` folds in the front end, and a zero the C did prove
# (`f &= (u8)x >> 8`) is reported at the operation that produced it.
OWN_MASK = "and:0"


def _sign_correction(detail: str, statement: str) -> bool:
    """True for the sign correction expand_divmod makes for a signed division or remainder by a
    power of two: shifts by 31 that the C never wrote. On a value that cannot be negative the
    correction is always zero, but the division itself is live."""
    return detail.endswith(SIGN_SHIFT) and bool(_DIVIDE.search(statement)) and "31" not in statement


def _statement(lines: list[str], line: int) -> str:
    """The statement that starts on LINE (1-based), through its `;` or `{`."""
    out = []
    for text in lines[line - 1:line + 11]:
        out.append(text)
        if ";" in text or "{" in text:
            break
    return "\n".join(out)


def available() -> bool:
    return os.path.exists(AGBCC)


def _version() -> str:
    return (str(os.path.getmtime(AGBCC)) if available() else "") + str(os.path.getmtime(__file__))


def compile_unit(src: str) -> list[tuple[str, str, int, str, str]]:
    """[(kind, file, line, function, detail)] for one C file (path relative to ROOT), sorted, no repeats."""
    pre = subprocess.run(CPP + ["-nostdinc", "-undef", "-I", "include", "-iquote", ".", src],
                         cwd=ROOT, capture_output=True, text=True)
    if pre.returncode:
        raise RuntimeError(f"{src}: preprocess failed\n{pre.stderr[-2000:]}")
    r = subprocess.run([AGBCC, "-O2", "-mthumb-interwork", "-o", os.devnull], input=pre.stdout, cwd=ROOT,
                       capture_output=True, text=True, env={**os.environ, "AGBCC_DEAD": "1"})
    found = set()
    sources: dict[str, list[str]] = {}
    for line in r.stderr.splitlines():
        part = line.split("\t")
        if len(part) == 6 and part[0] == "AGBCC_DEAD":
            _, kind, file, ln, func, detail = part
            if kind == "unreachable" and detail not in EFFECTS:
                continue
            file = os.path.normpath(file)
            if kind == "zero":
                if file not in sources:
                    try:
                        sources[file] = open(os.path.join(ROOT, file), errors="replace").read().splitlines()
                    except OSError:
                        sources[file] = []
                statement = _statement(sources[file], int(ln))
                if detail == OWN_MASK or not _OPERATOR.search(statement) or _sign_correction(detail, statement):
                    continue
            found.add((KIND[kind], file, int(ln), func, detail))
    if r.returncode:
        raise RuntimeError(f"{src}: compile failed\n{r.stderr[-2000:]}")
    return sorted(found)


def _key(src: str) -> str:
    """A unit's result depends on its text and every header; hashing all of include/ once is cheap."""
    h = hashlib.sha1((_version() + _headers()).encode())
    h.update(open(os.path.join(ROOT, src), "rb").read())
    return h.hexdigest()


_HEADERS = None


def _headers() -> str:
    global _HEADERS
    if _HEADERS is None:
        h = hashlib.sha1()
        for base, _, names in sorted(os.walk(os.path.join(ROOT, "include"))):
            for n in sorted(names):
                h.update(open(os.path.join(base, n), "rb").read())
        _HEADERS = h.hexdigest()
    return _HEADERS


def unit(src: str) -> list[tuple[str, str, int, str, str]]:
    """compile_unit, cached under build/deadcode by content hash."""
    os.makedirs(CACHE, exist_ok=True)
    path = os.path.join(CACHE, src.replace("/", "__") + ".json")
    key = _key(src)
    try:
        saved = json.load(open(path))
        if saved["key"] == key:
            return [tuple(x) for x in saved["sites"]]
    except (OSError, ValueError, KeyError):
        pass
    sites = compile_unit(src)
    json.dump({"key": key, "sites": sites}, open(path, "w"))
    return sites


def units() -> list[str]:
    out = []
    for base, _, names in os.walk(os.path.join(ROOT, "src")):
        for n in names:
            rel = os.path.relpath(os.path.join(base, n), ROOT)
            if n.endswith(".c") and not rel.startswith("src/lib/"):
                out.append(rel)
    return sorted(out)


def tree(jobs: int = 8) -> dict[tuple[str, int, str], set[str]]:
    """{(file, line, kind): {functions}} across every unit; a header site appears once."""
    out: dict[tuple[str, int, str], set[str]] = {}
    with ThreadPoolExecutor(jobs) as pool:
        for sites in pool.map(unit, units()):
            for kind, file, line, func, _ in sites:
                out.setdefault((file, line, kind), set()).add(func)
    return out


def main(argv=None) -> int:
    args = (argv if argv is not None else sys.argv[1:])
    if not available():
        sys.exit("no instrumented compiler: run sh tools/scope/build_agbcc.sh")
    if args == ["--all"]:
        found = tree()
        for (file, line, kind), funcs in sorted(found.items()):
            print(f"{file}:{line}: warning [{kind}] in {', '.join(sorted(funcs)[:3])}")
        print(f"{len(found):,} sites", file=sys.stderr)
        return 0
    for src in args:
        for kind, file, line, func, detail in compile_unit(os.path.relpath(os.path.abspath(src), ROOT)):
            print(f"{file}:{line}: warning [{kind}] {detail} in {func}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

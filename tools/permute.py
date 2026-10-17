#!/usr/bin/env python3
"""Set up a decomp-permuter run for one function.

  permute.py src/fn/sub_08033568.c [FUNC] [--run N_SECONDS] [-j THREADS]
             [--focus auto|LINES] [--context N] [--weight PASS=W ...]

Creates build/permute/FUNC/run-*/ with base.c (preprocessed C), target.o (the
original function assembled and linked), compile.sh (agbcc) and settings.toml.
With --run, runs the permuter for that long (all cores by default) and stops
early at an exact match; the best candidates land in that run's output-*/
directories, and progress.tsv records when each better score appeared.

--focus restricts every mutation to some source lines of SRC: `auto` takes the
lines tools/regdiag.py blames for the differing instructions and registers;
otherwise give lines like `120-135,140`. --context widens each line by N.

Needs the project's decomp-permuter fork: tools/setup.sh fetches the pinned
upstream into build/tools/decomp-permuter and applies
tools/patches/decomp-permuter-agbcc.patch (crash fixes, agbcc passes, focus).
PERMUTER=/path overrides the location.
"""
import argparse
import json
import os
import re
import shlex
import shutil
import signal
import subprocess
import sys
import tempfile
import time

import build
import check
import split
from romlib import ROOT

def _find_permuter():
    if os.environ.get("PERMUTER"):
        return os.environ["PERMUTER"]
    local = os.path.join(ROOT, "build", "tools", "decomp-permuter")
    if os.path.isdir(local):
        return local
    # next to the repo, or next to the main checkout when run from ../wt/NAME
    for base in (os.path.dirname(ROOT), os.path.dirname(os.path.dirname(os.path.dirname(ROOT)))):
        p = os.path.join(base, "decomp-permuter")
        if os.path.isdir(p):
            return p
    return os.path.join(os.path.dirname(ROOT), "decomp-permuter")


PERMUTER = os.path.abspath(_find_permuter())
# The permuter needs toml; the project venv has it.
VENV_PYTHON = os.path.join(ROOT, "build", "venv", "bin", "python")
PYTHON = VENV_PYTHON if os.path.exists(VENV_PYTHON) else sys.executable


def check_permuter():
    if not os.path.exists(os.path.join(PERMUTER, "permuter.py")):
        sys.exit(f"decomp-permuter not found at {PERMUTER}: run tools/setup.sh or set PERMUTER")
    if not os.path.exists(os.path.join(PERMUTER, "src", "agbcc_passes.py")) \
            and not os.environ.get("PERMUTER_UPSTREAM"):  # for comparisons
        sys.exit(f"{PERMUTER} lacks the project patch: run tools/setup.sh "
                 "(applies tools/patches/decomp-permuter-agbcc.patch)")


def outputs(directory):
    """[(score, path)] of the candidates the permuter saved, best first."""
    out = []
    for name in os.listdir(directory):
        m = re.match(r"output-(\d+)-\d+$", name)
        if m:
            out.append((int(m.group(1)), os.path.join(directory, name)))
    return sorted(out)


def run_search(command, directory, seconds, stop_at_match=True, log=None):
    """Return False on the time budget or an exact match; a crashed search is
    an error. Records each new best score with its time in progress.tsv."""
    start = time.time()
    proc = subprocess.Popen(command, cwd=directory, start_new_session=True,
                            stdout=log, stderr=subprocess.STDOUT if log else None)
    best = None
    progress = open(os.path.join(directory, "progress.tsv"), "a")
    try:
        while True:
            try:
                proc.wait(timeout=1)
                break
            except subprocess.TimeoutExpired:
                pass
            got = outputs(directory)
            if got and (best is None or got[0][0] < best):
                best = got[0][0]
                progress.write(f"{time.time() - start:.1f}\t{best}\n")
                progress.flush()
            if (best == 0 and stop_at_match) or time.time() - start > seconds:
                raise subprocess.TimeoutExpired(command, seconds)
    except (subprocess.TimeoutExpired, KeyboardInterrupt) as error:
        try:
            os.killpg(proc.pid, signal.SIGTERM)
        except ProcessLookupError:
            pass
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            os.killpg(proc.pid, signal.SIGKILL)
            proc.wait()
        if isinstance(error, KeyboardInterrupt):
            raise
        return False
    finally:
        progress.close()
    if proc.returncode:
        raise RuntimeError(f"permuter failed (exit {proc.returncode}); run directory: {directory}")
    return True


def prepare_source(src, func):
    """(base.c text, {base.c line: line of src}) for one isolated function."""
    # The upstream strip_other_fns regex can mistake a compact if/loop body
    # for another function and silently delete it. Our units already isolate
    # ROM functions: require that shape and retain static inline helpers.
    names = split.c_functions(os.path.join(ROOT, src))
    if names != [func]:
        raise ValueError(f"isolate {func} in a file with one ROM function before permuting; found {names}")
    # Keep line markers, then drop them, remembering where each line came from.
    pre = build.sh(build.CPP + ["-nostdinc", "-undef"] + build.DEFINES +
                   ["-I", "include", "-iquote", ".", src])
    out, line_map = [], {}
    cur_file, cur_line = None, 0
    want = os.path.normpath(os.path.join(ROOT, src))
    for ln in pre.splitlines():
        m = re.match(r'^# (\d+) "([^"]*)"', ln)
        if m:
            cur_line = int(m.group(1))
            cur_file = os.path.normpath(os.path.join(ROOT, m.group(2)))
            continue
        # Remove only the file-scope macro include. A broad asm/string regex
        # also swallowed empty-asm register/memory clobbers inside the
        # candidate, so a near match could start from different code.
        if re.match(r'^[ \t]*asm\s*\(\s*"\.include (?:\\.|[^"\\\n])*"\s*\)\s*;[ \t]*$', ln):
            ln = ""
        out.append(ln)
        if cur_file == want:
            line_map[len(out)] = cur_line
        cur_line += 1
    return "\n".join(out) + "\n", line_map


def parse_lines(spec):
    lines = set()
    for part in spec.replace(" ", ",").split(","):
        if not part:
            continue
        a, _, b = part.partition("-")
        lines.update(range(int(a), int(b or a) + 1))
    return lines


def ram_symbol_addresses(text, symbols):
    """RAM globals the source names, for perm_literal_address. ROM data stays
    symbolic: a literal ROM address would break the shift test."""
    names = set(re.findall(r"\b[A-Za-z_]\w*\b", text))
    return {n: symbols[n][0] for n in sorted(names & set(symbols))
            if symbols[n][1] in ("OBJECT", "NOTYPE") and 0x02000000 <= symbols[n][0] < 0x04000000}


def referenced_names(pre, symbols):
    """Symbols a candidate can refer to: names in the source, plus helpers
    the compiler calls on its own (_call_via_rN, division)."""
    names = set(re.findall(r"\b[A-Za-z_]\w*\b", pre))
    return names | {n for n in symbols if n.startswith(("_call_via_", "__div", "__mod", "__udiv", "__umod"))}


def write_fast_compile(d, src, pre, symbols, address, cc, slow):
    """compile.sh without Python: agbcc, as and ld against a symbol stub made
    once here. Under load each process start costs tens of milliseconds, and
    the per-candidate Python wrapper (nm, a stub assembly, ld) dominated the
    search. The base object must come out byte-identical to the checked path."""
    stub_s, stub_o = os.path.join(d, "symbols-stub.s"), os.path.join(d, "symbols-stub.o")
    base_raw = os.path.join(d, "base-raw.o")
    asm = subprocess.run(
        [os.path.join(ROOT, build.compiler_for(src)), *build.cflags_for(src), "-o", "-"],
        input=pre, text=True, capture_output=True, check=True).stdout
    defined = set(re.findall(r"^(\w+):$", asm, re.M))
    open(os.path.join(d, "base-raw.s"), "w").write(asm + "\t.text\n\t.align 2, 0\n")
    build.sh(build.AS + ["-o", base_raw, os.path.join(d, "base-raw.s")])
    base_addr, _ = check.link_object(base_raw, os.path.join(d, "base-checked.o"), syms=symbols, address=address)
    names = referenced_names(pre, symbols) - defined
    stub = ["\t.syntax unified"]
    for name in sorted(names):
        if name in symbols:
            v = symbols[name][0]
            stub += [f"\t.global {name}", f"\t.set {name}, 0x{v:08X}",
                     f"\t.global {name}__addr", f"\t.set {name}__addr, 0x{v & ~1:08X}"]
    open(stub_s, "w").write("\n".join(stub) + "\n")
    build.sh(build.AS + ["-o", stub_o, stub_s])
    q = shlex.quote
    alias = r"""s/^(\w+):$/$1:\n\t.global $1__addr\n$1__addr:/ unless /^\./"""
    open(cc, "w").write(f"""#!/bin/sh
# invoked as: compile.sh input.c -o output.o
# Generated by tools/permute.py; compile-checked.sh is the reference path.
t=$(mktemp -d "${{3%/*}}/cc.XXXXXX") || exit 1
trap 'rm -rf "$t"' EXIT
{q(os.path.join(ROOT, build.compiler_for(src)))} {" ".join(map(q, build.cflags_for(src)))} -o "$t/raw.s" < "$1" 2> "$t/err" || exit 1
grep -q error "$t/err" && exit 1
perl -pe {q(alias)} "$t/raw.s" > "$t/c.s" || exit 1
printf '\t.text\n\t.align 2, 0\n' >> "$t/c.s"
{" ".join(map(q, build.AS))} -o "$t/c.o" "$t/c.s" || exit 1
arm-none-eabi-ld -Ttext=0x{base_addr:08X} -e 0 -o "$3" "$t/c.o" {q(stub_o)}
""")
    os.chmod(cc, 0o755)
    # The fast path must reproduce the checked path's base object.
    fast_o, checked_o = os.path.join(d, "base-fast.o"), os.path.join(d, "base-checked2.o")
    subprocess.run([cc, os.path.join(d, "base.c"), "-o", fast_o], check=True)
    subprocess.run([slow, os.path.join(d, "base.c"), "-o", checked_o], check=True)

    def text(o):  # symbol names in <...> are labels only (the scorer drops them too)
        return re.sub(r"<[^>]*>", "", build.sh(["arm-none-eabi-objdump", "-d", o]).split("\n", 3)[-1])
    if text(fast_o) != text(checked_o):
        print("fast compile.sh differs from the checked path; using the checked one")
        shutil.copy(slow, cc)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--run", type=int, default=0, help="seconds to run the permuter")
    ap.add_argument("--debug", action="store_true", help="score the base once, showing the diff")
    ap.add_argument("-j", type=int, default=os.cpu_count() or 4, help="worker processes (default: all cores)")
    ap.add_argument("--focus", help="'auto' (regdiag's lines) or source lines like 120-135,140")
    ap.add_argument("--context", type=int, default=1, help="widen each focus line by N lines")
    ap.add_argument("--weight", action="append", default=[], metavar="PASS=W",
                    help="override a randomization pass weight (repeatable)")
    ap.add_argument("--no-stop", action="store_true", help="keep searching after an exact match")
    ap.add_argument("--log", help="write the permuter's output to this file")
    a = ap.parse_args()
    if a.run < 0 or a.j < 1:
        ap.error("--run must be nonnegative and -j must be positive")
    src = a.src if a.src.endswith(".c") else os.path.join("src", "fn", a.src + ".c")
    func = a.func or os.path.basename(src)[:-2]
    alias = dict(re.findall(r"^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$", open(os.path.join(ROOT, src)).read(), re.M))
    label = func = alias.get(func, func)  # cpp expands "#define Name sub_X"
    parent = os.path.join(ROOT, "build", "permute", func)
    os.makedirs(parent, exist_ok=True)
    d = tempfile.mkdtemp(prefix="run-", dir=parent)
    symbols = check.elf_symbols()
    if func not in symbols:
        ap.error(f"{func} is absent from the baseline ELF; rebuild first")
    address = symbols[func][0] & ~1

    # base.c: preprocessed, without the top-level asm(".include") pycparser can't parse
    try:
        pre, line_map = prepare_source(src, func)
    except ValueError as error:
        ap.error(str(error))
    open(os.path.join(d, "base.c"), "w").write(pre)
    focus = []
    if a.focus:
        if a.focus == "auto":
            import regdiag
            src_lines = regdiag.analyze(os.path.join(ROOT, src), func).focus_lines()
            if not src_lines:
                print("regdiag finds no differing lines; searching the whole function")
        else:
            src_lines = parse_lines(a.focus)
        wide = {x + k for x in src_lines for k in range(-a.context, a.context + 1)}
        focus = sorted(b for b, l in line_map.items() if l in wide)
        print(f"focus: {len(src_lines)} source lines -> {len(focus)} base.c lines", flush=True)

    # target.o: the original function, from the split asm
    build.sh([sys.executable, "tools/split.py"])
    asm = os.path.join(ROOT, "build", "asm", "nonmatching", label + ".s")
    if not os.path.exists(asm):
        sys.exit(f"no original asm for {label} (expected {asm})")
    lines = open(asm).readlines()
    body = "".join(lines[:split.c_replacement_end(lines, 0, len(lines))]).replace(label, func)
    tgt = os.path.join(d, "target.s")
    open(tgt, "w").write('\t.include "asm/macros.inc"\n' + body)
    target_raw = os.path.join(d, "target-unlinked.o")
    build.sh(build.AS + ["-I", ".", "-o", target_raw, tgt])
    check.link_object(target_raw, os.path.join(d, "target.o"), syms=symbols, address=address)

    config = os.path.join(d, "compile.json")
    with open(config, "w") as f:
        # Only the names the source can mention: parsing the whole symbol
        # table took most of each candidate's compile time.
        names = referenced_names(pre, symbols)
        json.dump({"compiler": os.path.join(ROOT, build.compiler_for(src)), "assembler": build.AS,
                   "cflags": build.cflags_for(src),
                   "symbols": {n: v for n, v in symbols.items() if n in names},
                   "address": address}, f)

    cc = os.path.join(d, "compile.sh")
    slow = os.path.join(d, "compile-checked.sh")
    open(slow, "w").write(f"""#!/bin/sh
# invoked as: compile-checked.sh input.c -o output.o (resolves symbols per call)
exec {shlex.quote(sys.executable)} {shlex.quote(os.path.join(ROOT, 'tools', 'permute_compile.py'))} {shlex.quote(config)} "$@"
""")
    os.chmod(slow, 0o755)
    write_fast_compile(d, src, pre, symbols, address, cc, slow)
    settings = [f'func_name = "{func}"', 'compiler_type = "gcc"']
    if focus:
        settings.append(f"focus_lines = {focus}")
    addresses = ram_symbol_addresses(pre, symbols)
    if addresses:
        settings.append("[symbol_addresses]")
        settings += [f"{n} = 0x{v:08X}" for n, v in addresses.items()]
    weights = dict(w.split("=", 1) for w in a.weight)
    if weights:
        settings.append("[weight_overrides]")
        settings += [f"{k} = {float(v)}" for k, v in weights.items()]
    open(os.path.join(d, "settings.toml"), "w").write("\n".join(settings) + "\n")
    print(d, flush=True)
    if a.run or a.debug:
        check_permuter()
        # GNU timeout is not supplied by macOS. Terminate the entire worker
        # process group so the permuter's compiler children cannot outlive it.
        cmd = [PYTHON, "-u", os.path.join(PERMUTER, "permuter.py"),
               d, "-j", str(a.j), "--best-only", "--stack-diffs"] + ([] if a.log else ["--quiet"])
        if a.debug:
            cmd.append("--debug")
        log = open(a.log, "w") if a.log else None
        try:
            finished = run_search(cmd, d, a.run or 60, stop_at_match=not a.no_stop, log=log)
        except RuntimeError as error:
            sys.exit(str(error))
        if a.debug:
            if not finished:
                sys.exit("permuter diagnostic did not finish")
            return
        outs = outputs(d)
        print("\n".join(f"{score}\t{path}" for score, path in outs[:5]) if outs else "no candidate output saved")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Check one C file against the ROM without a full build, and show a diff.

  check.py src/fn/sub_08033568.c [--quiet]
  check.py sub_08033568              (same; looks in src/fn/)

Compiles the file exactly as build.py does, links it alone at the address of
its first function (every other symbol resolved from the last full build's
build/boktai3.elf), and compares the bytes with baserom.gba. Prints a
compact aligned disassembly hunks (or --full), with differences marked '!'. Exit status 0 on
a match. A full `tools/build.py` run is still needed before committing (it
checks the layout of the whole ROM); this is the fast inner loop.
"""
import argparse
import difflib
import hashlib
import json
import os
import re
import sys
import tempfile

import anchordiff
import build
from romlib import ROOT, load_rom, off

try:
    import capstone
except ImportError:  # pragma: no cover
    capstone = None


def elf_symbols():
    elf = os.path.join(ROOT, "build", "boktai3.elf")
    if not os.path.exists(elf):
        sys.exit("build/boktai3.elf missing: run tools/build.py once first")
    # readelf, not nm: nm clears bit 0 of Thumb function symbols
    out = build.sh(["arm-none-eabi-readelf", "-sW", elf])
    syms = {}
    for ln in out.splitlines():
        p = ln.split()
        if len(p) == 8 and p[0][:-1].isdigit() and p[6] != "UND":
            syms[p[7]] = (int(p[1], 16), p[3])
    return syms


def disasm(code, addr, *, arm=False):
    width = 4 if arm else 2
    if capstone is None:
        return [(addr + i, code[i:i + width].hex()) for i in range(0, len(code), width)]
    md = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_ARM if arm else capstone.CS_MODE_THUMB)
    out, i = [], 0
    while i < len(code):
        ins = next(md.disasm(code[i:i + 4], addr + i), None)
        if ins is None:
            value = int.from_bytes(code[i:i + width], "little")
            out.append((addr + i, f".{width}byte 0x{value:0{width * 2}x}"))
            i += width
        else:
            out.append((addr + i, f"{ins.mnemonic} {ins.op_str}".strip()))
            i += ins.size
    return out


def link_object(obj, elf, *, syms=None, address=None):
    """Resolve a standalone object as in the ROM; also used by the permuter.

    A supplied symbol snapshot keeps a search reproducible if the full build
    changes while it is running. Output and stubs are private to this object.
    """
    defined = []
    undefined = []
    for ln in build.sh(["arm-none-eabi-nm", obj]).splitlines():
        p = ln.split()
        if len(p) == 2 and p[0] == "U":
            undefined.append(p[1])
        elif len(p) == 3 and p[1] in "Tt" and not p[2].endswith("__addr") and not p[2].startswith("."):
            defined.append((int(p[0], 16), p[2]))
    if not defined:
        sys.exit("no functions defined in " + obj)
    defined.sort()
    if syms is None:
        syms = elf_symbols()
    first = defined[0][1]
    if address is not None:
        addr = address
    elif first not in syms:
        m = re.search(r"_([0-9A-F]{8})$", first)
        if not m:
            sys.exit(f"cannot find the address of {first} (not in build/boktai3.elf)")
        addr = int(m.group(1), 16)
    else:
        addr = syms[first][0] & ~1
    addr -= defined[0][0]

    stub = ["\t.syntax unified"]
    missing = []
    ram = build.ram_symbols()
    for name in undefined:
        key = name[:-6] if name.endswith("__addr") else name
        if key in syms:
            v, t = syms[key]
            # Thumb functions keep bit 0 (BL ignores it, a .word needs it);
            # NAME__addr is the plain even address.
            if name.endswith("__addr"):
                v &= ~1
            stub += [f"\t.global {name}", f"\t.set {name}, 0x{v:08X}"]
        elif key in ram:
            stub += [f"\t.global {name}", f"\t.set {name}, 0x{ram[key]:08X}"]
        else:
            m = re.search(r"_([0-9A-Fa-f]{8})$", name)
            if m:
                stub += [f"\t.global {name}", f"\t.set {name}, 0x{m.group(1)}"]
            else:
                missing.append(name)
    if missing:
        sys.exit("unresolved symbols: " + ", ".join(missing))
    stub_base = elf + "_stub"
    with open(os.path.join(ROOT, stub_base + ".s"), "w") as f:
        f.write("\n".join(stub) + "\n")
    build.sh(build.AS + ["-o", stub_base + ".o", stub_base + ".s"])
    build.sh(["arm-none-eabi-ld", f"-Ttext=0x{addr:08X}", "-e", "0", "-o", elf,
              obj, stub_base + ".o"])
    return addr, [n for _, n in defined]


def comparison(orig, mine, addr, *, arm=False, target_size=None):
    """Byte acceptance is independent of the human-readable alignment."""
    different = [addr + i for i in range(max(len(orig), len(mine)))
                 if orig[i:i + 1] != mine[i:i + 1]]
    return {
        "status": "match" if orig == mine else "mismatch",
        "address": addr, "candidate_bytes": len(mine),
        "target_bytes": target_size,
        "extent_basis": "reviewed" if target_size is not None else "candidate-length",
        "size_delta": len(mine) - target_size if target_size is not None else None,
        "different_byte_count": len(different), "different_addresses": different,
        "original_sha256": hashlib.sha256(orig).hexdigest(),
        "candidate_sha256": hashlib.sha256(mine).hexdigest(),
        "original": disasm(orig, addr, arm=arm),
        "candidate": disasm(mine, addr, arm=arm),
    }


def realigned(result, mask=False):
    """Differing instructions after tools/anchordiff.py's realignment (listings cut
    at shared calls, then aligned piecewise; mask=True ignores registers)."""
    rows = lambda key: [f"{a:08X} {t}" for a, t in result[key]]
    return sum(max(i2 - i1, j2 - j1) for _, _, (_, i1, i2, j1, j2)
               in anchordiff.hunks(rows("original"), rows("candidate"), mask))


def fuzzy_percent(result):
    """Instruction-text similarity (0-100), with absolute addresses masked so a
    size difference early in the function does not penalise every branch.
    Big functions: score edits by realigned() instead (docs/BIG_FUNCTIONS.md)."""
    norm = lambda rows: [re.sub(r"0x[0-9a-f]{7,8}", "ADDR", x[1]) for x in rows]
    matcher = difflib.SequenceMatcher(None, norm(result["original"]), norm(result["candidate"]),
                                      autojunk=False)
    return 100.0 if result["status"] == "match" else min(99.9, 100 * matcher.ratio())


def record_fuzzy(src, scores):
    """Write `// fuzzy: NN.N%` as the first line of each NONMATCHING draft,
    using the score of the function its #else branch INCLUDE_ASMs."""
    path = os.path.join(ROOT, src)
    text = open(path).read()
    block = re.compile(r"(#\s*ifdef\s+NONMATCHING[^\n]*\n)(//\s*fuzzy:[^\n]*\n)?(.*?#\s*else.*?#\s*endif)", re.S)

    def fix(m):
        names = re.findall(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)', m.group(3))
        pct = next((scores[n] for n in names if n in scores), None)
        return m.group(0) if pct is None else f"{m.group(1)}// fuzzy: {pct:.1f}%\n{m.group(3)}"
    open(path, "w").write(block.sub(fix, text))


def inventory():
    import progress
    funcs, sizes = progress.load_functions()
    by_label = {f"sub_{a:08X}": n for n, a in funcs}
    addr = {n: a for n, a in funcs}
    return by_label, addr, sizes


def draft_extent(src, defined):
    """Bytes of the ROM functions a NONMATCHING build of src defines."""
    by_label, _, sizes = inventory()
    return sum(sizes.get(by_label.get(n, n), 0) for n in defined)


def draft_scores(src, obj, mine, rom, arm):
    """Per-draft fuzzy scores: each draft's emitted bytes against its own ROM
    function, so drafts inside a multi-function unit are scored separately."""
    from split import nonmatching_functions
    by_label, addr, sizes = inventory()
    syms = []
    for ln in build.sh(["arm-none-eabi-nm", obj]).splitlines():
        p = ln.split()
        if len(p) == 3 and p[1] in "Tt" and not p[2].endswith("__addr") and not p[2].startswith("."):
            syms.append((int(p[0], 16), p[2]))
    syms.sort()
    base = syms[0][0] if syms else 0
    scores = {}
    for name, _ in nonmatching_functions(os.path.join(ROOT, src)):
        here = [o for o, n in syms if n == name]
        key = by_label.get(name, name)
        if not here or key not in addr:
            continue
        start = here[0] - base
        a = addr[key]
        # An inventory span can contain formerly unlabeled trailing functions.
        # Their recovered C belongs to the same reviewed byte range.
        def inside_span(n):
            m = re.fullmatch(r"sub_([0-9A-Fa-f]{8})", n)
            return n not in addr and m is not None and a < int(m[1], 16) < a + sizes[key]
        end = next((o - base for o, n in syms if o - base > start and not inside_span(n)), len(mine))
        part = comparison(rom[off(a):off(a) + sizes[key]], mine[start:end], a, arm=arm)
        scores[name] = fuzzy_percent(part)
    return scores


def render_diff(result, context=3, full=False):
    """Align decoded instructions; retain both addresses and literal differences.

    Pools are not classified as instructions: this remains a decoded-byte view,
    and the independent byte summary is authoritative.
    """
    left, right = result["original"], result["candidate"]
    matcher = difflib.SequenceMatcher(None, [x[1] for x in left],
                                    [x[1] for x in right], autojunk=False)
    groups = [matcher.get_opcodes()] if full else matcher.get_grouped_opcodes(context)
    lines = []
    for group in groups:
        lines.append("@@ original / candidate @@")
        for tag, a, b, c, d in group:
            for i in range(max(b - a, d - c)):
                x = left[a + i] if a + i < b else None
                y = right[c + i] if c + i < d else None
                fmt = lambda v: f"{v[0]:08X} {v[1]}" if v else ""
                lines.append(f"{' ' if tag == 'equal' else '!'} {fmt(x):48} | {fmt(y)}")
    if not lines and result["different_byte_count"]:
        lines.append("Decoded text is identical; resolved bytes differ (see byte addresses in JSON).")
    return "\n".join(lines)


def main():
    build.wait_for_cloud_setup()
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("--quiet", action="store_true")
    ap.add_argument("--full", action="store_true", help="show all decoded rows")
    ap.add_argument("--context", type=int, default=3)
    ap.add_argument("--json", action="store_true", help="machine-readable comparison")
    ap.add_argument("--target-size", type=lambda v: int(v, 0),
                    help="reviewed body plus pool/padding extent, in bytes")
    ap.add_argument("--nonmatching", action="store_true",
                    help="compile #ifdef NONMATCHING drafts; compare against the functions' full size")
    ap.add_argument("--record", action="store_true",
                    help="with --nonmatching: write the fuzzy score into the draft's source")
    a = ap.parse_args()
    if a.nonmatching:
        build.DEFINES.append("-DNONMATCHING")
    if a.context < 0 or (a.target_size is not None and a.target_size <= 0):
        ap.error("context must be nonnegative and target size must be positive")
    src = a.src
    if not src.endswith(".c"):
        src = os.path.join("src", "fn", src + ".c")
    src = os.path.relpath(os.path.join(ROOT, src), ROOT)
    parent = os.path.join(ROOT, "build", "check")
    os.makedirs(parent, exist_ok=True)
    work = os.path.relpath(tempfile.mkdtemp(prefix="run-", dir=parent), ROOT)
    base = os.path.join(work, os.path.basename(src)[:-2])
    obj = base + ".o"
    build.build_c(src, obj)
    addr, defined = link_object(obj, base + ".elf")
    build.sh(["arm-none-eabi-objcopy", "-O", "binary", "-j", ".text", base + ".elf", base + ".bin"])
    mine = open(os.path.join(ROOT, base + ".bin"), "rb").read()
    rom = load_rom(verify=False)
    if a.nonmatching and a.target_size is None:
        a.target_size = draft_extent(src, defined)
    extent = a.target_size if a.target_size is not None else len(mine)
    if off(addr) < 0 or off(addr) + extent > len(rom):
        sys.exit("target extent outside ROM")
    orig = rom[off(addr):off(addr) + extent]
    arm = os.path.basename(build.compiler_for(src)) == "agbcc_arm"
    result = comparison(orig, mine, addr, arm=arm, target_size=a.target_size)
    result.update(functions=defined, artifacts=work, fuzzy_match_percent=fuzzy_percent(result))
    if a.nonmatching:
        result["draft_fuzzy"] = draft_scores(src, obj, mine, rom, arm)
        for name, pct in result["draft_fuzzy"].items():
            print(f"draft {name}: fuzzy {pct:.1f}%")
        if a.record:
            record_fuzzy(src, result["draft_fuzzy"])
    ok = result["status"] == "match"
    if not ok:
        result["anchordiff"] = {"structural": realigned(result, mask=True), "total": realigned(result)}
    with open(os.path.join(ROOT, work, "comparison.json"), "w") as f:
        json.dump(result, f, indent=2)
    with open(os.path.join(ROOT, work, "full.diff"), "w") as f:
        f.write(render_diff(result, full=True) + "\n")
    if a.json:
        print(json.dumps(result))
        sys.exit(0 if ok else 1)
    if not a.quiet and (not ok or a.full):
        print(render_diff(result, context=a.context, full=a.full))
    print(f"{result['different_byte_count']} differing bytes; fuzzy {result['fuzzy_match_percent']:.1f}%; "
          f"extent={result['extent_basis']}; artifacts: {work}")
    if not ok:
        print(f"realigned: {result['anchordiff']['total']} differing insns, "
              f"{result['anchordiff']['structural']} ignoring registers (tools/anchordiff.py {work})")
    names = ", ".join(defined)
    print(f"{names}: {'MATCH' if ok else 'MISMATCH'} ({len(mine)} bytes at 0x{addr:08X})")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()

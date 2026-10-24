#!/usr/bin/env python3
"""Splice decompiled C into the build.

Scans src/**/*.c for function definitions.  Every C file is a "unit" that must
cover a contiguous run of functions in ROM order.  The generated code
(gen/code_sym.s) is cut around those functions into asm segments, and a
linker script places asm segments and C objects in ROM order:

  build/asm/seg_NNN.s        asm between C units (all labels made global)
  build/link.ld              link order + RAM/ROM symbol definitions
  build/units.txt            "asm build/asm/seg_000.s" / "c src/foo.c" lines

Function names resolve to addresses through gbadisasm's sub_XXXXXXXX naming or
symbols/functions.csv.
"""
import csv
import glob
import os
import re
import sys

from romlib import ROOT

FUNC_START = re.compile(r"^\t(?:thumb|arm|non_word_aligned_thumb)_func_start (\S+)")
FUNC_LABEL = re.compile(r"^(\w+): @ 0x([0-9A-F]{8})")
LABEL = re.compile(r"^(\w+):")
SET_LABEL = re.compile(r"^\t\.set (\w+),")
INSN = re.compile(r"^\t(?!\.)(?!\w+_func_start)[a-z]")
NONENTRIES = os.path.join(ROOT, "symbols", "function_nonentries.csv")


EXTRA_STARTS = os.path.join(ROOT, "symbols", "build_extra_functions.csv")


def extra_function_starts(path=EXTRA_STARTS):
    """Reviewed function starts the disassembler left as plain labels."""
    if not os.path.exists(path):
        return set()
    with open(path, newline="") as source:
        return {int(row["address"], 16) for row in csv.DictReader(source)}


def function_nonentries(path=NONENTRIES):
    """Reviewed disassembler labels inside real function bodies."""
    if not os.path.exists(path):
        return set()
    entries = set()
    with open(path, newline="") as source:
        for row in csv.DictReader(source):
            address = int(row["address"], 16)
            if (address in entries or address & 1 or
                    not 0x08000000 <= address < 0x0824DAFA or not row["evidence"].strip()):
                raise ValueError("invalid reviewed non-entry boundary")
            entries.add(address)
    return entries


def remove_nonentry_starts(code, addresses):
    """Keep interior labels/instructions, removing only their function directive."""
    result = []
    for i, line in enumerate(code):
        label = FUNC_LABEL.match(code[i + 1]) if i + 1 < len(code) else None
        if FUNC_START.match(line) and label and int(label.group(2), 16) in addresses:
            continue
        result.append(line)
    return result
C_FUNC = re.compile(r"^(?!static\b)[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;]*\)\s*$|^(?!static\b)[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;]*\)\s*\{", re.M)


INCLUDE_ASM = re.compile(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)')


NONMATCHING_IF = re.compile(r"^#\s*ifdef\s+NONMATCHING\b")
FUZZY = re.compile(r"^//\s*fuzzy:\s*([0-9.]+)%", re.M)


def split_nonmatching(raw):
    """Split source at `#ifdef NONMATCHING ... #else ... #endif` blocks.

    Returns (default, drafts): the text the normal build compiles (the #else
    branch kept) and a list of (draft_text, fallback_text) pairs. Draft C never
    counts as matched code; its #else branch must INCLUDE_ASM the function.
    """
    out, drafts = [], []
    lines = raw.split("\n")
    i = 0
    while i < len(lines):
        if not NONMATCHING_IF.match(lines[i].strip()):
            out.append(lines[i])
            i += 1
            continue
        depth, branch, draft, fallback = 0, "draft", [], []
        i += 1
        while i < len(lines):
            s = lines[i].strip()
            if re.match(r"^#\s*if", s):
                depth += 1
            elif re.match(r"^#\s*endif", s):
                if depth == 0:
                    break
                depth -= 1
            elif depth == 0 and re.match(r"^#\s*else", s):
                branch = "fallback"
                i += 1
                continue
            (draft if branch == "draft" else fallback).append(lines[i])
            i += 1
        i += 1
        drafts.append(("\n".join(draft), "\n".join(fallback)))
        out.extend(fallback)
    return "\n".join(out), drafts


def nonmatching_functions(path):
    """[(name, fuzzy percent or None)] for each NONMATCHING draft in a unit."""
    with open(path) as source:
        _, drafts = split_nonmatching(source.read())
    found = []
    for draft, fallback in drafts:
        m = FUZZY.search(draft)
        for name in INCLUDE_ASM.findall(fallback):
            found.append((name, float(m.group(1)) if m else None))
    return found


def c_functions(path, with_asm=True):
    """Functions a C unit covers, in file order: C definitions plus
    INCLUDE_ASM(dir, name) placeholders (unless with_asm=False).
    NONMATCHING drafts are skipped; their INCLUDE_ASM fallbacks count."""
    with open(path) as source:
        raw = source.read()
    raw, _ = split_nonmatching(raw)
    src = raw
    # "#define Name sub_XXXXXXXX" aliases a known name to its address label
    alias = dict(re.findall(r"^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$", raw, re.M))
    src = re.sub(r"/\*.*?\*/", "", src, flags=re.S)
    src = re.sub(r"//[^\n]*", "", src)
    src = re.sub(r"^#(?:[^\n]*\\\n)*[^\n]*$", "", src, flags=re.M)  # with backslash continuations
    found = []
    depth, upto = 0, 0
    # a definition: identifier( ... ) followed (possibly after newline) by {
    for m in re.finditer(r"\b(\w+)\s*\(([^;{}()]|\([^()]*\))*\)\s*\{", src):
        name = m.group(1)
        depth += src.count("{", upto, m.start()) - src.count("}", upto, m.start())
        upto = m.start()
        if name in ("if", "for", "while", "switch", "return", "sizeof") or depth > 0:
            continue  # a statement or a macro use inside a body, e.g. STEP(x) { ... }
        # Storage class/return type may be on preceding lines. Stop at the
        # previous declaration or body boundary, not merely the last newline.
        decl_start = max(src.rfind(c, 0, m.start()) for c in ";{}") + 1
        if re.search(r"\bstatic\b", src[decl_start:m.start()]):
            continue  # static (inline) helper: not a ROM function of its own
        found.append((m.start(), name))
    # a function-defining macro at top level, e.g. STATE_COMMAND_WAIT(sub_X, 0x1b)
    # (no trailing semicolon; the macro expands to the definition)
    for m in re.finditer(r"^([A-Z][A-Z0-9_]+)\(\s*(\w+)\s*[,)][^\n;]*\)[ \t]*$", src, re.M):
        if src.count("{", 0, m.start()) == src.count("}", 0, m.start()):
            found.append((m.start(), m.group(2)))
    if with_asm:
        for m in INCLUDE_ASM.finditer(src):
            found.append((m.start(), m.group(1)))
    return [alias.get(n, n) for _, n in sorted(found)]


def write_nonmatching(code, funcs):
    """build/asm/nonmatching/<name>.s for every function (used by INCLUDE_ASM)."""
    d = os.path.join(ROOT, "build", "asm", "nonmatching")
    os.makedirs(d, exist_ok=True)
    for name, addr, i, j, nonword in funcs:
        # Same plain alias build.py adds after C function labels, so a unit
        # whose first function is INCLUDE_ASM can still anchor relative labels.
        body = [ln + f"\n\t.global {name}__addr\n{name}__addr:" if ln.startswith(f"{name}:") else ln
                for ln in code[i:j]]
        globs = set()
        for ln in body:
            m = LABEL.match(ln) or SET_LABEL.match(ln)
            if m:
                globs.add(m.group(1))
        # agbcc emits divided syntax; gbadisasm output is unified
        text = ("\t.syntax unified\n" + "".join(f"\t.global {g}\n" for g in sorted(globs)) +
                "\n".join(body) + "\n\t.syntax divided\n")
        p = os.path.join(d, name + ".s")
        if not os.path.exists(p) or open(p).read() != text:
            open(p, "w").write(text)


def c_replacement_end(code, start, end, covered_addresses=(), covered_ranges=(), includes_asm=False):
    """First retained trailing-data line; C replaces instructions/literal pools."""
    if includes_asm:
        # write_nonmatching already includes the complete original block.
        # Retaining its incbin tail separately would emit those bytes twice.
        return end
    for k in range(start, end):
        if code[k].lstrip().startswith(".incbin"):
            # A verified C unit may also define functions that gbadisasm left
            # in binary blobs after the last recognized entry point. Consume
            # those explicitly covered blobs; retain unrelated trailing data.
            blob = re.match(r'\s*\.incbin "baserom\.gba",\s*(0x[0-9a-fA-F]+),\s*(0x[0-9a-fA-F]+)', code[k])
            if blob:
                address = 0x08000000 + int(blob.group(1), 16)
                following = address + int(blob.group(2), 16)
                if any(lo <= address and following <= hi for lo, hi in covered_ranges):
                    continue  # reviewed exact body split around symbolized literals
            if blob and 0x08000000 + int(blob.group(1), 16) in covered_addresses:
                continue
            # Data with the function's own instructions after it (a jump table
            # gbadisasm left as a blob) is interior: the C body emits it too.
            if blob and any(INSN.match(line) for line in code[k + 1:end]):
                continue
            # U33J's last instruction ends at 0824DAFA. The next two bytes
            # are zero alignment before data at 0824DAFC, already emitted
            # by build_c's final .align 2,0. Keeping the terminal padding
            # blob as well shifts every data address by two bytes.
            if (blob and (int(blob.group(1), 16), int(blob.group(2), 16)) == (0x24DAFA, 2)
                    and not any(line.strip() for line in code[k + 1:end])):
                continue
            end = k
            while end > start and re.match(r"^\w+:\s*$", code[end - 1]):
                end -= 1
            break
    return end


def apply_extra_boundaries(code, path):
    """Split only reviewed incbin entries in memory; never rewrite shared gen/."""
    if not os.path.exists(path):
        return code
    with open(path, newline="") as source:
        rows = list(csv.DictReader(source))
    entries = {}
    for row in rows:
        address = int(row["address"], 16)
        isa = row["isa"]
        if isa not in ("arm", "thumb") or address % (4 if isa == "arm" else 2):
            raise ValueError("invalid reviewed function ISA/alignment")
        if address in entries:
            raise ValueError("duplicate reviewed function boundary")
        entries[address] = isa
    found = set()
    result = []
    for line in code:
        blob = re.fullmatch(r'\s*\.incbin "baserom\.gba",\s*(0x[0-9a-fA-F]+),\s*(0x[0-9a-fA-F]+)', line)
        if not blob:
            result.append(line)
            continue
        start = 0x08000000 + int(blob.group(1), 16)
        end = start + int(blob.group(2), 16)
        cuts = sorted(address for address in entries if start <= address < end)
        if not cuts:
            result.append(line)
            continue
        cursor = start
        for address, following in zip(cuts, cuts[1:] + [end]):
            if cursor < address:
                result.append(f'\t.incbin "baserom.gba", {cursor - 0x08000000:#x}, {address - cursor:#x}')
            name = f"sub_{address:08X}"
            result += [f"\t{entries[address]}_func_start {name}",
                       f"{name}: @ 0x{address:08X}",
                       f'\t.incbin "baserom.gba", {address - 0x08000000:#x}, {following - address:#x}']
            found.add(address)
            cursor = following
    if found != set(entries):
        raise ValueError("reviewed function boundary is not inside an incbin: " +
                         ", ".join(f"{a:08X}" for a in sorted(set(entries) - found)))
    return result


def main():
    code = open(os.path.join(ROOT, "gen", "code_sym.s")).read().split("\n")
    code = remove_nonentry_starts(code, function_nonentries())
    boundary_path = os.path.join(ROOT, "symbols", "build_extra_functions.csv")
    code = apply_extra_boundaries(code, boundary_path)
    with open(boundary_path, newline="") as source:
        replacement_ranges = {}
        for row in csv.DictReader(source):
            if row.get("replacement_end"):
                lo, hi = int(row["address"], 16), int(row["replacement_end"], 16)
                if hi <= lo or hi % 2:
                    raise ValueError("invalid reviewed replacement endpoint")
                replacement_ranges[lo] = hi
    # function blocks: [start_line, end_line) per function, in order
    starts = [i for i, ln in enumerate(code) if FUNC_START.match(ln)]
    funcs = []
    for k, i in enumerate(starts):
        name = FUNC_START.match(code[i]).group(1)
        j = starts[k + 1] if k + 1 < len(starts) else len(code)
        addr = None
        m = FUNC_LABEL.match(code[i + 1]) if i + 1 < len(code) else None
        if m:
            addr = int(m.group(2), 16)
        nonword = "non_word_aligned" in code[i]
        funcs.append((name, addr, i, j, nonword))
    index = {f[0]: n for n, f in enumerate(funcs)}

    write_nonmatching(code, funcs)
    with open(os.path.join(ROOT, "symbols", "functions.csv"), newline="") as source:
        csv_addr = {r[1]: r[0].upper() for r in csv.reader(source)
                    if len(r) > 1 and re.fullmatch(r"[0-9A-Fa-f]{8}", r[0])}
    csv_name = {a: n for n, a in csv_addr.items()}
    units = []  # (first_func_idx, last_func_idx, path)
    for path in sorted(glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True)):
        rel = os.path.relpath(path, ROOT)
        names = c_functions(path)
        if not names:
            sys.exit(f"{rel}: no function definitions found (a file the build would silently ignore)")
        idx = []
        for n in names:
            if n not in index and f"sub_{csv_addr.get(n, '')}" in index:
                n = f"sub_{csv_addr[n]}"  # name promoted on main after gen/code_sym.s was built
            if n not in index and csv_name.get(n[4:]) in index:
                n = csv_name[n[4:]]  # "#define Name sub_ADDR" for a function code_sym.s labels by its csv name
            if n not in index and re.fullmatch(r"sub_[0-9A-F]{8}", n):
                # "hidden" function: code the disassembler did not split off
                # (e.g. inside the previous function's trailing data). It is
                # replaced along with the block it sits in; it only needs to
                # lie between this unit's first and last known function.
                continue
            if n not in index:
                sys.exit(f"{rel}: function {n} is not a known function start")
            idx.append(index[n])
        if not idx:
            sys.exit(f"{rel}: none of {names} is a known function start "
                     "(a #define alias must map to a sub_XXXXXXXX label)")
        if idx != sorted(idx) or idx != list(range(idx[0], idx[0] + len(idx))):
            sys.exit(f"{rel}: functions must be contiguous and in ROM order "
                     f"(got {[funcs[i][0] for i in idx]})")
        if funcs[idx[0]][4]:
            sys.exit(f"{rel}: unit cannot start at a non-word-aligned function")
        units.append((idx[0], idx[-1], rel))
    units.sort()
    for a, b in zip(units, units[1:]):
        if a[1] >= b[0]:
            sys.exit(f"{a[2]} and {b[2]} overlap")

    header = ['\t.include "asm/macros.inc"', "\t.syntax unified", "\t.text"]
    out_units = []
    seg_no = 0
    line = 0

    def emit_seg(lo, hi):
        nonlocal seg_no
        body = code[lo:hi]
        if not any(l.strip() for l in body):
            return
        globs = set()
        for ln in body:
            m = LABEL.match(ln) or SET_LABEL.match(ln)
            if m:
                globs.add(m.group(1))
        path = os.path.join(ROOT, "build", "asm", f"seg_{seg_no:03d}.s")
        with open(path, "w") as f:
            f.write("\n".join(header) + "\n")
            f.write("".join(f"\t.global {g}\n" for g in sorted(globs)))
            f.write("\n".join(body) + "\n")
        out_units.append(("asm", os.path.relpath(path, ROOT)))
        seg_no += 1

    aliases = []
    addr_name = re.compile(r"(?:^|_)([0-9A-F]{8})(?:__addr)?$")
    for first, last, rel in units:
        emit_seg(line, funcs[first][2])
        out_units.append(("c", rel))
        # The C object replaces only the last function's instructions (and its
        # literal pools); trailing undisassembled data in that block stays asm.
        exact_names = c_functions(os.path.join(ROOT, rel), with_asm=False)
        # A worker may retain its address-style definition after the CSV name
        # was promoted. Keep references from the generated assembly linkable,
        # including the even-address labels used for interior offsets.
        generated_exact_names = set()
        for name in exact_names:
            generated = name
            if name not in index and f"sub_{csv_addr.get(name, '')}" in index:
                generated = f"sub_{csv_addr[name]}"
            elif name not in index and csv_name.get(name[4:]) in index:
                generated = csv_name[name[4:]]
            generated_exact_names.add(generated)
            if generated != name:
                aliases.append(f"{generated} = {name};")
                aliases.append(f"{generated}__addr = {name}__addr;")
        covered = {int(n[4:], 16) for n in generated_exact_names
                   if re.fullmatch(r"sub_[0-9A-F]{8}", n)}
        ranges = [(a, replacement_ranges[a]) for a in covered if a in replacement_ranges]
        end = c_replacement_end(code, funcs[last][2], funcs[last][3], covered, ranges,
                                includes_asm=funcs[last][0] not in generated_exact_names)
        # Labels that disappear with the asm are redefined relative to the
        # unit's first function, so references from elsewhere still resolve.
        base_name, base_addr = funcs[first][0], funcs[first][1]
        c_names = {funcs[i][0] for i in range(first, last + 1)}
        for k in range(funcs[first][2], end):
            m = LABEL.match(code[k]) or SET_LABEL.match(code[k])
            if not m or m.group(1) in c_names:
                continue
            am = addr_name.search(m.group(1))
            if am:
                # based on the plain (non-Thumb) alias label build.py emits in the C object
                aliases.append(f"{m.group(1)} = {base_name}__addr + {int(am.group(1), 16) - base_addr:#x};")
        line = end
    emit_seg(line, len(code))
    open(os.path.join(ROOT, "build", "c_labels.ld"), "w").write("\n".join(aliases) + "\n")

    with open(os.path.join(ROOT, "build", "units.txt"), "w") as f:
        for kind, p in out_units:
            f.write(f"{kind} {p}\n")
    ndec = sum(b - a + 1 for a, b, _ in units)
    print(f"{len(units)} C units ({ndec} functions), {seg_no} asm segments", file=sys.stderr)


if __name__ == "__main__":
    main()

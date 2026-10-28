#!/usr/bin/env python3
"""Decompilation progress: functions/bytes in src/*.c vs all code.

  progress.py            human-readable summary
  progress.py --json F   objdiff v2 report for decomp.dev
  progress.py --markdown F   PROGRESS.md tracker (per-region bars), plus F with
                             a .json suffix: category -> unit -> function tree
                             with sizes, status and source files (read by the
                             tools/claude-mod popup)
  progress.py --list-pinned  TSV of matched functions that use register pins or
                             asm hints (the Register-pinned category), for cleanup
"""
import argparse
import csv
import json
from pathlib import Path
import re
import subprocess
import sys

from romlib import ROOT
from split import c_functions, nonmatching_functions, split_nonmatching

CODE_END = 0x0824DAFA
INVENTORY = Path(ROOT) / "symbols/progress_functions.csv"
LIBRARIES = Path(ROOT) / "symbols/progress_libraries.csv"
SUBSYSTEMS = Path(ROOT) / "symbols/progress_subsystems.csv"
AREAS = Path(ROOT) / "symbols/progress_areas.csv"
CATEGORIES = Path(ROOT) / "symbols/progress_categories.csv"
ASM = Path(ROOT) / "symbols/progress_asm.csv"
ASM_SUFFIX = " (assembly)"


def load_libraries(funcs, sizes, path=LIBRARIES):
    return load_groups(funcs, sizes, path, "libraries")


def load_groups(funcs, sizes, path, prefix):
    """Reviewed contiguous report spans and evidence-backed display names.

    These presentation groups do not change build symbols or C credit.
    Bounds use the existing progress spans, including their alignment/tails.
    """
    boundaries = {a for _, a in funcs} | {CODE_END}
    known = {a for _, a in funcs}
    libraries = []
    with Path(path).open(newline="") as f:
        for row in csv.DictReader(f):
            start, end = int(row["start"], 16), int(row["end"], 16)
            if start not in boundaries or end not in boundaries or start >= end:
                raise ValueError(f"{row['name']}: invalid group boundaries")
            if libraries and start < libraries[-1]["end"]:
                raise ValueError("group ranges must be sorted and nonoverlapping")
            members = [(n, a) for n, a in funcs if start <= a < end]
            if sum(sizes[n] for n, _ in members) != end - start:
                raise ValueError(f"{row['name']}: group span is not contiguous")
            names = {}
            for relative in filter(None, row["name_files"].split(";")):
                with (Path(path).parent.parent / relative).open(newline="") as nf:
                    for label in csv.DictReader(nf):
                        a = int(label["addr"], 16)
                        if not start <= a < end:
                            continue  # Some reviewed SDK maps also contain RAM/data.
                        if a not in known or not label["evidence"]:
                            raise ValueError(f"{relative}: unreviewed group label {a:08X}")
                        if a in names and names[a] != label["name"]:
                            raise ValueError(f"conflicting display names at {a:08X}")
                        names[a] = label["name"]
            if not row["evidence"]:
                raise ValueError(f"{row['name']}: missing group evidence")
            libraries.append({"name": row["name"], "start": start, "end": end,
                              "names": names, "prefix": prefix, "category": row.get("category") or ""})
    return libraries


def load_areas(path=AREAS):
    """Reviewed report areas as {id: (label, [(start, end, regex)])}. A unit
    belongs to an area when its first address is in a range or its name matches."""
    areas = {}
    with Path(path).open(newline="") as f:
        for row in csv.DictReader(f):
            if not row["evidence"]:
                raise ValueError(f"{row['id']}: missing area evidence")
            rng = (int(row["start"], 16), int(row["end"], 16)) if row["start"] else None
            rx = re.compile(row["name_regex"]) if row["name_regex"] else None
            areas.setdefault(row["id"], (row["name"], []))[1].append((rng, rx))
    return areas


def load_categories(path=CATEGORIES):
    """Top-level subsystems, in display order, as {id: (label, [(start, end, regex)])}.
    A game unit takes the category of its progress_subsystems.csv row, else the
    first rule (in file order) whose name regex matches, within the rule's address
    range if it has one, else the first address-only rule containing its start."""
    with Path(path).open(newline="") as f:
        rows = list(csv.DictReader(f))
    categories = {}
    for row in rows:
        rng = (int(row["start"], 16), int(row["end"], 16)) if row["start"] else None
        rx = re.compile(row["name_regex"]) if row["name_regex"] else None
        categories.setdefault(row["id"], (row["name"], []))[1].append((rng, rx))
    return categories


def category_of(categories, group):
    if group.get("category"):
        if group["category"] not in categories:
            raise ValueError(f"{group['name']}: unknown category {group['category']}")
        return group["category"]
    # Name rules first, in file order, then address-only rules.
    for named in (True, False):
        for cid, (_, rules) in categories.items():
            for rng, rx in rules:
                if bool(rx) == named and (not rx or rx.search(group["name"])) and (
                        not rng or rng[0] <= group["start"] < rng[1]):
                    return cid
    raise ValueError(f"{group['name']}: no category rule matches")


def load_asm_ranges(path=ASM):
    """Reviewed half-open ranges of library code that was hand-written assembly
    in the original SDK. Unmatched functions in them are reported as intentional
    assembly rather than as missing decompilation work."""
    ranges = []
    with Path(path).open(newline="") as f:
        for row in csv.DictReader(f):
            if not row["evidence"]:
                raise ValueError(f"{row['name']}: missing assembly evidence")
            start, end = int(row["start"], 16), int(row["end"], 16)
            if start >= end:
                raise ValueError(f"{row['name']}: invalid assembly range")
            ranges.append((start, end))
    return ranges


def intentional_asm(funcs, done, asm_ranges):
    """Unmatched functions inside the reviewed assembly ranges. They stay
    assembly by design, so headline totals leave them out; the Intentional
    assembly category reports them on their own."""
    return {n for n, a in funcs if n not in done and any(lo <= a < hi for lo, hi in asm_ranges)}


def unit_areas(areas, unit_name, address):
    base = unit_name.split("/", 1)[-1].replace(" (auto, tentative)", "").replace(" (tentative)", "")
    ids = []
    for aid, (_, rules) in areas.items():
        if any((rng and rng[0] <= address < rng[1]) or (rx and rx.search(base)) for rng, rx in rules):
            ids.append(aid)
    if unit_name.endswith(ASM_SUFFIX):
        if "intentional_asm" not in ids:
            ids.append("intentional_asm")
    elif unit_name.lower().startswith("libraries/"):
        ids.append("libraries")
    return ids


def write_inventory(path, addresses):
    validate_addresses(addresses)
    with Path(path).open("w", newline="") as f:
        writer = csv.writer(f, lineterminator="\n")
        writer.writerow(["address"])
        writer.writerows([f"{a:08X}"] for a in addresses)


def validate_addresses(addresses):
    if not addresses or addresses != sorted(set(addresses)):
        raise ValueError("function addresses must be nonempty, unique and sorted")
    if addresses[0] < 0x08000000 or addresses[-1] >= CODE_END:
        raise ValueError("function addresses outside code range")
    if any(a & 1 for a in addresses):
        raise ValueError("function addresses must be halfword aligned")


def load_functions(inventory=INVENTORY, symbols=None, extra=()):
    with Path(inventory).open(newline="") as f:
        addresses = [int(row["address"], 16) for row in csv.DictReader(f)]
    validate_addresses(addresses)
    if extra:
        addresses = sorted(set(addresses) | set(extra))
        validate_addresses(addresses)
    symbols = symbols or Path(ROOT) / "symbols/functions.csv"
    with Path(symbols).open(newline="") as f:
        names = {int(row["addr"], 16): row["name"] for row in csv.DictReader(f)}
    funcs = [(names.get(a, f"sub_{a:08X}"), a) for a in addresses]
    if len({n for n, _ in funcs}) != len(funcs):
        raise ValueError("duplicate function names in symbol map")
    sizes = {n: b - a for (n, a), b in zip(funcs, addresses[1:] + [CODE_END])}
    return funcs, sizes


def source_boundaries(inventory, source_root, symbols=None):
    """Addresses of C definitions missing from the inventory, e.g. a tail that
    was split off an assembly span and matched as its own function. Their
    address comes from functions.csv or a sub_XXXXXXXX name; names with no
    known address are left for collect_done to reject."""
    with Path(inventory).open(newline="") as f:
        known = {int(row["address"], 16) for row in csv.DictReader(f)}
    symbols = symbols or Path(ROOT) / "symbols/functions.csv"
    with Path(symbols).open(newline="") as f:
        addrs = {row["name"]: int(row["addr"], 16) for row in csv.DictReader(f)}
    missing = set()
    for p in sorted(Path(source_root).rglob("*.c")):
        for n in c_functions(p, with_asm=False):
            m = re.fullmatch(r"sub_([0-9A-F]{8})", n)
            a = int(m.group(1), 16) if m else addrs.get(n)
            if a is not None and a not in known and 0x08000000 <= a < CODE_END and not a & 1:
                missing.add(a)
    return sorted(missing)


def collect_done(funcs, source_root):
    # Resolve address aliases even if a named symbol is defined as sub_XXXXXXXX.
    known = {n: n for n, _ in funcs}
    known.update({f"sub_{a:08X}": n for n, a in funcs})
    done = set()
    for p in sorted(Path(source_root).rglob("*.c")):
        for n in c_functions(p, with_asm=False):
            if n not in known:
                raise ValueError(f"{p}: {n} has no progress boundary; add it with tools/add_function.py")
            done.add(known[n])
    return done


def uncovered_tails(funcs, sizes, done, code_sizes, slack=3):
    """Matched functions whose compiled code is shorter than the span progress
    credits them: the remainder is assembly hidden after the C (a function
    gbadisasm left as raw bytes) and must get its own boundary. code_sizes
    maps a start address to its linked symbol size; slack allows alignment
    padding. Returns [(name, address, credited, compiled)]."""
    return [(n, a, sizes[n], code_sizes[a]) for n, a in funcs
            if n in done and a in code_sizes and sizes[n] - code_sizes[a] > slack]


def elf_code_sizes(elf):
    out = subprocess.run(["arm-none-eabi-nm", "-S", str(elf)], capture_output=True, text=True,
                         check=True).stdout
    sizes = {}
    for ln in out.splitlines():
        p = ln.split()
        if len(p) == 4 and p[2] in "Tt":
            a = int(p[0], 16) & ~1
            sizes[a] = max(sizes.get(a, 0), int(p[1], 16))
    return sizes


def check_tails(elf, source_root=Path(ROOT) / "src"):
    """Messages for matched functions that would credit hidden assembly."""
    split = source_boundaries(INVENTORY, source_root)
    funcs, sizes = load_functions(INVENTORY, extra=split)
    done = collect_done(funcs, source_root)
    return [f"{n} ({a:08X}) is credited {span} bytes but compiles to {size}; register the tail at "
            f"{a + size:08X} with tools/add_function.py {a + size:08X}"
            for n, a, span, size in uncovered_tails(funcs, sizes, done, elf_code_sizes(elf))]


MARK = "// MATCHING_CONCESSION: register-pinning"
HINT = re.compile(r'\bregister\b[^;]*\basm\s*\(\s*"\w+"\s*\)|\basm\s*(?:volatile\s*)?\(\s*""'
                  r'|\basm\s*\(\s*"(?:r\d+|sl|fp|ip|sb|lr)"\s*\)')


def _bodies(text):
    """{name: (is_static, item)} for the function definitions and
    function-like macros of a C file or header."""
    from merge_unit import items
    bodies = {}
    for item in items(text):
        m = re.match(r"#\s*define\s+(\w+)\(", item)
        if m:
            bodies[m.group(1)] = (True, item)
            continue
        code = re.sub(r"^(\s*(//[^\n]*|/\*.*?\*/)\s*)+", "", item, flags=re.S)  # leading comments
        m = re.match(r"(static\s+)?[A-Za-z_][\w\s\*]*?\b(\w+)\s*\([^;{]*\)\s*\{", code, re.S)
        if m:
            bodies[m.group(2)] = (bool(m.group(1)), item)
    # "#define Name sub_XXXXXXXX" aliases, as split.c_functions reports them
    for name, label in re.findall(r"^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$", text, re.M):
        if name in bodies:
            bodies.setdefault(label, bodies[name])
    return bodies


def _hinted_helpers(bodies, known=frozenset()):
    """Static helpers/macros that use hints, directly or via another such helper."""
    helpers = set(known)
    while True:
        uses = re.compile(r"\b(" + "|".join(map(re.escape, helpers)) + r")\s*\(") if helpers else None
        more = {n for n, (static, body) in bodies.items() if static and n not in helpers and
                (HINT.search(body) or (uses and uses.search(body)))}
        if not more:
            return helpers
        helpers |= more


def header_helpers(roots=("include", "src")):
    """Hinted static inline helpers and macros defined in headers."""
    bodies = {}
    for root in roots:
        for path in sorted((Path(ROOT) / root).rglob("*.h")):
            bodies.update(_bodies(path.read_text()))
    return _hinted_helpers(bodies)


def hinted_functions(text, names, shared=frozenset()):
    """Functions of a unit that use register pins or asm barriers, directly or
    through a static helper (in the unit or a header) that does."""
    bodies = _bodies(text)
    helpers = _hinted_helpers(bodies, shared)
    uses = re.compile(r"\b(" + "|".join(map(re.escape, helpers)) + r")\s*\(") if helpers else None
    return [n for n in names if n in bodies and (HINT.search(bodies[n][1]) or
                                               (uses and uses.search(bodies[n][1])))]


HINT_KINDS = [
    ("pin", re.compile(r'\bregister\b[^;]*\basm\s*\(\s*"\w+"\s*\)|\basm\s*\(\s*"(?:r\d+|sl|fp|ip|sb|lr)"\s*\)')),
    ("clobber", re.compile(r'\basm\s*(?:volatile\s*)?\(\s*""[^;]*:\s*"(?:r\d+|sl|fp|ip|lr)"')),
    ("operand", re.compile(r'\basm\s*(?:volatile\s*)?\(\s*""\s*:[^;:]*\(')),
    ("barrier", re.compile(r'\basm\s*(?:volatile\s*)?\(\s*""\s*(?:\)|:\s*:\s*:\s*"memory")')),
]


def hint_kinds(text):
    return [k for k, r in HINT_KINDS if r.search(text)]


def collect_pinned(funcs, source_root):
    """Matched functions that rely on register pins or empty asm() hints
    (barriers, "+r"/"r" operands, "rN"/"memory" clobbers), directly or through
    a static helper or macro in the unit or a header. Detected from the source
    itself, outside NONMATCHING drafts, so a missing MATCHING_CONCESSION marker
    does not hide one."""
    aliases = {f"sub_{a:08X}": n for n, a in funcs}
    shared = header_helpers()
    pinned = set()
    for path in Path(source_root).rglob("*.c"):
        text, _ = split_nonmatching(path.read_text())
        names = c_functions(path, with_asm=False)
        pinned.update(aliases.get(n, n) for n in hinted_functions(text, names, shared))
    return pinned


def list_pinned(funcs, sizes, pinned, source_root):
    """name, address, bytes, hint kinds and file of each pinned function. Kinds:
    pin = register asm("rN"), operand = asm("" : "+r"(x)) etc., clobber =
    asm("" ::: "rN"), barrier = asm("") / "memory", helper = only through a
    hinted static helper or macro."""
    addr = dict(funcs)
    aliases = {f"sub_{a:08X}": n for n, a in funcs}
    shared = header_helpers()
    rows = []
    for path in sorted(Path(source_root).rglob("*.c")):
        text, _ = split_nonmatching(path.read_text())
        bodies = _bodies(text)
        for n in c_functions(path, with_asm=False):
            name = aliases.get(n, n)
            if name in pinned:
                kinds = hint_kinds(bodies[n][1]) if n in bodies else []
                rows.append((sizes[name], name, addr[name], ",".join(kinds) or "helper",
                             path.relative_to(ROOT)))
    print("name\taddress\tbytes\tkinds\tfile")
    for size, name, a, kinds, path in sorted(rows):
        print(f"{name}\t{a:08X}\t{size}\t{kinds}\t{path}")


def measures(total_b, done_b, count, done_count, *, units=None, complete_units=None, fuzzy_b=None):
    return {
        "fuzzy_match_percent": 100 * (done_b if fuzzy_b is None else fuzzy_b) / total_b,
        "total_code": str(total_b), "matched_code": str(done_b),
        "matched_code_percent": 100 * done_b / total_b,
        "total_functions": count, "matched_functions": done_count,
        "total_units": count if units is None else units,
        "complete_units": done_count if complete_units is None else complete_units,
        "matched_functions_percent": 100 * done_count / count,
        "complete_code": str(done_b), "complete_code_percent": 100 * done_b / total_b,
    }


def data_measures():
    """RAM objects from symbols/ram.csv (docs/RAM.md): total = inventoried
    bytes, matched = declared once in a shared header, complete = no file
    declares them any other way. No ROM needed."""
    import ramlayout
    m = ramlayout.measures()
    total = m["total_data"]
    pct = lambda v: 100 * v / total if total else 0.0
    return {"total_data": str(total), "matched_data": str(m["matched_data"]),
            "matched_data_percent": pct(m["matched_data"]), "complete_data": str(m["complete_data"]),
            "complete_data_percent": pct(m["complete_data"])}


def data_fields(total, matched):
    pct = 100 * matched / total if total else 0.0
    return {"total_data": str(total), "matched_data": str(matched), "matched_data_percent": pct,
            "complete_data": str(matched), "complete_data_percent": pct}


def rom_data_units(units=None):
    """objdiff units for ROM data (tools/romdata.py, docs/DATA_TRACKING.md): one per
    symbols/rom_data.csv row, named "ROM data: KIND/UNIT", each in exactly one
    category rom_KIND, with data measures only and one .rodata section item."""
    import romdata
    units = romdata.measure() if units is None else units
    out = []
    for u in units:
        fields = data_fields(u["size"], u["matched"])
        complete = u["matched"] == u["size"]
        out.append({"name": f"{romdata.LABEL_PREFIX}{u['label']}/{u['unit']}",
                    "measures": {"fuzzy_match_percent": fields["matched_data_percent"],
                                 "total_code": "0", "matched_code": "0", "matched_code_percent": 0.0,
                                 **fields, "total_functions": 0, "matched_functions": 0,
                                 "matched_functions_percent": 0.0, "complete_code": "0",
                                 "complete_code_percent": 0.0, "total_units": 1, "complete_units": int(complete)},
                    "sections": [{"name": ".rodata", "size": str(u["size"]),
                                  "fuzzy_match_percent": fields["matched_data_percent"],
                                  "metadata": {"virtual_address": str(u["start"])}}],
                    "functions": [],
                    "metadata": {"complete": complete,
                                 "progress_categories": [romdata.CATEGORY_PREFIX + u["kind"]]}})
    return out


def add_rom_data(report, units=None):
    """Headline data measures = ROM data only (RAM is tracked in PROGRESS.md,
    not reported). One data-only category per kind; the headline unit counts
    stay those of the code."""
    import romdata
    data_units = rom_data_units(units)
    report["units"] += data_units
    for kind, label in romdata.KINDS:
        cid = romdata.CATEGORY_PREFIX + kind
        selected = [u for u in data_units if u["metadata"]["progress_categories"] == [cid]]
        if not selected:
            continue
        m = {key: selected[0]["measures"][key] for key in selected[0]["measures"]}
        total = sum(int(u["measures"]["total_data"]) for u in selected)
        matched = sum(int(u["measures"]["matched_data"]) for u in selected)
        m.update(data_fields(total, matched), fuzzy_match_percent=100 * matched / total,
                 total_units=len(selected), complete_units=sum(u["metadata"]["complete"] for u in selected))
        report["categories"].append({"id": cid, "name": romdata.LABEL_PREFIX + label, "measures": m})
    total = sum(int(u["measures"]["total_data"]) for u in data_units)
    matched = sum(int(u["measures"]["matched_data"]) for u in data_units)
    report["measures"].update(data_fields(total, matched))
    return report


def rom_data_lines(units=None):
    """PROGRESS.md: ROM data matched per kind (scripts apart), identified, LZ77 recompression."""
    import romdata
    units = romdata.measure() if units is None else units
    total = sum(u["size"] for u in units)
    matched = sum(u["matched"] for u in units)
    data = [u for u in units if u["kind"] != "scripts"]
    dt, dm, di = (sum(u[k] for u in data) for k in ("size", "matched", "identified"))
    lz_e, lz_c = sum(u["lz_exact"] for u in units), sum(u["lz_content"] for u in units)
    lines = ["## ROM data", "",
             f"**ROM data matched: {100 * matched / total:.2f}%** ({matched:,} / {total:,} bytes, scripts "
             f"included; decomp.dev's data measure). A byte is matched when a project tool decodes it to an "
             f"editable form and re-encodes it byte-exact, verified by `tools/data_roundtrip.py` "
             f"(`symbols/data_roundtrip.csv`; [docs/DATA_TRACKING.md](docs/DATA_TRACKING.md)).", "",
             f"Data without scripts: **{100 * dm / dt:.2f}%** matched ({dm:,} / {dt:,} bytes); format identified "
             f"(a decoder exists, not a match): {100 * di / dt:.1f}%. LZ77 blocks matched by content: "
             f"{lz_e + lz_c:,} bytes, of which tools/lz77.py recompresses {lz_e:,} byte-exact "
             f"({100 * lz_e / max(lz_e + lz_c, 1):.1f}%). Named data symbols: {romdata.named_symbols()}.", "",
             "| ROM data | Bytes | Matched | Identified | |", "|---|---:|---:|---:|---|"]
    for kind, label, size, m, i, _ in romdata.by_kind(units):
        if kind != "scripts":
            lines.append(f"| {label} | {size:,} | {100 * m / size:.1f}% | {100 * i / size:.1f}% | `{bar(m / size, 20)}` |")
    sc = [u for u in units if u["kind"] == "scripts"]
    st, sm, sd = (sum(u[k] for u in sc) for k in ("size", "matched", "decompiled"))
    lines += ["", "## Scripts (GCL)", "",
              f"The event scripts, tracked like code but apart from the game's C (on decomp.dev they are the "
              f"`ROM data: Scripts (GCL)` category): {st:,} bytes of GCL bytecode and its tables. "
              f"**Matched (decompiles, recompiles byte-exact and is named): {100 * sm / st:.1f}%** ({sm:,} bytes); "
              f"round-trips but unnamed: {100 * sd / st:.1f}% ({sd:,} bytes). Per procedure, from "
              f"`gcl/progress.csv`; no GCL compiler exists yet."]
    return lines


def ram_lines(m):
    if not int(m.get("total_data", 0)):
        return []
    return [f"RAM objects with one shared declaration (`symbols/ram.csv`, tracked here only): "
            f"**{m['matched_data_percent']:.1f}%** ({int(m['matched_data']):,} / {int(m['total_data']):,} bytes); "
            f"no other declaration left: {m['complete_data_percent']:.1f}%.", ""]


def collect_fuzzy(funcs, source_root):
    """{name: fuzzy percent} for NONMATCHING drafts (recorded by check.py
    --nonmatching --record; unrecorded drafts count as 0)."""
    known = {n: n for n, _ in funcs}
    known.update({f"sub_{a:08X}": n for n, a in funcs})
    fuzzy = {}
    for p in sorted(Path(source_root).rglob("*.c")):
        for n, pct in nonmatching_functions(p):
            if n in known:
                fuzzy[known[n]] = min(pct or 0.0, 99.9)
    return fuzzy


def make_report(funcs, sizes, done, libraries=(), pinned=frozenset(), subsystems=(), fuzzy=None, areas=None,
                asm_ranges=(), categories=None):
    """objdiff v2 report. With categories, every unit is named Category/Unit, belongs
    to one top-level category, and units are listed category by category, so
    decomp.dev's treemap shows each category as one region."""
    fuzzy = fuzzy or {}
    areas = areas or {}
    score = lambda n: 100 if n in done else fuzzy.get(n, 0)
    asm = intentional_asm(funcs, done, asm_ranges)
    counted = [(n, a) for n, a in funcs if n not in asm]
    libraries = sorted([*libraries, *subsystems], key=lambda group: group["start"])
    for left, right in zip(libraries, libraries[1:]):
        if left["end"] > right["start"]:
            raise ValueError("report groups overlap")
    order = {cid: k for k, cid in enumerate(categories or ())}
    units = []
    grouped = {}
    for n, a in funcs:
        library = next((lib for lib in libraries if lib["start"] <= a < lib["end"]), None)
        top = None
        assembly = library and n in asm
        if categories:
            prefix = library.get("prefix", "libraries") if library else None
            # Assembly units sit with the libraries: the boot crt0 is SDK code too.
            top = "libraries" if prefix == "libraries" or assembly else category_of(
                categories, library or {"name": f"{a:08X}", "start": a})
            unit_name = f"{categories[top][0]}/{library['name'] if library else f'{a:08X}'}"
        else:
            unit_name = f"{library.get('prefix', 'libraries')}/{library['name']}" if library else f"functions/{a:08X}"
        if assembly:
            unit_name += ASM_SUFFIX
        if unit_name not in grouped:
            unit = {"name": unit_name, "functions": [], "metadata": {"progress_categories": []},
                    "_first": a, "_top": top}
            units.append(unit)
            grouped[unit_name] = unit
        unit = grouped[unit_name]
        category = "register_pinned" if n in pinned else "no_register_pinning"
        if category not in unit["metadata"]["progress_categories"]:
            unit["metadata"]["progress_categories"].append(category)
        unit["functions"].append({
            "name": library["names"].get(a, n) if library else n,
            "size": str(sizes[n]), "fuzzy_match_percent": score(n),
            "address": str(a - (library["start"] if library else a)),
            "metadata": {"virtual_address": str(a)}})
    for unit in units:
        cats = unit["metadata"]["progress_categories"]
        top = unit.pop("_top")
        if top and not unit["name"].endswith(ASM_SUFFIX):
            cats.append(top)
        cats += [c for c in unit_areas(areas, unit["name"], unit["_first"]) if c not in cats]
        if "intentional_asm" in areas and "intentional_asm" not in cats:
            cats.append("decompilable")
        items = unit["functions"]
        total_b = sum(int(item["size"]) for item in items)
        matched_items = [item for item in items if item["fuzzy_match_percent"] == 100]
        done_b = sum(int(item["size"]) for item in matched_items)
        complete = len(matched_items) == len(items)
        fuzzy_b = sum(int(item["size"]) * item["fuzzy_match_percent"] / 100 for item in items)
        unit["measures"] = measures(total_b, done_b, len(items), len(matched_items),
                                   units=1, complete_units=int(complete), fuzzy_b=fuzzy_b)
        unit["metadata"]["complete"] = complete
    if categories:
        units.sort(key=lambda u: (min((order[c] for c in u["metadata"]["progress_categories"] if c in order),
                                      default=len(order)), u["_first"]))
    for unit in units:
        unit.pop("_first")
    categories_out = []
    for cid, label in [("no_register_pinning", "No register pinning"), ("register_pinned", "Register-pinned")]:
        members = [n for n, _ in counted if (n in pinned) == (cid == "register_pinned")]
        if not members:
            continue
        matched = [n for n in members if n in done]
        selected = [u for u in units if cid in u["metadata"]["progress_categories"]]
        categories_out.append({"id": cid, "name": label, "measures": measures(
            sum(sizes[n] for n in members), sum(sizes[n] for n in matched),
            len(members), len(matched), units=len(selected),
            complete_units=sum(u["metadata"]["complete"] for u in selected),
            fuzzy_b=sum(sizes[n] * score(n) / 100 for n in members))})
    labels = {**{c: l for c, (l, _) in (categories or {}).items()},
              **{a: l for a, (l, _) in areas.items() if a not in (categories or {})}}
    labels.setdefault("libraries", "Libraries")
    labels["decompilable"] = "Decompilable code (excludes intentional assembly)"
    for cid, label in labels.items():
        selected = [u for u in units if cid in u["metadata"]["progress_categories"]]
        if not selected:
            continue
        total_b = sum(int(u["measures"]["total_code"]) for u in selected)
        done_b = sum(int(u["measures"]["matched_code"]) for u in selected)
        count = sum(u["measures"]["total_functions"] for u in selected)
        categories_out.append({"id": cid, "name": label, "measures": measures(
            total_b, done_b, count, sum(u["measures"]["matched_functions"] for u in selected),
            units=len(selected), complete_units=sum(u["metadata"]["complete"] for u in selected),
            fuzzy_b=sum(u["measures"]["fuzzy_match_percent"] * int(u["measures"]["total_code"]) / 100 for u in selected))})
    counted_units = [u for u in units if not u["name"].endswith(ASM_SUFFIX)]
    return {"version": 2, "categories": categories_out, "measures": measures(sum(sizes[n] for n, _ in counted),
            sum(sizes[n] for n in done), len(counted), len(done), units=len(counted_units),
            complete_units=sum(u["metadata"]["complete"] for u in counted_units),
            fuzzy_b=sum(sizes[n] * score(n) / 100 for n, _ in counted)), "units": units}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--json", metavar="FILE")
    parser.add_argument("--markdown", metavar="FILE")
    parser.add_argument("--list-pinned", action="store_true",
                        help="print matched functions that use pins/asm hints, smallest first")
    parser.add_argument("--inventory", type=Path, default=INVENTORY)
    args = parser.parse_args()
    split = source_boundaries(args.inventory, Path(ROOT) / "src")
    if split:
        print("warning: C functions without an inventory boundary (counted as splits; add them with "
              "tools/add_function.py): "
              + ", ".join(f"{a:08X}" for a in split), file=sys.stderr)
    funcs, sizes = load_functions(args.inventory, extra=split)
    done = collect_done(funcs, Path(ROOT) / "src")
    pinned = collect_pinned(funcs, Path(ROOT) / "src") & done
    if args.list_pinned:
        list_pinned(funcs, sizes, pinned, Path(ROOT) / "src")
        return
    asm = intentional_asm(funcs, done, load_asm_ranges())
    counted = [(n, a) for n, a in funcs if n not in asm]
    total_b = sum(sizes[n] for n, _ in counted)
    done_b = sum(sizes[n] for n in done)
    print(f"functions: {len(done)}/{len(counted)} ({100 * len(done) / len(counted):.2f}%)")
    print(f"code bytes: {done_b}/{total_b} ({100 * done_b / total_b:.3f}%)")
    print(f"intentional assembly (not counted above): {len(asm)} functions, {sum(sizes[n] for n in asm)} bytes")
    pinned_b = sum(sizes[n] for n in pinned)
    print(f"register-pinned: {100 * pinned_b / total_b:.3f}% of total code; no register pinning: {100 * (done_b - pinned_b) / total_b:.3f}%")
    fuzzy = {n: p for n, p in collect_fuzzy(funcs, Path(ROOT) / "src").items() if n not in done}
    draft_b = sum(sizes[n] for n in fuzzy)
    print(f"nonmatching drafts: {len(fuzzy)} functions, {draft_b} bytes ({100 * draft_b / total_b:.3f}% of code)")
    if args.markdown or args.json:
        categories = load_categories()
        libraries = load_libraries(funcs, sizes)
        subsystems = load_groups(funcs, sizes, SUBSYSTEMS, "game")
        report = make_report(funcs, sizes, done, libraries, pinned, subsystems, fuzzy, load_areas(),
                             load_asm_ranges(), categories)
        ram = data_measures()
        print(f"RAM data: {ram['matched_data']}/{ram['total_data']} bytes in a shared header "
              f"({ram['matched_data_percent']:.2f}%), {ram['complete_data']} complete")
        add_rom_data(report)
        m = report["measures"]
        print(f"ROM data (reported as data): {m['matched_data']}/{m['total_data']} bytes matched "
              f"({m['matched_data_percent']:.2f}%)")
    if args.markdown:
        write_markdown(args.markdown, counted, sizes, done, total_b, done_b, pinned_b, fuzzy, report, categories,
                       asm_b=sum(sizes[n] for n in asm), ram=ram)
        write_tree(Path(args.markdown).with_suffix(".json"), report, {a for n, a in funcs if n in pinned},
                   Path(ROOT) / "src")
    if args.json:
        Path(args.json).write_text(json.dumps(report, indent=2) + "\n")


def source_files(source_root):
    """Each function's C unit (repo-relative) and the name it has there, by
    address, from definitions, INCLUDE_ASM placeholders and NONMATCHING drafts."""
    with (Path(ROOT) / "symbols/functions.csv").open(newline="") as f:
        addrs = {row["name"]: int(row["addr"], 16) for row in csv.DictReader(f)}
    files = {}
    for p in sorted(Path(source_root).rglob("*.c")):
        rel = p.relative_to(ROOT).as_posix()
        raw = p.read_text()
        alias = dict(re.findall(r"^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$", raw, re.M))
        for defined in c_functions(p) + [n for n, _ in nonmatching_functions(p)]:
            n = alias.get(defined, defined)
            m = re.fullmatch(r"sub_([0-9A-F]{8})", n)
            a = int(m.group(1), 16) if m else addrs.get(n)
            if a is not None:
                files.setdefault(a, (rel, defined))
    return files


def write_tree(out, report, pinned_addrs, source_root):
    """Compact category -> unit -> function JSON: each function is
    [name, address, bytes, percent, pinned, file], plus the name in the C file
    when that differs. Percent is 100 when matched, else the NONMATCHING
    draft's fuzzy score or 0. File indexes `files`; -1 means
    src/fn/sub_<address>.c and -2 that no C unit holds it yet."""
    files = source_files(source_root)
    paths = sorted({p for a, (p, _) in files.items() if p != f"src/fn/sub_{a:08X}.c"})
    index = {p: i for i, p in enumerate(paths)}
    categories = {}
    for unit in report["units"]:
        top, name = unit["name"].split("/", 1)
        if name.endswith(ASM_SUFFIX) or not unit["functions"]:  # ROM data units have no functions
            continue
        fns = []
        for f in unit["functions"]:
            a = int(f["metadata"]["virtual_address"])
            path, defined = files.get(a, (None, None))
            fn = [f["name"], f"{a:08X}", int(f["size"]), round(f["fuzzy_match_percent"], 1),
                  int(a in pinned_addrs), -2 if path is None else index.get(path, -1)]
            if defined and defined not in (f["name"], f"sub_{a:08X}"):
                fn.append(defined)
            fns.append(fn)
        categories.setdefault(top, []).append({"name": name, "functions": fns})
    tree = {"version": 1, "files": paths,
            "categories": [{"name": c, "units": us} for c, us in categories.items()]}
    Path(out).write_text(json.dumps(tree, separators=(",", ":")) + "\n")


def bar(frac, width=30):
    n = round(frac * width)
    return "█" * n + "░" * (width - n)


# Broader sections above the report categories. decomp.dev's treemap stops at
# category -> unit -> function, so this level only exists in PROGRESS.md.
SECTIONS = [("Gameplay", ["player", "monsters", "bosses", "field", "actors", "minigames"]),
            ("Presentation and UI", ["effects", "ui"]),
            ("Engine and system", ["engine", "system", "link"]),
            ("Libraries", ["libraries"])]


def family(unit):
    """Display family of a unit: its first word once numbering and the
    assembly suffix are dropped ("Ghoul 2" -> "Ghoul", "Scene camera" -> "Scene")."""
    name = unit.removesuffix(ASM_SUFFIX)
    return re.sub(r" \d+$", "", name).split()[0]


def stats(units):
    get = lambda key: sum(int(u["measures"][key]) for u in units)
    return get("total_code"), get("matched_code"), get("total_functions"), get("matched_functions")


def stat_cells(units, width=20):
    tb, db, tf, df = stats(units)
    return f"{df}/{tf} | {100 * db / max(tb, 1):.1f}% | `{bar(db / max(tb, 1), width)}`"


def subsystem_lines(report, categories):
    """Section -> category -> family -> unit tables, with the functions still
    to match listed per unit (draft score in brackets)."""
    labels = {cid: label for cid, (label, _) in categories.items()}
    labels["libraries"] = "Libraries"
    by_label = {}
    asm_units = []
    for unit in report["units"]:
        top, name = unit["name"].split("/", 1)
        if name.endswith(ASM_SUFFIX):  # reported once, below, outside the totals
            asm_units.append(unit)
            continue
        by_label.setdefault(top, []).append((name, unit))
    seen = {cid for _, cids in SECTIONS for cid in cids}
    sections = SECTIONS + [("Other", [cid for cid in labels if cid not in seen])]
    lines = ["", "## By subsystem", "",
             "Sections group the report categories; inside each category, units sharing a first word "
             "form a family. The same units (category -> unit -> function) are on decomp.dev.", "",
             "| Section / category | Functions | Bytes matched | |", "|---|---:|---:|---|"]
    body = []
    for section, cids in sections:
        cats = [(labels[c], by_label[labels[c]]) for c in cids if labels.get(c) in by_label]
        if not cats:
            continue
        lines.append(f"| **{section}** | {stat_cells([u for _, us in cats for _, u in us])} |")
        body += ["", f"### {section}"]
        for label, units in cats:
            lines.append(f"| &nbsp;&nbsp;{label} | {stat_cells([u for _, u in units])} |")
            families = {}
            for name, unit in units:
                families.setdefault(family(name), []).append((name, unit))
            tb, db, tf, df = stats([u for _, u in units])
            body += ["", f"<details><summary><b>{label}</b>: {df}/{tf} functions, "
                     f"{100 * db / max(tb, 1):.1f}% of bytes</summary>", "",
                     "| Family / unit | Functions | Bytes matched | |", "|---|---:|---:|---|"]
            for fam, members in sorted(families.items(), key=lambda kv: -stats([u for _, u in kv[1]])[0]):
                if len(members) > 1:
                    body.append(f"| **{fam}** | {stat_cells([u for _, u in members])} |")
                for name, unit in members:
                    indent = "&nbsp;&nbsp;" if len(members) > 1 else ""
                    body.append(f"| {indent}{name} | {stat_cells([unit])} |")
            remaining = [(name, f) for name, unit in units for f in unit["functions"]
                         if f["fuzzy_match_percent"] != 100]
            if remaining:
                body += ["", f"Still to match ({len(remaining)}):", ""]
                for name, unit in units:
                    left = [f for f in unit["functions"] if f["fuzzy_match_percent"] != 100]
                    if left:
                        body.append(f"- {name}: " + ", ".join(
                            f"`{f['name']}` {f['size']} B" + (f" ({f['fuzzy_match_percent']:.0f}%)"
                                                              if f["fuzzy_match_percent"] else "")
                            for f in left))
            body += ["", "</details>"]
    if asm_units:
        tb, _, tf, _ = stats(asm_units)
        lines.append(f"| *Intentional assembly (not counted)* | {tf} functions | {tb:,} bytes | |")
        body += ["", "### Intentional assembly", "",
                 "Handwritten assembly that stays assembly (symbols/progress_asm.csv); "
                 "left out of every total above.", ""]
        body += [f"- {u['name']}: {u['measures']['total_functions']} functions, "
                 f"{int(u['measures']['total_code']):,} bytes" for u in asm_units]
    return lines + body


def write_markdown(out, funcs, sizes, done, total_b, done_b, pinned_b=0, fuzzy=None, report=None,
                   categories=None, asm_b=0, ram=None):
    fuzzy = fuzzy or {}
    draft_b = sum(sizes[n] for n in fuzzy)
    mean = sum(sizes[n] * p for n, p in fuzzy.items()) / draft_b if draft_b else 0
    regions = [(0x08000000, 0x08040000), (0x08040000, 0x08080000), (0x08080000, 0x080C0000),
               (0x080C0000, 0x08100000), (0x08100000, 0x08140000), (0x08140000, 0x08180000),
               (0x08180000, 0x081C0000), (0x081C0000, 0x08200000), (0x08200000, 0x08240000),
               (0x08240000, 0x0824DAFA)]
    lines = ["# Decompilation progress", "",
             "Generated by `python3 tools/progress.py --markdown PROGRESS.md`, which also writes "
             "`PROGRESS.json` (the category -> unit -> function tree the `tools/claude-mod` popup reads).", "",
             f"**Code matched in C: {100 * done_b / total_b:.2f}%** "
             f"({done_b:,} / {total_b:,} bytes) — **{len(done)} / {len(funcs)} functions**", "",
             f"Without register pinning: **{100 * (done_b - pinned_b) / total_b:.3f}%**. "
             f"Register-pinned: **{100 * pinned_b / total_b:.3f}%** of total code (included above).", "",
             f"Nonmatching drafts (`#ifdef NONMATCHING`, not counted above): **{len(fuzzy)} functions**, "
             f"{100 * draft_b / total_b:.3f}% of code, mean fuzzy match {mean:.1f}%.", "",
             f"Intentional assembly (GBA header, crt0, SDK assembly; not counted above): {asm_b:,} bytes.", "",
             *ram_lines(ram or {}),
             "```", f"{bar(done_b / total_b, 50)} {100 * done_b / total_b:.2f}%", "```", "",
             "| Region | Functions | Matched | Bytes matched | |", "|---|---:|---:|---:|---|"]
    for lo, hi in regions:
        fs = [(n, a) for n, a in funcs if lo <= a < hi]
        tb = sum(sizes[n] for n, _ in fs)
        db = sum(sizes[n] for n, _ in fs if n in done)
        nd = sum(1 for n, _ in fs if n in done)
        if not fs:
            continue
        lines.append(f"| `{lo:08X}–{hi:08X}` | {len(fs)} | {nd} | {100 * db / max(tb, 1):.1f}% | `{bar(db / max(tb, 1), 20)}` |")
    lines += ["", *rom_data_lines()]
    if report and categories:
        lines += subsystem_lines(report, categories)
    Path(out).write_text("\n".join(lines) + "\n")


if __name__ == "__main__":
    main()

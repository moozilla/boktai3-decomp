#!/usr/bin/env python3
"""Decompilation progress: functions/bytes in src/*.c vs all code.

  progress.py            human-readable summary
  progress.py --json F   objdiff v2 report for decomp.dev
  progress.py --markdown F   PROGRESS.md tracker (per-region bars)
"""
import argparse
import csv
import json
from pathlib import Path
import re

from romlib import ROOT
from split import c_functions, nonmatching_functions

CODE_END = 0x0824DAFA
INVENTORY = Path(ROOT) / "symbols/progress_functions.csv"
EXTRA_BOUNDARIES = Path(ROOT) / "symbols/progress_extra_functions.csv"
LIBRARIES = Path(ROOT) / "symbols/progress_libraries.csv"
SUBSYSTEMS = Path(ROOT) / "symbols/progress_subsystems.csv"


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
            if any(lib["name"] == row["name"] for lib in libraries):
                raise ValueError("group names must be unique")
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
                              "names": names, "prefix": prefix})
    return libraries


def assembly_addresses(path):
    """Read only function-start addresses, never instructions or ROM bytes."""
    code = Path(path).read_text()
    return [int(a, 16) for a in re.findall(
        r"^\t(?:thumb|arm|non_word_aligned_thumb)_func_start \S+\n\w+: @ 0x([0-9A-F]{8})",
        code, re.M)]


def write_inventory(path, addresses):
    validate_addresses(addresses)
    with Path(path).open("w", newline="") as f:
        writer = csv.writer(f, lineterminator="\n")
        writer.writerow(["address"])
        writer.writerows([f"{a:08X}"] for a in addresses)


def inventory_addresses(assembly, extras=EXTRA_BOUNDARIES):
    # A few SDK functions have verified C definitions but gbadisasm leaves
    # their bytes inside the preceding function's span. Keep these reviewed
    # exceptions explicit rather than accepting arbitrary source definitions.
    with Path(extras).open(newline="") as f:
        extra = [int(row["address"], 16) for row in csv.DictReader(f)]
    if extra:
        validate_addresses(extra)
    return sorted(set(assembly_addresses(assembly)) | set(extra))


def validate_addresses(addresses):
    if not addresses or addresses != sorted(set(addresses)):
        raise ValueError("function addresses must be nonempty, unique and sorted")
    if addresses[0] < 0x08000000 or addresses[-1] >= CODE_END:
        raise ValueError("function addresses outside code range")
    if any(a & 1 for a in addresses):
        raise ValueError("function addresses must be halfword aligned")


def load_functions(inventory=INVENTORY, symbols=None):
    with Path(inventory).open(newline="") as f:
        addresses = [int(row["address"], 16) for row in csv.DictReader(f)]
    validate_addresses(addresses)
    symbols = symbols or Path(ROOT) / "symbols/functions.csv"
    with Path(symbols).open(newline="") as f:
        names = {int(row["addr"], 16): row["name"] for row in csv.DictReader(f)}
    funcs = [(names.get(a, f"sub_{a:08X}"), a) for a in addresses]
    if len({n for n, _ in funcs}) != len(funcs):
        raise ValueError("duplicate function names in symbol map")
    sizes = {n: b - a for (n, a), b in zip(funcs, addresses[1:] + [CODE_END])}
    return funcs, sizes


def collect_done(funcs, source_root):
    # Resolve address aliases even if a named symbol is defined as sub_XXXXXXXX.
    known = {n: n for n, _ in funcs}
    known.update({f"sub_{a:08X}": n for n, a in funcs})
    done = set()
    for p in sorted(Path(source_root).rglob("*.c")):
        for n in c_functions(p, with_asm=False):
            if n not in known:
                raise ValueError(f"{p}: {n} has no progress boundary; refresh inventory")
            done.add(known[n])
    return done


MARK = "// MATCHING_CONCESSION: register-pinning"
HINT = re.compile(r'\bregister\b[^;]*\basm\s*\(\s*"\w+"\s*\)|\basm\s*(?:volatile\s*)?\(\s*""')


def hinted_functions(text, names):
    """Functions of a unit that use register pins or asm barriers, directly or
    through a static helper that does (a unit-wide marker is attributed so)."""
    from merge_unit import items
    bodies = {}
    for item in items(text):
        code = re.sub(r"^(\s*(//[^\n]*|/\*.*?\*/)\s*)+", "", item, flags=re.S)  # leading comments
        m = re.match(r"(static\s+)?[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\)\s*\{", code, re.S)
        if m:
            bodies[m.group(2)] = (bool(m.group(1)), item)
    helpers = {n for n, (static, body) in bodies.items() if static and HINT.search(body)}
    uses = re.compile(r"\b(" + "|".join(map(re.escape, helpers)) + r")\s*\(") if helpers else None
    return [n for n in names if n in bodies and (HINT.search(bodies[n][1]) or
                                               (uses and uses.search(bodies[n][1])))]


def collect_pinned(funcs, source_root):
    aliases = {f"sub_{a:08X}": n for n, a in funcs}
    pinned = set()
    for path in Path(source_root).rglob("*.c"):
        text = path.read_text()
        if MARK not in text:
            continue
        # A one-function file is pinned by its marker. In a multi-function unit
        # the functions using pins or barriers (directly or via a static helper).
        names = c_functions(path, with_asm=False)
        if len(names) > 1:  # in a unit, the hints themselves say which functions
            pinned.update(aliases.get(n, n) for n in hinted_functions(text, names))
        else:
            pinned.update(aliases.get(n, n) for n in names)
    return pinned


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


def make_report(funcs, sizes, done, libraries=(), pinned=frozenset(), subsystems=(), fuzzy=None):
    fuzzy = fuzzy or {}
    score = lambda n: 100 if n in done else fuzzy.get(n, 0)
    libraries = sorted([*libraries, *subsystems], key=lambda group: group["start"])
    for left, right in zip(libraries, libraries[1:]):
        if left["end"] > right["start"]:
            raise ValueError("report groups overlap")
    units = []
    grouped = {}
    for n, a in funcs:
        library = next((lib for lib in libraries if lib["start"] <= a < lib["end"]), None)
        unit_name = f"{library.get('prefix', 'libraries')}/{library['name']}" if library else f"functions/{a:08X}"
        category = "register_pinned" if n in pinned else "no_register_pinning"
        if library and n in pinned:
            unit_name += "/register-pinned"
        if unit_name not in grouped:
            unit = {"name": unit_name, "functions": [], "metadata": {"progress_categories": [category]}}
            units.append(unit)
            grouped[unit_name] = unit
        matched = n in done
        grouped[unit_name]["functions"].append({
            "name": library["names"].get(a, n) if library else n,
            "size": str(sizes[n]), "fuzzy_match_percent": score(n),
            "address": str(a - (library["start"] if library else a)),
            "metadata": {"virtual_address": str(a)}})
    for unit in units:
        items = unit["functions"]
        total_b = sum(int(item["size"]) for item in items)
        matched_items = [item for item in items if item["fuzzy_match_percent"] == 100]
        done_b = sum(int(item["size"]) for item in matched_items)
        complete = len(matched_items) == len(items)
        fuzzy_b = sum(int(item["size"]) * item["fuzzy_match_percent"] / 100 for item in items)
        unit["measures"] = measures(total_b, done_b, len(items), len(matched_items),
                                   units=1, complete_units=int(complete), fuzzy_b=fuzzy_b)
        unit["metadata"]["complete"] = complete
    categories = []
    for cid, label in [("no_register_pinning", "No register pinning"), ("register_pinned", "Register-pinned")]:
        members = [n for n, _ in funcs if (n in pinned) == (cid == "register_pinned")]
        if not members:
            continue
        matched = [n for n in members if n in done]
        selected = [u for u in units if cid in u["metadata"]["progress_categories"]]
        categories.append({"id": cid, "name": label, "measures": measures(
            sum(sizes[n] for n in members), sum(sizes[n] for n in matched),
            len(members), len(matched), units=len(selected),
            complete_units=sum(u["metadata"]["complete"] for u in selected),
            fuzzy_b=sum(sizes[n] * score(n) / 100 for n in members))})
    return {"version": 2, "categories": categories, "measures": measures(sum(sizes.values()),
            sum(sizes[n] for n in done), len(funcs), len(done), units=len(units),
            complete_units=sum(u["metadata"]["complete"] for u in units),
            fuzzy_b=sum(sizes[n] * score(n) / 100 for n, _ in funcs)), "units": units}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--json", metavar="FILE")
    parser.add_argument("--markdown", metavar="FILE")
    parser.add_argument("--inventory", type=Path, default=INVENTORY)
    parser.add_argument("--refresh-inventory", metavar="ASSEMBLY", type=Path,
                        help="locally export address-only metadata from gen/code_sym.s")
    parser.add_argument("--check-inventory", metavar="ASSEMBLY", type=Path)
    args = parser.parse_args()
    if args.refresh_inventory:
        write_inventory(args.inventory, inventory_addresses(args.refresh_inventory))
    funcs, sizes = load_functions(args.inventory)
    if args.check_inventory:
        if [a for _, a in funcs] != inventory_addresses(args.check_inventory):
            parser.error("progress inventory differs from generated function boundaries")
    done = collect_done(funcs, Path(ROOT) / "src")
    pinned = collect_pinned(funcs, Path(ROOT) / "src") & done
    total_b = sum(sizes.values())
    done_b = sum(sizes[n] for n in done)
    print(f"functions: {len(done)}/{len(funcs)} ({100 * len(done) / len(funcs):.2f}%)")
    print(f"code bytes: {done_b}/{total_b} ({100 * done_b / total_b:.3f}%)")
    pinned_b = sum(sizes[n] for n in pinned)
    print(f"register-pinned: {100 * pinned_b / total_b:.3f}% of total code; no register pinning: {100 * (done_b - pinned_b) / total_b:.3f}%")
    fuzzy = {n: p for n, p in collect_fuzzy(funcs, Path(ROOT) / "src").items() if n not in done}
    draft_b = sum(sizes[n] for n in fuzzy)
    print(f"nonmatching drafts: {len(fuzzy)} functions, {draft_b} bytes ({100 * draft_b / total_b:.3f}% of code)")
    if args.markdown:
        write_markdown(args.markdown, funcs, sizes, done, total_b, done_b, pinned_b, fuzzy)
    if args.json:
        libraries = load_libraries(funcs, sizes)
        subsystems = load_groups(funcs, sizes, SUBSYSTEMS, "game")
        Path(args.json).write_text(json.dumps(make_report(funcs, sizes, done, libraries, pinned, subsystems, fuzzy), indent=2) + "\n")


def bar(frac, width=30):
    n = round(frac * width)
    return "█" * n + "░" * (width - n)


def write_markdown(out, funcs, sizes, done, total_b, done_b, pinned_b=0, fuzzy=None):
    fuzzy = fuzzy or {}
    draft_b = sum(sizes[n] for n in fuzzy)
    mean = sum(sizes[n] * p for n, p in fuzzy.items()) / draft_b if draft_b else 0
    regions = [(0x08000000, 0x08040000), (0x08040000, 0x08080000), (0x08080000, 0x080C0000),
               (0x080C0000, 0x08100000), (0x08100000, 0x08140000), (0x08140000, 0x08180000),
               (0x08180000, 0x081C0000), (0x081C0000, 0x08200000), (0x08200000, 0x08240000),
               (0x08240000, 0x0824DAFA)]
    lines = ["# Decompilation progress", "",
             "Generated by `python3 tools/progress.py --markdown PROGRESS.md`.", "",
             f"**Code matched in C: {100 * done_b / total_b:.2f}%** "
             f"({done_b:,} / {total_b:,} bytes) — **{len(done)} / {len(funcs)} functions**", "",
             f"Without register pinning: **{100 * (done_b - pinned_b) / total_b:.3f}%**. "
             f"Register-pinned: **{100 * pinned_b / total_b:.3f}%** of total code (included above).", "",
             f"Nonmatching drafts (`#ifdef NONMATCHING`, not counted above): **{len(fuzzy)} functions**, "
             f"{100 * draft_b / total_b:.3f}% of code, mean fuzzy match {mean:.1f}%.", "",
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
    Path(out).write_text("\n".join(lines) + "\n")


if __name__ == "__main__":
    main()

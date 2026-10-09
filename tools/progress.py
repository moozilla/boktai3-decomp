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
from split import c_functions

CODE_END = 0x0824DAFA
INVENTORY = Path(ROOT) / "symbols/progress_functions.csv"
EXTRA_BOUNDARIES = Path(ROOT) / "symbols/progress_extra_functions.csv"


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


def measures(total_b, done_b, count, done_count):
    return {
        "fuzzy_match_percent": 100 * done_b / total_b,
        "total_code": str(total_b), "matched_code": str(done_b),
        "matched_code_percent": 100 * done_b / total_b,
        "total_functions": count, "matched_functions": done_count,
        "total_units": count, "complete_units": done_count,
        "matched_functions_percent": 100 * done_count / count,
        "complete_code": str(done_b), "complete_code_percent": 100 * done_b / total_b,
    }


def make_report(funcs, sizes, done):
    units = []
    for n, a in funcs:
        matched = n in done
        units.append({"name": f"functions/{a:08X}",
                      "measures": measures(sizes[n], sizes[n] if matched else 0, 1, int(matched)),
                      "functions": [{"name": n, "size": str(sizes[n]),
                                     "fuzzy_match_percent": 100 if matched else 0,
                                     "metadata": {"virtual_address": str(a)}}],
                      "metadata": {"complete": matched}})
    return {"version": 2, "measures": measures(sum(sizes.values()),
            sum(sizes[n] for n in done), len(funcs), len(done)), "units": units}


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
    total_b = sum(sizes.values())
    done_b = sum(sizes[n] for n in done)
    print(f"functions: {len(done)}/{len(funcs)} ({100 * len(done) / len(funcs):.2f}%)")
    print(f"code bytes: {done_b}/{total_b} ({100 * done_b / total_b:.3f}%)")
    if args.markdown:
        write_markdown(args.markdown, funcs, sizes, done, total_b, done_b)
    if args.json:
        Path(args.json).write_text(json.dumps(make_report(funcs, sizes, done), indent=2) + "\n")


def bar(frac, width=30):
    n = round(frac * width)
    return "█" * n + "░" * (width - n)


def write_markdown(out, funcs, sizes, done, total_b, done_b):
    regions = [(0x08000000, 0x08040000), (0x08040000, 0x08080000), (0x08080000, 0x080C0000),
               (0x080C0000, 0x08100000), (0x08100000, 0x08140000), (0x08140000, 0x08180000),
               (0x08180000, 0x081C0000), (0x081C0000, 0x08200000), (0x08200000, 0x08240000),
               (0x08240000, 0x0824DAFA)]
    lines = ["# Decompilation progress", "",
             "Generated by `python3 tools/progress.py --markdown PROGRESS.md`.", "",
             f"**Code matched in C: {100 * done_b / total_b:.2f}%** "
             f"({done_b:,} / {total_b:,} bytes) — **{len(done)} / {len(funcs)} functions**", "",
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

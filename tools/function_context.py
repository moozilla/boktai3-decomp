#!/usr/bin/env python3
"""Attribute mGBA harness execution coverage to disassembled functions.

Coverage is set membership: the harness stores one bit per executed halfword.
This tool never infers call counts or semantic function names from it.
"""
from __future__ import annotations

import argparse
import csv
import json
import re
import sys
from dataclasses import asdict, dataclass
from pathlib import Path

ROM_BASE = 0x08000000
ROM_SIZE = 0x1000000
# symbolize.py's CODE_END: one byte past the last code region.
CODE_END = 0x0824DAFA
FUNC_LABEL = re.compile(r"^([A-Za-z_.$][\w.$]*):\s*@\s*0x([0-9A-Fa-f]{8})")
START_DIRECTIVE = re.compile(r"\s*(thumb_func_start|arm_func_start|non_word_aligned_thumb_func_start)\s+(\S+)")
# Known ROM ranges that hold data/overlays rather than ordinary function bodies.
DATA_GAPS = ((0x08000350, 0x0800145C), (0x0822F248, 0x0822F648))


@dataclass(frozen=True)
class Function:
    address: int
    name: str
    mode: str
    end: int

    def __post_init__(self):
        if self.mode not in ("arm", "thumb"):
            raise ValueError(f"invalid function mode {self.mode!r}")
        if not ROM_BASE <= self.address < self.end <= CODE_END:
            raise ValueError(f"invalid function bounds {self.address:#010x}..{self.end:#010x}")
        if self.address & 1:
            raise ValueError(f"function address is not halfword aligned: {self.address:#010x}")


def parse_functions(asm_path: Path) -> list[Function]:
    """Read function starts from gbadisasm's code_sym.s labels/directives."""
    lines = asm_path.read_text(errors="replace").splitlines()
    pending_label: tuple[str, int] | None = None
    declared: tuple[str, str] | None = None
    starts: dict[int, tuple[str, str]] = {}
    for line in lines:
        d = START_DIRECTIVE.match(line)
        if d:
            directive, name = d.groups()
            mode = "arm" if directive == "arm_func_start" else "thumb"
            declared = (name, mode)
            if pending_label and pending_label[0] == name:
                starts[pending_label[1]] = (name, mode)
                pending_label = None
            continue
        m = FUNC_LABEL.match(line)
        if m:
            name, addr = m.group(1), int(m.group(2), 16)
            if declared and declared[0] == name:
                starts[addr] = declared
            else:
                pending_label = (name, addr)
    ordered = sorted((a, *info) for a, info in starts.items())
    functions = []
    for i, (addr, name, mode) in enumerate(ordered):
        if not ROM_BASE <= addr < CODE_END:
            continue
        next_addr = ordered[i + 1][0] if i + 1 < len(ordered) else CODE_END
        # Never let a prior function's inferred span absorb known data overlays.
        end = min(next_addr, CODE_END)
        for gap_start, gap_end in DATA_GAPS:
            if gap_start <= addr < gap_end:
                end = addr  # Malformed/non-code entry in a known data overlay.
                break
            if addr < gap_start < end:
                end = gap_start
        if end > addr:
            functions.append(Function(addr, name, mode, end))
    return functions


def load_exec(path: Path) -> bytearray:
    data = path.read_bytes()
    expected = ROM_SIZE // 2
    if len(data) != expected:
        raise ValueError(f"{path}: expected {expected} bytes (one byte per ROM halfword), got {len(data)}")
    return bytearray(data)


def visited_functions(functions: list[Function], coverage: bytearray) -> dict[int, dict[str, bool]]:
    """Map function address to entry/inside observation; no execution counts."""
    result = {}
    for fn in functions:
        lo = (fn.address - ROM_BASE) // 2
        hi = (fn.end - ROM_BASE + 1) // 2
        mode_bit = 1 if fn.mode == "thumb" else 2
        bits = coverage[lo:hi]
        if any(b & mode_bit for b in bits):
            result[fn.address] = {"entry_executed": bool(bits and bits[0] & mode_bit),
                                  "any_halfword_executed": True, "mode": fn.mode}
    return result


def build_report(functions, coverage_by_tag, selected, baseline):
    visits = {tag: visited_functions(functions, cov) for tag, cov in coverage_by_tag.items()}
    allowed_modes = bytearray(ROM_SIZE // 2)
    for fn in functions:
        lo = (fn.address - ROM_BASE) // 2
        hi = (fn.end - ROM_BASE) // 2
        mode_bit = 1 if fn.mode == "thumb" else 2
        for i in range(lo, hi):
            allowed_modes[i] |= mode_bit
    coverage_diagnostics = {}
    for tag, cov in coverage_by_tag.items():
        coverage_diagnostics[tag] = {
            "unattributed_mode_halfwords": sum(1 for i, value in enumerate(cov)
                                                if (value & 0x03) & ~allowed_modes[i])
        }
    selected_set = set().union(*(set(visits[t]) for t in selected)) if selected else set()
    baseline_set = set().union(*(set(visits[t]) for t in baseline)) if baseline else set()
    all_tags = list(coverage_by_tag)
    rows = []
    for fn in functions:
        addr = fn.address
        if addr not in selected_set:
            continue
        tags = [t for t in selected if addr in visits[t]]
        others = [t for t in all_tags if t not in selected and addr in visits[t]]
        row = asdict(fn)
        row.update({
            "boundary_method": "function start to next function start, clipped at known data overlays",
            "boundary_note": "Embedded literal/data pools can still be inside inferred spans; review assembly.",
            "unattributed_mode_halfwords_by_tag": coverage_diagnostics,
            "selected_segments": tags,
            "baseline_segments": [t for t in baseline if addr in visits[t]],
            "other_segments": others,
            "exclusive_to_selected_vs_baseline": addr not in baseline_set,
            "selected_entry_observed": any(visits[t][addr]["entry_executed"] for t in selected if addr in visits[t]),
            "selected_any_halfword_observed": True,
        })
        rows.append(row)
    rows.sort(key=lambda r: (not r["exclusive_to_selected_vs_baseline"], -len(r["selected_segments"]), r["address"]))
    return {"evidence": "observed halfword execution in the function's declared ARM/Thumb mode; reserved bits ignored; no call counts or semantic labels inferred",
            "boundary_note": "Spans can still include embedded literal/data pools; inspect assembly before treating a hit as function code.",
            "excluded_data_gaps": [[f"0x{a:08X}", f"0x{b:08X}"] for a, b in DATA_GAPS],
            "selected": selected, "baseline": baseline,
            "coverage_diagnostics": coverage_diagnostics, "functions": rows}


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--asm", required=True, type=Path, help="gen/code_sym.s")
    p.add_argument("--coverage", action="append", required=True, metavar="TAG=FILE.exec",
                   help="segment coverage; repeat for each cov-*.exec file")
    p.add_argument("--segment", action="append", required=True, help="selected TAG (repeatable)")
    p.add_argument("--baseline", action="append", default=[], help="baseline TAG (repeatable)")
    p.add_argument("--format", choices=("json", "csv"), default="csv")
    p.add_argument("-o", "--output", type=Path)
    args = p.parse_args(argv)
    try:
        coverage = {}
        for spec in args.coverage:
            tag, sep, filename = spec.partition("=")
            if not sep or not tag or tag in coverage:
                raise ValueError(f"invalid or duplicate --coverage {spec!r}; expected unique TAG=FILE")
            coverage[tag] = load_exec(Path(filename))
        missing = (set(args.segment) | set(args.baseline)) - set(coverage)
        if missing:
            raise ValueError("coverage missing for tags: " + ", ".join(sorted(missing)))
        report = build_report(parse_functions(args.asm), coverage, args.segment, args.baseline)
        if args.format == "json":
            output = json.dumps(report, indent=2) + "\n"
        else:
            import io
            stream = io.StringIO()
            fields = ["address", "end", "name", "mode", "exclusive_to_selected_vs_baseline", "selected_entry_observed",
                      "selected_any_halfword_observed", "boundary_method", "boundary_note",
                      "unattributed_mode_halfwords_by_tag", "selected_segments", "baseline_segments", "other_segments"]
            writer = csv.DictWriter(stream, fieldnames=fields)
            writer.writeheader()
            for row in report["functions"]:
                writer.writerow({k: (";".join(row[k]) if isinstance(row[k], list) else
                                     json.dumps(row[k], sort_keys=True) if isinstance(row[k], dict) else
                                     f"0x{row[k]:08X}" if k in ("address", "end") else row[k]) for k in fields})
            output = stream.getvalue()
        if args.output:
            args.output.write_text(output)
        else:
            sys.stdout.write(output)
    except (OSError, ValueError) as exc:
        p.error(str(exc))


if __name__ == "__main__":
    main()

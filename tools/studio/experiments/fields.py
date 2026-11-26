"""Translate constrained C lvalues to field offsets measured by stock agbcc."""
from bisect import bisect_right
import csv
from dataclasses import dataclass, asdict
from functools import lru_cache
import json
from pathlib import Path
import re
import subprocess

from source_layout import report_path

ROOT = Path(__file__).resolve().parents[3]
AGBCC = ROOT / "build/tools/agbcc/agbcc"
TOKEN = re.compile(r"([A-Za-z_]\w*)((?:\[[0-9]{1,4}\]){0,3})", re.ASCII)
OPERATOR = re.compile(r"->|\.")
# The symbol is declared as a byte pointer because different game routines
# overlay it. Observe named members through its existing persistent-header type.
TYPED_VIEWS = {"gSessionState": ("engine/shared/session_save.h", "struct SessionSave *")}
WORD = re.compile(r"^\s*\.word\s+(-?(?:0x[\da-fA-F]+|\d+))\s*$", re.M)


@dataclass(frozen=True)
class Field:
    path: str
    root: str
    address: int
    offsets: tuple[int, ...]
    width: int
    signed: bool
    header: str

    def to_dict(self):
        return asdict(self)

    def command(self):
        return f"observe {self.address:X} {self.width} " + " ".join(f"{offset:X}" for offset in self.offsets)

    def decode(self, hex_value):
        value = int(hex_value, 16)
        if self.signed and value & (1 << (8 * self.width - 1)):
            value -= 1 << (8 * self.width)
        return value


def parse(path):
    """Read a designator without letting source expressions enter the compiler."""
    if not isinstance(path, str) or not 1 <= len(path) <= 128:
        raise ValueError("Use a field path such as gPlayers[0]->position.x")
    pos = 0
    operator = ""
    parts = []
    while True:
        item = TOKEN.match(path, pos)
        if not item:
            raise ValueError(f"Invalid field path {path!r}")
        indices = tuple(int(value) for value in re.findall(r"[0-9]+", item[2]))
        parts.append((operator, item[1], indices))
        pos = item.end()
        if pos == len(path):
            return parts
        separator = OPERATOR.match(path, pos)
        if not separator or len(parts) == 16:
            raise ValueError(f"Invalid field path {path!r}")
        operator, pos = separator[0], separator.end()


def layout_request(parts, header, typed_view=None):
    """For each ->, measure where to read the next pointer from the current base."""
    root = "experiment_typed_root" if typed_view else parts[0][1]
    actual = root
    based = f"(*(typeof(&{root}))0)"
    offsets, indices = [], []
    for operator, name, subscripts in parts:
        if operator == "->":
            offsets.append(f"(unsigned)&({based})")
            based = f"(*(typeof({actual}))0).{name}"
            actual += f"->{name}"
        elif operator == ".":
            based += f".{name}"
            actual += f".{name}"
        for index in subscripts:
            indices.append((index, f"sizeof({actual}) / sizeof(({actual})[0])"))
            actual += f"[{index}]"
            based += f"[{index}]"
    offsets.append(f"(unsigned)&({based})")
    if len(offsets) > 8:
        raise ValueError("A field path supports at most seven pointer dereferences")
    # The bitwise operation makes float, pointer, and aggregate leaves fail
    # compilation. The signedness test runs on the unpromoted type.
    width = f"sizeof((typeof({actual}))0)"
    guard = f"(((typeof({actual}))0) | 0)"
    signed = f"((typeof({actual}))-1) < ((typeof({actual}))0)"
    exprs = offsets + [width, guard, signed] + [bound for _, bound in indices]
    source = f'#include "engine/shared/global.h"\n#include "{header}"\n'
    if typed_view:
        source += f"extern {typed_view}experiment_typed_root;\n"
    source += "const unsigned experiment_layout[] = {\n"
    source += ",\n".join(exprs) + "\n};\n"
    return source, len(offsets), indices


def compile_layout(source, path):
    # Like tools/scope/structure.py, measure the original target ABI via a
    # constant table. No execution or sample ROM data enters the compiler.
    if not AGBCC.is_file():
        raise RuntimeError("Stock agbcc is missing. Set up the repository toolchain first.")
    cpp = subprocess.run(["cc", "-E", "-nostdinc", "-undef", "-I", "include", "-x", "c", "-"],
                         input=source, capture_output=True, text=True, cwd=ROOT, timeout=15)
    if cpp.returncode:
        raise ValueError(f"Could not preprocess {path!r} in its declared header")
    compiler = subprocess.run([AGBCC, "-O2", "-mthumb-interwork", "-fhex-asm", "-o", "-"],
                              input=cpp.stdout, capture_output=True, text=True, cwd=ROOT, timeout=15)
    if compiler.returncode:
        detail = next((line.split(": ", 1)[-1] for line in compiler.stderr.splitlines()
                       if "In " not in line), "unsupported field type")
        raise ValueError(f"Could not measure {path!r}: {detail}")
    table = compiler.stdout.partition("experiment_layout:\n")[2].partition(".size")[0]
    return [int(value, 0) for value in WORD.findall(table)]


@lru_cache(maxsize=4)
def ram_symbols(stamp):
    with (ROOT / "symbols/ram.csv").open() as file:
        return {row["name"]: row for row in csv.DictReader(file)}


def measure(path):
    parts = parse(path)
    root = parts[0][1]
    symbol = ram_symbols((ROOT / "symbols/ram.csv").stat().st_mtime_ns).get(root)
    if not symbol:
        raise ValueError(f"Unknown RAM symbol {root!r}")
    address = int(symbol["addr"], 16)
    header = symbol["header"].removeprefix("include/")
    if not re.fullmatch(r"[\w/.-]+\.h", header) or ".." in header.split("/") or not (ROOT / "include" / header).is_file():
        raise ValueError(f"{root} has no shared type declaration")
    typed_view = None
    if root in TYPED_VIEWS and len(parts) > 1 and not parts[0][2] and parts[1][0] == "->":
        header, typed_view = TYPED_VIEWS[root]
    source, depth, indices = layout_request(parts, header, typed_view)
    values = compile_layout(source, path)
    if len(values) != depth + 3 + len(indices):
        raise ValueError(f"{path!r} did not compile to a static field layout")
    offsets = tuple(values[:depth])
    width, _, signed = values[depth:depth + 3]
    if width not in {1, 2, 4} or signed not in {0, 1} or any(not 0 <= o <= 0x40000 for o in offsets):
        raise ValueError(f"{path!r} is outside the supported 8-, 16-, and 32-bit scalar layout")
    if any(index >= bound for (index, _), bound in zip(indices, values[depth + 3:])):
        raise ValueError(f"An array index is out of bounds in {path!r}")
    return Field(path, root, address, offsets, width, bool(signed), f"include/{header}")


class FunctionIndex:
    """Resolve a PC only inside a known function's byte extent."""
    def __init__(self, report, names):
        self.addresses = {}
        for category in report["categories"]:
            for unit in category["units"]:
                for name, address, extent, _, _, file, *aliases in unit["functions"]:
                    start = int(address, 16)
                    if extent <= 0:
                        continue
                    source = report_path(report, address, file, ROOT)
                    self.addresses[start] = {
                        "address": address, "extent": extent, "name": names.get(start, name),
                        "source": source, "defined": aliases[0] if aliases else name, "url": f"/code#{address}",
                    }
        self.starts = sorted(self.addresses)
        self.by_name = {name: addr for addr, name in names.items()}

    def lookup(self, pc):
        pc &= ~1
        index = bisect_right(self.starts, pc) - 1
        if index >= 0:
            start = self.starts[index]
            function = self.addresses[start]
            if pc < start + function["extent"]:
                return {**function, "offset": pc - start}
        return None


@lru_cache(maxsize=4)
def functions(stamps):
    with (ROOT / "symbols/functions.csv").open() as file:
        names = {int(row["addr"], 16): row["name"] for row in csv.DictReader(file)}
    return FunctionIndex(json.loads((ROOT / "PROGRESS.json").read_text()), names)


def function_index():
    return functions(tuple((ROOT / path).stat().st_mtime_ns for path in ("symbols/functions.csv", "PROGRESS.json")))

#!/usr/bin/env python3
"""Find groups of floor switches whose common script requires every switch."""
import argparse
from collections import defaultdict
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/assets"))
from stages import Walker, load_rom, num


class PlateWalker(Walker):
    def puzzles(self, scene):
        self.groups = defaultdict(dict)
        self.scene(scene)
        puzzles = []
        for callback, plates in self.groups.items():
            if len(plates) < 2:
                continue
            # A conjunctive callback needs every indexed r708 bit. If the
            # placement calls start with a literal, take the saved bit from
            # the callback's own guard.
            tests = {f"(r708[0x133.0]:t4(48,{i}) == 1)" for i in plates}
            lines = [line for _, line in self.text(callback)]
            guards = [line for line in lines
                      if line.startswith("if ") and "||" not in line and all(t in line for t in tests)]
            if not guards:
                continue
            bit = r"r708\[0x[0-9a-f]+\.\d+\]:t4"
            flags = {flag for _, flag in plates.values() if re.fullmatch(bit, flag)}
            if not flags:
                flags = {match for line in guards for match in re.findall(rf"({bit}) == 0", line)
                         if f"{match} = 1" in lines}
            if len(flags) != 1:
                continue
            flag, = flags
            puzzles.append({"plates": [name for name, _ in plates.values()], "cond": [f"({flag} == 0)"]})
        return puzzles

    def call(self, pid, ops, conds, trap, depth, spawner):
        if pid == 1083 and len(ops) >= 7:
            name, index, callback = num(ops[0]), num(ops[2]), num(ops[5])
            if None not in (name, index, callback):
                self.groups[callback][index] = (name, ops[3])
            return
        super().call(pid, ops, conds, trap, depth, spawner)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path, default=ROOT / "build/autopilot/plate-puzzles.json")
    args = parser.parse_args()
    stages = json.loads((ROOT / "build/assets/stages.json").read_text())
    walker = PlateWalker(load_rom())
    puzzles = {row["scene"]: found for row in stages if (found := walker.puzzles(row["scene"]))}
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(puzzles, indent=2) + "\n")
    print(f"{sum(map(len, puzzles.values()))} plate puzzles in {len(puzzles)} scenes")


if __name__ == "__main__":
    main()

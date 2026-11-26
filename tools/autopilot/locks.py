#!/usr/bin/env python3
"""Extract keyed door requirements from GCL into build/autopilot/locks.json.

Run after tools/assets/stages.py json. Proc 1183 and 1184 wrap proc 1185,
which names the HP box in arg1 and passes arg7 (unlock bit), arg8 (key
item) to the door actor. Proc 1186 consumes the item and sets the bit.
"""
import argparse
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/assets"))
from stages import Walker, load_rom, num


class LockWalker(Walker):
    def scene_locks(self, scene):
        self.locks = []
        self.scene(scene)
        return self.locks

    def call(self, pid, ops, conds, trap, depth, spawner):
        if pid in (1183, 1184, 1185):
            if len(ops) >= 8:
                box, bit, item = map(num, (ops[0], ops[6], ops[7]))
                if box is not None and bit is not None and item is not None and 0 <= bit < 32 and item >= 0:
                    lock = {"box": f"{box:04X}", "bit": bit, "item": item}
                    if lock not in self.locks:
                        self.locks.append(lock)
            return
        super().call(pid, ops, conds, trap, depth, spawner)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--scene", action="append", type=int, help="limit extraction to this scene")
    parser.add_argument("--out", type=Path, default=ROOT / "build/autopilot/locks.json")
    args = parser.parse_args()
    scenes = args.scene
    if scenes is None:
        scenes = [s["scene"] for s in json.loads((ROOT / "build/assets/stages.json").read_text())]
    walker = LockWalker(load_rom())
    locks = {scene: found for scene in scenes if (found := walker.scene_locks(scene))}
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(locks, indent=2) + "\n")
    print(f"{sum(map(len, locks.values()))} keyed doors in {len(locks)} scenes")


if __name__ == "__main__":
    main()

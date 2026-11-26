#!/usr/bin/env python3
"""Find floor traps that start purification while the player holds a coffin."""
import argparse
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/assets"))
from stages import Walker, load_rom, num


class EscortWalker(Walker):
    def scene_targets(self, scene):
        self.targets = []
        self.scene(scene)
        return self.targets

    def call(self, pid, ops, conds, trap, depth, spawner):
        # Proc 6180 starts the Pile Driver dialogue. The trap only calls it
        # while Player_SaveStateSnapshot reports that a coffin is held.
        if pid == 6180 and trap and any("gs[0x85c]:t1 == 6" in c for c in conds):
            box = num(trap)
            if box is not None:
                progress = [c for c in conds if "r708[0x3c]" in c and "gs[0x85c]" not in c]
                target = {"box": f"{box:04X}", "cond": progress}
                if target not in self.targets:
                    self.targets.append(target)
            return
        super().call(pid, ops, conds, trap, depth, spawner)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--scene", action="append", type=int)
    parser.add_argument("--out", type=Path, default=ROOT / "build/autopilot/escort-targets.json")
    args = parser.parse_args()
    scenes = args.scene if args.scene is not None else [
        row["scene"] for row in json.loads((ROOT / "build/assets/stages.json").read_text())
    ]
    walker = EscortWalker(load_rom())
    targets = {scene: found for scene in scenes if (found := walker.scene_targets(scene))}
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(targets, indent=2) + "\n")
    print(f"{sum(map(len, targets.values()))} purification entrances in {len(targets)} scenes")


if __name__ == "__main__":
    main()

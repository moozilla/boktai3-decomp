"""Boot story saves in the Japanese cartridge and record their first encounters."""
from contextlib import closing
from collections import Counter
import json
from pathlib import Path

from frames import ArtTranslator
from localize import Localizer
from player import BOOT
from runtime import Game, OUT, ROOT, saves


def story_bank(directory):
    if directory:
        return directory
    for path in [Path(saves.BANK), Path.home() / "git/boktai3trans/saves"]:
        if path.is_dir():
            return path
    raise ValueError("Specify the directory containing story checkpoints with --bank")


def scene(game):
    return int.from_bytes(game.read(0x030039F0, 2), "little")


def survey(memory, args):
    target = OUT / "story" / ("japanese" if args.japanese else "english")
    target.mkdir(parents=True, exist_ok=True)
    paths = [p for p in sorted(story_bank(args.bank).glob("[0-9][0-9][0-9][0-9] *.sgm"))
             if args.first <= int(p.name[:4]) <= args.last]
    if not paths:
        raise ValueError("No numbered VBA story checkpoints in that range")

    for path in paths:
        if (ROOT / "build/STOP").exists():
            break
        slot = path.name[:4]
        log = target / (slot + ".jsonl")
        field = saves.fields(saves.current(saves.read_sgm(str(path))))
        with log.open("w") as stream:
            resolver = Localizer(memory, stream, enabled=not args.japanese)
            resolver.graphics = ArtTranslator()
            with closing(Game(resolver, path)) as game:
                for delay, button in BOOT:
                    game.step(delay)
                    game.tap(button)
                # Let the file selection and map transition complete. Checkpoint
                # zero and some cutscenes immediately advance to a new room.
                for _ in range(16):
                    game.step(30)
                    if scene(game) == field["scene"]:
                        break
                visited = {scene(game)}
                arrived = game.frame
                game.step(260)
                game.image().save(target / (slot + "-arrival.png"))
                for _ in range(6):
                    game.tap("A")
                    game.step(130)
                    visited.add(scene(game))
                game.image().save(target / (slot + "-after.png"))
        rows = [json.loads(line) for line in log.read_text().splitlines()]
        # Evidence from the common title and file selection remains in the raw
        # transcript. Only post-arrival encounters count in this story summary.
        events = [e for e in rows if e["frame"] >= arrived]
        misses = {e["key"]: e["source"] for e in events
                  if e["event"] in ("miss", "needs-review") and e.get("source")}
        report = {"slot": slot, "checkpoint": path.stem[5:], "saved_scene": field["scene"],
                  "observed_scenes": sorted(visited), "arrived_frame": arrived,
                  "counts": dict(Counter(e["event"] for e in events)), "pending": misses}
        (target / (slot + ".json")).write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n")
        print(f"{slot}  {path.stem[5:]}  |  scenes {sorted(visited)}  |  {len(misses)} pending", flush=True)

    reports = [json.loads(p.read_text()) for p in sorted(target.glob("[0-9][0-9][0-9][0-9].json"))]
    missing = {k: v for r in reports for k, v in r["pending"].items()}
    summary = {"checkpoints": len(reports), "scenes": sorted({s for r in reports for s in r["observed_scenes"]}),
               "pending": missing}
    (target / "summary.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2) + "\n")
    print(f"{len(reports)} checkpoint records in {target.relative_to(ROOT)}")

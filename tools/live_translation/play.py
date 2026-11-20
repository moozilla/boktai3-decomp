#!/usr/bin/env python3
"""Play Boktai 3 with live English, replay its proof, or edit an encounter."""
import argparse
from contextlib import closing
import json
from pathlib import Path
import sys

from runtime import Game, OUT, prison_save
from frames import ArtTranslator
from graphics import REGIONS
from localize import Localizer
from translator import Memory, Ollama
from vision import ArtIndex


def arguments():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--db", type=Path, default=OUT / "translations.sqlite")
    parser.add_argument("--model", help="Enable an Ollama model. Offline memory is the default.")
    parser.add_argument("--endpoint", default="http://127.0.0.1:11434")
    parser.add_argument("--japanese", action="store_true", help="Observe the original cartridge")
    sub = parser.add_subparsers(dest="cmd")
    live = sub.add_parser("play", help="Open the interactive player (default)")
    source = live.add_mutually_exclusive_group()
    source.add_argument("--save", type=Path, help="A battery save or a VBA .sgm checkpoint")
    source.add_argument("--new", action="store_true", help="Start without an existing save")
    landing = live.add_mutually_exclusive_group()
    landing.add_argument("--from-boot", action="store_true", help="Show the entire startup")
    landing.add_argument("--title", action="store_true", help="Start on the translated title screen")
    sub.add_parser("proof", help="Capture paired Japanese and English game frames")
    replay = sub.add_parser("replay", help="Capture the prison or replay a harness input file")
    replay.add_argument("--inputs", type=Path, help="Controller route, e.g. tests/prologue.txt")
    source = replay.add_mutually_exclusive_group()
    source.add_argument("--save", type=Path, help="Battery save or VBA .sgm checkpoint")
    source.add_argument("--new", action="store_true", help="Boot the input route with a blank save")
    sub.add_parser("review", help="Show original, English and provenance").add_argument("key", nargs="?")
    edit = sub.add_parser("correct", help="Change a previously encountered translation")
    edit.add_argument("key")
    edit.add_argument("english")
    term = sub.add_parser("term", help="Add or change a glossary term")
    term.add_argument("japanese", nargs="?")
    term.add_argument("english", nargs="?")
    art = sub.add_parser("art", help="List selected graphics or inspect one original crop")
    art.add_argument("asset", nargs="?")
    mark = sub.add_parser("mark", help="Teach an encountered graphic a new lettering region")
    mark.add_argument("asset", help="Key from the art command")
    mark.add_argument("region", help="Local identifier for this label, such as sign")
    mark.add_argument("box", type=int, nargs=4, metavar=("LEFT", "TOP", "RIGHT", "BOTTOM"))
    mark.add_argument("source", help="Japanese transcription")
    mark.add_argument("english", nargs="?", help="Leave out to request the active model")
    return parser.parse_args()


def review(memory, key):
    rows = memory.db.execute("SELECT * FROM entries WHERE ? IS NULL OR key=? ORDER BY key", (key, key))
    found = False
    for row in rows:
        found = True
        print(f'{row["key"]}  [{row["origin"]}]\n{row["source"]}\n{row["english"] or "(awaiting translation)"}')
        if key:
            issue = memory.db.execute("SELECT reason FROM issues WHERE key=?", (key,)).fetchone()
            if issue:
                print("Review needed:", issue[0])
            print(json.dumps(json.loads(row["context"]), ensure_ascii=False, indent=2))
    if key and not found:
        raise ValueError(f"No encounter for {key} in this memory")


def artwork(memory, key):
    index = ArtIndex(memory)
    rows = index.db.execute("SELECT * FROM artwork WHERE ? IS NULL OR asset=? ORDER BY asset", (key, key))
    found = False
    for row in rows:
        found = True
        ctx = json.loads(row["context"])
        builtin = ctx.get("typography") or str(ctx.get("frame_index")) in REGIONS.get(ctx.get("package"), {})
        state = "authored lettering" if builtin else row["state"]
        print(f'{row["asset"]:26s} {ctx["width_px"]:3d} x {ctx["height_px"]:<3d}  {state}  {row["preview"]}')
        if key:
            print(json.dumps(ctx, ensure_ascii=False, indent=2))
            for region in index.regions(key):
                print(region["id"], region["box"], region["source"], region["english"] or "")
            if row["note"]:
                print(row["note"])
    if key and not found:
        raise ValueError(f"No captured artwork for {key}")


def battery(args):
    source = getattr(args, "save", None)
    if getattr(args, "new", False):
        return None
    return source or prison_save()


def dispatch(args, memory):
    if args.cmd == "proof":
        from proof import run
        return run(memory)
    if args.cmd == "correct":
        memory.correct(args.key, args.english)
        print("Correction saved for", args.key)
        return
    if args.cmd == "review":
        return review(memory, args.key)
    if args.cmd == "art":
        return artwork(memory, args.asset)
    if args.cmd == "mark":
        index = ArtIndex(memory)
        index.mark(args.asset, [{"id": args.region, "box": args.box, "source": args.source,
                                 "english": args.english}], "human correction")
        print("Marked", args.asset, args.region)
        return
    if args.cmd == "term":
        if args.japanese is None:
            for row in memory.db.execute("SELECT g.japanese,g.english,o.origin FROM glossary g LEFT JOIN term_origins o USING(japanese) ORDER BY 1"):
                print(f'{row[0]}   {row[1]}   [{row[2] or "local"}]')
        elif args.english is None:
            raise ValueError("Specify an English spelling for this term")
        else:
            memory.add_term(args.japanese, args.english)
            print("Glossary updated")
        return
    target = OUT / ("japanese" if args.japanese else "english")
    route = getattr(args, "inputs", None)
    if route:
        target = OUT / "routes" / route.stem / target.name
    target.mkdir(parents=True, exist_ok=True)
    with (target / "transcript.jsonl").open("w") as log:
        resolver = Localizer(memory, log, enabled=not args.japanese)
        resolver.graphics = ArtTranslator()
        with closing(Game(resolver, battery(args))) as game:
            if args.cmd == "replay":
                if route:
                    from routes import run
                    run(game, route, target)
                else:
                    from proof import prison
                    prison(game, target)
                print(target)
            else:
                from player import play
                start = "opening"
                if getattr(args, "from_boot", False) or getattr(args, "new", False):
                    start = "boot"
                elif getattr(args, "save", None) or getattr(args, "title", False):
                    start = "title"
                play(game, start=start)


def main():
    args = arguments()
    OUT.mkdir(parents=True, exist_ok=True)
    memory = Memory(args.db, Ollama(args.endpoint, args.model) if args.model else None)
    try:
        dispatch(args, memory)
    except (ValueError, OSError) as exc:
        sys.exit(str(exc))
    finally:
        memory.db.close()


if __name__ == "__main__":
    main()

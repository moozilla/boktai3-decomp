"""Command-line entry point for the same experiment engine used by Studio."""
import argparse
import json
from pathlib import Path
import subprocess
import sys

from . import centaur, fields, graphics, pixel_proof, recipe, runner, sweep


def proof_branch(parent):
    """The second query begins at the first query's paused frame."""
    spec = parent["recipe"]
    return recipe.validate({
        **spec,
        "title": "Does movement resume from this paused frame?",
        "continue": False,
        "fork": {"run": parent["id"], "trial": "menu", "frame": 60},
        "duration": 90,
        "trials": [
            {"id": "paused", "label": "Stay in the menu", "input": "wait 30\nhold UP 60"},
            {"id": "unpause", "label": "Close the menu", "input": "tap B\nwait 20\nhold UP 60"},
        ],
        "checks": [
            {"statement": "The paused fork holds the same position.", "trial": "paused", "from": 30,
             "field": "gPlayers[0]->position.z", "compare": "eq", "value": 0},
            {"statement": "Closing the menu from the same frame restores movement.", "trial": "unpause",
             "from": 30, "field": "gPlayers[0]->position.z", "compare": "ne", "value": 0},
        ],
    })


def artifact_paths(directory):
    return {name: str((directory / path).relative_to(runner.ROOT)) for name, path in
            (("result", "result.json"), ("transcript", "transcript.txt"), ("contact_sheet", "contact.png"))}


def build_pixel_proof(directory, result):
    from PIL import Image, ImageDraw

    x, y = 117, 88
    evidence = graphics.inspect(directory, result, "field", 60, x, y, runner.ROM)
    if len(evidence["objects"]) != 1 or len(evidence["objects"][0]["submissions"]) != 1:
        raise RuntimeError("The sprite proof did not find one matching hardware object and submission")
    submission = evidence["objects"][0]["submissions"][0]
    if (submission["runtime_pc"] != "030000E0" or submission["actor"]["member"] != "gPlayers[0]->animation" or
            submission["asset"]["hash"] != "DE23"):
        raise RuntimeError("The sprite proof did not recover Django's renderer, typed owner and ROM art")
    slot = evidence["objects"][0]["slot"]
    rendered = pixel_proof.execute(directory, result, "field", 60, x, y, slot)
    if not rendered["changed"]:
        raise RuntimeError("Hiding Django's hardware slot did not change the selected pixel")
    evidence["render_check"] = rendered
    runner.atomic_json(directory / "pixel.json", evidence)
    lines = [
        f"Field frame 60, pixel ({x}, {y})",
        f"OBJ slot {evidence['objects'][0]['slot']} in the exact captured OAM upload",
        f"{submission['runtime_pc']} {submission['function']['name']} in {submission['function']['source']}",
        f"{submission['actor']['member']} at {submission['drawable']}",
        f"{submission['asset']['archive']}/{submission['asset']['hash']} from {submission['asset']['tile_source']}",
        f"Hiding just OBJ {slot} changed RGB {rendered['before']} -> {rendered['after']}.",
        f"The control reproduced the original image. Both replays have state SHA-256 "
        f"{rendered['matching_final_state_sha256']}.",
    ]
    with Image.open(directory / "field/f0060.png") as image:
        panel = Image.new("RGB", (540, 368), "#181b25")
        panel.paste(image.resize((480, 320), Image.Resampling.NEAREST), (30, 30))
    draw = ImageDraw.Draw(panel)
    dx, dy = 30 + x * 2, 30 + y * 2
    draw.rectangle((dx - 4, dy - 4, dx + 6, dy + 6), outline="#ffe19d")
    draw.line((dx + 7, dy - 2, dx + 32, dy - 2, dx + 48, dy - 19), fill="#ffe19d")
    draw.text((dx + 51, dy - 27), "Django's sprite submission", fill="#ffe19d",
              stroke_width=2, stroke_fill="#181b25")
    draw.text((30, 11), lines[0], fill="white")
    panel.save(directory / "pixel.png")
    with (directory / "transcript.txt").open("a") as log:
        log.write("\nSprite provenance\n" + "\n".join(lines) + "\n")
    return {"evidence": str((directory / "pixel.json").relative_to(runner.ROOT)),
            "frame": str((directory / "pixel.png").relative_to(runner.ROOT)),
            "pixel": [x, y],
            "isolation_contact_sheet": str((directory / "pixel-proofs" / rendered["id"] / "contact.png").relative_to(runner.ROOT))}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("proof", help="Run and check the example experiment")
    run = commands.add_parser("run", help="Run a recipe JSON file")
    run.add_argument("recipe", type=Path)
    run.add_argument("--trans", type=Path, help="Local translation checkout with story checkpoints")
    cohort = commands.add_parser("sweep", help="Apply the same recipe to several story checkpoints")
    cohort.add_argument("recipe", type=Path)
    cohort.add_argument("--checkpoint", action="append", default=[], help="Checkpoint ID, repeat to add")
    cohort.add_argument("--all-story", action="store_true", help="Use the complete available story bank")
    cohort.add_argument("--trans", type=Path, help="Local translation checkout with story checkpoints")
    show = commands.add_parser("show", help="Read a completed run's JSON result")
    show.add_argument("id")
    commands.add_parser("checkpoints", help="List available starting saves")
    measure = commands.add_parser("fields", help="Resolve scalar fields from the target headers")
    measure.add_argument("paths", nargs="+")
    inspect = commands.add_parser("snapshot", help="Read additional named fields from a captured frame")
    inspect.add_argument("id")
    inspect.add_argument("trial")
    inspect.add_argument("frame", type=int)
    inspect.add_argument("paths", nargs="+")
    pixel = commands.add_parser("pixel", help="Trace a sampled pixel into native sprite queues")
    pixel.add_argument("id")
    pixel.add_argument("trial")
    pixel.add_argument("frame", type=int)
    pixel.add_argument("x", type=int)
    pixel.add_argument("y", type=int)
    isolate = commands.add_parser("pixel-check", help="Replay the frame with one OBJ slot hidden")
    isolate.add_argument("id")
    isolate.add_argument("trial")
    isolate.add_argument("frame", type=int)
    isolate.add_argument("x", type=int)
    isolate.add_argument("y", type=int)
    isolate.add_argument("slot", type=int)
    import_live = commands.add_parser("import-live", help="Turn a Centaur recording into a replay recipe")
    import_live.add_argument("id")
    args = parser.parse_args()
    try:
        match args.command:
            case "sweep":
                spec = recipe.validate(runner.read_json(args.recipe))
                data = sweep.execute(spec, sweep.select(args.checkpoint, args.all_story, args.trans), args.trans)
            case "show":
                data = runner.read_json(runner.run_dir(args.id) / "result.json")
            case "checkpoints":
                data = runner.checkpoints()
            case "fields":
                data = [fields.measure(path).to_dict() for path in args.paths]
            case "import-live":
                data = centaur.import_recording(args.id)
            case "pixel" | "pixel-check":
                directory = runner.run_dir(args.id)
                original = runner.read_json(directory / "result.json")
                if args.command == "pixel-check":
                    data = pixel_proof.execute(directory, original, args.trial, args.frame, args.x, args.y, args.slot)
                else:
                    data = graphics.inspect(directory, original, args.trial, args.frame, args.x, args.y, runner.ROM)
            case "snapshot":
                data = runner.inspect_frame({"run": args.id, "trial": args.trial, "frame": args.frame}, args.paths)
            case "proof" | "run":
                spec = runner.sample_recipe() if args.command == "proof" else recipe.validate(runner.read_json(args.recipe))
                directory = runner.init_run(spec)
                result = runner.execute(directory, spec, getattr(args, "trans", None))
                if args.command == "proof":
                    field, menu, idle = result["trials"]
                    terminal = field["snapshots"][-1]["fields"][0]
                    writer = (terminal["last_write"] or {}).get("function") or {}
                    controlled = menu["snapshots"][-1]["fields"][0]["value"], idle["snapshots"][-1]["fields"][0]["value"]
                    if (writer.get("name") != "Terrain_SampleSurface" or
                            controlled != (field["snapshots"][0]["fields"][0]["value"],) * 2 or
                            not all(check["state"] == "confirmed" for check in result["checks"])):
                        raise RuntimeError("Proof did not distinguish the field, menu and idle trials")
                data = {"id": result["id"], **artifact_paths(directory)}
                if args.command == "proof":
                    data["pixel"] = build_pixel_proof(directory, result)
                    branch = proof_branch(result)
                    branch_path = runner.init_run(branch)
                    forked = runner.execute(branch_path, branch)
                    if not all(check["state"] == "confirmed" for check in forked["checks"]):
                        raise RuntimeError("The captured paused frame did not reproduce the original behavior")
                    data["forked_experiment"] = {"id": forked["id"], **artifact_paths(branch_path)}
                    attack = runner.sample_recipe("attack")
                    attack_path = runner.init_run(attack)
                    attack_run = runner.execute(attack_path, attack)
                    if not all(check["state"] == "confirmed" for check in attack_run["checks"]):
                        raise RuntimeError("The attack comparison did not reproduce its expected observations")
                    data["attack_experiment"] = {"id": attack_run["id"], **artifact_paths(attack_path)}
                    pause_cause = runner.sample_recipe("pause-cause")
                    cause_path = runner.init_run(pause_cause)
                    cause = runner.execute(cause_path, pause_cause)
                    external = cause["trials"][1]["snapshots"][-1]["fields"][1]
                    if (not all(check["state"] == "confirmed" for check in cause["checks"]) or
                            external["last_write"] or external["last_intervention"]["value"] != 4):
                        raise RuntimeError("The typed pause intervention did not reproduce the expected behavior")
                    data["pause_cause_experiment"] = {"id": cause["id"], **artifact_paths(cause_path)}
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        parser.exit(1, f"{runner.public_path(str(error))}\n")
    print(json.dumps(data, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()

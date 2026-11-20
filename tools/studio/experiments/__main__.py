"""Command-line entry point for the same experiment engine used by Studio."""
import argparse
import json
from pathlib import Path
import subprocess
import sys

from . import fields, recipe, runner


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


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("proof", help="Run and check the example experiment")
    run = commands.add_parser("run", help="Run a recipe JSON file")
    run.add_argument("recipe", type=Path)
    run.add_argument("--trans", type=Path, help="Local translation checkout with story checkpoints")
    show = commands.add_parser("show", help="Read a completed run's JSON result")
    show.add_argument("id")
    commands.add_parser("checkpoints", help="List available starting saves")
    measure = commands.add_parser("fields", help="Resolve scalar fields from the target headers")
    measure.add_argument("paths", nargs="+")
    args = parser.parse_args()
    try:
        match args.command:
            case "show":
                data = runner.read_json(runner.run_dir(args.id) / "result.json")
            case "checkpoints":
                data = runner.checkpoints()
            case "fields":
                data = [fields.measure(path).to_dict() for path in args.paths]
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
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        parser.exit(1, f"{runner.public_path(str(error))}\n")
    print(json.dumps(data, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()

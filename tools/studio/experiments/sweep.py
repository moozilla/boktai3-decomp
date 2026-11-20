"""Run the same question across real story checkpoints."""
import csv
from collections import Counter
import subprocess
from uuid import uuid4

from . import checks, recipe, runner

SWEEPS = runner.ROOT / "build/studio/experiments/sweeps"


def select(ids, all_story, trans):
    available = {row["id"]: row for row in runner.checkpoints(trans)}
    if all_story:
        if ids:
            raise ValueError("Choose individual checkpoints or all story saves")
        ids = [key for key, row in available.items() if row["kind"] == "bank"]
    if not ids or len(ids) != len(set(ids)):
        raise ValueError("Choose one or more distinct checkpoints")
    choices = []
    for identifier in ids:
        if identifier not in available:
            raise ValueError(f"Checkpoint {identifier!r} is unavailable")
        choices.append(available[identifier])
    return choices


def write_ledger(directory, base, cohort):
    runner.atomic_json(directory / "result.json", {"recipe": base, "checkpoints": cohort})
    headers = ["checkpoint", "location", "run", "claim", "verdict", "measurement", "operator", "expected", "prerequisites"]
    preview = directory / "answers.tmp"
    with preview.open("w", newline="") as output:
        table = csv.DictWriter(output, fieldnames=headers, delimiter="\t")
        table.writeheader()
        for row in cohort:
            keys = {"checkpoint": row["id"], "location": row["label"], "run": row["run"]}
            for answer in row.get("checks", []):
                table.writerow({**keys, "claim": answer["statement"], "verdict": answer["state"],
                                "measurement": answer["measurement"], "operator": answer["compare"],
                                "expected": answer["value"],
                                "prerequisites": " | ".join(
                                    f"{checks.expression(p)} {'met' if p['meets'] else 'unresolved' if p['meets'] is None else 'unmet'}"
                                    for p in answer.get("prerequisites", []))})
            if not row.get("checks"):
                table.writerow({**keys, "claim": row.get("error", ""), "verdict": row["state"]})
    preview.replace(directory / "answers.tsv")


def contact_sheet(directory, cohort, title):
    from PIL import Image, ImageDraw

    columns = 8
    width, height = 148, 110
    rows = (len(cohort) + columns - 1) // columns
    page = Image.new("RGB", (columns * width + 16, rows * height + 54), "#181b25")
    draw = ImageDraw.Draw(page)
    draw.text((14, 12), title, fill="#f4f2ed")
    draw.text((14, 29), "Starting frames · confirmed / contradicted / unresolved", fill="#bbc5d5")
    for i, entry in enumerate(cohort):
        x, y = 14 + i % columns * width, 50 + i // columns * height
        if entry["state"] == "complete":
            still = runner.JOBS / entry["run"] / entry["trial"] / "f0000.png"
            with Image.open(still) as shot:
                page.paste(shot.resize((120, 80), Image.Resampling.NEAREST), (x, y))
            tally = Counter(check["state"] for check in entry["checks"])
            counts = " / ".join(str(tally[state]) for state in ("confirmed", "contradicted", "unresolved"))
            label = f"{entry['id']}  {counts}"
        else:
            draw.rectangle((x, y, x + 119, y + 79), fill="#322630")
            label = f"{entry['id']}  run failed"
        draw.text((x, y + 83), label, fill="#dce5ed")
    page.save(directory / "checkpoints.png")


def prepare(base, choices):
    """Persist an inspectable execution plan before launching its first core."""
    if base["fork"]:
        raise ValueError("A checkpoint sweep starts from saves or captures, not a captured experimental branch")
    directory = SWEEPS / uuid4().hex
    directory.mkdir(parents=True)
    runner.atomic_json(directory / "plan.json", {"recipe": base, "choices": choices})
    write_ledger(directory, base, [])
    runner.status(directory, "queued", "Waiting to run checkpoints", done=0, total=len(choices))
    return directory


def sweep_dir(identifier):
    if not isinstance(identifier, str) or not recipe.RUN_ID.fullmatch(identifier):
        raise ValueError("Invalid sweep ID")
    directory = SWEEPS / identifier
    if not (directory / "status.json").is_file():
        raise FileNotFoundError("Sweep not found")
    return directory


def recent():
    if not SWEEPS.is_dir():
        return []
    paths = (p for p in SWEEPS.iterdir() if recipe.RUN_ID.fullmatch(p.name) and (p / "plan.json").is_file())
    output = []
    for directory in sorted(paths, key=lambda p: p.stat().st_mtime_ns, reverse=True)[:20]:
        try:
            plan = runner.read_json(directory / "plan.json")
            state = runner.read_status(directory)
        except FileNotFoundError:
            continue
        output.append({**state, "title": plan["recipe"]["title"]})
    return output


def readback(directory):
    plan = runner.read_json(directory / "plan.json")
    return {**runner.read_json(directory / "result.json"), "choices": plan["choices"],
            "status": runner.read_status(directory)}


def execute(base, choices, trans=None, *, directory=None):
    directory = directory or prepare(base, choices)
    cohort = []
    for checkpoint in choices:
        runner.status(directory, "running", f"Running {checkpoint['label']}",
                      done=len(cohort), total=len(choices))
        kind = checkpoint["kind"]
        spec = recipe.validate({**base, "checkpoint": checkpoint["id"],
                                "continue": base["continue"] and kind in {"save", "bank"}})
        case = runner.init_run(spec, sweep_id=directory.name)
        row = {**checkpoint, "run": case.name}
        try:
            result = runner.execute(case, spec, trans=trans)
        except (OSError, RuntimeError, ValueError, subprocess.SubprocessError) as error:
            row.update(state="failed", error=runner.public_path(str(error)))
        else:
            row.update(state="complete", checks=result["checks"],
                       trial=result["trials"][0]["id"], game_frame=result["trials"][0]["snapshots"][0]["frame"])
        cohort.append(row)
        write_ledger(directory, base, cohort)
    contact_sheet(directory, cohort, base["title"])
    runner.status(directory, "complete", "Checkpoint sweep complete", done=len(cohort), total=len(choices))
    output = directory.relative_to(runner.ROOT) if directory.is_relative_to(runner.ROOT) else directory
    return {
        "id": directory.name,
        "result": str(output / "result.json"),
        "answers": str(output / "answers.tsv"),
        "contact_sheet": str(output / "checkpoints.png"),
        "runs": [{"checkpoint": row["id"], "run": row["run"], "state": row["state"]} for row in cohort],
    }

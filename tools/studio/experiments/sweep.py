"""Run the same question across real story checkpoints."""
import csv
import subprocess
from uuid import uuid4

from . import recipe, runner

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
    headers = ["checkpoint", "location", "run", "claim", "verdict", "measurement", "operator", "expected"]
    preview = directory / "answers.tmp"
    with preview.open("w", newline="") as output:
        table = csv.DictWriter(output, fieldnames=headers, delimiter="\t")
        table.writeheader()
        for row in cohort:
            keys = {"checkpoint": row["id"], "location": row["label"], "run": row["run"]}
            for answer in row.get("checks", []):
                table.writerow({**keys, "claim": answer["statement"], "verdict": answer["state"],
                                "measurement": answer["measurement"], "operator": answer["compare"],
                                "expected": answer["value"]})
            if not row.get("checks"):
                table.writerow({**keys, "claim": row.get("error", ""), "verdict": row["state"]})
    preview.replace(directory / "answers.tsv")


def execute(base, choices, trans=None):
    if base["fork"]:
        raise ValueError("A checkpoint sweep starts from saves or captures, not a captured experimental branch")
    directory = SWEEPS / uuid4().hex
    directory.mkdir(parents=True)
    cohort = []
    write_ledger(directory, base, cohort)
    for checkpoint in choices:
        kind = checkpoint["kind"]
        spec = recipe.validate({**base, "checkpoint": checkpoint["id"],
                                "continue": base["continue"] and kind in {"save", "bank"}})
        case = runner.init_run(spec)
        row = {**checkpoint, "run": case.name}
        try:
            result = runner.execute(case, spec, trans=trans)
        except (OSError, RuntimeError, ValueError, subprocess.SubprocessError) as error:
            row.update(state="failed", error=runner.public_path(str(error)))
        else:
            row.update(state="complete", checks=result["checks"])
        cohort.append(row)
        write_ledger(directory, base, cohort)
    return {
        "id": directory.name,
        "result": str((directory / "result.json").relative_to(runner.ROOT)),
        "answers": str((directory / "answers.tsv").relative_to(runner.ROOT)),
        "runs": [{"checkpoint": row["id"], "run": row["run"], "state": row["state"]} for row in cohort],
    }

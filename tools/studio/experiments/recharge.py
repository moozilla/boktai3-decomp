"""Answer one player question entirely through the public experiment tools."""
from contextlib import contextmanager
import csv
import json
from pathlib import Path
import subprocess
import sys
from uuid import uuid4

from . import recipe, runner

FOLDER = runner.ROOT / "build/studio/experiments/recharge"
TITLE = "How much faster does holding A recharge Django?"
GAUGE = "gGameState->unk876"
INTENSITY = "gGameState->unk874"
ENERGY = "gPlayers[0]->currentStatB"
ENERGY_CAP = "gPlayers[0]->maximumStatB"
COVERAGE = "gGameState->flags868"
BASELINE = "gSessionState->solarBaseline"
SENSOR = "gSolarMeasurement"
RECHARGE = "AreaEffect_AccumulatePlayerResource"
ADDER = "Player_AddEnergy"
FIELDS = [ENERGY, GAUGE, INTENSITY, SENSOR, COVERAGE, BASELINE, ENERGY_CAP,
          "gAreaEffectManager->resourceFraction", "gAreaEffectManager->idleFrame"]
PROBES = [RECHARGE, ADDER, "Solar_UpdateMeasurement", "Solar_GetLevelForIntensity"]
FIRST, LAST = 300, 540
BASELINE_VALUE = 230
ENERGY_SEED = 100
ARMS = {"shade": "Outside coverage", "ambient": "Passive", "hold": "Hold A"}


def measurement_recipe(bars, sensor, coverage):
    arms = []
    for arm, label in ARMS.items():
        actions = {BASELINE: BASELINE_VALUE, ENERGY: ENERGY_SEED}
        if arm != "shade":
            actions[COVERAGE] = coverage | 2
        commands = (f"lux {sensor}\nwait 240\n" + ("hold A 300" if arm == "hold" else "wait 300"))
        arms.append({"id": arm, "label": label, "input": commands,
                     "interventions": [{"frame": 0, "field": path, "value": value} for path, value in actions.items()]})
    return {
        "version": recipe.VERSION, "title": f"{TITLE} · {bars} SUN bars", "checkpoint": "test-save", "continue": True,
        "duration": LAST, "sample_every": 60, "observe": FIELDS, "probe": PROBES, "trials": arms,
        "checks": [
            {"statement": "The gauge reached the selected SUN level.", "trial": "ambient", "field": GAUGE,
             "frame": LAST, "compare": "eq", "value": bars},
            {"statement": "The same light gave no energy outside coverage.", "trial": "shade", "field": ENERGY,
             "from": FIRST, "to": LAST, "compare": "eq", "value": 0},
        ],
    }


def reference_example():
    return measurement_recipe(5, 188, 0x24)


def survey_recipe():
    return {
        "version": recipe.VERSION, "title": "Read the reference recharge setup", "checkpoint": "test-save",
        "continue": True, "duration": 1, "sample_every": 1, "observe": [COVERAGE],
        "trials": [{"id": "reference", "input": "wait 1"}],
    }


def prepare():
    directory = FOLDER / uuid4().hex
    directory.mkdir(parents=True)
    runner.status(directory, "queued", "Waiting to test the 11 SUN levels", done=0, total=11)
    runner.atomic_json(FOLDER / "latest.json", {"id": directory.name})
    return directory


def latest():
    key = runner.read_json(FOLDER / "latest.json")["id"]
    if not recipe.RUN_ID.fullmatch(key):
        raise ValueError("Invalid recharge study ID")
    return FOLDER / key


@contextmanager
def tool_process(directory):
    """Run the public transport in a separate process and retain every message."""
    with (directory / "tool-transcript.jsonl").open("w") as transcript, (directory / "tool-stderr.txt").open("w") as errors:
        command = [sys.executable, "-m", "tools.studio.experiments", "tool-server"]
        process = subprocess.Popen(command, cwd=runner.ROOT, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                   stderr=errors, text=True, bufsize=1)
        counter = 0

        def call(tool, **arguments):
            nonlocal counter
            counter += 1
            request = {"id": counter, "tool": tool, "arguments": arguments}
            transcript.write(json.dumps({"request": request}, ensure_ascii=False) + "\n")
            transcript.flush()
            process.stdin.write(json.dumps(request, ensure_ascii=False) + "\n")
            process.stdin.flush()
            reply = process.stdout.readline()
            if not reply:
                raise RuntimeError(f"Tool {tool} closed the JSON stream")
            answer = json.loads(reply)
            transcript.write(json.dumps({"reply": answer}, ensure_ascii=False) + "\n")
            transcript.flush()
            if answer.get("id") != counter or "error" in answer:
                raise RuntimeError(f"Tool {tool} failed. {answer.get('error', 'Unexpected reply ID')}")
            return answer["result"]

        try:
            yield call
        finally:
            process.stdin.close()
            process.stdout.close()
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()


def raw_input(bars, low, high):
    if bars == 0:
        return 255
    if bars == 10:
        return BASELINE_VALUE - high - 20  # saturate safely despite sensor timing noise
    return BASELINE_VALUE - (low + high) // 2


def field_url(run, trial, frame, field):
    return f"/experiments#run/{run}/{frame}/{trial}/field/{FIELDS.index(field)}"


def assess_arm(call, run, trial, bars, bounds, threshold):
    samples = call("recorded_fields", run=run, trial=trial, frames=[FIRST, (FIRST + LAST) // 2, LAST],
                   paths=[ENERGY, GAUGE, INTENSITY, SENSOR, ENERGY_CAP, BASELINE])
    point = [sample["fields"] for sample in samples]
    lo, hi = bounds
    if any(p[GAUGE] != bars or not lo <= p[INTENSITY] <= hi or abs(p[SENSOR] - threshold) > 2 or
           p[BASELINE] != BASELINE_VALUE for p in point):
        raise RuntimeError(f"SUN level {bars} did not settle within its verified bounds in {trial}")
    start, end = point[0], point[-1]
    if start[ENERGY] >= start[ENERGY_CAP] or end[ENERGY] >= end[ENERGY_CAP]:
        raise RuntimeError(f"Energy hit the cap in {trial} at {bars} SUN bars")
    probe = call("probe_values", run=run, trial=trial, function=RECHARGE, register=1, first=FIRST, last=LAST)
    mode = {"shade": 0, "ambient": 1, "hold": 2}[trial] if bars else 0
    if probe["values"] != [{"value": mode, "count": LAST - FIRST}]:
        raise RuntimeError(f"SUN level {bars} did not stay in recharge mode {mode} in {trial}")
    writers = call("find_writers", run=run, trial=trial, field=ENERGY, first=FIRST, last=LAST - 1)["writers"]
    gain = end[ENERGY] - start[ENERGY]
    expected = {"shade": 0, "ambient": 2, "hold": 16}[trial] * bars
    if gain != expected or (gain and not any(w["function"] and w["function"]["name"] == ADDER for w in writers)):
        raise RuntimeError(f"Unexpected recharge measurement in {trial} at {bars} SUN bars")
    return {
        "gain": gain, "start": start[ENERGY], "end": end[ENERGY], "mode": mode, "entries": probe["count"],
        "intensity": end[INTENSITY], "sensor_reading": end[SENSOR], "writers": writers,
        "url": field_url(run, trial, LAST, ENERGY),
        "image": f"/api/experiments/runs/{run}/artifact/{trial}/f{LAST:04d}.png",
    }


def run_study(directory):
    result = {"id": directory.name, "title": TITLE, "rows": [], "window": [FIRST, LAST],
              "baseline": BASELINE_VALUE, "start_energy": ENERGY_SEED, "arms": ARMS}
    with tool_process(directory) as call:
        runner.status(directory, "running", "Checking the reference save", done=0, total=11)
        choices = call("list_checkpoints")
        if not any(c["id"] == "test-save" for c in choices):
            raise RuntimeError("The reference save is missing")
        survey = call("run_recipe", recipe_document=survey_recipe())
        start = {"run": survey["run"], "trial": "reference", "frame": 0}
        coverage = call("read_field", **start, field=COVERAGE)["fields"][0]["value"]
        calibration = call("read_field", **start, field=BASELINE)["fields"][0]["value"]
        if coverage is None or coverage & 2:
            raise RuntimeError("The reference start must have the global area coverage bit clear")
        result["setup"] = {"survey": survey["run"], "coverage_before": coverage, "coverage_after": coverage | 2,
                           "calibration_before": calibration}
        lower = call("read_solar_table", symbol="gUnk_08E8798A")
        upper = call("read_solar_table", symbol="gUnk_08E87974")
        rom = survey["identity"]["rom"]
        if rom != lower["rom_sha256"] or rom != upper["rom_sha256"]:
            raise RuntimeError("The ROM table and the running game have different fingerprints")
        result["tables"] = {"lower": lower, "upper": upper}
        result["source"] = {}
        for name, pc in (("calibration", "0822C198"), ("intensity", "0822C388"), ("gauge", "0822C238"),
                         ("coverage", "0804E10C"), ("accumulator", "0804E618"),
                         ("lower", lower["reader_pc"]), ("upper", upper["reader_pc"])):
            result["source"][name] = call("open_c", pc=pc)
        for lo, hi in zip(lower["entries"], upper["entries"]):
            bars = lo["bars"]
            status = f"Measuring {bars} SUN bars in three trials"
            runner.status(directory, "running", status, done=bars, total=11)
            bounds = lo["value"], hi["value"]
            threshold = raw_input(bars, *bounds)
            measured = call("run_recipe", recipe_document=measurement_recipe(bars, threshold, coverage))
            if measured["identity"]["rom"] != rom or not all(c["state"] == "confirmed" for c in measured["checks"]):
                raise RuntimeError(f"The experiment checks did not pass at {bars} SUN bars")
            run = measured["run"]
            cells = {trial: assess_arm(call, run, trial, bars, bounds, threshold) for trial in ARMS}
            result["rows"].append({"bars": bars, "bounds": [lo, hi], "raw_threshold": threshold, "run": run,
                                   "gauge_url": field_url(run, "ambient", LAST, GAUGE), "cells": cells})
            runner.atomic_json(directory / "result.json", result)
        unique = sorted({writer["pc"] for row in result["rows"] for cell in row["cells"].values()
                         for writer in cell["writers"]})
        result["writers"] = {pc: call("open_c", pc=pc) for pc in unique}
    return result


def capture_sheet(directory, result):
    from PIL import Image, ImageDraw

    canvas = Image.new("RGB", (805, 640), "#181b25")
    draw = ImageDraw.Draw(canvas)
    draw.text((13, 9), TITLE, fill="#f4f0ec")
    draw.text((13, 26), "Actual captured frames after 240 measured frames", fill="#bbc5d5")
    for j, label in enumerate(ARMS.values()):
        draw.text((71 + 242 * j, 47), label, fill="#ffe4aa")
    for i, level in enumerate((0, 5, 10)):
        row = result["rows"][level]
        y = 68 + 185 * i
        draw.text((13, y + 13), f"SUN {level}", fill="#ffe4aa")
        run = runner.run_dir(row["run"])
        for j, arm in enumerate(ARMS):
            x = 71 + 242 * j
            with Image.open(run / arm / f"f{LAST:04d}.png") as shot:
                canvas.paste(shot, (x, y))
            data = row["cells"][arm]
            draw.text((x, y + 163), f"+{data['gain']} ENE ({data['start']} to {data['end']})", fill="#dce5ed")
    canvas.save(directory / "contact.png")


def publish(directory, result):
    with (directory / "recharge.tsv").open("w", newline="") as source:
        table = csv.writer(source, delimiter="\t")
        table.writerow(["bars", "intensity_min", "intensity_max", "raw_sensor_threshold", *ARMS, "run"])
        for row in result["rows"]:
            table.writerow([row["bars"], *(v["value"] for v in row["bounds"]), row["raw_threshold"],
                            *(row["cells"][arm]["gain"] for arm in ARMS), row["run"]])
    text = [TITLE, "",
            f"Tested the reference save after setting its dark baseline to {BASELINE_VALUE}.",
            f"Starts with {ENERGY_SEED} ENE, below the stock maximum. Each row starts from the same checkpoint.",
            "The passive and hold-A trials enable the game's global area coverage bit.",
            f"Read ENE and SUN at frames {FIRST}, {(FIRST+LAST)//2}, and {LAST}.",
            "All 240 entries in each measurement window were recorded in the expected recharge mode.", "",
            "SUN | Intensity | Outside coverage | Passive | Hold A | Run"]
    for row in result["rows"]:
        lo, hi = (v["value"] for v in row["bounds"])
        values = " | ".join(f"+{row['cells'][arm]['gain']}" for arm in ARMS)
        text.append(f"{row['bars']:3} | {lo:3}–{hi:<3} | {values} | {row['run']}")
    text.extend(["", "Holding A gives eight times the passive rate on this loadout with coverage and SUN above zero.",
                 "Full SUN without coverage restored zero ENE. Cartridge thresholds are raw sensor counts.",
                 "Stock ROM bounds: gUnk_08E8798A and gUnk_08E87974. The normal gauge mapping uses C branches.",
                 "Stock mode logic: AreaEffect_AccumulatePlayerResource. Observed ENE writer: Player_AddEnergy.",
                 "tool-transcript.jsonl contains all requests and replies, including the code and live values."])
    (directory / "transcript.txt").write_text("\n".join(text) + "\n")
    capture_sheet(directory, result)
    runner.atomic_json(directory / "result.json", result)


def execute(directory=None):
    directory = directory or prepare()
    try:
        result = run_study(directory)
        publish(directory, result)
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        runner.status(directory, "failed", runner.public_path(str(error)), total=11)
        raise
    runner.status(directory, "complete", "11 SUN levels measured", done=11, total=11)
    root = directory.relative_to(runner.ROOT)
    return {"id": directory.name, **{key: str(root / name) for key, name in
            {"table": "recharge.tsv", "result": "result.json", "transcript": "transcript.txt",
             "tool_transcript": "tool-transcript.jsonl", "contact_sheet": "contact.png"}.items()}}

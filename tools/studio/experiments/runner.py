"""Prepare a shared savestate, run trials, and publish their evidence."""
import csv
import gzip
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import sys
import threading
import time
from uuid import uuid4

from . import annotations, centaur, checks, fields, recipe, references

ROOT = fields.ROOT
HERE = Path(__file__).resolve().parent
JOBS = ROOT / "build/studio/experiments/runs"
HARNESS = ROOT / "build/studio/experiments/harness"
ROM = ROOT / "baserom.gba"
REFERENCE = ROOT / "tests/saves/ShinBok2.sav"
CONTINUE = ROOT / "tests/save_menus_field.txt"
RUN_ID = recipe.RUN_ID
BUILD_LOCK = threading.Lock()


def atomic_json(path, value):
    temporary = path.with_suffix(".tmp")
    temporary.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n")
    temporary.replace(path)


def read_json(path):
    return json.loads(path.read_text())


def fingerprint(path):
    with path.open("rb") as source:
        digest = hashlib.sha256()
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
        return digest.hexdigest()


def public_path(path):
    return str(path).replace(str(ROOT), ".").replace(str(Path.home()), "~")


EXAMPLES = {"pause": "Pause and movement", "attack": "Tap or hold attack"}


def sample_recipe(name="pause"):
    if name not in EXAMPLES:
        raise FileNotFoundError("Experiment example not found")
    return recipe.validate(read_json(HERE / f"{name}.json"))


def init_run(spec):
    directory = JOBS / uuid4().hex
    directory.mkdir(parents=True)
    atomic_json(directory / "recipe.json", spec)
    status(directory, "queued", "Waiting for the emulator")
    return directory


def status(directory, state, message, **info):
    data = dict(id=directory.name, state=state, message=message, pid=os.getpid(), updated=time.time(), **info)
    atomic_json(directory / "status.json", data)
    return data


def read_status(directory):
    data = read_json(directory / "status.json")
    pid = data.pop("pid", None)
    updated = data.pop("updated", None)
    if data["state"] in {"queued", "running"}:
        active = True
        if pid and pid != os.getpid():
            try:
                os.kill(pid, 0)
            except ProcessLookupError:
                active = False
            except PermissionError:
                pass
        if not active or (updated is not None and time.time() - updated > 360):
            data.update(state="interrupted", message="The recording stopped before completing. Rerun its saved recipe.")
    return data


def run_dir(identifier):
    if not isinstance(identifier, str) or not RUN_ID.fullmatch(identifier):
        raise ValueError("Invalid run ID")
    directory = JOBS / identifier
    if not (directory / "status.json").is_file():
        raise FileNotFoundError("Run not found")
    return directory


def checkpoint_paths(trans=None):
    yield {"id": "boot", "label": "Power on", "kind": "boot"}
    if REFERENCE.is_file():
        yield {"id": "test-save", "label": "Reference save", "kind": "save", "path": REFERENCE}
    yield from centaur.bookmarks()
    yield from centaur.recordings()
    if trans is None:
        candidates = (ROOT.parent / "boktai3trans", ROOT.parent.parent / "boktai3trans")
        trans = next((path for path in candidates if (path / "saves").is_dir()), None)
    if not trans or not (directory := Path(trans) / "saves").is_dir():
        return
    if str(ROOT / "tools/assets") not in sys.path:
        sys.path.insert(0, str(ROOT / "tools/assets"))
    import saves
    for row in saves.bank(str(directory)):
        if "error" in row:
            continue
        identifier = "bank-" + row["file"].split(" ", 1)[0]
        if recipe.CHECKPOINT_ID.fullmatch(identifier):
            yield {"id": identifier, "label": row["label"], "kind": "bank",
                   "story": row["story"], "scene": row["scene"], "path": directory / row["file"]}


def checkpoints(trans=None):
    return [{k: v for k, v in c.items() if k != "path"} for c in checkpoint_paths(trans)]


def build_harness():
    with BUILD_LOCK:
        command = ["make", "-s", "-f", "tools/emu/Makefile", str(HARNESS.relative_to(ROOT))]
        process = subprocess.run(command, cwd=ROOT, text=True, capture_output=True, timeout=60)
        if process.returncode:
            raise RuntimeError("Could not build the observation harness. " + public_path(process.stderr[-1800:]))
    return HARNESS


def run_harness(binary, directory, lines, rtc, battery=None):
    script = directory / "commands.txt"
    script.write_text("\n".join(lines) + "\n")
    env = {key: value for key, value in os.environ.items() if not key.startswith("BOKTAI3_")}
    env.update(BOKTAI3_RTC=str(rtc), BOKTAI3_OBSERVATIONS_ONLY="1")
    if battery:
        env["BOKTAI3_SAV"] = str(battery)
    log = directory / "harness.log"
    with log.open("wb") as output:
        process = subprocess.run([binary, ROM, script, directory], cwd=ROOT,
                                 stdout=output, stderr=subprocess.STDOUT, env=env, timeout=45)
    if process.returncode:
        raise RuntimeError("mGBA: " + public_path(log.read_text(errors="replace")[-1600:]))


def initial_commands(spec, starting_lux, forked, starting_capture):
    result = ["trace off"]
    if forked:
        result.append("load parent")
    if starting_capture:
        result.append("loadstate captured.ss")
    result.extend((f"lux {starting_lux}", f"rtc {spec['rtc']}"))
    if spec["continue"]:
        # Keep the known reference boot inputs shared with the harness tests.
        prefix = CONTINUE.read_text().split("\nsave ingame\n", 1)[0]
        result.extend(recipe.instruction(*step) for step in recipe.compile_steps(prefix, recipe.MAX_PREPARE))
    result.extend(recipe.instruction(*step) for step in recipe.compile_steps(spec["prepare"], recipe.MAX_PREPARE))
    result.extend(("shot fork", "save fork"))
    return result


def prepare_battery(checkpoint, directory):
    if checkpoint["kind"] == "save":
        return checkpoint["path"]
    if checkpoint["kind"] == "bank":
        if str(ROOT / "tools/assets") not in sys.path:
            sys.path.insert(0, str(ROOT / "tools/assets"))
        import saves
        path = directory / "checkpoint.sav"
        saves.write_sav(saves.current(saves.read_sgm(checkpoint["path"])), path)
        return path
    return None


def captured_frame(reference, rom_id):
    """Resolve a validated local run, trial and sample to a checked savestate."""
    identifier, trial_id, frame = reference["run"], reference["trial"], reference["frame"]
    if not isinstance(trial_id, str) or not recipe.TRIAL_ID.fullmatch(trial_id) or type(frame) is not int:
        raise ValueError("Invalid captured frame")
    directory = run_dir(identifier)
    result = read_json(directory / "result.json")
    if result["identity"]["rom"] != rom_id:
        raise ValueError("The frame was recorded with a different ROM")
    trial = next((row for row in result["trials"] if row["id"] == trial_id), None)
    point = next((row for row in trial["snapshots"] if row["relative"] == frame), None) if trial else None
    if not point or not point.get("state_sha256"):
        raise ValueError("Choose a captured sample that has a saved state")
    image = directory / trial_id / f"f{frame:04d}.png"
    data = gzip.decompress(image.with_suffix(".ss.gz").read_bytes())
    if hashlib.sha256(data).hexdigest() != point["state_sha256"]:
        raise ValueError("The saved frame changed on disk")
    return result, trial, point, image, data


def prepare_fork(spec, directory, rom_id):
    """Restore only a captured sample of a completed local run."""
    fork = spec["fork"]
    if not fork:
        return None, None
    result, trial, point, image, data = captured_frame(fork, rom_id)
    (directory / "parent.ss").write_bytes(data)
    checkpoint = {**result["checkpoint"], "kind": "fork",
                  "label": f"{trial['label']} at frame {point['relative']}",
                  "fork": {**fork, "state_sha256": point["state_sha256"]}}
    return {"checkpoint": checkpoint, "point": point}, image


def inspect_frame(reference, paths):
    """Read newly selected fields from a frame without advancing it."""
    paths = recipe.strings(paths, recipe.MAX_FIELDS, "Observed fields")
    if not paths:
        raise ValueError("Observe at least one typed field")
    result, trial, point, _, state = captured_frame(reference, fingerprint(ROM))
    observed = [fields.measure(path) for path in paths]
    binary = build_harness()
    with tempfile.TemporaryDirectory(prefix="inspect-", dir=HARNESS.parent) as temporary:
        directory = Path(temporary)
        (directory / "frame.ss").write_bytes(state)
        commands = ["trace off", "load frame", *(f.command() for f in observed), "sample frame"]
        run_harness(binary, directory, commands, result["recipe"]["rtc"])
        snaps, _ = parse_events(directory / "observations.tsv", observed, fields.function_index())
    if len(snaps) != 1 or snaps[0]["frame"] != point["frame"]:
        raise RuntimeError("The readback did not restore the requested frame")
    return {"run": reference["run"], "trial": trial["id"], "frame": point["relative"],
            "game_frame": point["frame"], "state_sha256": point["state_sha256"],
            "fields": [{**field.to_dict(), "value": row["value"], "resolved_address": row["address"]}
                       for field, row in zip(observed, snaps[0]["fields"])]}


def light_at_sample(commands, starting_lux):
    """Luminance belongs to the emulator peripheral, outside raw GBA states."""
    light = starting_lux
    found = {}
    for command in commands:
        verb, _, arg = command.partition(" ")
        if verb == "sample":
            found[arg] = light
        elif verb == "lux":
            light = int(arg)
    return found, light


def format_address(address):
    return f"{address:08X}"


def parse_events(path, observed, index):
    """Replay observations in file order. An unobserved value invalidates its writer."""
    snapshots = {}
    latest = [None] * len(observed)
    previous = [None] * len(observed)
    writes = []
    activity = [0] * len(observed)
    binding_epoch = [0] * len(observed)
    with path.open() as source:
        for row in csv.DictReader(source, delimiter="\t"):
            field_index = int(row["field"])
            field = observed[field_index]
            address = int(row["address"], 16)
            value = field.decode(row["value"]) if address else None
            frame, sequence = int(row["frame"]), int(row["sequence"])
            event = {"frame": frame, "sequence": sequence, "field": field_index,
                     "address": row["address"] if address else None, "value": value}
            match row["kind"]:
                case "write" | "bind":
                    pc = int(row["pc"], 16)
                    kind = row["kind"]
                    context = []
                    for pair in (row.get("context") or "").split(","):
                        if pair:
                            entered, resume = pair.split(":")
                            context.append({"entry": entered, "resume": resume,
                                            "function": index.lookup(int(entered, 16))})
                    event.update(kind=kind, pc=format_address(pc), function=index.lookup(pc), context=context)
                    if kind == "write":
                        event["before"] = field.decode(row["before"])
                        latest[field_index] = event
                        activity[field_index] += 1
                    else:
                        event["before_address"] = row["before"]
                        binding_epoch[field_index] += 1
                        latest[field_index] = None
                    writes.append(event)
                case "sample":
                    writer = latest[field_index]
                    if writer and (writer["address"], writer["value"]) != (event["address"], value):
                        latest[field_index] = writer = None
                    before = previous[field_index]
                    if (before is not None and before["address"] != event["address"] and
                            before["binding"] == binding_epoch[field_index]):
                        # The pointer may also have moved through an unobserved path.
                        binding_epoch[field_index] += 1
                    event["binding"] = binding_epoch[field_index]
                    state = "unbound" if value is None else "initial"
                    if before is not None and value is not None:
                        if before["binding"] != event["binding"]:
                            state = "rebound"
                            if writer and not (before["sequence"] < writer["sequence"]):
                                writer = None
                        elif before["value"] == value:
                            state = "unchanged"
                        elif writer and before["sequence"] < writer["sequence"]:
                            state = "observed"
                        else:
                            state = "unobserved"
                    snap = snapshots.setdefault(row["label"], {"label": row["label"], "frame": frame, "fields": []})
                    snap["fields"].append({**event, "previous": before["value"] if before else None,
                                           "state": state, "last_write": writer,
                                           "writes_since_sample": activity[field_index]})
                    previous[field_index] = event
                    activity[field_index] = 0
                case other:
                    raise RuntimeError(f"Unknown observer event {other!r}")
    return list(snapshots.values()), writes


def read_probes(path, probe_names, index, start_frame):
    results = [{"name": name, "function": index.lookup(address), "frames": {}}
               for name, address in probe_names]
    if not results:
        return results
    with path.open() as source:
        for entry in csv.DictReader(source, delimiter="\t"):
            result = results[int(entry["probe"][1:])]
            f = int(entry["frame"]) - start_frame
            result["frames"][f] = result["frames"].get(f, 0) + 1
    for result in results:
        result["count"] = sum(result["frames"].values())
    return results


def analyze_trial(directory, observed, index, probes, sensor):
    from PIL import Image
    snapshots, writes = parse_events(directory / "observations.tsv", observed, index)
    if not snapshots or snapshots[0]["label"] != "f0000":
        raise RuntimeError("mGBA did not return a starting frame")
    start_frame = snapshots[0]["frame"]
    for snap in snapshots:
        snap["relative"] = int(snap["label"][1:])
        if snap["frame"] != start_frame + snap["relative"] or len(snap["fields"]) != len(observed):
            raise RuntimeError("The sample timeline disagrees with the emulator")
        name = snap["label"]
        state = directory / f"{name}.ss"
        raw = state.read_bytes()
        snap["state_sha256"] = hashlib.sha256(raw).hexdigest()
        snap["sensor"] = sensor[name]
        state.with_suffix(".ss.gz").write_bytes(gzip.compress(raw, compresslevel=3, mtime=0))
        state.unlink()
        with Image.open(directory / f"{name}.ppm") as image:
            image.save(directory / f"{name}.png")
        (directory / f"{name}.ppm").unlink()
    trial = {"snapshots": snapshots, "write_count": len(writes),
             "probes": read_probes(directory / "probes.tsv", probes, index, start_frame)}
    return trial, writes


def build_proof_artifacts(directory, result):
    from PIL import Image, ImageDraw, ImageFont
    trials = result["trials"]
    points = [0, len(trials[0]["snapshots"]) // 2, len(trials[0]["snapshots"]) - 1]
    width = 260
    height = 190
    sheet = Image.new("RGB", (width * len(points) + 160, len(trials) * height + 48), "#181b25")
    draw = ImageDraw.Draw(sheet)
    font = ImageFont.load_default()
    draw.text((16, 15), result["recipe"]["title"], fill="#ffffff", font=font)
    for j, trial in enumerate(trials):
        y = j * height + 45
        draw.text((16, y + 12), trial["label"], fill="#fff0c7", font=font)
        for i, point in enumerate(points):
            snap = trial["snapshots"][point]
            x = 160 + i * width
            draw.text((x, y + 1), f"Frame {snap['relative']:04d}", fill="#aab4c9", font=font)
            with Image.open(directory / trial["id"] / f"{snap['label']}.png") as image:
                sheet.paste(image, (x, y + 18))
    sheet.save(directory / "contact.png")

    lines = [result["recipe"]["title"], f"Run {result['id']}", "",
             f"Checkpoint: {result['checkpoint']['label']}",
             f"ROM SHA-256: {result['identity']['rom']}",
             f"Fork SHA-256: {result['identity']['fork']}",
             f"mGBA harness SHA-256: {result['identity']['harness']}",
             "Write evidence covers value-changing CPU stores within the trial.", ""]
    if result["checkpoint"].get("fork"):
        fork = result["checkpoint"]["fork"]
        lines.extend([f"Forked from run {fork['run']} / {fork['trial']} / frame {fork['frame']}",
                      f"Captured parent state SHA-256: {fork['state_sha256']}", ""])
    for check in result["checks"]:
        estimate = check["measurement"] if check["measurement"] is not None else "unknown"
        lines.append(f"{check['state'].upper()}: {check['statement']} ({estimate} {check['sign']} {check['value']})")
    lines.append("")
    for trial in trials:
        lines.append(trial["label"])
        for f, start, end in zip(result["fields"], trial["snapshots"][0]["fields"], trial["snapshots"][-1]["fields"]):
            provenance = end["last_write"]
            tail = ""
            if provenance:
                who = provenance["function"]["name"] if provenance["function"] else "unmapped"
                tail = f" (last changed by {who} at {provenance['pc']}, game frame {provenance['frame']})"
                compiler = provenance.get("compiler", {})
                if "file" in compiler:
                    tail += f" [{compiler['instruction']} · {compiler['file']}:{compiler['line']}]"
                context = [link["function"]["name"] for link in provenance.get("context", []) if link["function"]]
                if context:
                    tail += f" {{active probed entries: {' → '.join(context)}}}"
            lines.append(f"  {f['path']}: {start['value']} -> {end['value']}{tail}")
        lines.extend(f"  {p['name']}: {p['count']} observed entries" for p in trial["probes"])
        lines.append("")
    (directory / "transcript.txt").write_text("\n".join(lines))


def execute(directory, spec, trans=None):
    """Build and run an immutable recipe. Publish failure details as job state."""
    try:
        status(directory, "running", "Measuring named fields with agbcc")
        observed = [fields.measure(path) for path in spec["observe"]]
        index = fields.function_index()
        probes = []
        for name in spec["probe"]:
            address = index.by_name.get(name)
            if address is None:
                raise ValueError(f"Unknown function {name!r}")
            probes.append((name, address))
        status(directory, "running", "Booting the shared starting state")
        binary = build_harness()
        prefix = directory / "prepare"
        prefix.mkdir()
        rom_id, harness_id = fingerprint(ROM), fingerprint(binary)
        parent, seed_picture = prepare_fork(spec, prefix, rom_id)
        external = None
        if parent:
            checkpoint = parent["checkpoint"]
            battery = None
            starting_lux = parent["point"]["sensor"]
        else:
            checkpoint = next((c for c in checkpoint_paths(trans) if c["id"] == spec["checkpoint"]), None)
            if not checkpoint:
                raise ValueError("The selected checkpoint is unavailable")
            battery = prepare_battery(checkpoint, prefix)
            starting_lux = 232
            if checkpoint["kind"] in {"bookmark", "recording"}:
                external, seed_picture = centaur.stage(checkpoint, prefix)
        boot_commands = initial_commands(spec, starting_lux, bool(parent), bool(external))
        run_harness(binary, prefix, boot_commands, spec["rtc"], battery)
        _, starting_lux = light_at_sample(boot_commands, starting_lux)
        if seed_picture and not any(step[0] for step in recipe.compile_steps(spec["prepare"], recipe.MAX_PREPARE)):
            from PIL import Image
            with Image.open(seed_picture) as image:
                image.save(prefix / "fork.ppm")
        fork = prefix / "fork.ss"
        identity = {"rom": rom_id, "fork": fingerprint(fork), "harness": harness_id,
                    "symbols": fingerprint(ROOT / "symbols/functions.csv"),
                    "ram_symbols": fingerprint(ROOT / "symbols/ram.csv"),
                    "extents": fingerprint(ROOT / "PROGRESS.json")}
        if battery:
            identity["battery"] = fingerprint(battery)
        if external:
            identity["live_state"] = fingerprint(external)
        head = subprocess.run(["git", "rev-parse", "HEAD"], cwd=ROOT, text=True, capture_output=True, check=False)
        identity["source_revision"] = head.stdout.strip() if head.returncode == 0 else None
        result = {"id": directory.name, "recipe": spec, "identity": identity,
                  "checkpoint": {k: v for k, v in checkpoint.items() if k != "path"},
                  "fields": [f.to_dict() for f in observed], "trials": []}
        writes_by_trial = {}
        for trial in spec["trials"]:
            status(directory, "running", f"Recording {trial['label']}")
            case = directory / trial["id"]
            case.mkdir()
            shutil.copyfile(fork, case / "fork.ss")
            cmds = ["trace off", "load fork", f"lux {starting_lux}", f"rtc {spec['rtc']}",
                    *(f.command() for f in observed)]
            cmds.extend(f"probe p{i} {address:08X}" for i, (_, address) in enumerate(probes))
            cmds.extend(recipe.timeline(recipe.compile_steps(trial["input"], spec["duration"]),
                                        spec["duration"], spec["sample_every"], save_states=True))
            run_harness(binary, case, cmds, spec["rtc"])
            shutil.copyfile(prefix / "fork.ppm", case / "f0000.ppm")
            sensor, _ = light_at_sample(cmds, starting_lux)
            evidence, writes_by_trial[trial["id"]] = analyze_trial(case, observed, index, probes, sensor)
            result["trials"].append({**trial, **evidence})
            (case / "fork.ss").unlink()
        start = result["trials"][0]["snapshots"][0]
        for trial in result["trials"]:
            sample = trial["snapshots"][0]
            if sample["frame"] != start["frame"] or [f["value"] for f in sample["fields"]] != [f["value"] for f in start["fields"]]:
                raise RuntimeError("The trials did not start from the same state")
        status(directory, "running", "Linking observed stores to exact C and compiler output")
        result["compiler"] = annotations.annotate(directory.name, [event for log in writes_by_trial.values() for event in log])
        for trial, events in writes_by_trial.items():
            atomic_json(directory / trial / "writes.json", events)
        atomic_json(directory / "functions.json", references.summarize(result, writes_by_trial))
        result["checks"] = checks.evaluate(spec["checks"], result)
        build_proof_artifacts(directory, result)
        atomic_json(directory / "result.json", result)
        status(directory, "complete", "Experiment complete")
        return result
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        status(directory, "failed", public_path(str(error)))
        raise

"""Local tools for asking a question and following its answer into matching C."""
from collections import Counter
import csv
import json
import re
import struct
import subprocess
import sys

from . import fields, graphics, recipe, runner


def run_result(run):
    directory = runner.run_dir(run)
    return directory, runner.read_json(directory / "result.json")


def list_checkpoints():
    return runner.checkpoints()


def run_recipe(recipe_document):
    spec = recipe.validate(recipe_document)
    directory = runner.init_run(spec)
    result = runner.execute(directory, spec)
    return {
        "run": result["id"], "title": spec["title"], "checkpoint": result["checkpoint"],
        "trials": [{"id": trial["id"], "frames": [s["relative"] for s in trial["snapshots"]]}
                   for trial in result["trials"]],
        "checks": result["checks"], "identity": result["identity"], "url": f"/experiments#run/{result['id']}",
        "transcript": str((directory / "transcript.txt").relative_to(runner.ROOT)),
    }


def read_field(run, trial, frame, field):
    return runner.inspect_frame({"run": run, "trial": trial, "frame": frame}, [field])


def recorded_fields(run, trial, frames, paths):
    """Read selected observations in bulk. Use read_field for additional fields."""
    _, result = run_result(run)
    selected = next((item for item in result["trials"] if item["id"] == trial), None)
    if not selected:
        raise ValueError("Unknown trial ID")
    if not isinstance(frames, list) or not 1 <= len(frames) <= recipe.MAX_SAMPLES:
        raise ValueError("Supply one or more captured frame numbers")
    frames = [recipe.bounded_int(frame, 0, recipe.MAX_FRAMES, "Sampled frame") for frame in frames]
    paths = recipe.strings(paths, recipe.MAX_FIELDS, "Observed fields")
    by_path = {row["path"]: slot for slot, row in enumerate(result["fields"])}
    samples = {s["relative"]: s for s in selected["snapshots"]}
    if not paths or any(path not in by_path for path in paths):
        raise ValueError("Use field paths recorded in this trial")
    if any(frame not in samples for frame in frames):
        raise ValueError("Use frame numbers recorded in this trial")
    return [{"frame": frame, "fields": {path: samples[frame]["fields"][by_path[path]]["value"] for path in paths},
             "url": f"/experiments#run/{run}/{frame}/{trial}"} for frame in frames]


def find_writers(run, trial, field, first=0, last=recipe.MAX_FRAMES):
    """Summarize actual CPU writers by instruction, ignoring authored input writes."""
    directory, result = run_result(run)
    case = next((item for item in result["trials"] if item["id"] == trial), None)
    index = next((i for i, item in enumerate(result["fields"]) if item["path"] == field), None)
    if not case or index is None:
        raise ValueError("Choose a trial and a field watched in this run")
    first = recipe.bounded_int(first, 0, recipe.MAX_FRAMES, "First frame")
    last = recipe.bounded_int(last, first, recipe.MAX_FRAMES, "Last frame")
    origin = case["snapshots"][0]["frame"]
    groups = {}
    for event in runner.read_json(directory / trial / "writes.json"):
        relative = event["frame"] - origin
        if event["field"] != index or event["kind"] != "write" or not first <= relative <= last:
            continue
        writer = groups.setdefault(event["pc"], {
            "pc": event["pc"], "function": event["function"], "compiler": event.get("compiler"),
            "count": 0, "first": relative, "first_value": event["before"],
        })
        writer.update(count=writer["count"] + 1, last=relative, last_value=event["value"])
    for writer in groups.values():
        frame = next((s["relative"] for s in case["snapshots"] if s["relative"] > writer["last"]),
                     case["snapshots"][-1]["relative"])
        writer["url"] = f"/experiments#run/{run}/{frame}/{trial}/field/{index}"
    rows = sorted(groups.values(), key=lambda row: (-row["count"], row["pc"]))
    return {"run": run, "field": field, "trial": trial, "count": len(rows), "writers": rows[:32]}


def probe_values(run, trial, function, register, first=0, last=recipe.MAX_FRAMES):
    """Summarize one selected register at observed entries."""
    directory, result = run_result(run)
    case = next((item for item in result["trials"] if item["id"] == trial), None)
    if not case or function not in result["recipe"]["probe"]:
        raise ValueError("Choose a recorded trial and function probe")
    register = recipe.bounded_int(register, 0, 15, "Register")
    first = recipe.bounded_int(first, 0, recipe.MAX_FRAMES, "First frame")
    last = recipe.bounded_int(last, first, recipe.MAX_FRAMES, "End frame")
    probe = "p" + str(result["recipe"]["probe"].index(function))
    origin = case["snapshots"][0]["frame"]
    counts = Counter()
    with (directory / trial / "probes.tsv").open() as trace:
        for row in csv.DictReader(trace, delimiter="\t"):
            if row["probe"] == probe and first <= int(row["frame"]) - origin < last:
                counts[int(row[f"r{register}"], 16)] += 1
    return {"count": sum(counts.values()), "distinct": len(counts),
            "values": [{"value": v, "count": c} for v, c in counts.most_common(32)]}


def open_c(pc):
    if not isinstance(pc, str) or not re.fullmatch(r"(?:0[xX])?[0-9a-fA-F]{8}", pc):
        raise ValueError("Use an eight-digit hexadecimal program counter")
    address = f"{int(pc, 16) & ~1:08X}"
    match = graphics.cpu_function(address, fields.function_index())
    if not match:
        raise ValueError(f"No indexed function contains {address}")
    for folder in (runner.ROOT / "tools/studio", runner.ROOT / "tools", runner.ROOT / "tools/assets"):
        if str(folder) not in sys.path:
            sys.path.insert(0, str(folder))
    import codemap

    # Use the same function-body extractor as Studio Code. The full page also
    # builds disassembly, call graphs and Atlas cross-references.
    source = codemap._source({"file": match["source"], "name": match["name"],
                              "defined": match["defined"], "addr": match["address"], "kinds": []})
    return {"pc": address, "function": match,
            "c": {"file": source[0], "line": source[1], "text": source[2]} if source else None}


SOLAR_TABLE = {
    "gUnk_08E87974": (0x08E87974, "0822C290", "Inclusive upper intensity bounds"),
    "gUnk_08E8798A": (0x08E8798A, "0822C2A0", "Inclusive lower intensity bounds"),
}


def read_solar_table(symbol):
    if symbol not in SOLAR_TABLE:
        raise ValueError("Choose the upper or lower SUN gauge bounds table")
    address, reader_pc, label = SOLAR_TABLE[symbol]
    with runner.ROM.open("rb") as rom:
        rom.seek(address - 0x08000000)
        values = struct.unpack("<11H", rom.read(22))
    return {"symbol": symbol, "address": f"{address:08X}", "reader_pc": reader_pc,
            "url": f"/code#{reader_pc}", "label": label, "rom_sha256": runner.fingerprint(runner.ROM),
            "entries": [{"bars": i, "value": v, "address": f"{address + 2*i:08X}"} for i, v in enumerate(values)]}


def descriptor(name, fn, description, properties, required=None):
    return (fn, {"name": name, "description": description, "input_schema": {
        "type": "object", "properties": properties, "required": list(properties) if required is None else required,
        "additionalProperties": False,
    }})


STRING = {"type": "string"}
FRAME = {"type": "integer", "minimum": 0, "maximum": recipe.MAX_FRAMES}
RUN = {"run": {"type": "string", "pattern": "^[a-f0-9]{32}$"}, "trial": STRING}
TOOLS = dict((item[1]["name"], item) for item in [
    descriptor("list_checkpoints", list_checkpoints, "List available starts for a real Boktai 3 experiment", {}),
    descriptor("run_recipe", run_recipe, "Execute a version 1 recipe and save every sampled frame and result",
               {"recipe_document": {"type": "object", "description": "Use the documented Studio experiment recipe format."}}),
    descriptor("read_field", read_field, "Restore one sampled frame and read a named typed C field",
               {**RUN, "frame": FRAME, "field": STRING}),
    descriptor("recorded_fields", recorded_fields, "Return selected captured scalar measurements for several frames",
               {**RUN, "frames": {"type": "array", "items": FRAME}, "paths": {"type": "array", "items": STRING}}),
    descriptor("find_writers", find_writers, "Find observed value-changing CPU stores to a named field, grouped by PC",
               {**RUN, "field": STRING, "first": FRAME, "last": FRAME}, ["run", "trial", "field"]),
    descriptor("probe_values", probe_values, "Count values of a register at named probe entries over [first, last)",
               {**RUN, "function": STRING, "register": {"type": "integer", "minimum": 0, "maximum": 15},
                "first": FRAME, "last": FRAME}, ["run", "trial", "function", "register"]),
    descriptor("open_c", open_c, "Open the matching C body for a program counter and its Studio Code link",
               {"pc": {"type": "string", "pattern": "^(0[xX])?[0-9A-Fa-f]{8}$"}}),
    descriptor("read_solar_table", read_solar_table, "Read the original ROM's inclusive SUN gauge boundary table",
               {"symbol": {"type": "string", "enum": list(SOLAR_TABLE)}}),
])


def describe():
    return [schema for _, schema in TOOLS.values()]


def dispatch(request):
    if not isinstance(request, dict):
        return {"error": "Send an object with tool, arguments, and optional id"}
    answer = {"id": request.get("id")}
    name = request.get("tool")
    if not isinstance(name, str) or name not in TOOLS:
        return {**answer, "error": "Unknown tool"}
    arguments = request.get("arguments", {})
    fn, schema = TOOLS[name]
    keys = schema["input_schema"]
    if (not isinstance(arguments, dict) or set(arguments) - keys["properties"].keys() or
            set(keys["required"]) - arguments.keys()):
        return {**answer, "error": "Tool arguments do not match the schema"}
    try:
        return {**answer, "result": fn(**arguments)}
    except (OSError, RuntimeError, ValueError, TypeError, subprocess.SubprocessError) as error:
        return {**answer, "error": runner.public_path(str(error))}


def serve(source, output):
    for line in source:
        if not line.strip():
            continue
        if len(line) > recipe.MAX_BODY:
            answer = {"error": "Tool request exceeds the supported size"}
        else:
            try:
                answer = dispatch(json.loads(line))
            except json.JSONDecodeError:
                answer = {"error": "Tool request is not valid JSON"}
        print(json.dumps(answer, ensure_ascii=False), file=output, flush=True)

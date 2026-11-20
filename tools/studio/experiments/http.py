"""A bounded localhost API over the same recipes that the CLI executes."""
from concurrent.futures import ThreadPoolExecutor
import json
import re
import threading
import urllib.parse

from . import fields, recipe, runner

API = "/api/experiments"
PAGE = "/experiments"
ASSETS = {
    f"{PAGE}/app.js": ("app.js", "application/javascript; charset=utf-8"),
    f"{PAGE}/app.css": ("app.css", "text/css; charset=utf-8"),
}
POOL = ThreadPoolExecutor(max_workers=2, thread_name_prefix="studio-experiment")
SLOTS = threading.BoundedSemaphore(4)
FIXED_ARTIFACTS = {
    "contact.png": "image/png",
    "recipe.json": "application/json",
    "transcript.txt": "text/plain; charset=utf-8",
}
TRIAL_ARTIFACTS = {
    "commands.txt": "text/plain; charset=utf-8",
    "observations.tsv": "text/tab-separated-values",
    "probes.tsv": "text/tab-separated-values",
    "writes.json": "application/json",
}


def body(handler):
    origin = handler.headers.get("Origin")
    if origin and urllib.parse.urlparse(origin).netloc != handler.headers.get("Host"):
        raise ValueError("Experiments accept same-origin requests")
    if handler.headers.get_content_type() != "application/json":
        raise ValueError("Send application/json")
    length = int(handler.headers.get("Content-Length", "0"))
    if not 0 < length <= recipe.MAX_BODY:
        raise ValueError("Invalid recipe body length")
    return json.loads(handler.rfile.read(length))


def recent():
    if not runner.JOBS.is_dir():
        return []
    directories = sorted((path for path in runner.JOBS.iterdir() if runner.RUN_ID.fullmatch(path.name)),
                         key=lambda path: path.stat().st_mtime_ns, reverse=True)
    output = []
    for directory in directories[:30]:
        try:
            state = runner.read_status(directory)
            spec = runner.read_json(directory / "recipe.json")
        except FileNotFoundError:
            continue
        output.append({**state, "title": spec["title"], "checkpoint": spec["checkpoint"]})
    return output


def artifact(directory, parts):
    if len(parts) == 1 and parts[0] in FIXED_ARTIFACTS:
        return directory / parts[0], FIXED_ARTIFACTS[parts[0]]
    if len(parts) == 2:
        trial, name = parts
        ids = {case["id"] for case in runner.read_json(directory / "recipe.json")["trials"]}
        if trial in ids:
            if re.fullmatch(r"f[0-9]{4}\.png", name):
                return directory / trial / name, "image/png"
            if name in TRIAL_ARTIFACTS:
                return directory / trial / name, TRIAL_ARTIFACTS[name]
    raise FileNotFoundError("No such evidence artifact")


def execute_and_release(directory, spec, trans):
    try:
        runner.execute(directory, spec, trans)
    except Exception as error:
        runner.status(directory, "failed", runner.public_path(str(error)))
    finally:
        SLOTS.release()


def post_run(handler):
    spec = recipe.validate(body(handler))
    if not SLOTS.acquire(blocking=False):
        handler.send(429, {"error": "The experiment queue is full"})
        return
    try:
        directory = runner.init_run(spec)
        POOL.submit(execute_and_release, directory, spec, getattr(handler.server, "trans", None))
    except Exception:
        SLOTS.release()
        raise
    handler.send(202, {"id": directory.name, "url": f"{PAGE}#run/{directory.name}"})


def catalog(handler):
    rows = fields.ram_symbols((fields.ROOT / "symbols/ram.csv").stat().st_mtime_ns)
    symbols = [{"name": row["name"], "declaration": row["decl"], "header": row["header"]}
               for row in rows.values() if row["header"] and row["decl"]]
    handler.send(200, {"checkpoints": runner.checkpoints(getattr(handler.server, "trans", None)),
                       "ram": symbols,
                       "limits": {"trials": recipe.MAX_TRIALS, "fields": recipe.MAX_FIELDS,
                                  "frames": recipe.MAX_FRAMES, "samples": recipe.MAX_SAMPLES}})


def routes(handler, method):
    path = handler.path
    if path not in (PAGE, PAGE + ".html") and path not in ASSETS and not path.startswith(API + "/"):
        return False
    try:
        if method == "GET" and path in (PAGE, PAGE + ".html"):
            handler.send(200, (runner.HERE / "index.html").read_bytes(), "text/html; charset=utf-8")
        elif method == "GET" and path in ASSETS:
            file, mime = ASSETS[path]
            handler.send(200, (runner.HERE / file).read_bytes(), mime)
        elif method == "GET" and path == API + "/examples":
            handler.send(200, [{"id": name, "label": label} for name, label in runner.EXAMPLES.items()])
        elif method == "GET" and (path == API + "/example" or path.startswith(API + "/example/")):
            name = path.removeprefix(API + "/example").removeprefix("/") or "pause"
            handler.send(200, runner.sample_recipe(name))
        elif method == "GET" and path == API + "/catalog":
            catalog(handler)
        elif path == API + "/runs":
            if method == "POST":
                post_run(handler)
            else:
                handler.send(200, recent())
        elif method == "POST" and path == API + "/fields":
            paths = body(handler)
            if not isinstance(paths, list) or not 1 <= len(paths) <= recipe.MAX_FIELDS:
                raise ValueError("Send an array of 1–16 named fields")
            handler.send(200, [fields.measure(path).to_dict() for path in paths])
        elif method == "POST" and path.startswith(API + "/runs/"):
            identifier, *parts = path.removeprefix(API + "/runs/").split("/")
            if len(parts) != 3 or parts[0] != "snapshot" or not re.fullmatch(r"[0-9]{1,4}", parts[2]):
                raise FileNotFoundError("No such readback route")
            paths = body(handler)
            if not SLOTS.acquire(blocking=False):
                handler.send(429, {"error": "The emulator queue is full"})
                return True
            try:
                reference = {"run": identifier, "trial": parts[1], "frame": int(parts[2])}
                handler.send(200, runner.inspect_frame(reference, paths))
            finally:
                SLOTS.release()
        elif method == "GET" and path.startswith(API + "/runs/"):
            identifier, *parts = path.removeprefix(API + "/runs/").split("/")
            directory = runner.run_dir(identifier)
            if not parts:
                handler.send(200, runner.read_status(directory))
            elif parts == ["result"]:
                handler.send(200, runner.read_json(directory / "result.json"))
            elif parts[0] == "artifact":
                file, mime = artifact(directory, parts[1:])
                handler.send(200, file.read_bytes(), mime)
            else:
                raise FileNotFoundError("No such run route")
        else:
            raise FileNotFoundError("No such experiment route")
    except FileNotFoundError:
        handler.send(404, {"error": "Experiment, result, or artifact not found"})
    except (ValueError, UnicodeError) as error:
        handler.send(400, {"error": runner.public_path(str(error))})
    except (OSError, RuntimeError) as error:
        handler.send(500, {"error": runner.public_path(str(error))})
    return True

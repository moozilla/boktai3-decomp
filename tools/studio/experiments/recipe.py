"""Validate a small controller language and schedule frame-aligned samples."""
import re

VERSION = 1
MAX_BODY = 64 * 1024
MAX_TRIALS = 4
MAX_FIELDS = 16
MAX_PROBES = 12
MAX_PREPARE = 3600
MAX_FRAMES = 1800
MAX_SAMPLES = 61
DEFAULT_RTC = 1128150000
IDENTIFIER = re.compile(r"[A-Za-z_]\w*", re.ASCII)
TRIAL_ID = re.compile(r"[a-z][a-z0-9_-]{0,23}")
CHECKPOINT_ID = re.compile(r"(?:boot|test-save|bank-[0-9]{4}|(?:bookmark|recording)-[A-Za-z0-9_-]{1,40})")
CAPTURE_PREFIXES = ("bookmark-", "recording-")
RUN_ID = re.compile(r"[0-9a-f]{32}")
KEYS = {"A", "B", "L", "R", "START", "SELECT", "UP", "DOWN", "LEFT", "RIGHT"}


def bounded_int(value, minimum, maximum, label):
    if type(value) is not int or not minimum <= value <= maximum:
        raise ValueError(f"{label} must be an integer from {minimum} through {maximum}")
    return value


def label(value, limit, name):
    if not isinstance(value, str) or not value.strip() or len(value) > limit or not value.isprintable():
        raise ValueError(f"{name} must be printable text, up to {limit} characters")
    return value.strip()


def compile_steps(text, maximum=MAX_FRAMES):
    """Return (frames, keys, lux) tuples. A light change has zero duration."""
    if not isinstance(text, str) or len(text) > 8000:
        raise ValueError("Controller input must be text under 8,000 characters")
    program = []
    elapsed = 0
    for number, raw in enumerate(text.splitlines(), 1):
        parts = raw.split("#", 1)[0].split()
        if not parts:
            continue
        try:
            match parts:
                case ["wait", count] if re.fullmatch(r"[0-9]{1,4}", count):
                    segment = [(bounded_int(int(count), 1, maximum, "Wait"), None, None)]
                case ["hold", keys, count] if re.fullmatch(r"[0-9]{1,4}", count) and valid_keys(keys):
                    segment = [(bounded_int(int(count), 1, maximum, "Hold"), keys.upper(), None)]
                case ["tap", keys] if valid_keys(keys):
                    segment = [(2, keys.upper(), None), (8, None, None)]
                case ["lux", value] if re.fullmatch(r"[0-9]{1,3}", value):
                    segment = [(0, None, bounded_int(int(value), 0, 255, "Sensor threshold"))]
                case _:
                    raise ValueError("use wait N, hold KEYS N, tap KEYS or lux 0–255")
        except ValueError as error:
            raise ValueError(f"Input line {number}: {error}") from error
        elapsed += sum(row[0] for row in segment)
        if elapsed > maximum or len(program) + len(segment) > 256:
            raise ValueError(f"Input is limited to {maximum:,} frames and 256 segments")
        program.extend(segment)
    return program


def valid_keys(text):
    keys = text.upper().split("+")
    return len(keys) == len(set(keys)) and all(key in KEYS for key in keys)


def strings(value, maximum, name):
    if not isinstance(value, list) or len(value) > maximum:
        raise ValueError(f"{name} must be a list with at most {maximum} entries")
    result = [label(item, 128, name) for item in value]
    if len(set(result)) != len(result):
        raise ValueError(f"{name} contains duplicates")
    return result


def validate(value):
    if not isinstance(value, dict) or type(value.get("version")) is not int or value["version"] != VERSION:
        raise ValueError(f"Recipe version must be {VERSION}")
    checkpoint = value.get("checkpoint", "test-save")
    if not isinstance(checkpoint, str) or not CHECKPOINT_ID.fullmatch(checkpoint):
        raise ValueError("Use a checkpoint ID from the checkpoint catalog")
    duration = bounded_int(value.get("duration"), 1, MAX_FRAMES, "Trial duration")
    interval = bounded_int(value.get("sample_every", 15), 1, duration, "Sample interval")
    if (duration + interval - 1) // interval + 1 > MAX_SAMPLES:
        raise ValueError(f"Increase the sample interval to keep at most {MAX_SAMPLES} frames per trial")
    prepare = value.get("prepare", "")
    compile_steps(prepare, MAX_PREPARE)
    fork = value.get("fork")
    if fork is not None:
        if not isinstance(fork, dict):
            raise ValueError("A fork must name a run, trial and frame")
        run_id, trial_id = fork.get("run"), fork.get("trial")
        if not isinstance(run_id, str) or not RUN_ID.fullmatch(run_id):
            raise ValueError("A fork requires a valid run ID")
        if not isinstance(trial_id, str) or not TRIAL_ID.fullmatch(trial_id):
            raise ValueError("A fork requires a valid trial ID")
        frame = bounded_int(fork.get("frame"), 0, MAX_FRAMES, "Fork frame")
        seed = fork.get("seed", "sample")
        if seed not in ("sample", "start") or (seed == "start" and frame != 0):
            raise ValueError("Use the original starting state only for frame zero")
        fork = {"run": run_id, "trial": trial_id, "frame": frame}
        if seed == "start":
            fork["seed"] = seed
    can_continue = checkpoint != "boot" and not checkpoint.startswith(CAPTURE_PREFIXES) and fork is None
    resume = value.get("continue", can_continue)
    if type(resume) is not bool or (resume and not can_continue):
        raise ValueError("Continue is available for battery saves")
    sprite_trace = value.get("sprite_trace", False)
    if type(sprite_trace) is not bool:
        raise ValueError("Sprite evidence must be true or false")
    fields = strings(value.get("observe", []), MAX_FIELDS, "Observed fields")
    functions = strings(value.get("probe", []), MAX_PROBES, "Probed functions")
    if not fields:
        raise ValueError("Observe at least one named integer field")
    if any(not IDENTIFIER.fullmatch(name) for name in functions):
        raise ValueError("Probe functions by C name")
    raw_trials = value.get("trials")
    if not isinstance(raw_trials, list) or not 1 <= len(raw_trials) <= MAX_TRIALS:
        raise ValueError(f"A recipe needs 1 to {MAX_TRIALS} trials")
    from .interventions import validate as validate_interventions
    trials = []
    seen = set()
    for trial in raw_trials:
        if not isinstance(trial, dict):
            raise ValueError("Each trial must be an object")
        key = trial.get("id")
        if not isinstance(key, str) or not TRIAL_ID.fullmatch(key) or key in seen:
            raise ValueError("Trial IDs must be unique lowercase slugs up to 24 characters")
        seen.add(key)
        script = trial.get("input", "")
        compile_steps(script, duration)
        item = {"id": key, "label": label(trial.get("label", key), 60, "Trial label"), "input": script}
        actions = validate_interventions(trial.get("interventions", []), fields, duration)
        if actions:
            item["interventions"] = actions
        trials.append(item)
    from .checks import validate_checks
    frames = set(range(0, duration, interval)) | {duration}
    assertions = validate_checks(value.get("checks", []), seen, fields, functions, frames)
    return {
        "version": VERSION,
        "title": label(value.get("title"), 120, "Title"),
        "checkpoint": checkpoint,
        "fork": fork,
        "continue": resume,
        "rtc": bounded_int(value.get("rtc", DEFAULT_RTC), 946684800, 4102444799, "RTC Unix time"),
        "prepare": prepare,
        "duration": duration,
        "sample_every": interval,
        "sprite_trace": sprite_trace,
        "observe": fields,
        "probe": functions,
        "checks": assertions,
        "trials": trials,
    }


def instruction(count, keys, lux):
    if lux is not None:
        return f"lux {lux}"
    return f"hold {keys} {count}" if keys else f"wait {count}"


def timeline(program, duration, interval, *, save_states=False, interventions=None):
    """Sample the shared start and the end of each interval before new inputs."""
    def snapshot(frame):
        name = f"f{frame:04d}"
        yield f"sample {name}"
        yield f"shot {name}"
        if save_states:
            yield f"save {name}"

    program = list(program)
    short = duration - sum(row[0] for row in program)
    if short:
        program.append((short, None, None))
    frame, next_sample = 0, interval
    plan = interventions or {}
    boundaries = iter(sorted(at for at in plan if at))
    next_action = next(boundaries, duration)
    yield from snapshot(0)
    yield from plan.get(0, ())
    for count, keys, lux in program:
        if lux is not None:
            yield instruction(count, keys, lux)
        while count:
            stop = min(duration, next_sample, next_action)
            chunk = min(count, stop - frame)
            yield instruction(chunk, keys, None)
            frame += chunk
            count -= chunk
            if frame == min(next_sample, duration):
                yield from snapshot(frame)
                next_sample += interval
            if frame == next_action:
                yield from plan.get(frame, ())
                next_action = next(boundaries, duration)

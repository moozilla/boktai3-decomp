"""Bring saved mGBA sessions into the experiment runner."""
from pathlib import Path
import re
import shutil

from . import recipe
from .fields import ROOT

ROOT_CAPTURE = ROOT / "build/centaur"
NAME = re.compile(r"[A-Za-z0-9_-]{1,40}")
INPUT = re.compile(r"(?:wait ([0-9]{1,8})|hold ([A-Z+]+) ([0-9]{1,8}))")


def bookmarks():
    folder = ROOT_CAPTURE / "bookmarks"
    if folder.is_dir():
        for path in sorted(folder.glob("*.ss")):
            if NAME.fullmatch(path.stem) and path.is_file():
                yield {"id": "bookmark-" + path.stem, "kind": "bookmark", "path": path,
                       "label": f"Live bookmark · {path.stem}"}


def recordings():
    if ROOT_CAPTURE.is_dir():
        for path in sorted(ROOT_CAPTURE.glob("*.txt")):
            if not NAME.fullmatch(path.stem):
                continue
            state = path.with_suffix(".ss")
            if not state.is_file():
                continue
            with path.open() as source:
                # A Centaur capture begins with comments and a same-name state.
                line = next((line.strip() for line in source if line.strip() and not line.lstrip().startswith("#")), "")
            if line == f"loadstate {state.name}":
                yield {"id": "recording-" + path.stem, "kind": "recording", "path": state,
                       "label": f"Live recording · {path.stem}"}


def stage(checkpoint, directory):
    path = checkpoint["path"]
    copied = directory / "captured.ss"
    shutil.copyfile(path, copied)
    # Centaur normally embeds the framebuffer in the mGBA PNG savestate.
    # A separate bookmark screenshot is an equivalent first-frame source.
    screenshot = path.with_suffix(".png")
    if screenshot.is_file():
        return copied, screenshot
    with copied.open("rb") as file:
        png = file.read(8) == b"\x89PNG\r\n\x1a\n"
    return copied, copied if png else None


def import_recording(identifier):
    info = next((r for r in recordings() if r["id"] == identifier), None)
    if info is None:
        raise FileNotFoundError("Live recording not found")
    lines = []
    total = 0
    with info["path"].with_suffix(".txt").open() as source:
        for raw in source:
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            if not lines and total == 0 and line == f"loadstate {info['path'].name}":
                continue
            match = INPUT.fullmatch(line)
            if not match or (match[2] and not recipe.valid_keys(match[2])):
                raise ValueError("This recording has a state change in its inputs. Import a continuous recording.")
            wait, keys, hold = match.groups()
            duration = min(int(wait or hold), recipe.MAX_FRAMES - total)
            if not duration:
                continue
            lines.append(recipe.instruction(duration, keys, None))
            total += duration
            if total == recipe.MAX_FRAMES or len(lines) == 256:
                break
    if not total:
        raise ValueError("The recording has no controller frames")
    return recipe.validate({
        "version": recipe.VERSION,
        "title": "What did this input change? " + info["path"].stem,
        "checkpoint": identifier,
        "continue": False,
        "duration": total,
        "sample_every": max(1, (total + 59) // 60),
        "observe": ["gPlayers[0]->position.x", "gPlayers[0]->position.z", "gGameState->life"],
        "probe": ["Player_Update", "Terrain_SampleSurface"],
        "trials": [
            {"id": "recorded", "label": "Recorded input", "input": "\n".join(lines)},
            {"id": "control", "label": "No input", "input": ""},
        ],
    })

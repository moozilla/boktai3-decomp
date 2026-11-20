"""Replay controller input from the project's headless harness route files."""
import re
from runtime import KEYS, OUT


def run(game, path, directory):
    shots = 0
    for number, raw in enumerate(path.read_text().splitlines(), start=1):
        words = raw.partition("#")[0].split()
        if not words:
            continue
        cmd, *args = words
        try:
            if cmd == "wait":
                game.step(frames(args[0]))
            elif cmd == "tap":
                game.step(2, buttons(args[0]))
                game.step(8)
            elif cmd == "hold":
                game.step(frames(args[1]), buttons(args[0]))
            elif cmd == "shot":
                if not re.fullmatch(r"[A-Za-z0-9_.-]+",args[0]):
                    raise ValueError("A screenshot needs a filename")
                game.image().save(directory / (args[0]+".png"))
                shots += 1
            elif cmd == "mark":
                game.resolver.record("route/"+args[0], event="mark", frame=game.frame)
            elif cmd in ("trace", "save", "dump", "tracefunc", "coverage"):
                # These harness commands export diagnostics and do not change
                # controller input or emulated execution.
                continue
            else:
                raise ValueError(f"Unsupported controller route command {cmd}")
        except (KeyError, IndexError, ValueError) as exc:
            raise ValueError(f"{path.name}:{number}: {exc}") from exc
    print(f"Captured {shots} frames in {directory.relative_to(OUT)}")


def buttons(text):
    return sum(KEYS[key] for key in set(text.upper().split("+")))


def frames(value):
    count = int(value)
    if count < 0:
        raise ValueError("Negative frame duration")
    return count

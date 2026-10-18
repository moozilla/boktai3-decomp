"""Run a built showcase ROM: warp into a scene (mGBA headless, the Centaur
scenario runner), save a state once it is up, then play it in the harness
for screenshots or a recording with sound.

Everything goes under build/showcase/ (ROM-derived).
"""
import os
import shutil
import subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(HERE)))
OUT = os.path.join(ROOT, "build", "showcase", "v3")
HARNESS = os.environ.get("HARNESS", os.path.join(ROOT, "tools", "emu", "harness"))


def state(rom, scene, entrance, name, save=None, wait=90, start="from field", extra=()):
    """Warp to scene/entrance and write a savestate; returns its path."""
    os.makedirs(OUT, exist_ok=True)
    scen = os.path.join(OUT, f"{name}.txt")
    with open(scen, "w") as f:
        f.write(f"{start}\n")
        for line in extra:
            f.write(line + "\n")
        f.write(f"warp {scene} {entrance}\nwait {wait}\nstate {name}\n")
    env = dict(os.environ, SCENARIO=scen)
    cmd = [os.path.join(ROOT, "tools", "mgba", "headless.sh"), os.path.join(ROOT, "tools", "mgba", "scenario.lua"), rom]
    if save:
        cmd.append(save)
    subprocess.run(cmd, env=env, check=True, stdout=subprocess.DEVNULL)
    path = os.path.join(ROOT, "build", "centaur", "scenarios", name, f"{name}.ss")
    assert os.path.exists(path), f"no state written: {path}"
    return path


def harness(rom, lines, name, quiet=True, env=None):
    """Run the harness with a script (lines); returns the output directory."""
    d = os.path.join(OUT, "runs", name)
    if os.path.exists(d):
        shutil.move(d, os.path.expanduser(f"~/.Trash/{name}-{os.getpid()}-{len(os.listdir(os.path.dirname(d)))}"))
    os.makedirs(d)
    script = os.path.join(d, "script.txt")
    with open(script, "w") as f:
        f.write("trace off\n" + "\n".join(lines) + "\n")
    with open(os.path.join(d, "log.txt"), "w") as log:
        subprocess.run([HARNESS, rom, script, d], stdout=log, stderr=log, check=True,
                       env=dict(os.environ, **(env or {})))
    for fn in os.listdir(d):
        if fn.endswith(".ppm"):
            p = os.path.join(d, fn)
            subprocess.run(["magick", p, p[:-4] + ".png"], check=True)
            os.remove(p)
    return d


def shots(rom, st, times, name, scale=2, montage=True, extra=()):
    """Screenshots at the given frame offsets after loading state st."""
    lines = [f"loadstate {st}"] + list(extra)
    t0 = 0
    for t in times:
        if t > t0:
            lines.append(f"wait {t - t0}")
            t0 = t
        lines.append(f"shot t{t:05d}")
    d = harness(rom, lines, name)
    if montage:
        out = os.path.join(OUT, f"{name}.png")
        files = sorted(os.path.join(d, f) for f in os.listdir(d) if f.startswith("t") and f.endswith(".png"))
        subprocess.run(["magick", "montage", "-font", "/System/Library/Fonts/Supplemental/Arial.ttf", *files, "-tile", f"{min(4, len(files))}x", "-geometry",
                        f"{240 * scale}x{160 * scale}+2+2", "-filter", "point", out], check=True)
        return out
    return d

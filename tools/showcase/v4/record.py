#!/usr/bin/env python3
"""Record a playthrough of Claude's Quiz Show with sound.

  python3 tools/showcase/v4/record.py [NAME]   -> build/showcase/v4/NAME.mp4

The run is played by a scripted player in the patched headless mGBA
(tools/mgba/scenario.lua: menu presses, `goto` to walk Django to each ghoul,
`swing` to fight), which logs every frame's keys from a savestate. The
harness then loads that state, replays the keys and records H.264/AAC
(tools/emu/harness `record`). Both are mGBA 0.11 with the same fixed clock,
so the replay matches the scripted run frame for frame.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(HERE)))
sys.path.insert(0, HERE)
import quiz  # noqa: E402

OUT = quiz.OUT


ASKING = "r700 0x346 2"     # play.ASKING: a choice menu is open
LEFT = "r700 0x340 2"       # quiz.LEFT: ghouls standing


def answer(n):
    """Page through dialogue until a choice opens, then pick option n."""
    return [f"until {ASKING} 1 3600 30", "wait 80"] + ["keys RIGHT 4", "wait 12"] * n + ["keys A 4", "wait 30"]


def fight(points, n):
    """Page through Claude's line until the ghouls are out, then fight them all."""
    pts = " ".join(f"{x} {z}" for x, z in points)
    return [f"until {LEFT} {n} 1200 30", "wait 30", f"fight {LEFT} {pts}", "wait 60"]


# the run's answers: (option, whether it is right)
RUN = [0, 0, 0, 1, 0, 0]


def script(q):
    """The run: in from the Sealed Prison's dungeon menu, Claude's opening,
    then six questions (two wrong, so two fights), the finale and back."""
    s = ["from field", f"warp {quiz.PRISON_MENU[0]} {quiz.PRISON_MENU[1]}", "wait 150", "state start", "keylog keys.txt",
         "wait 60", "keys A 4", "wait 200"]
    s += answer(0)                      # Play Claude's Quiz Show? -> Let's play!
    spawns = q.spawn_points()
    for k, opt in enumerate(RUN):
        s += answer(opt)
        right = quiz.QUESTIONS[k][2]
        if right is not None and opt != right:
            s += fight(spawns[k], quiz.QUESTIONS[k][5])
    s += [f"until {ASKING} 2 1800 30", "wait 240", "quit"]   # the finale, then back at the menu
    return s


def main():
    name = sys.argv[1] if len(sys.argv) > 1 else "quiz_run"
    q = quiz.Quiz()
    q.build()
    q.show.save(quiz.ROM)
    lines = script(q)
    scen = os.path.join(OUT, f"{name}.txt")
    open(scen, "w").write("\n".join(lines) + "\n")
    d = os.path.join(ROOT, "build", "centaur", "scenarios", name)
    subprocess.run([os.path.join(ROOT, "tools", "mgba", "headless.sh"), os.path.join(ROOT, "tools", "mgba", "scenario.lua"),
                    quiz.ROM], env=dict(os.environ, SCENARIO=scen), check=True, stdout=subprocess.DEVNULL)
    keys = open(os.path.join(d, "keys.txt")).read()
    rd = os.path.join(OUT, "runs", name)
    os.makedirs(rd, exist_ok=True)
    mp4 = os.path.join(OUT, f"{name}.mp4")
    with open(os.path.join(rd, "script.txt"), "w") as f:
        f.write(f"trace off\nloadstate {os.path.join(d, 'start.ss')}\nrecord {mp4}\n{keys}stoprecord\n")
    subprocess.run([os.path.join(ROOT, "tools", "emu", "harness"), quiz.ROM, os.path.join(rd, "script.txt"), rd],
                   check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print(mp4)


if __name__ == "__main__":
    main()

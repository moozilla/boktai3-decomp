"""Record a scene with sound, pressing A on each dialogue page after it has
been on screen for 30 + 1.8 * characters frames.

The harness's `waitpc` stops when Text_LookupString (0821ADD4) is called
with the page's string id in r0, which is when the window starts typing it.
"""
import os
import subprocess

import run

LOOKUP = 0x0821ADD4
SCRIPT_START = 0x08020DF0     # SceneMessages_ScriptStart: a cutscene starts


def page_frames(chars, label=16):
    """30 + 1.8 frames per character from the page's lookup, and at least the
    typing (about a character a frame at the fastest message speed, the
    speaker label included) plus a short beat."""
    return int(max(30 + 1.8 * chars, chars + label + 24))


def pointers(rom, state):
    """Addresses events may name, read from the savestate: {GS} GameState
    (*0x02000710), {SENSOR} the solar sensor task (*0x03002604)."""
    import struct
    d = run.harness(rom, [f"loadstate {state}", "wait 1", "dump gs 02000710:4", "dump sensor 03002604:4"], "pointers")
    rd = lambda n: struct.unpack("<I", open(os.path.join(d, n + ".bin"), "rb").read())[0]
    return {"GS": rd("gs"), "SENSOR": rd("sensor")}


def gamestate(rom, state):
    return pointers(rom, state)["GS"]


def fast_text(rom, state):
    """Harness lines that set the message speed option (GameState+0x12) to
    the fastest, 0."""
    return [f"poke {gamestate(rom, state) + 0x12:08X}:2 0"]


def script(state, pages, out=None, tail=240, pre=(), post=(), shots=False, slow=1.0, events=()):
    """Harness lines: load the state, record to `out`, and turn each page.
    pages: [(string id, characters)] in the order they are shown; pre/post:
    extra harness lines before the first page and after the last; events:
    [(page index, frames after its A press, harness line)], e.g. lux, rtc or
    freeze changes timed against the cutscene (page index -1: from the
    cutscene's start instead)."""
    lines = [f"loadstate {state}"] + list(pre)
    if out:
        lines.append(f"record {out}")
    first = [(d, ln) for k, d, ln in events if k == -1]
    if first:      # timed from the cutscene's start
        lines += [f"watchpc {SCRIPT_START:08X} -2147483648 2147483647", "waitpc 20000"] + timed(first)
    if pages:
        lines.append(f"watchpc {LOOKUP:08X} {pages[0][0]} {pages[0][0]}")
    for i, (sid, n) in enumerate(pages):
        lines.append("waitpc 20000")
        lines.append(f"wait {int(page_frames(n) * slow)}")
        if shots:
            lines.append(f"shot p{i:03d}")
        if i + 1 < len(pages):     # arm before the press: the next lookup comes during it
            lines.append(f"watchpc {LOOKUP:08X} {pages[i + 1][0]} {pages[i + 1][0]}")
        lines.append("tap A")
        lines += timed([(d, ln) for k, d, ln in events if k == i], 10)
    lines += list(post)
    lines.append(f"wait {tail}")
    if out:
        lines.append("stoprecord")
    return lines


def timed(evs, already=0):
    """Harness lines that run each (frame offset, line) at its offset, counted
    from `already` frames ago."""
    out, now = [], already
    for d, ln in sorted(evs, key=lambda e: e[0]):
        if d > now:
            out.append(f"wait {d - now}")
            now = d
        out.append(ln)
    return out


def record(rom, state, pages, name, out, pre=(), events=(), **kw):
    """Record; event lines may use {GS+0x876} style placeholders (pointers())."""
    ptr = pointers(rom, state)

    def fill(ln):
        import re
        return re.sub(r"\{(\w+)\+0x([0-9A-Fa-f]+)\}", lambda m: f"{ptr[m.group(1)] + int(m.group(2), 16):08X}", ln)
    events = [(k, d, fill(ln)) for k, d, ln in events]
    pre = [f"poke {ptr['GS'] + 0x12:08X}:2 0"] + list(pre)
    d = run.harness(rom, script(state, pages, out, pre=pre, events=events, **kw), name)
    dur = subprocess.run(["ffprobe", "-v", "error", "-show_entries", "format=duration", "-of", "csv=p=0", out],
                         capture_output=True, text=True).stdout.strip()
    print(f"{out}: {dur} s")
    return d

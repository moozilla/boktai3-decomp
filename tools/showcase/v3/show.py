"""The v3 showcase's building blocks: scenes, cutscenes on a timeline, and
the sprites, palettes and dialogue they use, written into a ROM copy.

A Show owns the ROM (tools/showcase/rom.py, the fan translation for its
font) and allocates what the scenes need:

* scenes: new scene entry procedures that load a real stage (its map,
  tileset, palette and terrain: the room's own stage procedure), place the
  player at our entrance and start a cutscene; the cutscene's end runs a
  continue procedure that warps to the next scene (proc_945, the door
  transition), so one run can travel through the game;
* cutscenes (docs/CUTSCENES.md) written as timelines: actions at frame
  offsets, compiled into director steps. Each receiver takes at most four
  messages per step (SceneMessages_Enqueue) and a cutscene names at most 16
  scripts, so all script actions at one instant become one case of the
  cutscene's single "do" procedure (a switch on its first argument), run
  with one message;
* dialogue: each message is a list of pages on borrowed string ids; the
  recorder (record.py) waits for each page's lookup and presses A after
  30 + 1.8 * characters frames;
* keyframe sprites (ScriptKeyframe, commands EAB4 add, 44CD frame, 997B
  glide, EBFF hide, 9C3F show, ED1F mirror) from sprite packages added to
  the CB05 archive, drawn in palette pools added to the pool table.
"""
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, HERE)
from gclasm import Arg, Call, Hash, Proc, Set, Text, chara, command, proc, r700, r708, switch  # noqa: E402,F401
from rom import Rom, message  # noqa: E402
import romx  # noqa: E402

# receivers (docs/CUTSCENES.md)
DIRECTOR, CAMERA, SCREEN, AUDIO = 0x4E69, 0xFF40, 0x730A, 0xD3A9
COUNTER = r700(0x119)        # the dialogue counter every scene's cutscenes use
STORY = r708(0x3C, 1)        # story progress

OT = "<LABEL=OTENKO>Master Otenko</LABEL>:\n"
DJ = "<LABEL=PLAYER>Django</LABEL>:\n"


def chars(page):
    """Characters a page types out (tags and newlines removed)."""
    return len(re.sub(r"<[^>]*>|\n", "", page.split(":\n", 1)[-1] if ":\n" in page else page))


class Show:
    def __init__(self, strings, slots, translated=True):
        self.r = Rom(translated)
        self.pools = romx.Pools(self.r)
        self.pkgs = {}
        self.strings = list(strings)   # borrowed string ids, used in order
        self.slots = list(slots)       # borrowed cutscene slots
        self.pages = []                # (string id, characters) of every dialogue page
        self.scenes = []

    # ---- allocation ----
    def proc(self, stmts=None, pid=None):
        """A new procedure (statements now, or set later with set_proc)."""
        pid = pid or self.r.new_proc_id()
        if stmts is not None:
            self.r.set_proc(pid, proc(stmts))
        return pid

    def set_proc(self, pid, stmts):
        self.r.set_proc(pid, proc(stmts))

    def text(self, pages):
        """A message of one or more pages; returns the first string id."""
        first = None
        for p in list(pages) + ["<PROC=0>\n<END>"]:
            sid = self.strings.pop(0)
            first = first if first is not None else sid
            self.r.set_string(sid, p)
            if not p.startswith("<PROC=0>"):
                self.pages.append((sid, chars(p)))
        return first

    def pool(self, colors):
        return self.pools.add(colors)

    def package(self, name, data):
        assert name < 0x8000 and name not in self.pkgs
        self.pkgs[name] = data
        return name

    def alias(self, name, source, pool):
        """Package `name`: the header of package `source` drawn in `pool`."""
        assert name < 0x8000 and name not in self.pkgs
        self.pkgs[name] = ("alias", source, pool)
        return name

    def save(self, path):
        self.pools.build()
        if self.pkgs:
            romx.add_resources(self.r, romx.PKG_TYPE, self.pkgs)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        self.r.save(path)


class Cutscene:
    """A cutscene table built on a timeline. Times are frames from the start
    of the current segment; say() ends a segment with a dialogue step (which
    lasts until the page is dismissed) and starts a new one."""

    def __init__(self, show, extra=()):
        self.show = show
        self.slot = show.slots.pop(0)
        self.dlg = show.r.new_proc_id()
        self.do = show.r.new_proc_id()
        self.keys = {self.dlg: 0x5C0A, self.do: 0x5C0B}
        for i, p in enumerate(extra):     # other scripts the table runs (e.g. 1675, 1676)
            self.keys[p] = 0x5C10 + i
        self.cases = []                   # the do procedure's cases
        self.msgs = []                    # first string id of each message, in order
        self.pages = []                   # (string id, characters) of this cutscene's pages
        self.steps = []
        self.events = []                  # (time, kind, payload) in the open segment
        self.end = 0

    # ---- scheduling ----
    def at(self, t, *items):
        """At frame t: GCL statements (run by the do procedure) and receiver
        messages (bytes from cam(), fade(), player() ...)."""
        for it in items:
            self.events.append((t, "msg" if isinstance(it, (bytes, bytearray)) else "gcl", it))
        self.end = max(self.end, t + 1)

    def hold(self, t):
        """The segment lasts at least until frame t."""
        self.end = max(self.end, t)

    def run(self, pid, *args):
        """A director message running script pid (registered in extra) with args."""
        return message(DIRECTOR, 1, 0, [self.keys[pid]] + (list(args) or [0]))

    def _case(self, stmts):
        self.cases.append(stmts)
        return message(DIRECTOR, 1, 0, [self.keys[self.do], len(self.cases) - 1])

    def flush(self):
        if not self.events:
            return
        times = sorted({t for t, _, _ in self.events})
        for k, t in enumerate(times):
            nxt = times[k + 1] if k + 1 < len(times) else max(self.end, t + 1)
            gcl = [x for tt, kind, x in self.events if tt == t and kind == "gcl"]
            msgs = [x for tt, kind, x in self.events if tt == t and kind == "msg"]
            step = list(msgs)
            if gcl:
                step.append(self._case(gcl))
            step.append(message(DIRECTOR, 1, 1, [nxt - t - 1], True))
            self._check(step)
            self.steps.append(step)
        self.events, self.end = [], 0

    def _check(self, step):
        per = {}
        for m in step:
            to = struct.unpack_from("<H", m)[0]
            per[to] = per.get(to, 0) + 1
        bad = {f"{k:04X}": v for k, v in per.items() if v > 4}
        assert not bad, f"more than 4 messages for a receiver in one step: {bad}"

    def say(self, pages, *items, top=False):
        """Show a message (pages: strings with a speaker label); the step
        lasts until it is dismissed. items: statements or messages that
        start with it. top: open the window at the top of the screen."""
        self.flush()
        n = len(self.show.pages)
        assert not top, "set story progress 190 for the scene instead (top_messages)"
        self.msgs.append((self.show.text(pages), top))
        self.pages += self.show.pages[n:]
        step = [message(DIRECTOR, 1, 2, []), message(DIRECTOR, 1, 0, [self.keys[self.dlg], 0])]
        gcl = [x for x in items if not isinstance(x, (bytes, bytearray))]
        step += [x for x in items if isinstance(x, (bytes, bytearray))]
        if gcl:
            step.append(self._case(gcl))
        self._check(step)
        self.steps.append(step)

    def step(self, *msgs):
        """A raw step (e.g. one that ends when a camera pan or animation ends)."""
        self.flush()
        self.steps.append(list(msgs))

    # ---- writing ----
    def start_statement(self, onfinish):
        keys = list(self.keys.items())
        return command(0x7348, d=self.slot, c=0, p=[Hash(k) for _, k in keys], P=[Proc(p) for p, _ in keys],
                       e=Proc(onfinish))

    def write(self, onfinish):
        """Write the table, the do procedure and the dialogue procedure; returns
        the dialogue procedure (calling it starts the cutscene)."""
        self.flush()
        r = self.show.r
        r.set_event(self.slot, self.steps + [[]])
        r.set_proc(self.do, proc([switch(Arg(1), [(i, s) for i, s in enumerate(self.cases)])] if self.cases else []))
        cases = [(0, [Set(COUNTER, COUNTER, 1, "+"), self.start_statement(onfinish)])]
        for i, (first, top) in enumerate(self.msgs):
            open_ = [Call(1022, Proc(self.dlg))]
            cases.append((1 + 2 * i, [Set(COUNTER, COUNTER, 1, "+"), command(0x1C64, r=Text(first))] + open_))
            cases.append((2 + 2 * i, [Set(COUNTER, COUNTER, 1, "+"), Call(1025), command(0x2502)]))
        r.set_proc(self.dlg, proc([switch(COUNTER, cases)]))
        return self.dlg


# ---- receiver messages ----
def wait(n):
    return message(DIRECTOR, 1, 1, [n], True)


def cam(x, z, frames=0, y=406, ends=False):
    if frames == 0:
        return message(CAMERA, 6, 1, [x, y, z], ends)
    return message(CAMERA, 6, 2, [x, y, z, frames], ends)


def cam_follow(frames=0):
    return message(CAMERA, 6, 5, [frames])


def fade(out, speed, r, g, b, bg=1, obj=1, other=1, strength=0):
    return message(SCREEN, 7, 3, [out, speed, r, g, b, bg, obj, other, strength, 0])


def quake(strength, frames):
    return message(SCREEN, 7, 6, [strength, frames])


def quake_stop():
    return message(SCREEN, 7, 7, [])


def particles(x, z, frames=40, y=320, a=128, b=24, kind=3):
    return message(SCREEN, 7, 0, [x, y, z, a, frames, b, kind, 0])


def bgm(song):
    return message(AUDIO, 8, 2, [song, 0])


def bgm_out(speed):
    return message(AUDIO, 8, 4, [speed, 0])


def sound(n):
    return message(AUDIO, 8, 0, [n, 0])


def top_messages():
    """Message windows open at the top of the screen while story progress
    is 190-191 (proc_1019): a statement for a scene that needs the bottom
    of the screen (the SUN gauge)."""
    return Set(STORY, 190)


def hud(mode):
    """Hud_SetMode (command C6AA): 1 the gameplay HUD, 7 none."""
    return command(0xC6AA, m=mode)


def actor(name, kind, cmd, *args, ends=False):
    return message(name, kind, cmd, list(args), ends)


# ---- keyframe sprite statements (ScriptKeyframe) ----
def kf_add(tag, pkg, x, z, y=256, frame=0, prio=2):
    return command(0xEAB4, n=Hash(tag), d=Hash(pkg), i=frame, m=0, p=[x, y, z], R=prio, A=0)


def kf_frame(tag, frame):
    return command(0x44CD, n=Hash(tag), p=frame)


def kf_glide(tag, x, z, frames, y=256):
    return command(0x997B, n=Hash(tag), p=[x, y, z], f=frames)


def kf_hide(tag):
    return command(0xEBFF, n=Hash(tag))


def kf_show(tag):
    return command(0x9C3F, n=Hash(tag))


def kf_mirror(tag, on=1):
    return command(0xED1F, n=Hash(tag), f=on)


def keyframe_manager(capacity):
    """The scene's keyframe manager (chara 8AF6 8AF6 m: capacity), which a
    stage procedure normally creates with room for one."""
    return chara(0x8AF6, 0x8AF6, m=capacity)


# ---- scenes ----
def scene_entry(show, stage, entrance, x, z, facing, arrive, start, extra=(), flags=56, bgm_id=None, cont=None):
    """A new scene: scene_start(flags), the shared setup, the real stage
    procedure, one entrance at (x, z) whose arrival runs `arrive` and then
    `start`, the extra statements (spawns), music, and a continue procedure.
    Returns the scene's procedure id (warp to it with this entrance)."""
    ents = show.proc([Call(916, x, z, facing, 0, 128, 0, entrance, Proc(arrive), Proc(start))]
                     + ([Call(956, Proc(cont))] if cont else []))
    body = [Call(861, flags), Call(864, 0), Call(553), Call(1265), Call(1), Call(stage), Call(ents), Call(905)]
    body += list(extra)
    if bgm_id is not None:
        body.append(Call(1048, bgm_id))
    return show.proc(body)


def calls(img, pid):
    """[(callee, [args as text])] of procedure pid (raw disassembly), nested blocks included."""
    import gcx
    out = []
    for _, t in gcx.disassemble(img, pid - 1, names=False):
        m = re.match(r"proc_(\d+)((?: \S+)*) <end>", t.strip())
        if m:
            out.append((int(m.group(1)), m.group(2).split()))
    return out


def stage_of(img, scene):
    """The procedure of `scene` that loads its stage (calls proc_884), its
    (map, tileset, hp) hashes and its BGM (proc_1048's argument)."""
    stage, song = None, None
    for c, args in calls(img, scene):
        if c == 1048 and args:
            song = int(args[0], 0)
        if stage is None and c not in (861, 864, 553, 1265, 1, 870, 905):
            for c2, a2 in calls(img, c):
                if c2 in (884, 885, 876):
                    stage = (c, tuple(int(x, 0) for x in a2[:3]))
    return stage, song

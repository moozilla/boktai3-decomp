"""Building blocks for a *playable* hack (showcase v4), on top of the v3
toolkit (tools/showcase/v3/show.py, romx.py, art.py).

v3 borrowed string ids and cutscene slots from parts of the game its video
never showed. A hack people play through can't do that, so PlayShow grows
the tables instead:

* strings: the script image's string table (section 1 of the gcx image,
  Text_LookupString reads it through a pointer set up at load, with no bounds
  check) is copied to free space with room for new ids after the game's 9,977,
  and the section offset is pointed at the copy;
* cutscenes: gEventBytecodePtrs (08E8755C, one literal at 08224EE0, no bounds
  check either) is copied with extra entries and the literal repointed.

New procedures already go in unused procedure slots (rom.py), and new
palette pools and sprite packages are appended (romx.py), so nothing the
game uses is overwritten.

Runtime dialogue (outside cutscenes) follows the game's NPC idiom (Johnny in
the scale quiz, gcl/examples/scale_quiz.md):

    proc_1016(0, 1)                       # talk mode: player held, HUD off
    r700[0xb.1] = 1
    command 1C64 r: text#N                # the message (consecutive string ids are its pages)
    proc_1019(NEXT)                       # open it; NEXT runs when it closes
    ...
    proc_1191(name, CHOICE_PROC, 0, 0, 1) # a choice menu; CHOICE_PROC gets the choice in arg1
    command 71E7 r: text#Q                # the question, its options in <ALTER> tags
    ...
    proc_1019(1026)                       # the last message: 1026 ends talk mode
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
V3 = os.path.join(os.path.dirname(HERE), "v3")
sys.path.insert(0, V3)
sys.path.insert(0, os.path.dirname(HERE))
from show import Show  # noqa: E402
from gclasm import Call, Set, Text, Var, command  # noqa: E402

BASE = 0x08000000
STRINGS = 9977                  # the game's string count
EVENTS = 0x08E8755C
EVENTS_LITERAL = 0x08224EE0     # the one load of gEventBytecodePtrs (sub_08224EBC)
END_TALK = 1026                 # closes talk mode: HUD back, player free
ASKING = Var(0x10000000 | 2 << 24 | 0x100000 | 0x346)   # r700[0x346]: 1 while a choice menu is open (for scripted players)


def msg_mode():
    """r700[0xb.1]:t4, the flag the game's talk scripts set before a message."""
    return Var(0x10000000 | 4 << 24 | 0x100000 | 1 << 16 | 0xB)


class PlayShow(Show):
    def __init__(self, new_strings=800, new_events=16, translated=True):
        super().__init__([], [], translated)
        r = self.r
        # strings: copy the table with room for more
        base = r.img.base
        old = base + r.img.sections[1]
        table = bytes(r.rom[old:old + 4 * STRINGS]) + b"\0" * (4 * new_strings)
        a = r.alloc(table, 4)
        struct.pack_into("<I", r.rom, base + 4, a - BASE - base)
        self.string_table = a
        self.strings = list(range(STRINGS, STRINGS + new_strings))
        r.set_string = self._set_string
        # cutscenes: copy gEventBytecodePtrs with extra entries
        n = 0
        while True:
            v = r.u32(EVENTS + 4 * n)
            if not (v == 0 or BASE <= v < BASE + 0x1000000):
                break
            n += 1
        a = r.alloc(bytes(r.rom[EVENTS - BASE:EVENTS - BASE + 4 * n]) + b"\0" * (4 * new_events), 4)
        r.u32(EVENTS_LITERAL, a)
        self.event_count = n
        self.slots = list(range(n, n + new_events))
        self._events = a
        rom_mod = sys.modules["rom"]
        orig = rom_mod.Rom.set_event

        def set_event(nn, steps, _r=r):
            saved = rom_mod.EVENTS
            rom_mod.EVENTS = self._events
            try:
                orig(_r, nn, steps)
            finally:
                rom_mod.EVENTS = saved
        r.set_event = set_event

    def _set_string(self, sid, text):
        from rom import STRING_BANK
        a = self.r.alloc(text.encode("ascii") + b"\0", 1)
        self.r.u32(self.string_table + 4 * sid, 0x80000000 | (a - STRING_BANK))

    # ---- runtime dialogue ----
    def message(self, pages):
        """A message of pages (consecutive string ids); returns its first id."""
        return self.text(pages)

    def say(self, pages, then=END_TALK, start=False):
        """Statements that show a message and run procedure `then` when it closes."""
        sid = self.message(pages)
        out = [Call(1016, 0, 1)] if start else []
        return out + [Set(ASKING, 0), Set(msg_mode(), 1), command(0x1C64, r=Text(sid)), Call(1019, then)]

    def ask(self, question, on_choice, name=0x4BB5):
        """Statements that open a choice menu: question is one page whose
        options are in <ALTER> tags; on_choice gets the choice (0, 1, ...) in arg1."""
        sid = self.message([question])
        return [Set(ASKING, 1), Set(msg_mode(), 1), Call(1191, name, on_choice, 0, 0, 0), command(0x71E7, r=Text(sid))]

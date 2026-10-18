"""Characters on a cutscene timeline: keyframe sprites with walk cycles,
and the game's own player and Otenko receivers.

World coordinates: 256 units a cell, y is height (a room's floor is often
256 or 512). On screen, +x goes down-right and +z down-left:
  screen dx = 3 * (x - z) / 32,  dy = 3 * (x + z - y) / 32.
"""
from show import actor, kf_add, kf_frame, kf_glide, kf_hide, kf_mirror, kf_show

PLAYER_STORY, PLAYER_FIELD, OTENKO = 0x2B06, 0xF5EB, 0x9CFE


def screen_to_world(dx, dy_up):
    """World (x, z, y) offsets that move a sprite dx right and dy_up up on screen."""
    k = dx * 32 / 6
    return int(k), int(-k), int(dy_up * 32 / 3)


class Sprite:
    """A keyframe sprite: package, tag, named frames; remembers where it is."""

    def __init__(self, cut, tag, pkg, frames, y=256):
        self.cut, self.tag, self.pkg, self.frames = cut, tag, pkg, frames
        self.x = self.z = 0
        self.y = y
        self.added = False
        self.mirrored = 0

    def fr(self, f):
        return self.frames[f] if isinstance(f, str) else f

    def add(self, t, x, z, frame=0, y=None, hidden=False, prio=2):
        self.x, self.z = x, z
        if y is not None:
            self.y = y
        items = [kf_add(self.tag, self.pkg, x, z, y=self.y, frame=self.fr(frame), prio=prio)]
        if self.mirrored:
            items.append(kf_mirror(self.tag, 1))
        if hidden:
            items.append(kf_hide(self.tag))
        self.cut.at(t, *items)
        self.added = True
        return self

    def frame(self, t, f, mirror=None):
        """Show frame f (re-applying the mirror, which a new frame resets)."""
        if mirror is not None:
            self.mirrored = mirror
        self.cut.at(t, kf_frame(self.tag, self.fr(f)), kf_mirror(self.tag, self.mirrored))

    def glide(self, t, x, z, frames, y=None):
        self.x, self.z = x, z
        if y is not None:
            self.y = y
        self.cut.at(t, kf_glide(self.tag, x, z, max(1, frames), y=self.y))

    def jump(self, t, x, z, y=None):
        """Move instantly (a one-frame glide)."""
        self.glide(t, x, z, 1, y)

    def hide(self, t):
        self.cut.at(t, kf_hide(self.tag))

    def show(self, t):
        self.cut.at(t, kf_show(self.tag))

    def mirror(self, t, on=1):
        self.mirrored = on
        self.cut.at(t, kf_mirror(self.tag, on))

    def cycle(self, t, frames, dur, step=8):
        """Step through frames (names or indices) from t for dur frames."""
        k = 0
        for tt in range(t, t + dur, step):
            self.frame(tt, frames[k % len(frames)])
            k += 1
        return t + dur

    def walk(self, t, x, z, dur, frames, step=8, end=None, y=None):
        """Glide to (x, z) over dur frames while cycling frames; end: the frame to stop on."""
        self.glide(t, x, z, dur, y)
        self.cycle(t, frames, dur, step)
        if end is not None:
            self.frame(t + dur, end)
        return t + dur


class Player:
    """The player character through the cutscene's player receiver (docs/CUTSCENES.md)."""

    def __init__(self, cut, name=PLAYER_FIELD):
        self.cut, self.name = cut, name

    def _m(self, cmd, *args):
        return actor(self.name, 2, cmd, *args)

    def face(self, t, d):
        self.cut.at(t, self._m(3, d))

    WALK = 9      # world units a frame (running is about 18)

    def walk(self, t, x, z, y=256):
        """Walk to (x, z); returns the frame he gets there (if his start is known)."""
        self.cut.at(t, self._m(7, x, y, z, 0))
        end = t
        if getattr(self, "pos", None) is not None:
            import math
            end = t + int(math.dist(self.pos, (x, z)) / self.WALK)
        self.pos = (x, z)
        return end

    def run(self, t, x, z, y=256):
        self.cut.at(t, self._m(33, x, y, z, 0))

    def place(self, t, x, z, y=256, d=0):
        self.cut.at(t, self._m(1, x, y, z, d))

    def hide(self, t):
        self.cut.at(t, self._m(0))

    def aim(self, t, d):
        self.cut.at(t, self._m(23, d))

    def shoot(self, t, d, n=0):
        self.cut.at(t, self._m(24, d, n))

    def aim_end(self, t, d):
        self.cut.at(t, self._m(26, d))

    def pose(self, t, *args):
        self.cut.at(t, self._m(32, *args))

    def raw(self, t, cmd, *args):
        self.cut.at(t, self._m(cmd, *args))


class Otenko:
    """Master Otenko, the scene NPC `character 1`: placed by proc_1675/1676
    (the cutscene must list them as scripts), then NPC commands (kind 5).

    In a field scene he already exists (he follows the player), and proc_1676
    then only sends him flying to the spot; place() sets his position
    outright first (command 64). He keeps his walking pose after a walk until told
    to face somewhere (the game's scenes follow a walk with a face command),
    so walk() sends one when he arrives (speed 10 a frame)."""

    SPEED = 10

    def __init__(self, cut, y=256):
        self.cut = cut
        self.y = y
        self.pos = None

    def place(self, t, x, z, d=3, y=None):
        y = self.y if y is None else y
        # set his position first (if he exists), so proc_1676's fly-to has nowhere to go;
        # if he doesn't exist yet, proc_1676 creates him there
        self.cut.at(t, actor(OTENKO, 5, 64, x, y + 32, z), actor(OTENKO, 5, 3, d))
        self.cut.at(t + 1, self.cut.run(1675))
        self.cut.at(t + 2, self.cut.run(1676, x, z, d, 1))
        self.cut.at(t + 4, actor(OTENKO, 5, 3, d))
        self.pos = (x, z)

    def face(self, t, d):
        self.cut.at(t, actor(OTENKO, 5, 3, d))

    def walk(self, t, x, z, y=None, face=1):
        """Walk to (x, z), then stand facing `face` (None: keep walking pose)."""
        y = self.y if y is None else y
        self.cut.at(t, actor(OTENKO, 5, 4, x, y, z))
        if self.pos is not None and face is not None:
            import math
            arrive = t + int(math.dist(self.pos, (x, z)) / self.SPEED) + 8
            self.cut.at(arrive, actor(OTENKO, 5, 3, face))
        self.pos = (x, z)
        return t

    def run(self, t, x, z, y=None):
        y = self.y if y is None else y
        self.cut.at(t, actor(OTENKO, 5, 5, x, y, z))
        self.pos = (x, z)

    def raw(self, t, cmd, *args):
        self.cut.at(t, actor(OTENKO, 5, cmd, *args))

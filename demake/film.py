"""A continuous controller replay at 20 samples per second, timed on the DMG."""
import numpy as np
from PIL import Image

from verify import A, DOWN, RIGHT, START, Replay, check


class Film(Replay):
    def __init__(self, out, manifest):
        self.filming = False
        self.frames = []
        super().__init__(out, manifest, "walk")
        self.run(START)
        self.run(0)
        self.filming = True

    def run(self, key, ticks=1):
        if not self.filming:
            return super().run(key, ticks)
        while ticks:
            count = min(ticks, 3 - self.ticks % 3)
            super().run(key, count)
            ticks -= count
            if self.ticks % 3 == 0:
                self.trace.append(["video", len(self.frames)])
                self.frames.append(self.model.render())


def film(out, manifest, host):
    r = Film(out, manifest)
    r.run(0, 10)
    r.walk(lambda x,z: (x,z) == (32,76))
    r.run(RIGHT | DOWN, 2)
    r.run(A)
    r.run(0, 28)
    check(r.state("enemy_phase") == 2, "opening shot in film")
    r.event(2)
    r.door(2)
    r.run(0, 12)
    r.verify_native(host)
    frames = []
    for i, frame in enumerate(r.frames):
        if host:
            path = out / f"walk-{i:04d}.ppm"
            with Image.open(path) as src:
                native = src.convert("L")
            converted = np.rint(np.asarray(native) / 85).astype(np.uint8) * 85
            check(np.array_equal(converted, np.asarray(frame)), f"native film frame {i}")
            path.unlink()
            frame = native
        frames.append(frame.resize((320,288), Image.Resampling.NEAREST))
    # Three DMG frames take 50.23 ms. GIF time is expressed in centiseconds.
    frames[0].save(out / "walk.gif", save_all=True, append_images=frames[1:], loop=0,
                   duration=50, disposal=1, optimize=True)
    return dict(frames=len(frames), ticks=r.ticks, sample_interval_ticks=3,
                native_checked=bool(host), missed_vblanks=r.missed_vblanks)

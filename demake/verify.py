"""Exercise the cartridge through JOYP. Compare DMG frames against libmgba when available."""
import json
import os
import shutil
import subprocess
from collections import deque
from pathlib import Path

import numpy as np
from PIL import Image

from art import ROOT
from gallery import contact
from reference import DMG

RIGHT, LEFT, UP, DOWN = 16, 32, 64, 128
A, B, SELECT, START = 1, 2, 4, 8
PAD = {RIGHT: (1, -1), LEFT: (-1, 1), UP: (-1, -1), DOWN: (1, 1),
       RIGHT | UP: (0, -1), RIGHT | DOWN: (1, 0), LEFT | UP: (-1, 0), LEFT | DOWN: (0, 1)}


def check(condition, description):
    if not condition:
        raise ValueError(description)


def playback(out):
    """Build against installed libmgba. Reference playback needs only the Python environment."""
    cc = shutil.which(os.environ.get("CC", "cc"))
    if not cc:
        return None
    flags = []
    pkg = shutil.which("pkg-config")
    if pkg:
        probe = subprocess.run([pkg, "--cflags", "--libs", "mgba"], capture_output=True, text=True)
        if probe.returncode == 0:
            import shlex
            flags = shlex.split(probe.stdout)
    if not flags:
        roots = [Path(os.environ["MGBA_PREFIX"])] if "MGBA_PREFIX" in os.environ else []
        roots += [Path(p) for p in ("/opt/homebrew/opt/mgba", "/usr/local/opt/mgba", "/usr/local", "/usr")]
        root = next((p for p in roots if (p / "include/mgba/core/core.h").is_file()), None)
        if root is None:
            return None
        flags = [f"-I{root}/include", f"-L{root}/lib", "-lmgba"]
    host = out / "playback"
    subprocess.run([cc, "-O2", "-Wall", "-Wextra", "-DENABLE_VFS", "-DENABLE_DIRECTORIES",
                    str(ROOT / "demake/playback.c"), "-o", host, *flags], check=True)
    return host


class Replay:
    def __init__(self, out, manifest, name):
        self.out, self.meta, self.name = out, manifest, name
        self.model = DMG((out / "boktai.gb").read_bytes())
        self.loop = manifest["engine"]["loop"]
        self.trace = []
        self.snaps = []
        self.ticks = 0
        self.missed_vblanks = 0
        self._ram = np.frombuffer(self.model.mem, dtype=np.uint8)
        self._window_x, self._window_y = np.meshgrid(np.arange(21), np.arange(19))
        self.run(0, 1)

    def state(self, key):
        return self.model.mem[self.meta["state"][key]]

    def pos(self):
        return self.state("player_x"), self.state("player_z")

    def run(self, key, ticks=1):
        if ticks == 0:
            return
        self.model.keys = key
        for _ in range(ticks):
            before, room = self.model.frames, self.model.bank
            self.model.until(self.loop)
            if self.ticks and room == self.model.bank:
                self.missed_vblanks += max(0, self.model.frames - before - 1)
            y = self._ram[0xfe00:0xfea0:4].astype(int)
            starts, stops = np.maximum(y - 16, 0), np.minimum(y, 144)
            visible = starts < stops
            loads = np.bincount(starts[visible], minlength=145) - np.bincount(stops[visible], minlength=145)
            self.model.peak_line = max(self.model.peak_line, int(np.cumsum(loads).max()))
            self.model.peak_oam = max(self.model.peak_oam, int(visible.sum()))
            check(self.model.peak_line <= 10, "OAM line overflow during replay")
            self.check_scroll()
        self.ticks += ticks
        if self.trace and self.trace[-1][0] == "tick" and self.trace[-1][2] == key:
            self.trace[-1][1] += ticks
        else:
            self.trace.append(["tick", ticks, key])

    def check_scroll(self):
        # Compare every tile the LCD might fetch, including partial edge tiles.
        # Both emulators could agree while the game itself loaded a wrong column.
        if self.state("mode"):
            return
        origin_x = (self.state("camera_x") + 256 * self.state("camera_x_hi")) // 8
        origin_y = (self.state("camera_y") + 256 * self.state("camera_y_hi")) // 8
        x, y = self._window_x + origin_x, self._window_y + origin_y
        hw = np.frombuffer(self.model.mem, dtype=np.uint8)
        rom = np.frombuffer(self.model.rom, dtype=np.uint8)
        expected = rom[self.model.bank * 0x4000 + 0x2000 + (y % 64) * 64 + x % 64]
        actual = hw[0x9800 + (y % 32) * 32 + x % 32]
        check(np.array_equal(expected, actual), f"BG stream differs from full map at camera tile {origin_x},{origin_y}")

    def wire(self, name, packets):
        incoming = b"".join(data for data, _ in packets)
        self.model.keys = 0
        self.model.serial_queue = deque(incoming)
        self.model.serial_replies = []
        self.model.serial_next = self.model.cycles + 4096
        while self.model.serial_queue:
            self.model.until(self.loop)
        replies = self.model.serial_replies
        offset = 0
        for data, status in packets:
            offset += len(data)
            check(replies[offset - 2:offset] == [0x81, status],
                  f"printer reply {replies[offset-2:offset]} expected identity and status {status}")
        path = self.out / (name + ".wire")
        path.write_bytes(bytes(v for pair in zip(incoming, replies) for v in pair))
        self.trace.append(["wire", path.name])

    def record(self, name, caption):
        frame = self.model.render()
        check(self.model.peak_line <= 10 and self.model.peak_oam <= 40, "OAM hardware budget")
        pixels = np.array(frame)
        check(len(np.unique(pixels)) == 4, f"{name} must render four distinct shades")
        frame.save(self.out / f"ref-{name}.png")
        raw = self.model.mem[0xC100:0xC180].hex()
        self.snaps.append(dict(name=name, caption=caption, state=raw, tick=self.ticks, bank=self.model.bank))
        self.trace.append(["frame", name])

    def floor(self, x, z):
        rom = self.model.rom
        start = self.model.bank * 0x4000
        w, h = rom[start + 2:start + 4]
        if not (0 <= x < w * 8 and 0 <= z < h * 8):
            return None
        cell = rom[start + 0x400 + z // 8 * w + x // 8]
        if cell == 255:
            return None
        level, kind = (cell & 15) * 8, cell >> 4
        return level - (z % 8 if kind == 1 else x % 8 if kind == 2 else 0)

    def move(self, pos, dx, dz):
        x, z = pos
        height = self.floor(x, z)
        other = self.floor(x + dx, z)
        if other is not None and abs(height - other) <= 1:
            x += dx
            height = other
        other = self.floor(x, z + dz)
        if other is not None and abs(height - other) <= 1:
            z += dz
        return x, z

    def walk(self, predicate):
        """Find D-pad presses with a decoded HP grid, replay each on the CPU."""
        start = self.pos()
        seen = {start: None}
        todo = deque([start])
        while todo:
            point = todo.popleft()
            if predicate(*point):
                break
            for key, (dx, dz) in PAD.items():
                dest = self.move(point, dx, dz)
                if dest not in seen:
                    seen[dest] = (point, key)
                    todo.append(dest)
        else:
            raise ValueError(f"no walkable path from {start} in bank {self.model.bank}")
        path = []
        while seen[point] is not None:
            before, key = seen[point]
            path.append((key, point))
            point = before
        for key, expected in reversed(path):
            self.run(key, 2)
            check(self.pos() == expected, f"walk to {expected} reached {self.pos()}")

    def event(self, kind, target=None):
        rows = self.meta["rooms"][self.model.bank - 1]["events"]
        e = next(e for e in rows if e[0] == kind and (target is None or e[5] == target))
        self.walk(lambda x, z: e[1] <= x <= e[3] and e[2] <= z <= e[4])
        return e

    def door(self, dest):
        self.event(1, dest)
        self.run(A)
        check(self.model.bank == dest, f"door did not enter bank {dest}")
        self.run(0)

    def verify_native(self, host):
        commands = []
        for event in self.trace:
            if event[0] == "tick":
                commands.append(f"tick {self.loop:x} {event[1]} {event[2]}")
            elif event[0] == "wire":
                commands.append(f"wire {self.loop:x} {event[1]}")
            else:
                name = event[1]
                commands.extend([f"frame {name}.ppm", "read c100 128"])
        (self.out / f"{self.name}.replay").write_text("\n".join(commands) + "\n")
        if not host:
            return
        proc = subprocess.run([host, "boktai.gb"], input="\n".join(commands) + "\n", cwd=self.out,
                              text=True, capture_output=True, check=True, timeout=120)
        lines = proc.stdout.splitlines()
        check(len(lines) == len(self.snaps), "mGBA replay returned an unexpected number of WRAM snapshots")
        for snap, state_line in zip(self.snaps, lines):
            name = snap["name"]
            check(snap["state"] == state_line.split()[1], f"mGBA WRAM differs at {name}")
            ppm = self.out / f"{name}.ppm"
            with Image.open(ppm) as im:
                img = im.convert("L")
            ppm.unlink()
            # mGBA expands internal 5-bit colors to 0/82/173/255.
            native = np.rint(np.array(img) / 85).astype(np.uint8) * 85
            reference = np.array(Image.open(self.out / f"ref-{name}.png"))
            delta = np.count_nonzero(native != reference)
            check(delta == 0, f"{name}: {delta} pixels differ between mGBA and reference")
            img.save(self.out / f"{name}.png")


def campaign(r):
    r.record("title", "Title   /   Original logo and flame")
    r.run(START)
    r.run(0, 2)
    r.record("opening", "2698   /   Opening cell")
    r.run(START)
    r.run(0)
    r.record("notes", "2698   /   Otenko's message")
    r.run(START)
    r.run(0)
    r.run(SELECT)
    r.run(RIGHT)
    r.record("sun-meter", "Sun meter   /   Thumb shadow")
    r.run(A)
    r.run(0)
    check(r.state("sun_level") == 3 and r.state("sun_source") == 1, "manual exposure")
    # Approach from the left of HP route point 0, aim along positive world X.
    r.walk(lambda x, z: (x, z) == (32, 76))
    r.run(RIGHT | DOWN, 2)
    r.run(A)
    r.run(0, 4)
    r.record("shot", "2698   /   Solar shot")
    r.run(0, 9)
    check(r.state("enemy_phase") == 2 and r.state("energy") == 2, "solar shot must hit the ghoul")
    r.run(0, 40)
    check(r.state("death_flag") == 0x10, "ghoul death animation frame marker")
    r.run(0, 85)
    check(r.state("enemy_phase") == 3, "ghoul must leave the draw list at death animation end")
    # A blocked move is proven on a boundary selected from the HP grid.
    r.walk(lambda x, z: r.move((x, z), 1, 0) == (x, z))
    wall = r.pos()
    r.run(RIGHT | DOWN, 16)
    check(r.pos() == wall, "player crossed a solid HP cell")
    r.event(2)
    check(r.state("energy") == 5, "first Solar Fruit should restore three pips")
    r.record("fruit", "2698   /   Solar Fruit collected")
    r.door(2)
    r.record("corridor", "2705   /   Entrance 57")
    r.door(3)
    r.record("skylight-room", "2712   /   Skylight chamber")
    r.event(3)
    r.run(0)
    r.run(A)
    r.run(0, 30)
    before = r.state("energy")
    check(before < 6, "need an empty pip to check the skylight")
    captured = False
    for _ in range(96):
        r.run(B)
        for slot in (0xC320, 0xC330):
            if r.model.mem[slot+2] == 3 and not captured:
                r.record("lunge", "2712   /   Skeleton lunge")
                captured = True
    check(captured, "the real skeleton route must reach a lunge")
    check(r.state("energy") == 6, "skylight failed to refill energy")
    r.record("skylight", "2712   /   Sunlight stored")
    r.door(2)
    r.door(1)
    r.event(2)
    check(r.state("energy") == 6 and r.model.mem[r.meta["state"]["pickups"]] & 1,
          "collected fruit flag across round trip")


def patrol(r):
    r.run(START)
    r.run(0)
    original = r.state("enemy_x"), r.state("enemy_z")
    r.run(0, 160)
    check((r.state("enemy_x"), r.state("enemy_z")) != original, "ghoul must move along the route")
    r.record("patrol", "2698   /   Ghoul on patrol")


def printer_packet(command, payload=b"", compressed=False, bad=False):
    body = bytes([command, int(compressed)]) + len(payload).to_bytes(2, "little") + payload
    checksum = (sum(body) + int(bad)) & 65535
    return b"\x88\x33" + body + checksum.to_bytes(2, "little") + b"\0\0"


def sunlight(r):
    r.run(START)
    r.run(0)
    r.run(SELECT)
    r.run(0)
    r.run(START)
    check(r.state("mode") == 3, "camera link page")
    init = printer_packet(1)
    print_page = printer_packet(2, bytes([1, 0, 0xe4, 0x40]))
    bad = printer_packet(4, bytes(640), bad=True)
    # Both raw and run-length encoded strips are normal Printer DATA.
    # The corrupt white strip must not dilute the prior black exposure.
    dark = printer_packet(4, bytes([0xff]) * 640)
    r.wire("dark-print", [(init, 0), (dark, 8), (bad, 1), (print_page, 6)])
    check(r.model.mem[r.meta["engine"]["photo_level"]] == 0, "dark photo must yield zero sunlight")
    bright = printer_packet(4, bytes([0xfd, 0]) * 5 + bytes([0x83, 0]), compressed=True)
    # 5*127 + 5 = 640 raw zero bytes; PRINT checksums cover the compressed wire data.
    r.wire("bright-print", [(init, 0), (bright, 8), (printer_packet(4), 8), (print_page, 6),
                            (printer_packet(15), 6), (printer_packet(15), 0)])
    check(r.model.mem[r.meta["engine"]["photo_level"]] == 3, "bright photo must yield full sunlight")
    r.run(0)
    r.record("sun-print", "Camera link   /   Bright print received")
    r.run(A)
    check(r.state("sun_level") == 3 and r.state("sun_source") == 2, "use measured exposure")


def verify_audio(out, host):
    """Capture real APU output, including the first percussion entry."""
    import wave
    proc = subprocess.run([host, "boktai.gb"], cwd=out, input="step 16 0\nstep 4 8\nstep 1 0\naudio 1080 score.pcm\n",
                          capture_output=True, text=True, check=True)
    _, rate, size = proc.stdout.strip().split()
    path = out / "score.pcm"
    samples = np.fromfile(path, dtype="<i2").reshape(-1, 2)
    check(len(samples) == int(size), "incomplete mGBA audio capture")
    check(np.std(samples[:int(rate) * 2]) > 32 and np.std(samples[-int(rate):]) > 32,
          "the cartridge produced silent audio")
    # Average four adjacent samples before decimating the 131072 Hz DMG stream.
    stride = 4 if int(rate) % 4 == 0 else 1
    audio = samples[:len(samples) // stride * stride].reshape(-1, stride, 2).mean(axis=1).astype("<i2")
    with wave.open(str(out / "prison.wav"), "wb") as wav:
        wav.setparams((2, 2, int(rate) // stride, len(audio), "NONE", "not compressed"))
        wav.writeframes(audio.tobytes())
    path.unlink()
    return {"rate": int(rate) // stride, "seconds": round(len(samples)/int(rate), 2),
            "rms": round(float(np.sqrt(np.mean(samples.astype(float)**2))), 2)}


def verify(out, manifest):
    host = playback(out)
    snapshots = []
    checks = []
    for name, script in (("campaign", campaign), ("patrol", patrol), ("sunlight", sunlight)):
        replay = Replay(out, manifest, name)
        script(replay)
        replay.verify_native(host)
        snapshots += replay.snaps
        checks.append(dict(replay=name, ticks=replay.ticks, peak_oam=replay.model.peak_oam,
                           peak_sprites_on_line=replay.model.peak_line, lowest_sp=hex(replay.model.lowest_sp),
                           missed_vblanks=replay.missed_vblanks, vblank_writes=replay.model.vblank_writes))
    rows = []
    for i in range(0, len(snapshots), 3):
        views = []
        for s in snapshots[i:i+3]:
            path = out / (("" if host else "ref-") + s["name"] + ".png")
            with Image.open(path) as im:
                views.append((s["caption"], im.resize((320, 288), Image.Resampling.NEAREST)))
        rows.append(("DMG  /  " + ("mGBA" if host else "reference CPU and PPU"), views))
    contact(rows, out / "contact.png")
    results = dict(native="libmgba" if host else None, checked=checks,
                   frames=[{k: v for k, v in s.items() if k != "state"} for s in snapshots])
    if host:
        results["audio"] = verify_audio(out, host)
    (out / "checks.json").write_text(json.dumps(results, indent=2) + "\n")
    print(f"{len(snapshots)} frames  {sum(s['ticks'] for s in checks)} game ticks  " +
          ("mGBA and reference pixels + WRAM agree" if host else "reference CPU and PPU verified"))
    return results

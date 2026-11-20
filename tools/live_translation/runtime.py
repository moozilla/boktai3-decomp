"""A private running Japanese cartridge and a per-encounter translation callback."""
import ctypes as C
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
from PIL import Image

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
OUT = ROOT / "build/live_translation"
sys.path[:0] = [str(ROOT / "tools"), str(ROOT / "tools/assets")]
from romlib import baserom_path, load_rom
from datafmt import text_bank
import saves

CALLBACK = C.CFUNCTYPE(C.c_uint32, *([C.c_uint32] * 8))
KEYS = dict(A=1, B=2, SELECT=4, START=8, RIGHT=16, LEFT=32, UP=64, DOWN=128, R=256, L=512)


def compile_host():
    subprocess.run(["make", "-s", "-f", "tools/live_translation/host.mk", "build/live_translation/runtime.so"],
                   cwd=ROOT, check=True)


class Game:
    def __init__(self, resolver, save=None):
        compile_host()
        self.rom = load_rom()
        self.core = C.CDLL(str(OUT / "runtime.so"))
        self.core.lt_pixels.restype = C.POINTER(C.c_uint8)
        self.scratch = 0x08F10000
        self.resolver = resolver
        self.installed = {}
        self.error = None
        self.tmp = tempfile.TemporaryDirectory(dir=OUT)
        battery = Path(self.tmp.name) / "game.sav"
        if save:
            shutil.copyfile(save, battery)
        self.callback = CALLBACK(self.encounter)
        ok = self.core.lt_open(bytes(Path(baserom_path())), bytes(battery) if save else None, self.callback)
        if not ok:
            raise RuntimeError("Could not initialize mGBA.")
        self.frame = 0

    def encounter(self, *args):
        try:
            return self.resolver(self, *args) or 0
        except BaseException as exc:
            self.error = exc
            return 0

    def read(self, addr, size):
        buf = C.create_string_buffer(size)
        self.core.lt_read(addr, buf, size)
        return buf.raw

    def install(self, data):
        if data not in self.installed:
            addr = (self.scratch + 3) & ~3
            end = addr + len(data)
            if end > 0x09000000 or any(self.rom[addr-0x08000000:end-0x08000000]):
                raise ValueError("No remaining scratch space in cartridge")
            self.core.lt_install(addr, data, len(data))
            self.installed[data] = addr
            self.scratch = end
        return self.installed[data]

    def step(self, n=1, keys=0):
        self.core.lt_step(n, keys)
        self.frame += n
        if self.error:
            raise self.error

    def rewind_line(self):
        frame = self.core.lt_rewind_line()
        if frame < 0:
            raise ValueError("No dialogue rewind point is available")
        self.frame = frame

    def tap(self, key):
        self.step(2, KEYS[key])
        self.step(8)

    def image(self):
        return Image.frombytes("RGBA", (240, 160), C.string_at(self.core.lt_pixels(), 240*160*4)).convert("RGB")

    def close(self):
        self.core.lt_close()
        self.tmp.cleanup()


def prison_save():
    """Original opening cutscene, reached via the documented scene entrance."""
    OUT.mkdir(parents=True, exist_ok=True)
    import struct
    regs = saves.current(saves.load(saves.TEMPLATE))
    v, g = bytearray(regs["vars"]), bytearray(regs["game"])
    struct.pack_into("<h", v, 0x3C, 2)
    struct.pack_into("<h", v, 0x70, 1)
    struct.pack_into("<I", g, 0x5A4, 2698)
    path = OUT / "prison.sav"
    saves.write_sav(dict(regs, vars=v, game=g), path)
    return path

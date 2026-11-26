"""Compare keypad-load counting with instruction-entry counting on the ROM."""

import os
from pathlib import Path
import subprocess
import tempfile
import time
import unittest

ROOT = Path(__file__).resolve().parents[2]
HARNESS = ROOT / "tools/emu/harness"
ROM = ROOT / "baserom.gba"
SCRIPT = """trace off
rtc 1763035200
lux 232
tick 08219B24
holdt NONE 611
holdt A 1
holdt NONE 67
holdt START 1
holdt NONE 5
holdt A 1
holdt NONE 20
dump ewram 02000000:40000
dump iwram 03000000:8000
shot end
"""


@unittest.skipUnless(HARNESS.is_file() and ROM.is_file(), "build the harness and provide baserom.gba")
class KeyTickTest(unittest.TestCase):
    def test_keypad_load_preserves_the_tick_replay(self):
        with tempfile.TemporaryDirectory(prefix="boktai-key-ticks-") as tmp:
            work = Path(tmp)
            script = work / "replay.txt"
            script.write_text(SCRIPT)
            ev = os.environ.copy()
            ev.pop("BOKTAI3_KEY_TICK_PC", None)
            ev.pop("BOKTAI3_SAV", None)
            results, timings = [], []
            for mode in ("entry", "keypad"):
                out = work / mode
                out.mkdir()
                if mode == "keypad":
                    ev["BOKTAI3_KEY_TICK_PC"] = "08219B2A"
                started = time.monotonic()
                result = subprocess.run(
                    [HARNESS, ROM, script, out], cwd=ROOT, env=ev,
                    capture_output=True, text=True, check=True, timeout=60,
                )
                timings.append(time.monotonic() - started)
                self.assertNotIn("holdt:", result.stderr)
                results.append((
                    [line for line in result.stderr.splitlines() if "] holdt " in line],
                    [(out / name).read_bytes() for name in ("ewram.bin", "iwram.bin", "end.ppm")],
                ))
            self.assertEqual(len(results[0][0]), 7)
            self.assertEqual(results[0], results[1])
            print(f"entry {timings[0]:.3f}s; keypad {timings[1]:.3f}s")


if __name__ == "__main__":
    unittest.main()

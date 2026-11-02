"""RAM sharing a Unicorn page with the modeled stack still receives its fixture."""
import os
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import semcheck


@unittest.skipIf(os.environ.get("CODEX_SANDBOX") == "seatbelt", "Unicorn requires JIT memory")
class StackEdgePages(unittest.TestCase):
    def test_engine_flags_are_seeded_and_stack_stays_zero(self):
        # A Thumb function returning immediately with bx lr.
        rom = bytes.fromhex("7047").ljust(semcheck.PAGE, b"\0")
        start = semcheck.ROM_BASE

        def fixture(seed, page):
            return bytes([page // semcheck.PAGE % 256]) * semcheck.PAGE

        with patch.object(semcheck, "page_bytes", side_effect=fixture):
            run = semcheck.Run(rom, start, start + 2, {}, 1000, [0] * 4)
        self.assertEqual(run.status, "ok")
        for address in [0x030053F4, semcheck.STACK_LO - 1, semcheck.STACK_TOP]:
            self.assertEqual(run.uc.mem_read(address, 1), fixture(1000, address)[:1])
        self.assertEqual(run.uc.mem_read(semcheck.STACK_LO, 1), b"\0")
        self.assertEqual(run.uc.mem_read(semcheck.STACK_TOP - 1, 1), b"\0")

"""tools/check.py removes its build/check/run-* scratch folder on exit; --keep leaves it.

Needs a built tree (build/boktai3.elf and baserom.gba); skipped otherwise."""
import glob
import os
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
UNIT = "src/sun_clock.c"


def runs():
    return set(glob.glob(os.path.join(ROOT, "build", "check", "run-*")))


@unittest.skipUnless(os.path.exists(os.path.join(ROOT, "build", "boktai3.elf"))
                     and os.path.exists(os.path.join(ROOT, "baserom.gba")), "needs a built tree")
class CheckCleanup(unittest.TestCase):
    def check(self, src, *flags):
        return subprocess.run([sys.executable, os.path.join(ROOT, "tools", "check.py"), src, *flags],
                              cwd=ROOT, capture_output=True, text=True)

    def test_scratch_folder_removed_on_match(self):
        before = runs()
        r = self.check(UNIT)
        self.assertIn("MATCH", r.stdout)
        self.assertEqual(runs() - before, set())
        self.assertNotIn("artifacts:", r.stdout)

    def test_scratch_folder_removed_on_mismatch_and_failure(self):
        before = runs()
        with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as d:
            bad = os.path.join(d, "bad.c")
            src = open(os.path.join(ROOT, UNIT)).read()
            open(bad, "w").write(src.replace("c->hourA > 23", "c->hourA > 22", 1))
            r = self.check(bad)
            self.assertIn("MISMATCH", r.stdout)
            broken = os.path.join(d, "broken.c")
            open(broken, "w").write(src + "\nthis is not C\n")
            r = self.check(broken)
            self.assertNotEqual(r.returncode, 0)
        self.assertEqual(runs() - before, set())

    def test_keep_leaves_the_folder(self):
        before = runs()
        r = self.check(UNIT, "--keep")
        new = runs() - before
        try:
            self.assertEqual(len(new), 1)
            (run,) = new
            self.assertTrue(os.path.exists(os.path.join(run, "comparison.json")))
            self.assertIn("artifacts: build/check/run-", r.stdout)
        finally:
            for run in new:
                shutil.rmtree(run, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()

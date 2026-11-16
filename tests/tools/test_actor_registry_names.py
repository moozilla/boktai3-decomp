"""The script command table's name column follows symbols/functions.csv."""
import os
import sys
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, "tools"))

from datafmt.keyed_callbacks import refresh_names  # noqa: E402
from symbolize import load_func_names  # noqa: E402


class ActorRegistryNames(unittest.TestCase):
    def test_refresh_masks_thumb_bit(self):
        text = "key,callback,name\n0101,0810031D,old\n0102,08000001,old\n"
        self.assertEqual(refresh_names(text, {0x0810031C: "Thing_Create"}),
                         "key,callback,name\n0101,0810031D,Thing_Create\n0102,08000001,sub_08000000\n")

    def test_committed_table_is_current(self):
        path = os.path.join(ROOT, "data", "actor_registry.csv")
        text = open(path, encoding="utf-8").read()
        self.assertEqual(text, refresh_names(text, load_func_names()),
                         "stale names: run tools/promote_names.py or tools/datafmt/check.py --extract actor_registry")


if __name__ == "__main__":
    unittest.main()

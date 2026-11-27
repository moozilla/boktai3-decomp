"""Exact source spellings for raw-field recovery."""
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

import rawfield


class AddressSpellingTests(unittest.TestCase):
    def test_untyped_pointer_can_keep_its_cast(self):
        source = "record = (void *)((u8 *)p + 0x40);"
        site, = rawfield.find_sites(source)
        self.assertEqual(rawfield.replacements(site, ["contact.position"], source),
                         ["&p->contact.position", "(void *)&p->contact.position"])

    def test_byte_pointer_preserves_const(self):
        source = "Emit((const u8 *)p + 0x20);"
        site, = rawfield.find_sites(source)
        self.assertEqual(rawfield.replacements(site, ["bytes[0]"], source),
                         ["p->bytes", "(const u8 *)p->bytes"])

    def test_dereference_is_replaced_by_a_value(self):
        source = "return *(u16 *)((u8 *)p + 0x20);"
        site, = rawfield.find_sites(source)
        self.assertEqual(rawfield.replacements(site, ["contact.flags"], source), ["p->contact.flags"])


if __name__ == "__main__":
    unittest.main()

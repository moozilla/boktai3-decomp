"""Synthetic fixtures only: no ROM or third-party source required."""
import importlib.util
from pathlib import Path
import struct
import unittest

spec = importlib.util.spec_from_file_location("soldec_audit", Path(__file__).parents[2] / "tools/soldec_audit.py")
audit = importlib.util.module_from_spec(spec)
spec.loader.exec_module(audit)


class AuditTests(unittest.TestCase):
    def test_hash_vectors(self):
        for word, value in [("", 0), ("if", 0xD86), ("eval", 0x64C0), ("return", 0xCD3A), ("switch", 0x4A6F)]:
            self.assertEqual(audit.strcode(word), value)
        with self.assertRaises(ValueError):
            audit.strcode("a\0b")

    def test_lengths_and_endianness(self):
        for data, wanted in [(b"\x3c", (1, 12)), (b"\x3d\xfe", (2, 254)),
                             (b"\x3e\x34\x12", (3, 0x1234)),
                             (b"\x3f\x56\x34\x12", (4, 0x123456))]:
            self.assertEqual(audit.compact_length(data, 0), wanted)
        with self.assertRaises(ValueError):
            audit.compact_length(b"\x3f\0\0", 0)

    def test_opaque_payload_and_stop(self):
        # Unknown bytes inside a bounded payload must remain opaque.
        result = audit.packets(b"\x62\x86\x0d\x32\xff\xff\0\xff", 0, 8)
        self.assertEqual(result["records"][0]["id"], 0xD86)
        self.assertEqual(result["next_offset"], 7)
        self.assertEqual(result["stop"], "end-marker")

    def test_fail_closed(self):
        for data in [b"\x39\0", b"\xff", b"\x61\0"]:
            with self.assertRaises(ValueError):
                audit.packets(data, 0, len(data))
        with self.assertRaises(ValueError):
            audit.packets(b"\x30\x30", 0, 2, max_records=1)
        with self.assertRaises(ValueError):
            audit.packets(b"\0", 0, 2)

    def test_table_and_no_automatic_naming(self):
        data = struct.pack("<IIII", 0xD86, 0x08000001, 0xFFFF, 0x02000001)
        rows = audit.audit_table(data, 0, 2, ["if"], {0x08000000})
        self.assertEqual(rows[0]["candidate_names"], ["if"])
        self.assertTrue(rows[0]["recognized_function_start"])
        self.assertFalse(rows[1]["thumb_rom_target"])
        self.assertEqual(rows[1]["candidate_names"], [])
        with self.assertRaises(ValueError):
            audit.audit_table(data, 0, 3)

    def test_hash_collision_is_preserved(self):
        # Different names can hash identically even in a tiny dictionary.
        self.assertEqual(audit.strcode("aA"), audit.strcode("b!"))
        data = struct.pack("<II", audit.strcode("aA"), 0x08000001)
        row = audit.audit_table(data, 0, 1, ["aA", "b!", "aA"])[0]
        self.assertEqual(row["candidate_names"], ["aA", "b!"])

    def test_starts_ignore_interior_labels(self):
        text = "\tthumb_func_start sub_08000000\nsub_08000000: @ 0x08000000\n_08000002: @ 0x08000002\n"
        self.assertEqual(audit.function_starts(text), {0x08000000})


if __name__ == "__main__":
    unittest.main()

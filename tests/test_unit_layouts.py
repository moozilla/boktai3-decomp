import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import layoutmerge
import unit_layouts


@unittest.skipUnless(Path(layoutmerge.AGBCC).is_file(), "agbcc is not built")
class LocalLayoutProbes(unittest.TestCase):
    def test_measures_nested_layout_and_union_aliases_with_stock_compiler(self):
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "unit.c"
            source.write_text("""#include "global.h"
struct Inner { unsigned char tag; short value; };
struct Outer { unsigned char prefix; struct Inner inner; unsigned flags : 3; };
union Aliases { unsigned word; unsigned short halves[2]; };
""")
            measured = {item["spell"]: item for item in unit_layouts.inspect(source)}
            outer = {field["name"]: field for field in measured["struct Outer"]["leaves"]}
            # agbcc gives an ordinary nested struct four-byte alignment.
            self.assertEqual(outer["inner.tag"]["off"], 4)
            self.assertEqual(outer["inner.value"]["off"], 6)
            self.assertEqual(outer["flags"]["bits"], 3)
            self.assertEqual(outer["flags"]["off"], 8)
            aliases = measured["union Aliases"]["leaves"]
            self.assertEqual({field["off"] for field in aliases}, {0})
            self.assertEqual(measured["union Aliases"]["size"], 4)

    def test_excludes_header_and_function_local_types(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "shared.h").write_text("struct Shared { int field; };\n")
            source = root / "unit.c"
            source.write_text("""#include "shared.h"
struct Local { int field; };
void Example(void) { struct Private { int field; }; }
""")
            self.assertEqual([item["spell"] for item in unit_layouts.inspect(source)], ["struct Local"])

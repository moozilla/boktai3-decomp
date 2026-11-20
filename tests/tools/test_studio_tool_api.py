"""Tool schema and source replies through the real line-based transport."""
import io
import json
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/studio"))
from experiments import fields, runner, tool_api


class ToolAPITests(unittest.TestCase):
    def test_transport_echoes_ids_and_accepts_later_requests_after_errors(self):
        source = io.StringIO('bad json\n'
                             '{"id":"known","tool":"list_checkpoints","arguments":{"extra":5}}\n'
                             '{"id":3,"tool":"list_checkpoints"}\n')
        output = io.StringIO()
        tool_api.serve(source, output)
        bad, extra, known = map(json.loads, output.getvalue().splitlines())
        self.assertIn("error", bad)
        self.assertEqual(extra["id"], "known")
        self.assertIn("error", extra)
        self.assertEqual(known["id"], 3)
        self.assertIn("boot", {row["id"] for row in known["result"]})

    def test_open_c_at_store_pc(self):
        result = tool_api.open_c("08159518")
        self.assertEqual(result["function"]["name"], "Player_AddEnergy")
        self.assertIn("p->f428 += d", result["c"]["text"])
        self.assertEqual(result["function"]["offset"], 36)

    @unittest.skipUnless(fields.AGBCC.is_file(), "Target compiler unavailable")
    def test_session_state_view_uses_original_c_layout(self):
        measured = fields.measure("gSessionState->solarBaseline")
        self.assertEqual((measured.offsets, measured.width, measured.signed), ((0, 4), 4, True))
        self.assertEqual(measured.header, "include/session_save.h")

    @unittest.skipUnless(runner.ROM.is_file(), "Local ROM unavailable")
    def test_solar_bounds_are_read_from_the_rom(self):
        lower = tool_api.read_solar_table("gUnk_08E8798A")
        upper = tool_api.read_solar_table("gUnk_08E87974")
        self.assertEqual(upper["entries"][5], {"address": "08E8797E", "bars": 5, "value": 49})
        self.assertEqual(lower["entries"][5]["value"], 35)
        self.assertEqual(upper["rom_sha256"], lower["rom_sha256"])

    def test_flagship_uses_sensor_input_and_keeps_coverage_control(self):
        from experiments import recipe, recharge

        spec = recipe.validate(recharge.reference_example())
        shade, passive, hold = spec["trials"]
        self.assertNotIn(recharge.GAUGE, {action["field"] for action in hold["interventions"]})
        self.assertNotIn(recharge.COVERAGE, {action["field"] for action in shade["interventions"]})
        self.assertEqual({action["field"]: action["value"] for action in passive["interventions"]}[recharge.COVERAGE],
                         0x26)
        self.assertEqual(hold["input"], "lux 188\nwait 240\nhold A 300")
        self.assertEqual(spec["duration"], 540)

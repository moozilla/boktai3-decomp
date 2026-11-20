"""Experiment contracts, attribution, and the socket-free Studio HTTP path."""
from http.server import BaseHTTPRequestHandler
import io
import json
import os
from pathlib import Path
import sys
import struct
import tempfile
import time
from types import SimpleNamespace
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/studio"))
from experiments import annotations, centaur, checks, fields, graphics, http, obj_pixels, recipe, runner, sweep


class RecipeTests(unittest.TestCase):
    def test_sample_axis_aligns_across_nondividing_waits_and_taps(self):
        input = "lux 23\nwait 3\ntap UP\nhold A+B 2\nlux 90"
        lines = list(recipe.timeline(recipe.compile_steps(input), 21, 5))
        self.assertEqual(lines, [
            "sample f0000", "shot f0000", "lux 23", "wait 3", "hold UP 2",
            "sample f0005", "shot f0005", "wait 5",
            "sample f0010", "shot f0010", "wait 3", "hold A+B 2",
            "sample f0015", "shot f0015", "lux 90", "wait 5",
            "sample f0020", "shot f0020", "wait 1", "sample f0021", "shot f0021",
        ])

    def test_script_disallows_host_io_memory_writes_and_unbounded_input(self):
        for text in ("loadstate ../../secret", "sample foo", "observe 02000000 4 0",
                     "shot a", "poke 02000000 1", "watch 03000000:4", "hold UP 1801",
                     "wait 0", "hold A+A 1", "lux 256", "tap invalid", "wait 1000\nwait 1000"):
            with self.subTest(text=text), self.assertRaises(ValueError):
                recipe.compile_steps(text)

    def test_validate_rejects_duplicate_trial_and_unobserved_check(self):
        spec = runner.sample_recipe()
        spec["trials"].append(spec["trials"][0])
        with self.assertRaisesRegex(ValueError, "unique"):
            recipe.validate(spec)
        spec["trials"].pop()
        spec["checks"][0]["field"] = "gPlayers[1]->position.x"
        with self.assertRaisesRegex(ValueError, "field delta"):
            recipe.validate(spec)

    def test_recipe_normalization_can_be_rerun(self):
        spec = runner.sample_recipe()
        self.assertEqual(recipe.validate(spec), spec)
        self.assertEqual(sum(1 for line in recipe.timeline([], 7, 4) if line.startswith("sample")), 3)

    def test_fork_is_restricted_to_a_run_and_captured_frame(self):
        spec = runner.sample_recipe()
        spec.update(fork={"run": "0" * 32, "trial": "menu", "frame": 60}, **{"continue": False})
        self.assertEqual(recipe.validate(spec)["fork"], spec["fork"])
        for bad in ("/tmp/slot.ss", "0" * 31):
            spec["fork"]["run"] = bad
            with self.assertRaisesRegex(ValueError, "run ID"):
                recipe.validate(spec)

    def test_outside_luminance_follows_setup_and_sample_order(self):
        commands = ["lux 86", *recipe.timeline(recipe.compile_steps("wait 5\nlux 12"), 10, 5, save_states=True)]
        values, end = runner.light_at_sample(commands, 232)
        self.assertEqual(values, {"f0000": 86, "f0005": 86, "f0010": 12})
        self.assertIn("save f0005", commands)
        self.assertEqual(end, 12)


class SpriteEvidenceTests(unittest.TestCase):
    def test_uses_2d_tile_row_and_checks_captured_color(self):
        oam = bytearray(struct.pack("<4H", 0x200, 0, 0, 0) * 128)
        struct.pack_into("<4H", oam, 24, 0x4000, 10, 0x601F, 0)
        vram = bytearray(0x8000)
        vram[0] = 9  # the second horizontal tile wraps from slot 31 to slot 0
        palette = bytearray(0x200)
        struct.pack_into("<H", palette, 2 * (6 * 16 + 9), 0x001F)
        matched = obj_pixels.matching_objects(oam, vram, palette, 0x1000, 18, 0, (255, 0, 0))
        self.assertEqual([(row["slot"], row["tile"]) for row in matched], [(3, 0)])
        self.assertEqual(obj_pixels.matching_objects(oam, vram, palette, 0x1000, 18, 0, (0, 0, 0)), [])
        self.assertEqual(obj_pixels.matching_objects(oam, vram, palette, 0x0000, 18, 0, (255, 0, 0)), [])

    def test_affine_double_size_coordinates_and_boundaries(self):
        oam = bytearray(0x400)
        for offset, word in zip((6, 14, 22, 30), (0, 256, -256, 0)):
            struct.pack_into("<h", oam, offset, word)
        attrs = (0x0306, 10, 0)
        self.assertEqual(obj_pixels.tile_xy(oam, attrs, 18, 14), (4, 4, 8))
        self.assertEqual(obj_pixels.tile_xy(oam, attrs, 20, 12), (2, 2, 8))
        self.assertIsNone(obj_pixels.tile_xy(oam, attrs, 10, 6))

    def test_duplicate_pending_submissions_keep_identity_unresolved(self):
        attrs = {"attr0": "0001", "attr1": "0002", "attr2": "0003"}
        direct = {**attrs, "queue": "d", "index": "1"}
        pending = [{**attrs, "queue": "p", "index": str(i)} for i in range(2)]
        self.assertEqual(graphics.sources_for([direct, *pending], 1, (1, 2, 3)), [direct])
        self.assertEqual(graphics.sources_for([direct, *pending], 5, (1, 2, 3)), pending)


class RecordingTests(unittest.TestCase):
    def test_imports_only_continuous_controller_input_from_the_named_capture(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(centaur, "ROOT_CAPTURE", Path(temp)):
            text = Path(temp) / "day-one.txt"
            state = text.with_suffix(".ss")
            state.write_bytes(b"local state")
            text.write_text("# Comment\nloadstate day-one.ss\nwait 2\nhold UP 4000\n")
            spec = centaur.import_recording("recording-day-one")
            self.assertEqual(spec["checkpoint"], "recording-day-one")
            self.assertEqual(spec["duration"], recipe.MAX_FRAMES)
            self.assertEqual(spec["trials"][0]["input"], "wait 2\nhold UP 1798")
            self.assertFalse(spec["continue"])
            text.write_text("loadstate day-one.ss\nwait 2\nloadstate ../outside.ss\n")
            with self.assertRaisesRegex(ValueError, "continuous recording"):
                centaur.import_recording("recording-day-one")
            text.write_text("loadstate ../outside.ss\nwait 2\n")
            with self.assertRaises(FileNotFoundError):
                centaur.import_recording("recording-day-one")


class SweepTests(unittest.TestCase):
    def test_selection_preserves_order_and_reports_missing_checkpoints(self):
        fixtures = [{"id": "bank-0040", "kind": "bank"}, {"id": "test-save", "kind": "save"}]
        with patch.object(sweep.runner, "checkpoints", return_value=fixtures):
            self.assertEqual(sweep.select(["test-save", "bank-0040"], False, None), list(reversed(fixtures)))
            self.assertEqual(sweep.select([], True, None), fixtures[:1])
            with self.assertRaisesRegex(ValueError, "unavailable"):
                sweep.select(["bank-1234"], False, None)


class FieldTests(unittest.TestCase):
    def test_compile_request_preserves_pointer_hops_and_symmetrically_bounds_array(self):
        parsed = fields.parse("gRoot[2]->next->values[3].position.x")
        _, hops, indices = fields.layout_request(parsed, "engine.h")
        self.assertEqual(hops, 3)
        self.assertEqual([x[0] for x in indices], [2, 3])

    def test_untrusted_text_cannot_become_c_source(self):
        for path in ("gFoo;#include", "gFoo+1", "gFoo[-1]", "gFoo->", "gFoo[01]#x", "gGameState->life ()"):
            with self.subTest(path=path), self.assertRaises(ValueError):
                fields.parse(path)

    @unittest.skipUnless(fields.AGBCC.is_file(), "Stock agbcc required for layout measurement")
    def test_real_target_layout_and_bound(self):
        x = fields.measure("gPlayers[0]->position.x")
        self.assertEqual((x.address, x.offsets, x.width, x.signed), (0x02000580, (0, 0x30), 2, True))
        with self.assertRaisesRegex(ValueError, "array index"):
            fields.measure("gPlayers[4]->position.x")
        with self.assertRaises(ValueError):
            fields.measure("gPlayers[0]->position")

    def test_function_resolution_respects_symbol_extents(self):
        index = fields.FunctionIndex({
            "files": ["src/one.c"],
            "categories": [{"units": [{"functions": [["One", "08002000", 8, 100, 0, 0]]}]}],
        }, {0x08002000: "One"})
        self.assertEqual(index.lookup(0x08002005)["offset"], 4)
        self.assertIsNone(index.lookup(0x08002008))
        self.assertIsNone(index.lookup(0x08001000))


class EvidenceTests(unittest.TestCase):
    def test_only_last_matching_write_can_be_attributed(self):
        watched = fields.Field("p->health", "p", 0x02000040, (0, 0x12), 2, True, "include/h.h")
        sample = """kind\tsequence\tframe\tfield\tlabel\taddress\tvalue\tbefore\tpc
sample\t1\t20\t0\tf0000\t02004112\t00000003\t-\t-
write\t2\t21\t0\t-\t02004112\t0000FFFF\t00000003\t08002002
sample\t3\t22\t0\tf0002\t02004112\t0000FFFF\t-\t-
sample\t4\t24\t0\tf0004\t02004112\t00000009\t-\t-
bind\t5\t25\t0\t-\t02008012\t00000009\t02004112\t08002004
sample\t6\t26\t0\tf0006\t02008012\t00000009\t-\t-
"""
        index = SimpleNamespace(lookup=lambda pc: {"name": "Apply", "offset": pc - 0x08002000})
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "events.tsv"
            path.write_text(sample)
            snapshots, events = runner.parse_events(path, [watched], index)
        rows = [x["fields"][0] for x in snapshots]
        self.assertEqual([x["state"] for x in rows], ["initial", "observed", "unobserved", "rebound"])
        self.assertIsNone(rows[2]["last_write"])
        self.assertIsNone(rows[3]["last_write"])
        self.assertIs(rows[1]["last_write"], events[0])
        self.assertEqual((events[0]["before"], events[0]["value"], events[0]["pc"]), (3, -1, "08002002"))
        self.assertEqual(events[1]["before_address"], "02004112")

    def test_scope_only_contributes_exact_compiler_rows(self):
        row = {"tag": "=", "rom_addr": "08002002"}
        self.assertEqual(annotations.exact_rows("header\nMATCH\n", [row]), {"08002002": row})
        self.assertFalse(annotations.exact_rows("MATCH [-g]", [row]))
        self.assertFalse(annotations.exact_rows("MATCH", [{**row, "tag": "!"}]))

    def test_field_checks_do_not_compare_distinct_lifetimes(self):
        check = {"statement": "Life fell", "trial": "x", "from": 0, "to": 10,
                 "field": "p->hp", "measure": "delta", "compare": "lt", "value": 0}
        def snap(frame, value, address, state):
            return {"relative": frame, "fields": [{"value": value, "address": address, "state": state}]}
        report = {"fields": [{"path": "p->hp"}], "trials": [{"id": "x", "snapshots": [
            snap(0, 20, "02004012", "initial"), snap(5, 14, "02008012", "rebound"),
            snap(10, 5, "02004012", "rebound"),
        ]}]}
        self.assertEqual(checks.evaluate([check], report)[0]["state"], "unresolved")

    def test_trace_checks_count_entries_in_the_half_open_frame_range(self):
        check = {"statement": "Function ran", "trial": "x", "from": 3, "to": 5,
                 "function": "Player_Update", "measure": "entries", "compare": "eq", "value": 3}
        result = {"fields": [], "trials": [{"id": "x", "probes": [
            {"name": "Player_Update", "frames": {"2": 2, "3": 1, "4": 2, "5": 5}},
        ]}]}
        self.assertEqual(checks.evaluate([check], result)[0]["state"], "confirmed")


class HTTPTests(unittest.TestCase):
    def request(self, method, path, payload=None, **headers):
        class Socket:
            def __init__(self, request):
                self.request = io.BytesIO(request)
                self.response = bytearray()

            def makefile(self, *args):
                return self.request

            def sendall(self, data):
                self.response.extend(data)

        class Handler(BaseHTTPRequestHandler):
            def send(self, status, data, kind="application/json"):
                self.send_response(status)
                self.send_header("Content-Type", kind)
                self.end_headers()
                self.wfile.write(json.dumps(data).encode() if isinstance(data, (dict, list)) else data)

            def do_GET(self):
                http.routes(self, "GET")

            def do_POST(self):
                http.routes(self, "POST")

            def log_message(self, *args):
                pass

        body = b"" if payload is None else json.dumps(payload).encode()
        headers = {"Host": "localhost:8642", "Content-Length": str(len(body)), **headers}
        wire = f"{method} {path} HTTP/1.0\r\n" + "".join(f"{k}: {v}\r\n" for k, v in headers.items())
        socket = Socket(wire.encode() + b"\r\n" + body)
        Handler(socket, ("127.0.0.1", 1), SimpleNamespace(trans="/nonexistent"))
        head, data = bytes(socket.response).split(b"\r\n\r\n", 1)
        return int(head.split()[1]), data

    def test_serves_real_page_scripts_and_example(self):
        code, body = self.request("GET", "/experiments")
        self.assertEqual(code, 200)
        self.assertIn(b"Frame evidence", body)
        self.assertEqual(self.request("GET", "/experiments/app.js")[0], 200)
        code, body = self.request("GET", "/api/experiments/example")
        self.assertEqual(code, 200)
        self.assertEqual(recipe.validate(json.loads(body)), runner.sample_recipe())

    def test_rejects_cross_origin_and_wrong_content_type(self):
        for headers in ({"Content-Type": "text/plain"},
                        {"Content-Type": "application/json", "Origin": "http://elsewhere.invalid"}):
            with self.subTest(headers=headers):
                code, _ = self.request("POST", "/api/experiments/runs", runner.sample_recipe(), **headers)
                self.assertEqual(code, 400)

    def test_dead_worker_does_not_leave_a_permanent_running_state(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(runner, "JOBS", Path(temp)):
            directory = runner.init_run(runner.sample_recipe())
            running = runner.read_json(directory / "status.json")
            running.update(state="running", pid=os.getpid() + 10000)
            runner.atomic_json(directory / "status.json", running)
            with patch.object(runner.os, "kill", side_effect=ProcessLookupError):
                code, body = self.request("GET", f"/api/experiments/runs/{directory.name}")
            self.assertEqual(code, 200)
            self.assertEqual(json.loads(body)["state"], "interrupted")

    def test_submit_publishes_normalized_recipe_and_hides_private_files(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(runner, "JOBS", Path(temp)):
            with patch.object(http.POOL, "submit") as dispatch, patch.object(http.SLOTS, "acquire", return_value=True):
                code, body = self.request("POST", "/api/experiments/runs", runner.sample_recipe(),
                                          **{"Content-Type": "application/json"})
            self.assertEqual(code, 202)
            identifier = json.loads(body)["id"]
            self.assertEqual(dispatch.call_count, 1)
            directory = runner.run_dir(identifier)
            self.assertEqual(runner.read_json(directory / "recipe.json"), runner.sample_recipe())
            (directory / "private.sav").touch()
            for path in ("private.sav", "field/../private.sav", "field/f0000.ss", "prepare/fork.ss"):
                with self.subTest(path=path):
                    self.assertEqual(self.request("GET", f"/api/experiments/runs/{identifier}/artifact/{path}")[0], 404)
            self.assertEqual(self.request("GET", f"/api/experiments/runs/{identifier}")[0], 200)

    @unittest.skipUnless(os.getenv("STUDIO_EXPERIMENT_ROM_TESTS") == "1" and runner.ROM.is_file(),
                         "Local ROM and explicit integration opt-in required")
    def test_actual_run_and_snapshot_fork_through_http(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(runner, "JOBS", Path(temp)):
            def run(spec):
                code, data = self.request("POST", "/api/experiments/runs", spec,
                                          **{"Content-Type": "application/json"})
                self.assertEqual(code, 202)
                job = json.loads(data)["id"]
                end = time.monotonic() + 45
                while time.monotonic() < end:
                    code, payload = self.request("GET", f"/api/experiments/runs/{job}")
                    self.assertEqual(code, 200)
                    state = json.loads(payload)
                    self.assertNotEqual(state["state"], "failed", state["message"])
                    if state["state"] == "complete":
                        break
                    time.sleep(0.1)
                self.assertEqual(state["state"], "complete")
                code, data = self.request("GET", f"/api/experiments/runs/{job}/result")
                self.assertEqual(code, 200)
                return json.loads(data)

            parent = run(runner.sample_recipe())
            self.assertTrue(all(c["state"] == "confirmed" for c in parent["checks"]))
            field = parent["trials"][0]["snapshots"][-1]["fields"][1]
            self.assertEqual(field["last_write"]["pc"], "0821C7F4")
            self.assertIn("Player_Update", [entry["function"]["name"] for entry in field["last_write"]["context"]])
            image = f"/api/experiments/runs/{parent['id']}/artifact/menu/f0060.png"
            self.assertEqual(self.request("GET", image)[1][:8], b"\x89PNG\r\n\x1a\n")
            code, payload = self.request("POST", f"/api/experiments/runs/{parent['id']}/snapshot/menu/60",
                                         ["gGameState->life", "gPlayers[0]->position.z"],
                                         **{"Content-Type": "application/json"})
            self.assertEqual(code, 200)
            result = json.loads(payload)
            self.assertEqual(result["game_frame"], parent["trials"][1]["snapshots"][4]["frame"])
            self.assertEqual([row["value"] for row in result["fields"]], [616, 2694])
            code, payload = self.request("GET", "/api/experiments/by-function/0821C754")
            self.assertEqual(code, 200)
            links = json.loads(payload)
            self.assertTrue(any(link["kind"] == "stores" and parent["id"] in link["url"] for link in links))
            self.assertTrue(any(link["kind"] == "entries" for link in links))
            code, payload = self.request("GET", f"/api/experiments/runs/{parent['id']}/pixel/field/60/117/88")
            self.assertEqual(code, 200)
            pixel = json.loads(payload)
            self.assertEqual(pixel["upload"]["upload_pc"], "082141E8")
            self.assertEqual(pixel["upload"]["upload_frame"] + 1, pixel["game_frame"])
            self.assertEqual(len(pixel["objects"]), 1)
            draw = pixel["objects"][0]["submissions"][0]
            self.assertEqual(draw["runtime_pc"], "030000E0")
            self.assertEqual(draw["function"]["source"], "src/iwram_code.c")
            self.assertEqual(draw["asset"]["hash"], "DE23")
            self.assertEqual(draw["actor"]["member"], "gPlayers[0]->animation")
            self.assertEqual(draw["actor"]["fields"][1]["value"], 2694)
            slot = pixel["objects"][0]["slot"]
            code, payload = self.request("POST", f"/api/experiments/runs/{parent['id']}/pixel-check/field/60/117/88/{slot}",
                                         {}, **{"Content-Type": "application/json"})
            proof = json.loads(payload)
            self.assertEqual(code, 200, proof)
            self.assertTrue(proof["changed"])
            self.assertEqual(proof["before"], [247, 0, 49])
            self.assertEqual(proof["after"], [0, 0, 0])
            self.assertEqual(proof["matching_final_state_sha256"], parent["trials"][0]["snapshots"][4]["state_sha256"])
            self.assertEqual(self.request("GET", proof["contact_sheet"])[1][:8], b"\x89PNG\r\n\x1a\n")
            bad = f"/api/experiments/runs/{parent['id']}/pixel/field/60/240/88"
            self.assertEqual(self.request("GET", bad)[0], 400)
            from experiments.__main__ import proof_branch
            child = run(proof_branch(parent))
            self.assertTrue(all(c["state"] == "confirmed" for c in child["checks"]))
            self.assertEqual(child["checkpoint"]["fork"]["run"], parent["id"])


if __name__ == "__main__":
    unittest.main()

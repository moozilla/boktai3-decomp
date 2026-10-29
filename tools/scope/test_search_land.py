"""The session-to-production boundary must verify or restore the source unit."""
from pathlib import Path
import json
import os
import subprocess
import tempfile
import unittest
from unittest import mock
import search_land
import scope


SOURCE = '#ifdef NONMATCHING\nint F(void){return 1;}\n#else\nINCLUDE_ASM("asm/nonmatching", F);\n#endif\n'


class FakeSession:
    def __init__(self):
        self.events = []

    def metadata(self):
        return {"function": "F", "origin": "src/f.c"}

    def context(self, node):
        return {"address": 0x08001000, "size": 4}

    def source(self, ref):
        return SOURCE

    def note(self, *args, **kwargs):
        self.events.append((args, kwargs))

    def set_metadata(self, **kwargs):
        self.events.append(kwargs)


class LandingTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name).resolve()
        (self.root / "src").mkdir()
        self.target = self.root / "src/f.c"
        self.target.write_text(SOURCE)
        self.session = FakeSession()
        self.node = {"id": "abcdef123456789"}
        self.patch = mock.patch.object(search_land, "ROOT", self.root)
        self.patch.start()
        self.build_patch = mock.patch.object(scope.build, "ROOT", str(self.root))
        self.build_patch.start()

    def tearDown(self):
        self.build_patch.stop()
        self.patch.stop()
        self.tmp.cleanup()

    def test_timeout_stops_formatter_process_group_before_restoration(self):
        process = mock.MagicMock()
        process.pid = 12345
        process.communicate.side_effect = [subprocess.TimeoutExpired("campaign", 90), ("", "")]
        process.__enter__.return_value = process
        with mock.patch("search_land.subprocess.Popen", return_value=process) as spawn, \
             mock.patch("search_land.os.killpg") as kill:
            with self.assertRaises(subprocess.TimeoutExpired):
                search_land.run_campaign(["campaign"], text=True, capture_output=True, timeout=90)
        self.assertTrue(spawn.call_args.kwargs["start_new_session"])
        kill.assert_called_once_with(12345, search_land.signal.SIGKILL)
        self.assertEqual(process.communicate.call_count, 2)

    def test_only_unmodified_exact_results_may_land(self):
        for result in ({"exact": False}, {"exact": True, "experiment": True},
                       {"exact": True, "experiment_build": "debug"}):
            with self.assertRaisesRegex(ValueError, "unmodified"):
                search_land.land(self.session, self.node, result)
        self.assertEqual(self.target.read_text(), SOURCE)

    def test_success_uses_session_base_and_cleans_compiler_knobs(self):
        def verify(args, **kwargs):
            directory = self.root / "build/scope/campaign"
            self.assertEqual((directory / "F/base.c").read_text(), SOURCE)
            self.assertEqual((directory / "F/best_match.c").read_text(), SOURCE)
            self.assertNotIn("AGBCC_FORCE", kwargs["env"])
            self.assertEqual(json.loads((directory / "queue.json").read_text())[0]["src"], "src/f.c")
            self.target.write_text("int F(void){return 1;}\n")
            return subprocess.CompletedProcess(args, 0, "F: unit MATCH", "")
        with mock.patch.dict(os.environ, {"AGBCC_FORCE": "a:0"}), \
             mock.patch("search_land.run_campaign", side_effect=verify), \
             mock.patch("search_land.production_concessions", return_value={"verdict": "NONE_FOUND", "pins": 0, "barriers": 0}):
            result = search_land.land(self.session, self.node, {"exact": True, "experiment_build": "normal no-g build"})
        self.assertTrue(result["production_unit_match"])
        self.assertTrue(self.session.events)
        self.assertNotIn("NONMATCHING", self.target.read_text())

    def test_failed_or_interrupted_verification_restores_source(self):
        for failure in (subprocess.CompletedProcess([], 1, "UNIT MISMATCH", ""), TimeoutError("interrupted")):
            def verify(*args, **kwargs):
                self.target.write_text("partial output")
                if isinstance(failure, Exception):
                    raise failure
                return failure
            with mock.patch("search_land.run_campaign", side_effect=verify):
                with self.assertRaises((ValueError, TimeoutError)):
                    search_land.land(self.session, self.node, {"exact": True})
            self.assertEqual(self.target.read_text(), SOURCE)
            self.assertEqual(self.session.events, [])

    def test_scratch_origin_needs_explicit_unit(self):
        with mock.patch.object(self.session, "metadata", return_value={"function": "F", "origin": "build/f.c"}):
            with self.assertRaisesRegex(ValueError, "--unit"):
                search_land.land(self.session, self.node, {"exact": True})

    def test_other_matching_function_macro_pins_restore_unit_marker(self):
        other = '#define PIN "r" "1"\nint Other(int x) { register int y asm(PIN)=x; return y; }\n'
        def verify(args, **kwargs):
            self.target.write_text(other + 'int F(void){return 1;}\n')
            return subprocess.CompletedProcess(args, 0, "F: unit MATCH", "")
        # Mock only the campaign process; production CPP must actually run.
        # subprocess.run is the shared module, so give CPP its real runner.
        actual_run = subprocess.run
        def dispatch(args, **kwargs):
            return verify(args, **kwargs) if "tools/scope/campaign.py" in args else actual_run(args, **kwargs)
        with mock.patch("search_land.run_campaign", side_effect=dispatch):
            result = search_land.land(self.session, self.node, {"exact": True, "concessions": {"pins": 0, "barriers": 0}})
        self.assertEqual(result["concessions"]["pins"], 1)
        self.assertTrue(self.target.read_text().startswith("// MATCHING_CONCESSION: register-pinning\n"))

    def test_production_counts_drop_neighbor_drafts_and_unused_helpers(self):
        self.target.write_text('''
static inline int Unused(int x) { register int p asm("r2")=x; return p; }
#ifdef NONMATCHING
int Draft(int x) { register int q asm("r3")=x; return q; }
#else
asm(".global Draft");
#endif
int F(void) { return 1; }
''')
        with mock.patch.object(scope.build, "DEFINES", ["-DNONMATCHING"]):
            result = search_land.production_concessions("src/f.c")
            self.assertEqual(scope.build.DEFINES, ["-DNONMATCHING"])
        self.assertEqual(result["pins"], 0)
        self.assertEqual(set(result["by_function"]), {"F"})

    def test_preprocessing_unknown_preserves_previous_marker(self):
        marker = "// MATCHING_CONCESSION: register-pinning\n"
        self.target.write_text(marker + SOURCE)
        def verify(args, **kwargs):
            self.target.write_text('int F(void){return 1;}\n')
            return subprocess.CompletedProcess(args, 0, "F: unit MATCH", "")
        with mock.patch("search_land.run_campaign", side_effect=verify), \
             mock.patch("search_land.production_concessions", return_value={"verdict": "UNKNOWN"}):
            search_land.land(self.session, self.node, {"exact": True})
        self.assertTrue(self.target.read_text().startswith(marker))

    def test_verified_unit_without_concessions_removes_stale_marker(self):
        marker = "// MATCHING_CONCESSION: register-pinning\n"
        self.target.write_text(marker + SOURCE)
        def verify(args, **kwargs):
            self.target.write_text(marker + 'int F(void){return 1;}\n')
            return subprocess.CompletedProcess(args, 0, "F: unit MATCH", "")
        with mock.patch("search_land.run_campaign", side_effect=verify), \
             mock.patch("search_land.production_concessions", return_value={"verdict": "NONE_FOUND", "pins": 0, "barriers": 0}):
            search_land.land(self.session, self.node, {"exact": True})
        self.assertNotIn(marker, self.target.read_text())

    def test_failed_verification_restores_existing_landed_marker(self):
        worker = self.root / "build/scope/campaign/F"
        worker.mkdir(parents=True)
        (worker / "landed").write_text("old successful record")
        def verify(args, **kwargs):
            self.target.write_text("partial output")
            (worker / "landed").write_text("new false record")
            return subprocess.CompletedProcess(args, 0, "missing unit verdict", "")
        with mock.patch("search_land.run_campaign", side_effect=verify):
            with self.assertRaises(ValueError):
                search_land.land(self.session, self.node, {"exact": True})
        self.assertEqual((worker / "landed").read_text(), "old successful record")
        self.assertEqual(self.target.read_text(), SOURCE)

    def test_failed_post_verification_marker_write_restores_source_and_landed_record(self):
        worker = self.root / "build/scope/campaign/F"
        def verify(args, **kwargs):
            self.target.write_text("verified candidate")
            (worker / "landed").write_text("false record")
            return subprocess.CompletedProcess(args, 0, "F: unit MATCH", "")
        original_write = search_land.write_atomic
        def fail_target(path, text):
            if Path(path) == self.target:
                raise OSError("synthetic disk full")
            return original_write(path, text)
        with mock.patch("search_land.run_campaign", side_effect=verify), \
             mock.patch("search_land.production_concessions", return_value={"verdict": "NONE_FOUND", "pins": 0, "barriers": 0}), \
             mock.patch("search_land.write_atomic", side_effect=fail_target):
            with self.assertRaises(OSError):
                search_land.land(self.session, self.node, {"exact": True})
        self.assertEqual(self.target.read_text(), SOURCE)
        self.assertFalse((worker / "landed").exists())

    def test_existing_undrafted_queue_path_rejected_before_artifact_writes(self):
        directory = self.root / "build/scope/campaign"
        directory.mkdir(parents=True)
        (directory / "queue.json").write_text(json.dumps([{"func": "F", "addr": "08001000", "kind": "draft"}]))
        with self.assertRaisesRegex(ValueError, "undrafted-function"):
            search_land.land(self.session, self.node, {"exact": True})
        self.assertFalse((directory / "F").exists())


if __name__ == "__main__":
    unittest.main()

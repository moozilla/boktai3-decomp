"""Integration seams that previously lost evidence or selected a stale HEAD."""
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest import mock
import subprocess
import types

sys.path.insert(0, str(Path(__file__).resolve().parent))
import search
from search_session import Session


class FakeEnvironment:
    version = 1
    calls = 0

    def context(self, source, function, origin):
        return {"version": self.version, "function": function, "origin": origin}

    def evaluate(self, source, function, origin, knobs, trials, expected_context=None):
        self.calls += 1
        return {"status": "different", "exact": False, "counts": "1 ROM / 1 draft",
                "experiment": bool(knobs), "semantic": {"verdict": "NOT_RUN"}}


class SearchIntegrationTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.path = Path(self.tmp.name)
        self.db = str(self.path / "test.sqlite3")
        self.source = self.path / "draft.c"
        self.source.write_text("int F(int x) { return x+1; }\n")
        self.env = FakeEnvironment()
        self.patch = mock.patch.object(search, "environment", return_value=self.env)
        self.patch.start()
        self.command("init", source=str(self.source), function="F")

    def tearDown(self):
        self.patch.stop()
        self.tmp.cleanup()

    def command(self, command, **kwargs):
        return search.dispatch({"command": command, "session": self.db, **kwargs})

    def test_try_retains_siblings_without_moving_head(self):
        head = self.command("status")["node"]
        files = []
        for index, expression in enumerate(("1+x", "x+2")):
            path = self.path / ("variant%d.c" % index)
            path.write_text("int F(int x){return %s;}" % expression)
            files.append(str(path))
        results = self.command("try", sources=files, parent=head, reason="compare source hypotheses")
        self.assertEqual(len(results), 2)
        self.assertEqual(self.command("status")["node"], head)
        self.assertTrue(all(r["parents"] == [head] for r in results))
        self.assertEqual(self.command("status")["nodes"], 3)

    def test_new_extern_abi_labels_are_not_instruction_bodies(self):
        self.source.write_text('extern int Table[] asm("RomTable");\nint F(int x){return x+1;}')
        self.command("import", source=str(self.source))
        self.source.write_text('int F(int x){asm("// nop");return x;}')
        with self.assertRaisesRegex(ValueError, "instruction bodies"):
            self.command("import", source=str(self.source))

    def test_import_uses_current_context_and_head_is_evaluated(self):
        self.env.version = 2
        self.source.write_text("int F(int x) { return 1+x; }\n")
        result = self.command("import", source=str(self.source), reason="commute")
        status = self.command("status")
        self.assertEqual(result["node"], status["node"])
        self.assertEqual(status["status"], "different")
        self.assertEqual(status["nodes"], 2)  # no unevaluated intermediate node
        with Session(self.db) as session:
            self.assertEqual(session.context()["version"], 2)

    def test_environment_refresh_selects_refreshed_head(self):
        old = self.command("status")["node"]
        self.env.version = 2
        result = self.command("evaluate")
        self.assertNotEqual(old, result["node"])
        self.assertEqual(result["node"], self.command("status")["node"])

    def test_dependency_change_during_compile_retries_without_stale_evidence(self):
        from search_eval import StaleContextError
        original = self.env.evaluate
        calls = []
        def changing(*args, **kwargs):
            calls.append(kwargs["expected_context"])
            if len(calls) == 1:
                self.env.version = 2
                raise StaleContextError("generated assembly appeared during compilation")
            return original(*args, **kwargs)
        old = self.command("status")["node"]
        with mock.patch.object(self.env, "evaluate", side_effect=changing):
            result = self.command("evaluate", force=True)
        self.assertEqual([c["version"] for c in calls], [1, 2])
        self.assertNotEqual(old, result["node"])
        with Session(self.db) as session:
            self.assertEqual(len(session.node(old)["evaluations"]), 1)
            self.assertEqual(session.context()["version"], 2)
            # The old node's lease was released before retrying the new node.
            self.assertTrue(session.claim(old, options={"knobs": {}, "semantic_trials": 0, "force_attempt": os.getpid()}))

    def test_repeated_context_changes_stop_after_bounded_retries(self):
        from search_eval import StaleContextError
        def changing(*args, **kwargs):
            self.env.version += 1
            raise StaleContextError("inputs keep changing")
        with mock.patch.object(self.env, "evaluate", side_effect=changing) as run:
            with self.assertRaises(StaleContextError):
                self.command("evaluate", force=True)
        self.assertEqual(run.call_count, 3)

    def test_cache_hit_and_forced_evaluation_both_work_with_leases(self):
        self.assertTrue(self.command("evaluate")["cache_hit"])
        self.assertEqual(self.env.calls, 1)
        self.assertFalse(self.command("evaluate", force=True)["cache_hit"])
        self.assertEqual(self.env.calls, 2)

    def test_experiments_do_not_replace_plain_evaluation(self):
        self.command("experiment", knobs={"AGBCC_FORCE": "16:0"})
        with Session(self.db) as session:
            plain = session.evaluation(options={"knobs": {}, "semantic_trials": 0})
            self.assertFalse(plain["result"]["experiment"])
            self.assertEqual(len(session.node()["evaluations"]), 2)

    def test_register_labels_are_accepted_but_instruction_bodies_rejected(self):
        self.source.write_text('int F(int x) { register int a asm("r1") = x; return a; }')
        self.command("import", source=str(self.source))
        self.source.write_text('int F(int x) { asm("mov r0, r1"); return x; }')
        with self.assertRaisesRegex(ValueError, "instruction bodies"):
            self.command("import", source=str(self.source))

    def test_explicit_trace_reference_reports_refreshed_node(self):
        self.env.scope = types.SimpleNamespace(WORK="")
        self.env.materialize = lambda source, origin: self.source
        old = self.command("status")["node"]
        self.env.version = 2
        with mock.patch("search_trace.collect", return_value={"allocations": [], "reloads": [],
                                                              "trace_matches_build": True}):
            result = self.command("trace", node=old)
        self.assertNotEqual(result["node"], old)
        self.assertEqual(result["node"], self.command("status")["node"])

    def test_unchanged_function_abi_aliases_do_not_block_import(self):
        self.db = str(self.path / "aliases.sqlite3")
        original = 'extern int Helper(void) asm("HelperABI");\nint F(int x) {return x+1;}\n'
        self.source.write_text(original)
        self.command("init", source=str(self.source), function="F")
        self.source.write_text(original.replace("x+1", "1+x"))
        self.command("import", source=str(self.source))
        self.source.write_text(original.replace("return x+1;", 'asm("" "nop"); return x+1;'))
        with self.assertRaisesRegex(ValueError, "instruction bodies"):
            self.command("import", source=str(self.source))


class NativeIsolationTests(unittest.TestCase):
    def environment(self):
        from search_eval import Evaluator
        evaluator = object.__new__(Evaluator)
        evaluator.scope = types.SimpleNamespace(preprocess=lambda *a, **k: "", rom_name=lambda *a: "F")
        return evaluator

    def test_native_crash_cannot_escape_and_is_not_repeated(self):
        evaluator = self.environment()
        child = subprocess.CompletedProcess([], -4, "", "native fault")
        with mock.patch("subprocess.run", return_value=child) as run:
            for _ in range(2):
                result = evaluator.semantic("void F(void){}", "F", b"", b"", 0, "draft.c", 4)
                self.assertEqual(result["verdict"], "UNAVAILABLE")
                self.assertIn("signal 4", result["detail"])
            self.assertEqual(run.call_count, 1)

    def test_wall_clock_timeout_is_not_equivalence(self):
        evaluator = self.environment()
        with mock.patch("subprocess.run", side_effect=subprocess.TimeoutExpired("worker", 10)):
            result = evaluator.semantic("void F(void){}", "F", b"", b"", 0, "draft.c", 4)
        self.assertEqual(result["verdict"], "TIMEOUT")
        self.assertFalse(result["proof"])

    def test_request_releases_scope_dump_locks(self):
        import scope
        with tempfile.TemporaryFile() as stream:
            fd = os.dup(stream.fileno())
            def operation(request):
                scope._LOCKS["synthetic"] = fd
                return "ok"
            with mock.patch.object(search, "dispatch", side_effect=operation):
                self.assertTrue(search.safe_dispatch({"command": "probe"})["ok"])
            self.assertNotIn("synthetic", scope._LOCKS)
            with self.assertRaises(OSError):
                os.fstat(fd)

    def test_materialized_sources_are_immutable_across_workers(self):
        from search_eval import Evaluator
        with tempfile.TemporaryDirectory() as directory:
            a, b = object.__new__(Evaluator), object.__new__(Evaluator)
            a.directory = b.directory = Path(directory)
            first = a.materialize("int F(void){return 1;}", "src/fn/F.c")
            second = b.materialize("int F(void){return 2;}", "src/fn/F.c")
            self.assertNotEqual(first, second)
            self.assertEqual(first.read_text(), "int F(void){return 1;}")
            self.assertEqual(second.read_text(), "int F(void){return 2;}")
            self.assertEqual(first, b.materialize("int F(void){return 1;}", "src/fn/F.c"))


class PreprocessedConcessionTests(unittest.TestCase):
    def count(self, source):
        from search_eval import preprocessed_concessions
        return preprocessed_concessions(source, "F")

    def test_expanded_pin_and_barrier_sites_in_reachable_helpers(self):
        pre = r'''
        static inline int H(int x) {
            register int hidden __asm__("r" "1") = x;
            __asm__ __volatile__("" : "+r"(hidden));
            return hidden;
        }
        static inline int Unused(int x) { register int y asm("r2") = x; return y; }
        int F(int x) { return H(x); }
        int Other(int x) { register int z asm("r3") = x; return z; }
        '''
        result = self.count(pre)
        self.assertEqual(result["pins"], 1)
        self.assertEqual(result["barriers"], 1)
        self.assertEqual(set(result["by_function"]), {"F", "H"})

    def test_cpp_macros_cannot_hide_pins(self):
        import scope
        source = '''#define PIN "r" "1"
#define BARRIER(x) asm volatile("" : "+r"(x))
int F(int x) { register int p asm(PIN) = x; BARRIER(p); return p; }
'''
        # Exercise the actual configured preprocessor; no ROM or compilation.
        command = scope.build.CPP + ["-x", "c", "-"]
        pre = subprocess.run(command, input=source, capture_output=True, text=True, check=True).stdout
        result = self.count(pre)
        self.assertEqual((result["pins"], result["barriers"]), (1, 1))

    def test_escaped_register_labels_and_file_scope_abi_aliases(self):
        result = self.count(r'''
        extern int Helper(void) asm("r1");
        int F(int x) { register int p asm("\x000072" "1") = x; return p; }
        ''')
        self.assertEqual(result["pins"], 1)

    def test_missing_definition_is_unknown_not_pin_free(self):
        result = self.count("int F(int);")
        self.assertEqual(result["verdict"], "UNKNOWN")
        self.assertIsNone(result["pins"])


class ClassificationContextTests(unittest.TestCase):
    def test_generated_assembly_presence_and_content_recontextualize_cached_nodes(self):
        import scope
        import search_eval
        import romlib
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for part in ("include", "symbols", "src", "build/asm/nonmatching", "tools/scope"):
                (root / part).mkdir(parents=True)
            for part in ("compiler", "rom", "build/boktai3.elf", "tools/scope/search_semantic.py"):
                (root / part).write_bytes(b"fixture")
            source = "int F(void) { return 1; }"
            (root / "src/draft.c").write_text(source)
            evaluator = object.__new__(search_eval.Evaluator)
            evaluator.directory = root / "scratch"
            evaluator.directory.mkdir()
            evaluator._hashes, evaluator.oracles = {}, {}
            evaluator.support = {}
            for name in ("loaded_scope", "loaded_fast", "loaded_structure", "loaded_trace", "loaded_semantic"):
                setattr(evaluator, name, "fixture")
            # This Thumb LDR reaches the literal pool at offset four. The
            # generated assembly additionally identifies offset two as data.
            code = bytes.fromhex("0048004700000000")
            fake = types.SimpleNamespace(
                rom_name=lambda *a: "F", inventory=lambda: ({}, {"F": 0x08000000}, {"F": len(code)}),
                INSTR=root / "compiler", arm_starts=lambda: [], rom_bytes=lambda: code,
                off=lambda a: 0, rom_data=scope.rom_data, _FAST={}, _PRE={"stale": True},
                _HEADS={"stale": True}, _ROMBYTES={"stale": True}, _MEMO={"stale": True},
                build=types.SimpleNamespace(compiler_for=lambda p: "agbcc", cflags_for=lambda p: [],
                    AS=["/usr/bin/true"], CPP=["/usr/bin/true"], DEFINES=[]))
            evaluator.scope = fake
            def evidence(source, function, origin, knobs, trials, expected_context=None):
                return dict(status="different", exact=False, target_data=expected_context["target_classification"]["data"])
            with mock.patch.object(search_eval, "ROOT", root), mock.patch.object(scope, "ROOT", str(root)), \
                 mock.patch.object(romlib, "baserom_path", return_value=root / "rom"), \
                 mock.patch.object(search, "environment", return_value=evaluator), \
                 mock.patch.object(evaluator, "evaluate", side_effect=evidence):
                db = str(root / "session.sqlite3")
                first = search.dispatch(dict(command="init", session=db, source=str(root / "src/draft.c"), function="F"))
                with Session(db) as session:
                    absent = session.context()
                    self.assertFalse(absent["target_classification"]["generated_assembly"]["present"])
                    first_node = session.node()["id"]
                assembly = root / "build/asm/nonmatching/F.s"
                assembly.write_text("_08000000:\n ldr r0, [pc, #0]\n .hword 0x4700\n .word 0\n")
                result = search.dispatch(dict(command="evaluate", session=db))
                self.assertFalse(result["cache_hit"])
                with Session(db) as session:
                    labeled = session.context()
                    self.assertNotEqual(first_node, session.node()["id"])
                    self.assertNotEqual(absent["target_classification"]["data"], labeled["target_classification"]["data"])
                    labeled_node = session.node()["id"]
                self.assertFalse(fake._PRE)
                self.assertFalse(fake._HEADS)
                # Even a content change with the same resolved ranges must
                # invalidate the dependency identity rather than reuse evidence.
                assembly.write_text(assembly.read_text() + "@ changed provenance\n")
                result = search.dispatch(dict(command="evaluate", session=db))
                self.assertFalse(result["cache_hit"])
                with Session(db) as session:
                    self.assertNotEqual(labeled_node, session.node()["id"])
                    self.assertEqual(labeled["target_classification"]["data"], session.context()["target_classification"]["data"])


@unittest.skipUnless(os.environ.get("SEARCH_ROM_TESTS") == "1", "opt-in real unmatched compiler trial")
class RealUnmatchedEnvironmentTests(unittest.TestCase):
    def test_ambient_knobs_cannot_change_exact_environment(self):
        from search_eval import Evaluator
        root = Path(__file__).resolve().parents[2]
        origin = "src/fn/sub_08110C9C.c"
        source = (root / origin).read_text()
        if "NONMATCHING" not in source:
            self.skipTest("trial function has since matched; select another unmatched draft")
        with tempfile.TemporaryDirectory(dir=root / "build") as directory:
            evaluator = Evaluator(directory)
            try:
                plain = evaluator.evaluate(source, "sub_08110C9C", origin)
                with mock.patch.dict(os.environ, {"AGBCC_FORCE": "83:1"}):
                    ambient = evaluator.evaluate(source, "sub_08110C9C", origin)
                    self.assertEqual(plain["code_hash"], ambient["code_hash"])
                experiment = evaluator.evaluate(source, "sub_08110C9C", origin, {"AGBCC_FORCE": "83:1"})
                self.assertFalse(experiment["exact"])
                self.assertTrue(experiment["experiment"])
                self.assertNotEqual(plain["code_hash"], experiment["code_hash"])
                with mock.patch.object(evaluator, "semantic", side_effect=SystemExit("missing emulator")):
                    tested = evaluator.evaluate(source, "sub_08110C9C", origin, semantic_trials=2)
                self.assertEqual(tested["status"], "different")
                self.assertEqual(tested["semantic"]["verdict"], "UNAVAILABLE")
            finally:
                evaluator.close()


if __name__ == "__main__":
    unittest.main()

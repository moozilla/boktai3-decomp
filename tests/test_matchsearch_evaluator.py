"""Acceptance-boundary checks, independent of a local copyrighted ROM."""

from __future__ import annotations

import contextlib
import importlib.util
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from matchsearch.evaluator import Evaluation, Evaluator, _concessions, _load_scope, byte_distance, clean_environment


def fake_evaluator(code: bytes, target: bytes) -> Evaluator:
    evaluator = Evaluator.__new__(Evaluator)
    evaluator._closed = False
    evaluator.function = "Target"
    evaluator.address = 0x08001000
    evaluator.environment_hash = "fixed-environment"
    evaluator.target_code = target
    evaluator.target_rows = [(0, 2, "movs r0, #1", "movs r0, #1", None)]
    evaluator.own_names = ("Target",)
    evaluator.scope = SimpleNamespace(
        function_lines=lambda *_: range(0), needs_g=lambda knobs: "AGBCC_NOTIE" in knobs,
        decode=lambda *args: [(0, 2, "movs r0, #2", "movs r0, #2", None)],
        align=lambda *args: ([(0, 0, "=")], {}), shape_distance=lambda *args: 0)
    evaluator._scope_context = contextlib.nullcontext
    evaluator.validate_environment = lambda: None
    evaluator._compiler = lambda debug: SimpleNamespace(bytes=lambda *args: (code, ""), data=[])
    return evaluator


class EvaluatorBoundaryTests(unittest.TestCase):
    def test_production_verdict_uses_exact_bytes_including_tail(self):
        e = fake_evaluator(b"\x01\x20", b"\x01\x20\x00\x00")
        result = e.evaluate("int Target(void) { return 1; }", preprocessed=True)
        self.assertEqual(result.instruction_diff, 0)  # a misleading perfect listing
        self.assertEqual(result.byte_mismatches, 2)
        self.assertFalse(result.exact)
        self.assertEqual(result.status, "compiled")

    def test_forced_match_is_never_a_production_match(self):
        e = fake_evaluator(b"same", b"same")
        plain = e.evaluate("source", preprocessed=True)
        forced = e.evaluate("source", preprocessed=True, diagnostic_knobs={"AGBCC_FORCE": "19:4"})
        self.assertTrue(plain.exact)
        self.assertFalse(forced.exact)
        self.assertTrue(forced.diagnostic_match)
        self.assertEqual(forced.status, "diagnostic_match")

    def test_debug_uid_diagnostic_is_never_a_production_match(self):
        e = fake_evaluator(b"same", b"same")
        result = e.evaluate("source", preprocessed=True, diagnostic_knobs={"AGBCC_NOTIE": "72"})
        self.assertFalse(result.exact)
        self.assertTrue(result.diagnostic_match)

    def test_clean_environment_preserves_access_and_removes_experiments(self):
        env = {"HTTPS_PROXY": "proxy", "PATH": "bin", "AGBCC_FORCE": "21:0", "CSE_KEEP": "42",
               "SCOPE_TRANSFORM": "arbitrary.py", "SCOPE_NEIGHBORS_ASM": "1"}
        self.assertEqual(clean_environment(env), {"HTTPS_PROXY": "proxy", "PATH": "bin"})

    def test_scope_context_restores_environment_and_caches_after_failure(self):
        e = Evaluator.__new__(Evaluator)
        e.scope = SimpleNamespace(WORK="old", _CPP1={"old": 1}, _HEADS={"old": 2}, _PRE={"old": 3})
        e.build = SimpleNamespace(CPP=["cpp"])
        e.cpp = ["cpp", "-iquote", "source-dir"]
        e.workdir = Path("worker")
        e._cpp_cache, e._header_cache, e._pre_cache = {}, {}, {}
        with patch.dict(os.environ, {"AGBCC_FORCE": "21:0", "SCOPE_TRANSFORM": "bad.py"}):
            with self.assertRaisesRegex(RuntimeError, "candidate failed"):
                with e._scope_context():
                    self.assertNotIn("AGBCC_FORCE", os.environ)
                    self.assertNotIn("SCOPE_TRANSFORM", os.environ)
                    self.assertEqual(e.build.CPP, e.cpp)
                    e.scope._PRE["new"] = 9
                    raise RuntimeError("candidate failed")
            self.assertEqual(os.environ["AGBCC_FORCE"], "21:0")
            self.assertEqual(e.scope.WORK, "old")
            self.assertEqual(e.scope._PRE, {"old": 3})
            self.assertEqual(e._pre_cache, {"new": 9})
            self.assertEqual(e.build.CPP, ["cpp"])

    def test_protocol_rejects_whitespace_and_nonknobs(self):
        e = fake_evaluator(b"same", b"same")
        for knobs in ({"AGBCC_FORCE": "19:4\nnew request"}, {"CPP": "false"}, {"AGBCC_X=x": "y"}):
            with self.assertRaises(ValueError):
                e.evaluate("source", preprocessed=True, diagnostic_knobs=knobs)

    def test_missing_capstone_raises_without_importing_scope_cli(self):
        with patch("matchsearch.evaluator.importlib.import_module", side_effect=ImportError("no capstone")) as imported:
            with self.assertRaisesRegex(RuntimeError, "build/venv/bin/python"):
                _load_scope(ROOT)
            imported.assert_called_once_with("capstone")

    def test_candidate_cannot_sneak_a_new_build_configuration_into_export(self):
        e = Evaluator.__new__(Evaluator)
        e.flags = ("-O2", "-mthumb-interwork", "-fhex-asm")
        e.build = SimpleNamespace(CFLAGS=list(e.flags))
        e._validate_source_settings("int f(void) { return 1; }")
        with self.assertRaisesRegex(ValueError, "compiler/flags"):
            e._validate_source_settings("// CFLAGS: -O0\nint f(void) { return 1; }")
        with self.assertRaisesRegex(ValueError, "compiler/flags"):
            e._validate_source_settings("// COMPILER: old_agbcc\nint f(void) { return 1; }")

    def test_concurrent_in_process_calls_cannot_mix_compiler_state(self):
        from concurrent.futures import ThreadPoolExecutor
        import time
        e = fake_evaluator(b"same", b"same")
        active = 0
        peak = 0
        def compile_one(source, *_):
            nonlocal active, peak
            active += 1
            peak = max(peak, active)
            time.sleep(0.01)
            active -= 1
            return source.encode(), ""
        e._compiler = lambda _: SimpleNamespace(bytes=compile_one, data=[])
        with ThreadPoolExecutor(max_workers=3) as pool:
            results = list(pool.map(lambda source: e.evaluate(source, preprocessed=True), ["one", "two", "three"]))
        self.assertEqual(peak, 1)
        self.assertEqual([r.code for r in results], [b"one", b"two", b"three"])

    def test_summary_is_json_safe_and_omits_large_payload(self):
        result = fake_evaluator(b"same", b"same").evaluate("source", preprocessed=True)
        summary = json.loads(json.dumps(result.to_dict()))
        self.assertNotIn("code", summary)
        self.assertNotIn("rows", summary)
        self.assertEqual(result.to_dict(include_code=True)["code_hex"], b"same".hex())

    def test_dependency_change_invalidates_a_session(self):
        with tempfile.TemporaryDirectory() as d:
            path = Path(d) / "header.h"
            path.write_text("old")
            e = Evaluator.__new__(Evaluator)
            e.repo = Path(d)
            st = path.stat()
            e._dependency_stamps = [(path, st.st_mtime_ns, st.st_size)]
            e.validate_environment()
            path.write_text("different")
            with self.assertRaisesRegex(RuntimeError, "dependency changed"):
                e.validate_environment()

    def test_byte_distance_is_symmetric_and_counts_extra_bytes(self):
        self.assertEqual(byte_distance(b"ab", b"acdef"), 4)
        self.assertEqual(byte_distance(b"acdef", b"ab"), 4)


@unittest.skipUnless(shutil.which("cc"), "a C compiler driver is required for include-resolution regressions")
class IncludeEnvironmentTests(unittest.TestCase):
    def fixture(self, root: Path, source: str) -> Evaluator:
        for directory in ("include", "src", "worker"):
            (root / directory).mkdir(exist_ok=True)
        e = Evaluator.__new__(Evaluator)
        e.repo = root
        e.source_path = root / "src" / "source.c"
        e.source_path.write_text(source)
        e.original_source = source
        e.workdir = root / "worker"
        e._input = e.workdir / "candidate.c"
        # Apple cpp is a legacy shell wrapper. Like scope's Darwin invocation,
        # use the compiler driver and always supply an explicit input filename.
        e.cpp = [shutil.which("cc"), "-E", "-iquote", str(e.source_path.parent)]
        e.flags = ()
        e.production_compiler = root / "test-compiler"
        e.build = SimpleNamespace(DEFINES=[], cflags_for=lambda _: (),
                                 compiler_for=lambda _: str(e.production_compiler))
        headers = sorted((root / "include").rglob("*.h"))
        extras = e._freeze_include_inputs(headers)
        e._dependency_stamps = [(p, p.stat().st_mtime_ns, p.stat().st_size)
                                for p in [*headers, *extras]]
        return e

    def test_real_cpp_discovers_and_freezes_source_local_header(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            (root / "src").mkdir()
            header = root / "src" / "local.h"
            header.write_text("#define VALUE 1\n")
            e = self.fixture(root, '#include "local.h"\nint f(void) { return VALUE; }\n')
            self.assertIn(header, e._include_inventory)
            e.validate_environment()
            header.write_text("#define VALUE 200\n")
            with self.assertRaisesRegex(RuntimeError, "dependency changed"):
                e.validate_environment()

    def test_new_shadow_invalidates_but_unrelated_source_file_does_not(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            (root / "include" / "nested").mkdir(parents=True)
            (root / "include" / "nested" / "common.h").write_text("#define VALUE 1\n")
            source = '#include "nested/common.h"\nint f(void) { return VALUE; }\n'
            e = self.fixture(root, source)
            (root / "src" / "another.c").write_text("int another;\n")
            e.validate_environment()
            (root / "src" / "nested").mkdir()
            (root / "src" / "nested" / "common.h").write_text("#define VALUE 2\n")
            with self.assertRaisesRegex(RuntimeError, "include lookup changed"):
                e.validate_environment()

    def test_new_directives_may_use_inventoried_headers_but_not_unfrozen_local_files(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            (root / "include").mkdir()
            (root / "include" / "known.h").write_text("#define VALUE 1\n")
            e = self.fixture(root, "int f(void) { return 1; }\n")
            e.validate_source_environment('#include "known.h"\nint f(void) { return VALUE; }\n')
            (root / "src" / "unknown.h").write_text("#define VALUE 2\n")
            with self.assertRaisesRegex(ValueError, "outside the frozen inventory"):
                e.validate_source_environment('#define HEADER "unknown.h"\n#include HEADER\n'
                                              'int f(void) { return VALUE; }\n')
            # Lookup information from a newly used, known header remains active.
            (root / "src" / "known.h").write_text("#define VALUE 3\n")
            with self.assertRaisesRegex(RuntimeError, "include lookup changed"):
                e.validate_environment()

    def test_body_moves_reuse_dependency_scan_and_cpp_environment_is_frozen(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            (root / "include").mkdir()
            (root / "include" / "known.h").write_text("#define VALUE 1\n")
            e = self.fixture(root, '#include "known.h"\nint f(void) { return VALUE; }\n')
            with patch.object(e, "_scan_include_inputs", side_effect=AssertionError("unnecessary CPP")):
                e.validate_source_environment('#include "known.h"\nint f(void) {\nint value = VALUE;\nreturn value;\n}\n')
            with patch.dict(os.environ, {"CPATH": str(root / "other")}):
                with self.assertRaisesRegex(RuntimeError, "preprocessor environment changed"):
                    e.validate_environment()


class TargetClassificationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # Exercise scope's actual generated-asm/fallback resolution without a
        # ROM or a compiler installation, and without changing shared build files.
        import ast
        import re
        names = {'rom_data_ranges', 'rom_pool_ranges', 'rom_data'}
        functions = []
        for path in ('romlib.py', 'scope/scope.py'):
            tree = ast.parse((ROOT / 'tools' / path).read_text())
            functions.extend(node for node in tree.body
                             if isinstance(node, ast.FunctionDef) and node.name in names)
        cls.scope_code = compile(ast.Module(body=functions, type_ignores=[]), 'scope-rom-data', 'exec')
        cls.scope_globals = {'os': os, 're': re}

    def fixture(self, root):
        e = Evaluator.__new__(Evaluator)
        e.repo = root
        e.function, e.address = 'Target', 0x08001000
        e.target_code = bytes.fromhex('0120704712345678')
        e.rom = e.target_code
        e.rom_path = root / 'rom.bin'
        e.rom_path.write_bytes(e.rom)
        e.source_path = root / 'target.c'
        e.source_path.write_text('int Target(void) { return 1; }')
        e.compiler = e.production_compiler = Path(sys.executable)
        e.cpp, e.assembler, e.flags = [sys.executable], [sys.executable], ()
        e._include_environment = {}
        e._freeze_include_inputs = lambda _: []
        e.build = SimpleNamespace(DEFINES=[], AS=e.assembler, cflags_for=lambda _: (),
                                  compiler_for=lambda _: str(e.compiler))
        namespace = {**self.scope_globals, 'ROOT': str(root)}
        exec(self.scope_code, namespace)
        e.scope = SimpleNamespace(rom_data=namespace['rom_data'])
        e._freeze_target_classification()
        e.identity = e._identity()
        return e

    def test_adding_missing_generated_asm_fences_cache_and_changes_context(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            before = self.fixture(root)
            self.assertEqual(before.target_data, [])
            path = root / 'build/asm/nonmatching/Target.s'
            path.parent.mkdir(parents=True)
            path.write_text('_08001004: .4byte 0x78563412\n')
            with self.assertRaisesRegex(RuntimeError, 'classification input changed'):
                before.validate_environment()
            after = self.fixture(root)
            self.assertEqual(after.target_data, [(4, 8)])
            self.assertNotEqual(before.identity, after.identity)
            self.assertEqual(after.identity['target_classification']['resolved_data_ranges'], [[4, 8]])

    def test_contents_and_removal_are_tracked_even_when_ranges_do_not_change(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            path = root / 'build/asm/nonmatching/Target.s'
            path.parent.mkdir(parents=True)
            path.write_text('_08001004: .4byte 0x78563412\n')
            first = self.fixture(root)
            path.write_text('_08001004: .4byte 0x78563412 @ annotation changed\n')
            with self.assertRaisesRegex(RuntimeError, 'classification input changed'):
                first.validate_environment()
            second = self.fixture(root)
            self.assertEqual(first.target_data, second.target_data)
            self.assertNotEqual(first.identity['target_classification'], second.identity['target_classification'])
            path.unlink()
            with self.assertRaisesRegex(RuntimeError, 'classification input changed'):
                second.validate_environment()


@unittest.skipUnless(shutil.which('cc'), 'a real preprocessor is required')
class PreprocessedConcessions(unittest.TestCase):
    def preprocess(self, source):
        # Explicit stdin works with GCC and Apple's cc -E, unlike bare cpp.
        return subprocess.run([shutil.which('cc'), '-E', '-P', '-x', 'c', '-'], input=source,
                              check=True, capture_output=True, text=True).stdout

    def test_macro_register_labels_cannot_hide_pins_in_evaluation(self):
        raw = 'int Target(int x) { register int a asm("r2")=x; register int b asm("r0"); return a; }'
        macro = '#define PIN_A "r2"\n#define PIN_B "r0"\n' + raw.replace('"r2"', 'PIN_A').replace('"r0"', 'PIN_B')
        self.assertEqual(self.preprocess(raw), self.preprocess(macro))
        e = fake_evaluator(b'same', b'same')
        e.preprocess, e._validate_source_settings = self.preprocess, lambda _: None
        for source in (raw, macro):
            result = e.evaluate(source)
            self.assertEqual(result.concessions['register_pins'], 2)
            self.assertFalse(result.pin_free)

    def test_transitive_inline_helpers_count_but_unreferenced_helpers_do_not(self):
        source = r'''
            #define PIN(x) __asm__(x)
            #define BARRIER() __asm__ volatile ("" ::: "memory")
            static inline int leaf(int x) { register int pinned PIN("r" "2")=x; BARRIER(); return pinned; }
            static __inline__ int middle(int x) { return leaf(x); }
            static inline int unused(int x) { register int y PIN("r7")=x; return y; }
            int unrelated(int x) { register int y PIN("r4")=x; return y; }
            int Target(int x) { const char *text="unused(x); asm(\"r6\")"; return middle(x); }
        '''
        result = _concessions(self.preprocess(source), ('Target',))
        self.assertEqual(result, {'register_pins': 1, 'asm_barriers': 1, 'inline_assembly': 0})

    def test_preprocessing_selects_active_body_and_expands_instruction_macro(self):
        source = '''
            #define CODE() asm("mov r0, r1")
            #if 0
            int Target(int x) { register int y asm("r4")=x; return y; }
            #else
            int Target(int x) { CODE(); return x; }
            #endif
        '''
        self.assertEqual(_concessions(self.preprocess(source), ('Target',)),
                         {'register_pins': 0, 'asm_barriers': 0, 'inline_assembly': 1})

    def test_global_binding_is_not_hidden_outside_target_body(self):
        source = 'register int global asm("r4"); int Target(void) { return global; }'
        self.assertEqual(_concessions(self.preprocess(source), ('Target',))['register_pins'], 1)

    def test_inline_prototype_and_unrecognized_helper_syntax_cannot_hide_barriers(self):
        # inline may be inherited from an earlier declaration. The K&R helper
        # is outside the fast declarator recognizer and must remain conservative.
        source = '''
            static inline int declared(int);
            static int declared(int x) { asm(""); return x; }
            static inline int oldstyle(x) int x; { asm(""); return x; }
            int Target(int x) { return declared(oldstyle(x)); }
        '''
        self.assertEqual(_concessions(self.preprocess(source), ('Target',))['asm_barriers'], 2)


class SpillRotationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # Load the two pure search functions from the actual scope source without
        # initializing its compiler/ROM environment.  This is a regression test
        # for a real rejected initial rotation, not a solved-function fixture.
        import ast
        tree = ast.parse((ROOT / "tools" / "scope" / "scope.py").read_text())
        funcs = [n for n in tree.body if isinstance(n, ast.FunctionDef) and n.name in {"spill_pick", "spill_shifts"}]
        ns = {}
        exec(compile(ast.Module(body=funcs, type_ignores=[]), "scope-functions", "exec"), ns)
        cls.shifts = staticmethod(ns["spill_shifts"])

    def test_initial_rotation_is_searchable(self):
        state = {"start": -1, "rotate": True, "regs": [0, 1, 2], "st": {0: "A", 1: "A", 2: "A"}}
        self.assertEqual(self.shifts(state, 1), [1])
        self.assertEqual(self.shifts(state, 2), [2])

    def test_forced_group_and_ambiguous_legacy_trace_are_not_rotated(self):
        state = {"start": -1, "rotate": False, "regs": [0, 1, 2], "st": {0: "A", 1: "A", 2: "A"}}
        self.assertEqual(self.shifts(state, 1), [])
        del state["rotate"]
        self.assertEqual(self.shifts(state, 1), [])

    def test_legacy_nonnegative_rotation_still_works(self):
        state = {"start": 0, "regs": [0, 1, 2], "st": {0: "A", 1: "L22", 2: "A"}}
        self.assertEqual(self.shifts(state, 0), [2])


if __name__ == "__main__":
    unittest.main()

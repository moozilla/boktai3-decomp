"""Compiler changes must invalidate cached objects, including environment overrides."""
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
import build


class CompilerCacheTests(unittest.TestCase):
    def test_changed_compiler_rebuilds_unchanged_source(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for path in ('include/gba', 'tools', 'build', 'asm'):
                (root / path).mkdir(parents=True)
            (root / 'tools/build.py').write_text('fixture')
            (root / 'asm/macros.inc').write_text('@ fixture\n')
            src = root / 'candidate.c'
            src.write_text('int f(void) { return 1; }\n')
            normal, old = root / 'agbcc', root / 'old_agbcc'
            normal.write_bytes(b'compiler one')
            old.write_bytes(b'compiler two')
            obj = root / 'build/candidate.o'
            compilers = []

            def compile_command(command, **kwargs):
                compilers.append(command[0])
                return subprocess.CompletedProcess(command, 0, 'f:\n bx lr\n', '')

            def assemble(command):
                obj.write_bytes(b'object')
                return ''

            with patch.object(build, 'ROOT', directory), \
                 patch.object(build, 'AGBCC', str(normal)), \
                 patch.object(build, 'preprocess', return_value=src.read_text()), \
                 patch.object(build.subprocess, 'run', side_effect=compile_command), \
                 patch.object(build, 'sh', side_effect=assemble):
                build.build_c('candidate.c', 'build/candidate.o')
                build.build_c('candidate.c', 'build/candidate.o')
                self.assertEqual(compilers, [str(normal)])
                # Same source and flags; only the environment-selected compiler changes.
                with patch.object(build, 'AGBCC', str(old)):
                    build.build_c('candidate.c', 'build/candidate.o')
                self.assertEqual(compilers, [str(normal), str(old)])
                src.write_text('// COMPILER: old_agbcc\nint f(void) { return 1; }\n')
                build.build_c('candidate.c', 'build/candidate.o')
                self.assertEqual(compilers[-1], str(old))
                src.write_text('// COMPILER: typo\nint f(void) { return 1; }\n')
                with self.assertRaisesRegex(SystemExit, 'unknown compiler'):
                    build.build_c('candidate.c', 'build/candidate.o')
            build.compiler_identity.cache_clear()


class CompilerInputTests(unittest.TestCase):
    def test_large_input_and_early_output_use_file_backed_stdin(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for path in ('include/gba', 'tools', 'build', 'asm'):
                (root / path).mkdir(parents=True)
            (root / 'tools/build.py').write_text('fixture')
            (root / 'asm/macros.inc').write_text('@ fixture\n')
            (root / 'candidate.c').write_text('int f(void) { return 1; }')
            compiler = root / 'agbcc'
            compiler.write_text(
                '#!' + sys.executable + '\n'
                'import os, stat, sys\n'
                'assert stat.S_ISREG(os.fstat(0).st_mode)\n'
                'sys.stdout.write(" " * 131072)\n'
                'sys.stdout.flush()\n'
                'assert sys.stdin.read() == "large input" * 65536\n'
                'sys.stdout.write("f:\\n bx lr\\n")\n')
            compiler.chmod(0o755)
            run = subprocess.run

            def bounded_run(command, **kwargs):
                return run(command, timeout=5, **kwargs)

            def assemble(command):
                (root / 'build/candidate.o').write_bytes(b'object')

            with patch.object(build, 'ROOT', directory), \
                 patch.object(build, 'AGBCC', str(compiler)), \
                 patch.object(build, 'preprocess', return_value='large input' * 65536), \
                 patch.object(build.subprocess, 'run', side_effect=bounded_run), \
                 patch.object(build, 'sh', side_effect=assemble):
                build.build_c('candidate.c', 'build/candidate.o')
            self.assertTrue((root / 'build/candidate.o.hash').exists())
            build.compiler_identity.cache_clear()


if __name__ == '__main__':
    unittest.main()

"""Concurrent assemblers can share both a PID and a diagnostic tag."""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
from subprocess import CompletedProcess
from tempfile import TemporaryDirectory
from threading import Barrier
import unittest
from unittest.mock import patch

from tools.scope import fast


class ConcurrentAssemble(unittest.TestCase):
    def test_workers_with_the_same_tag_read_their_own_source(self):
        ready = Barrier(2)

        def assemble(args, **kwargs):
            ready.wait(timeout=5)
            Path(args[-2]).write_bytes(Path(args[-1]).read_bytes())
            return CompletedProcess(args, 0, "", "")

        with TemporaryDirectory() as temp, patch.object(fast.subprocess, "run", assemble):
            def compile_value(value):
                return fast.assemble(["as"], value, temp, tag="verify")

            with ThreadPoolExecutor(max_workers=2) as pool:
                results = list(pool.map(compile_value, ["first", "second"]))
            self.assertEqual([obj.split(b"\t", 1)[0] for obj, _ in results], [b"first", b"second"])
            self.assertEqual(list(Path(temp).iterdir()), [])

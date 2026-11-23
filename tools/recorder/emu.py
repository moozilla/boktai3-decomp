"""Drive a replay of a recorded session (tools/recorder/runner.mjs serve) from Python.

    with Replay(session) as r:
        r.seek(899)                    # machine at the end of frame 899 (hash-checked)
        r.run(1, trace=dir)            # frame 900 in trace mode -> dir/f00000900.bin
        r.solar(20); r.run(60)         # perturb, then compare
"""
import json
import os
import subprocess
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
RUNNER = ROOT / 'tools/recorder/runner.mjs'
TRACE = np.dtype([('site', '<u4'), ('a', '<u4'), ('b', '<u4'), ('c', '<u4'), ('inv', '<u4')])


class ReplayError(RuntimeError):
    pass


class Replay:
    def __init__(self, session_path):
        self.session = Path(session_path)
        self.bad = []          # snapshots whose restore drifted
        self.restarts = 0
        self._start()

    def _start(self):
        self.proc = subprocess.Popen(['node', str(RUNNER), 'serve', str(self.session)], cwd=ROOT,
                                     stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True, bufsize=1)
        first = self._read()
        if not first.get('ready'):
            raise ReplayError(f'runner did not start: {first}')
        self.frames = first['frames']

    def _read(self):
        line = self.proc.stdout.readline()
        if not line:
            raise ReplayError('runner exited')
        return json.loads(line)

    def cmd(self, **kw):
        self.proc.stdin.write(json.dumps(kw) + '\n')
        self.proc.stdin.flush()
        r = self._read()
        if 'error' in r:
            self.bad = r.get('badSnapshots', self.bad)
            raise ReplayError(r['error'], r)
        if 'fatal' in r:
            raise ReplayError(r['fatal'])
        return r

    def seek(self, frame):
        """Machine at the end of FRAME. A snapshot only restores where the live wasm call
        stack matches the one it was taken at; otherwise the runner restarts from power-on
        (and skips ahead through matching snapshots)."""
        for _ in range(4):
            try:
                return self.cmd(op='seek', frame=int(frame), bad=self.bad)
            except ReplayError as e:
                info = e.args[1] if len(e.args) > 1 else {}
                if not info.get('restart'):
                    raise
                self.close()
                self.restarts += 1
                self._start()
        raise ReplayError(f'could not reach frame {frame}')

    def run(self, frames=1, trace=None):
        kw = {'op': 'run', 'frames': int(frames)}
        if trace:
            kw['trace'] = str(trace)
        return self.cmd(**kw)

    def solar(self, level):
        return self.cmd(op='solar', level=level)

    def keys(self, mask):
        return self.cmd(op='keys', mask=mask)

    def poke(self, addr, size, value):
        return self.cmd(op='poke', addr=int(addr), size=int(size), value=int(value))

    def read(self, addr, length):
        return bytes.fromhex(self.cmd(op='read', addr=int(addr), len=int(length))['hex'])

    def value(self, addr, width, signed=False):
        v = int.from_bytes(self.read(addr, width), 'little')
        if signed and v >> (8 * width - 1):
            v -= 1 << (8 * width)
        return v

    def shot(self, path):
        return self.cmd(op='shot', file=str(path))

    def status(self):
        return self.cmd(op='status')

    def close(self):
        if self.proc.poll() is None:
            try:
                self.proc.stdin.write('{"op":"quit"}\n')
                self.proc.stdin.flush()
                self.proc.wait(timeout=10)
            except (OSError, subprocess.TimeoutExpired):
                self.proc.kill()

    def __enter__(self):
        return self

    def __exit__(self, *a):
        self.close()


def load_trace(path):
    return np.fromfile(path, TRACE)

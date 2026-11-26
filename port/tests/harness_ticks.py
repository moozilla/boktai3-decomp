#!/usr/bin/env python3
"""Compare fast and instruction-stepped ROM observations on the same movie."""
import os
from pathlib import Path
import subprocess
import time

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'build/emu/tick-test'


def run(command, headless=False):
    name = command + ('-headless' if headless else '')
    directory = OUT / name
    directory.mkdir(parents=True, exist_ok=True)
    script = directory / 'harness.txt'
    script.write_text(f'''trace off
rtc 1763035200
lux 232
{command} 08219B24
tickdump samples 0:24 2000710+5a4:4,3005260:6
holdt NONE 12
holdt RIGHT 2
holdt NONE 10
''')
    env = {**os.environ, 'BOKTAI3_NO_COVERAGE': '1'}
    env.pop('BOKTAI3_SAV', None)
    env.pop('BOKTAI3_HEADLESS', None)
    if headless:
        env['BOKTAI3_HEADLESS'] = '1'
    start = time.monotonic()
    result = subprocess.run([ROOT / 'build/emu/harness', ROOT / 'baserom.gba', script, directory],
                            capture_output=True, text=True, env=env, timeout=120, check=True)
    assert 'holdt:' not in result.stderr, result.stderr
    assert not (directory / 'total.exec').exists()
    data = (directory / 'samples.bin').read_bytes()
    assert len(data) == 240, result.stderr
    print(f'{name}: {time.monotonic() - start:.2f}s for {len(data) // 10} ticks', flush=True)
    return data


if __name__ == '__main__':
    slow = run('tick')
    fast = run('fasttick')
    headless = run('fasttick', headless=True)
    assert fast == slow == headless, 'Input tick observations differ'
    assert any(fast[(i * 10) + 4] & 0x10 for i in range(12, 16)), 'Right press was not observed'
    print('MATCH')

#!/usr/bin/env python3
"""Run the original RTC boot C on host and wasm, comparing frame-3 mGBA RAM."""
import argparse
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rom', default='baserom.gba')
    parser.add_argument('--hardware-root')
    args = parser.parse_args()
    os.chdir(ROOT)
    out = Path('build/port/rtc-test')
    out.mkdir(parents=True, exist_ok=True)
    extra = ['--hardware-root', args.hardware_root] if args.hardware_root else []
    for target in ('host', 'wasm32'):
        subprocess.run([sys.executable, 'port/build.py', '--slice', '--rtc', '--target', target, *extra], check=True)
    subprocess.run(['build/port/slice-host-rtc/boot_test', args.rom, str(out / 'host.bin')], check=True)
    subprocess.run(['node', 'port/tests/rtc_wasm.cjs', 'build/port/slice-wasm32-rtc/boot.wasm',
                    args.rom, str(out / 'wasm.bin')], check=True)
    reference = out / 'reference'
    reference.mkdir(exist_ok=True)
    with (out / 'harness.log').open('w') as log:
        subprocess.run(['tools/emu/harness', args.rom, 'port/tests/rtc_reference.txt', str(reference)],
                       env=dict(os.environ, TZ='UTC'), stdout=log, stderr=subprocess.STDOUT, check=True)
    expected = (reference / 'clock_frame3.bin').read_bytes() + (reference / 'rtc_frame3.bin').read_bytes()
    assert len(expected) == 44
    assert '\tsound_init\t0822FE60\t' in (reference / 'probes.tsv').read_text()
    assert (out / 'host.bin').read_bytes() == expected
    assert (out / 'wasm.bin').read_bytes() == expected
    print('RTC boot: all 44 clock/RTC bytes match host, wasm and mGBA frame 3 (UTC)')


if __name__ == '__main__': main()

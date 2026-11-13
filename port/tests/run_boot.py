#!/usr/bin/env python3
"""Build/run original early boot C on host and wasm; compare frame-1 mGBA RAM."""
import argparse
import json
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rom', default='baserom.gba')
    parser.add_argument('--hardware-root')
    parser.add_argument('--no-reference', action='store_true', help='skip optional mGBA comparison')
    args = parser.parse_args()
    os.chdir(ROOT)
    out = Path('build/port/boot-test')
    out.mkdir(parents=True, exist_ok=True)
    extra = ['--hardware-root', args.hardware_root] if args.hardware_root else []
    for target in ('host', 'wasm32'):
        subprocess.run([sys.executable, 'port/build.py', '--slice', '--target', target, *extra], check=True)
    subprocess.run(['build/port/slice-host/boot_test', args.rom, str(out / 'native_clock.bin')], check=True)
    subprocess.run(['node', 'port/tests/boot_wasm.js', 'build/port/slice-wasm32/boot.wasm',
                    args.rom, str(out / 'wasm_clock.bin')], check=True)
    native = (out / 'native_clock.bin').read_bytes()
    wasm = (out / 'wasm_clock.bin').read_bytes()
    assert native == wasm and len(wasm) == 0x24
    reference_checked = False
    if not args.no_reference:
        reference = out / 'reference'
        reference.mkdir(exist_ok=True)
        with (out / 'reference.log').open('w') as log:
            subprocess.run(['tools/emu/harness', args.rom, 'port/tests/boot_reference.txt', str(reference)],
                           stdout=log, stderr=subprocess.STDOUT, check=True)
        assert (reference / 'clock_frame1.bin').read_bytes() == wasm
        trace = (reference / 'probes.tsv').read_text()
        assert '\tclock_clear\t082284F4\t' in trace
        reference_checked = True
        print('mGBA frame 1: 03005430:24 matches native and wasm game C')
    (out / 'result.json').write_text(json.dumps({'native': 'pass', 'wasm32': 'pass',
        'mgba_frame_1': 'pass' if reference_checked else 'skipped',
        'range': '03005430:24', 'scope': 'AgbMain reset prefix through Clock_ClearState'}, indent=2) + '\n')


if __name__ == '__main__':
    main()

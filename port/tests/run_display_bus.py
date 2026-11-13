#!/usr/bin/env python3
"""Build a focused test of shared core/device display and interrupt registers."""
import argparse
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'port/core'))
from link_slice import build_slice


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--hardware-root')
    parser.add_argument('--cc', default=os.environ.get('PORT_CC', '/opt/homebrew/opt/llvm/bin/clang'))
    args = parser.parse_args()
    os.chdir(ROOT)
    args.target = 'wasm32'
    args.mmio = True
    args.rtc = args.video_irqs = False
    args.ppu_root = None
    build_slice(ROOT, args, extra_units=['port/tests/display_bus.c'], suffix='-display-bus',
                extra_exports=['port_test_display_setup', 'port_test_display_vcount',
                               'port_test_display_status', 'port_run_scanline'])
    subprocess.run(['node', 'port/tests/display_bus.cjs',
                    'build/port/slice-wasm32-display-bus/boot.wasm'], check=True)


if __name__ == '__main__': main()

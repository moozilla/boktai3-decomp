#!/usr/bin/env python3
"""Rerun core runtime tests using the host compiler; no ROM needed."""
import os
from pathlib import Path
import platform
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def host_flags():
    if sys.platform == 'darwin':
        # arm64's mandatory 4 GiB page-zero reservation covers the GBA map.
        return ['-arch', 'x86_64'], ['-Wl,-no_pie,-pagezero_size,0x10000']
    return [], ['-no-pie']


def main():
    os.chdir(ROOT)
    out = Path('build/port/tests')
    out.mkdir(parents=True, exist_ok=True)
    cc = os.environ.get('PORT_HOST_CC', 'clang')
    cflags, ldflags = host_flags()
    binary = out / 'core_test'
    subprocess.run([cc, *cflags, '-std=c99', '-O2', '-Wall', '-Wextra', '-Werror',
                    '-Iport/include', 'port/core/memory.c', 'port/core/timing.c',
                    'port/tests/core_test.c', *ldflags, '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)


if __name__ == '__main__':
    main()

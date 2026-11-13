#!/usr/bin/env python3
"""Build the offline browser demonstration; generated code/artifacts stay in build/."""
import argparse
import base64
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--hardware-root')
    parser.add_argument('--ppu-root', default='port')
    args = parser.parse_args()
    command = [sys.executable, 'port/build.py', '--slice', '--mmio', '--ppu-root', args.ppu_root]
    if args.hardware_root: command += ['--hardware-root', args.hardware_root]
    subprocess.run(command, cwd=ROOT, check=True)
    out = ROOT / 'build/demo'
    out.mkdir(parents=True, exist_ok=True)
    module = ROOT / 'build/port/slice-wasm32/boot.wasm'
    shutil.copyfile(module, out / 'boot.wasm')
    html = (ROOT / 'port/demo/index.html').read_text()
    html = html.replace('<script src="replay.js"></script>',
        '<script>globalThis.PORT_WASM_BASE64="' + base64.b64encode(module.read_bytes()).decode() + '";</script>\n' +
        '<script>' + (ROOT / 'port/demo/replay.js').read_text() + '</script>')
    html = html.replace('<script src="viewer.js"></script>',
                        '<script>' + (ROOT / 'port/demo/viewer.js').read_text() + '</script>')
    (out / 'index.html').write_text(html)
    print('Open build/demo/index.html directly in a browser. No server required.')


if __name__ == '__main__': main()

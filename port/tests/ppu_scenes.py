#!/usr/bin/env python3
"""Regenerate PPU scene captures from the user's checkpoint save bank.

Example: build/venv/bin/python port/tests/ppu_scenes.py --bank ../boktai3trans/saves
All battery saves, RAM snapshots and screenshots remain under build/ppu/story/.
"""
import argparse
import gzip
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
from ppu_regression import ROOT, BUILD, build, compare, run

sys.path.insert(0, str(ROOT))
from tools.assets.saves import current, read_sgm, write_sav

BOOT = '''trace off
wait 300
tap START
wait 60
tap A
wait 150
tap START
wait 120
tap A
wait 120
tap START
wait 300
tap START
wait 120
tap START
wait 120
tap A
wait 150
tap A
wait 300
'''


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--bank', type=Path, required=True)
    ap.add_argument('--checkpoints', default='0002,0013,0022,0032,0045,0064,0069,0086',
                    help='comma-separated checkpoint numbers, or all')
    ap.add_argument('--out', type=Path, default=Path('build/ppu/story'))
    ap.add_argument('--wasm', action='store_true')
    ap.add_argument('--resume', action='store_true', help='replay complete captures instead of recapturing them')
    args = ap.parse_args()
    build(capture=True)
    root = args.out.resolve()
    root.mkdir(parents=True, exist_ok=True)
    rows = []
    checkpoints = (sorted({p.name.split(' ', 1)[0] for p in args.bank.glob('[0-9][0-9][0-9][0-9] *.sgm')})
                   if args.checkpoints == 'all' else args.checkpoints.split(','))
    for checkpoint in checkpoints:
        if (ROOT / 'build/STOP').exists():
            print('build/STOP found: stopping between checkpoints', flush=True)
            break
        matches = list(args.bank.glob(checkpoint + ' *.sgm'))
        if len(matches) != 1:
            raise RuntimeError(f'expected one checkpoint {checkpoint}, found {len(matches)}')
        directory = root / checkpoint
        directory.mkdir(parents=True, exist_ok=True)
        save = directory / 'input.sav'
        names = ('field', 'animated', 'action', 'menu', 'resumed')
        complete = all((directory / (name + '.ppu.gz')).is_file() and
                       (directory / (name + '.ppm')).is_file() for name in names)
        if not (args.resume and complete):
            write_sav(current(read_sgm(str(matches[0]))), str(save))
            script = directory / 'capture.txt'
            script.write_text(BOOT + '''shot field
wait 30
shot animated
hold A 20
wait 15
shot action
tap START
wait 60
shot menu
tap B
wait 60
shot resumed
''')
            env = dict(os.environ, BOKTAI3_SAV=str(save))
            with (directory / 'harness.log').open('w') as log:
                run([BUILD / 'ppu_capture', ROOT / 'baserom.gba', script, directory], env=env,
                    stdout=subprocess.DEVNULL, stderr=log)
            for path in directory.glob('*.ppu'):
                with path.open('rb') as src, gzip.open(str(path) + '.gz.tmp', 'wb', compresslevel=1) as dst:
                    shutil.copyfileobj(src, dst)
                Path(str(path) + '.gz.tmp').replace(str(path) + '.gz')
                path.unlink()
        count = compare(directory)
        rows.append(dict(checkpoint=checkpoint, label=matches[0].stem, mismatches=count))
        (root / 'results.json').write_text(json.dumps(rows, indent=2) + '\n')
        print(f'{matches[0].stem}: {count} pixel mismatches', flush=True)
        if args.wasm:
            run(['node', 'port/tests/ppu_wasm.mjs', BUILD / 'ppu.wasm', directory])
    if not rows:
        raise SystemExit('no scenes captured')
    raise SystemExit(any(r['mismatches'] for r in rows))


if __name__ == '__main__':
    main()

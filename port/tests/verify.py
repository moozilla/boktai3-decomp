#!/usr/bin/env python3
"""Replay short checkpoint movies against the ROM or sweep the complete bank.

Build the port and its agent types first, then run from the repository root:

    make -f tools/emu/Makefile build/emu/harness
    build/venv/bin/python port/tests/verify.py --bank build/saves
    build/venv/bin/python port/tests/verify.py --bank build/saves --all --resume

The checkpoint saves come from tools/assets/saves.py. Each failure leaves
movie.json, harness.txt and named tick differences under build/autopilot.
"""
import argparse
import json
import os
from pathlib import Path
import signal
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CASES = [
    ('sealed-prison', '0002', 'smoke'),
    ('san-miguel', '0007', 'smoke'),
    ('lifeless-town-swords', '0010', 'swords'),
    ('lifeless-town-after-armor', '0011', 'swords'),
    ('lifeless-town-reset', '0010', 'reset'),
    ('ancient-tree', '0019', 'smoke'),
    ('white-forest', '0028', 'smoke'),
    ('pirate-island', '0037', 'smoke'),
    ('dark-castle', '0051', 'smoke'),
    ('lost-paradise', '0067', 'smoke'),
    ('credits', '0082', 'smoke'),
]


def run_case(save, mode, out, frames, seed):
    out.mkdir(parents=True, exist_ok=True)
    cmd = ['node', 'tools/autopilot/stress.mjs', '--save', str(save), '--out', str(out),
           '--mode', mode, '--frames', str(frames), '--seed', str(seed)]
    with (out / 'run.log').open('w') as stream:
        process = subprocess.Popen(cmd, cwd=ROOT, stdout=stream, stderr=subprocess.STDOUT,
                                   start_new_session=True)
        try:
            return process.wait(timeout=540)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            process.wait()
            stream.write('\nTimed out after 540 seconds\n')
            return 124


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--bank', type=Path, default=ROOT / 'build/saves')
    parser.add_argument('--all', action='store_true', help='fuzz every checkpoint for 2800 frames')
    parser.add_argument('--resume', action='store_true', help='keep completed results from a bank sweep')
    parser.add_argument('--case', help='run one named regression or checkpoint ID')
    parser.add_argument('--frames', type=int, default=2800)
    parser.add_argument('--seed', type=int, default=1)
    args = parser.parse_args()
    if args.resume and not args.all:
        parser.error('--resume requires --all')
    if not (ROOT / 'build/port/game/game.wasm').is_file():
        parser.error('build/port/game/game.wasm is missing. Run port/build_game.py first')
    if not (ROOT / 'build/port/agent/types.json').is_file():
        parser.error('build/port/agent/types.json is missing. Run port/agent/gametypes.py first')
    if not os.environ.get('BOKTAI3_HARNESS') and not (ROOT / 'build/emu/harness').is_file():
        parser.error('build/emu/harness is missing. Run make -f tools/emu/Makefile build/emu/harness')
    bank = args.bank.resolve()
    cases = [(p.stem, p.stem, 'fuzz') for p in sorted(bank.glob('*.sav'))] if args.all else CASES
    if args.case:
        cases = [case for case in cases if case[0] == args.case]
    if not cases:
        parser.error('no checkpoints to verify')
    out_root = ROOT / 'build/autopilot' / (f'fuzz-{args.seed}-{args.frames}' if args.all else 'verify')
    failures = 0
    for name, checkpoint, mode in cases:
        if (ROOT / 'build/STOP').exists():
            print('Stopped at a checkpoint boundary')
            return 2
        save = bank / f'{checkpoint}.sav'
        out = out_root / name
        if not save.is_file():
            parser.error(f'{save} is missing')
        if args.resume and (out / 'result.json').is_file():
            result = json.loads((out / 'result.json').read_text())
            status = bool(result['difference'] or result['fault'])
        else:
            status = run_case(save, mode, out, args.frames, args.seed)
        print(f'{name} {"DIFFER" if status else "MATCH"} {out.relative_to(ROOT)}/run.log', flush=True)
        if status:
            failures += 1
            if not args.all:
                return 1
    print(f'{len(cases) - failures}/{len(cases)} checkpoint movies matched')
    return bool(failures)


if __name__ == '__main__':
    sys.exit(main())

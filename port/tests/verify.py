#!/usr/bin/env python3
"""Replay short checkpoint movies against the ROM or sweep the complete bank.

Build the port and its agent types first, then run from the repository root:

    make -f tools/emu/Makefile build/emu/harness
    build/venv/bin/python port/tests/verify.py --bank build/saves
    build/venv/bin/python port/tests/verify.py --all --seed 1 --seed 2 --seed 3 --resume

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
import time

ROOT = Path(__file__).resolve().parents[2]
CASES = [
    ('sealed-prison', '0002', 'smoke', 2698),
    ('san-miguel', '0007', 'smoke', 2553),
    ('world-map', '0028', 'smoke', 2569),
    ('lifeless-town-swords', '0010', 'swords', 3091),
    ('lifeless-town-after-armor', '0011', 'swords', 3095),
    ('lifeless-town-reset', '0010', 'reset', 3091),
    ('ancient-tree', '0019', 'smoke', 3363),
    ('white-forest', '0030', 'smoke', 4006),
    ('pirate-island', '0039', 'smoke', 4572),
    ('dark-castle', '0054', 'smoke', 5111),
    ('lost-paradise', '0069', 'smoke', 6138),
    ('credits', '0086', 'smoke', 6595),
]


def run_case(save, mode, out, frames, seed, scene):
    out.mkdir(parents=True, exist_ok=True)
    (out / 'result.json').unlink(missing_ok=True)
    cmd = ['node', 'tools/autopilot/stress.mjs', '--save', str(save), '--out', str(out),
           '--mode', mode, '--frames', str(frames), '--seed', str(seed)]
    if scene is not None:
        cmd += ['--scene', str(scene)]
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
    parser.add_argument('--out-root', type=Path, default=ROOT / 'build/autopilot',
                        help='separate output root for a different wasm build')
    parser.add_argument('--all', action='store_true', help='fuzz every checkpoint for 2800 frames')
    parser.add_argument('--resume', action='store_true', help='keep completed results from a bank sweep')
    parser.add_argument('--case', help='run one named regression or checkpoint ID')
    parser.add_argument('--frames', type=int, default=2800)
    parser.add_argument('--seed', type=int, action='append',
                        help='fuzz seed (repeat for multiple seeds, defaults to 1)')
    args = parser.parse_args()
    seeds = list(dict.fromkeys(args.seed or [1]))
    if any(not 1 <= seed <= 0xffffffff for seed in seeds):
        parser.error('--seed must be between 1 and 4294967295')
    if args.seed and not args.all:
        parser.error('--seed requires --all')
    if not 1 <= args.frames <= 20000:
        parser.error('--frames must be between 1 and 20000')
    if args.resume and not args.all:
        parser.error('--resume requires --all')
    if not (ROOT / 'build/port/game/game.wasm').is_file():
        parser.error('build/port/game/game.wasm is missing. Run port/build_game.py first')
    if not (ROOT / 'build/port/agent/types.json').is_file():
        parser.error('build/port/agent/types.json is missing. Run port/agent/gametypes.py first')
    if not os.environ.get('BOKTAI3_HARNESS') and not (ROOT / 'build/emu/harness').is_file():
        parser.error('build/emu/harness is missing. Run make -f tools/emu/Makefile build/emu/harness')
    bank = args.bank.resolve()
    cases = [(p.stem, p.stem, 'fuzz', None) for p in sorted(bank.glob('*.sav'))] if args.all else CASES
    if args.case:
        cases = [case for case in cases if case[0] == args.case]
    if not cases:
        parser.error('no checkpoints to verify')
    base = args.out_root.resolve()
    started = time.monotonic()
    failures = 0
    for seed, (name, checkpoint, mode, scene) in (
        (seed, case) for seed in seeds for case in cases
    ):
        if (ROOT / 'build/STOP').exists():
            print('Stopped at a checkpoint boundary')
            return 2
        save = bank / f'{checkpoint}.sav'
        out_root = base / (f'fuzz-{seed}-{args.frames}' if args.all else 'verify')
        out = out_root / name
        if not save.is_file():
            parser.error(f'{save} is missing')
        label = f'{name} seed {seed}' if args.all else name
        if args.resume and (out / 'result.json').is_file():
            result = json.loads((out / 'result.json').read_text())
            status = bool(result['difference'] or result['fault'])
        else:
            status = run_case(save, mode, out, args.frames, seed, scene)
        outcome = f'DIFFER (status {int(status)})' if status else 'MATCH'
        print(f'{label} {outcome} {os.path.relpath(out, ROOT)}/run.log', flush=True)
        if status:
            failures += 1
            if not args.all:
                return 1
    total = len(seeds) * len(cases)
    print(f'{total - failures}/{total} checkpoint movies matched in {time.monotonic() - started:.1f}s')
    return bool(failures)


if __name__ == '__main__':
    sys.exit(main())

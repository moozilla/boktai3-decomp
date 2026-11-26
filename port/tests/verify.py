#!/usr/bin/env python3
"""Replay short checkpoint movies against the ROM or sweep the complete bank.

Build the port and its agent types first, then run from the repository root:

    make -f tools/emu/Makefile build/emu/harness
    make port-verify
    build/venv/bin/python port/tests/verify.py --all --seed 1 --seed 2 --seed 3 --resume

The checkpoint saves come from tools/assets/saves.py. Each failure leaves
movie.json, harness.txt and named tick differences under build/autopilot.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import json
import os
from pathlib import Path
import signal
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = Path(__file__).with_name('verify-movies.json')


def run_case(save, out, frames, seed, recording):
    out.mkdir(parents=True, exist_ok=True)
    (out / 'result.json').unlink(missing_ok=True)
    cmd = ['node', 'tools/autopilot/stress.mjs', '--save', str(save), '--out', str(out),
           '--frames', str(frames), '--seed', str(seed)]
    if recording:
        replay = out / 'fixture.json'
        replay.write_text(json.dumps(recording) + '\n')
        cmd += ['--from', str(replay)]
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
    parser.add_argument('--jobs', type=int, choices=(1, 2), default=1,
                        help='number of simultaneous checkpoint comparisons for a bank sweep')
    args = parser.parse_args()
    seeds = list(dict.fromkeys(args.seed or [1]))
    if any(not 1 <= seed <= 0xffffffff for seed in seeds):
        parser.error('--seed must be between 1 and 4294967295')
    if args.seed and not args.all:
        parser.error('--seed requires --all')
    if args.jobs != 1 and not args.all:
        parser.error('--jobs requires --all')
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
    if args.all:
        cases = [(p.stem, p.stem, None) for p in sorted(bank.glob('*.sav'))]
    else:
        manifest = json.loads(FIXTURES.read_text())
        cases = [(name, spec['checkpoint'],
                  {**manifest['movies'][spec['movie']], 'scene': spec['scene']})
                 for name, spec in manifest['cases'].items()]
    if args.case:
        cases = [case for case in cases if case[0] == args.case]
    if not cases:
        parser.error('no checkpoints to verify')
    for _, checkpoint, _ in cases:
        save = bank / f'{checkpoint}.sav'
        if not save.is_file():
            parser.error(f'{save} is missing')
    base = args.out_root.resolve()
    started = time.monotonic()

    def compare_case(item):
        seed, (name, checkpoint, recording) = item
        if (ROOT / 'build/STOP').exists():
            return 2
        save = bank / f'{checkpoint}.sav'
        out_root = base / (f'fuzz-{seed}-{args.frames}' if args.all else 'verify')
        out = out_root / name
        label = f'{name} seed {seed}' if args.all else name
        result_path = out / 'result.json'
        status = 0
        if not args.resume or not result_path.is_file():
            status = run_case(save, out, args.frames, seed, recording)
        if not result_path.is_file() and status == 0:
            status = 1
        result = json.loads(result_path.read_text()) if result_path.is_file() else {}
        difference = result.get('difference')
        fault = result.get('fault')
        if difference and 'tick' in difference:
            outcome = f"DIFFER at tick {difference['tick']} ({', '.join(difference['changes'])})"
        elif fault and result.get('faultAt'):
            outcome = f"DIFFER before tick {result['faultAt']['tick']} ({fault.splitlines()[0]})"
        elif difference or fault or status:
            outcome = f'DIFFER (status {status or 1})'
        else:
            outcome = 'MATCH'
        print(f'{label} {outcome} {os.path.relpath(out, ROOT)}/run.log', flush=True)
        return int(bool(difference or fault or status))

    work = [(seed, case) for seed in seeds for case in cases]
    if args.jobs == 1:
        outcomes = []
        for item in work:
            outcome = compare_case(item)
            outcomes.append(outcome)
            if outcome == 2 or (outcome and not args.all):
                break
    else:
        with ThreadPoolExecutor(args.jobs) as pool:
            try:
                outcomes = list(pool.map(compare_case, work))
            except KeyboardInterrupt:
                pool.shutdown(cancel_futures=True)
                raise
    if 2 in outcomes:
        print('Stopped at a checkpoint boundary')
        return 2
    failures = outcomes.count(1)
    total = len(seeds) * len(cases)
    print(f'{outcomes.count(0)}/{total} checkpoint movies matched in {time.monotonic() - started:.1f}s')
    return bool(failures)


if __name__ == '__main__':
    sys.exit(main())

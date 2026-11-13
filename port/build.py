#!/usr/bin/env python3
"""Compile the game's unchanged C for wasm32 or the host (no ROM build)."""
import argparse
import concurrent.futures
import hashlib
import json
import os
import re
from pathlib import Path
import shutil
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[1]
LLVM = Path('/opt/homebrew/opt/llvm/bin')
sys.path.insert(0, str(ROOT / 'port/core'))
from prepare import prepare_headers, prepare_source


def sources():
    return sorted(ROOT.glob('src/*.c')) + sorted(ROOT.glob('src/fn/*.c')) + sorted(ROOT.glob('src/lib/*.c'))


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--target', choices=['wasm32', 'host'], default='wasm32')
    ap.add_argument('--cc', default=os.environ.get('PORT_CC', str(LLVM / 'clang') if (LLVM / 'clang').exists() else 'clang'))
    ap.add_argument('-j', '--jobs', type=int, default=4)
    ap.add_argument('--source', action='append', help='repo-relative source (repeatable); default: every game C unit')
    ap.add_argument('--force', action='store_true')
    args = ap.parse_args()
    os.chdir(ROOT)
    out = Path('build/port') / args.target
    out.mkdir(parents=True, exist_ok=True)
    headers = out / 'include'
    prepare_headers(ROOT, headers, args.target)
    flags = ['-std=gnu89', '-O2', '-fno-strict-aliasing', '-fwrapv', '-fno-builtin', '-Wno-everything',
             '-Wno-error=implicit-int', '-Wno-error=implicit-function-declaration',
             '-Wno-error=int-conversion', '-Wno-error=incompatible-pointer-types',
             '-Iport/include', '-I' + str(headers), '-iquote', '.', '-include', 'port/core/compat.h']
    if args.target == 'wasm32':
        flags += ['--target=wasm32', '-ffreestanding']
    units = [ROOT / p for p in args.source] if args.source else sources()
    header_hash = hashlib.sha256()
    for p in sorted(ROOT.glob('include/**/*.h')) + sorted(ROOT.glob('port/**/*.h')):
        header_hash.update(str(p.relative_to(ROOT)).encode())
        header_hash.update(p.read_bytes())
    identity = subprocess.check_output([args.cc, '--version'])
    context = identity + repr(flags).encode() + header_hash.digest() + (ROOT / 'port/core/prepare.py').read_bytes()

    def compile_one(src):
        name = str(src.relative_to(ROOT))
        obj = out / (name + '.o')
        obj.parent.mkdir(parents=True, exist_ok=True)
        stamp = obj.with_suffix('.sha256')
        key = hashlib.sha256(context + src.read_bytes()).hexdigest()
        if not args.force and obj.exists() and stamp.exists() and stamp.read_text() == key:
            return {'source': name, 'status': 'cached', 'object': str(obj)}
        prepared = out / 'prepared' / name
        prepared.parent.mkdir(parents=True, exist_ok=True)
        prepared.write_text('#line 1 "' + name + '"\n' + prepare_source(src, args.target))
        command = [args.cc, *flags, '-c', str(prepared), '-o', str(obj)]
        result = subprocess.run(command, text=True, capture_output=True)
        log = obj.with_suffix('.log')
        log.write_text(result.stdout + result.stderr)
        if result.returncode == 0:
            stamp.write_text(key)
        else:
            obj.unlink(missing_ok=True)
        return {'source': name, 'status': 'ok' if result.returncode == 0 else 'failed',
                'object': str(obj), 'log': str(log)}

    start = time.monotonic()
    results = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for result in pool.map(compile_one, units):
            results.append(result)
            if result['status'] == 'failed':
                print('FAIL ' + result['source'], flush=True)
            if len(results) % 500 == 0:
                print(f'{len(results)}/{len(units)} checked', flush=True)
    failed = [r for r in results if r['status'] == 'failed']
    report = {'target': args.target, 'units': len(results), 'failed': len(failed),
              'seconds': round(time.monotonic() - start, 2), 'results': results}
    (out / 'compile.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f"{args.target}: {len(results) - len(failed)}/{len(results)} compiled; {len(failed)} failed")
    return bool(failed)


if __name__ == '__main__':
    sys.exit(main())

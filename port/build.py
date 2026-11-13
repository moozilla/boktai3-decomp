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
from link_slice import build_slice, find_linker
from addresses import read_addresses, bind_ir
from mmio import instrument_mmio
from audit import audit_link


def sources():
    return sorted(ROOT.glob('src/*.c')) + sorted(ROOT.glob('src/fn/*.c')) + sorted(ROOT.glob('src/lib/*.c'))


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--target', choices=['wasm32', 'host'], default='wasm32')
    ap.add_argument('--cc', default=os.environ.get('PORT_CC', str(LLVM / 'clang') if (LLVM / 'clang').exists() else 'clang'))
    ap.add_argument('-j', '--jobs', type=int, default=4)
    ap.add_argument('--source', action='append', help='repo-relative source (repeatable); default: every game C unit')
    ap.add_argument('--force', action='store_true')
    ap.add_argument('--compile-only', action='store_true', help='skip final linking after a bound-object compile')
    ap.add_argument('--mmio', action='store_true', help='route volatile scalar accesses through port-hw in linked builds')
    ap.add_argument('--link-game', action='store_true', help='bind absolute data addresses and attempt the complete wasm game link')
    ap.add_argument('--video-irqs', action='store_true', help='include the original VBlank handler in the boot slice')
    ap.add_argument('--rtc', action='store_true', help='extend the boot slice through RTC date synchronization')
    ap.add_argument('--slice', action='store_true', help='link the tested early-boot vertical slice')
    ap.add_argument('--ppu-root', help='port-ppu port directory when testing before integration')
    ap.add_argument('--hardware-root', help='port-hw port directory when testing before integration')
    args = ap.parse_args()
    os.chdir(ROOT)
    if args.link_game:
        args.mmio = True
    if args.slice:
        return build_slice(ROOT, args)
    out = Path('build/port') / (args.target + ('-bound' if args.link_game else ''))
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
    address_map = read_addresses(ROOT) if args.link_game else {}
    if args.link_game:
        if args.target != 'wasm32':
            ap.error('--link-game requires wasm32')
        hardware = Path(args.hardware_root).resolve() if args.hardware_root else ROOT / 'port'
        flags += ['-Iport/core', '-I' + str(hardware / 'include'), '-I' + str(hardware / 'hw')]
        units += [ROOT / 'port/core/bus.c', ROOT / 'port/core/devices.c']
        units += [ROOT / p for p in ['port/core/memory.c', 'port/core/timing.c', 'port/core/boot.c', 'port/core/entry.c', 'port/core/callback.c', 'port/core/veneers.c']]
        # Keep external worker paths out of committed code and serialized names.
        # The MMIO pass retains the original RTC C and routes every GPIO edge.
        units += [p for p in sorted((hardware / 'hw').glob('*.c')) if p.name != 'rtc_sdk.c']
    header_hash = hashlib.sha256()
    header_groups = [('game', ROOT / 'include'), ('port', ROOT / 'port')]
    if args.link_game and hardware != ROOT / 'port':
        header_groups.append(('hardware', hardware))
    for label, directory in header_groups:
        for p in sorted(directory.rglob('*.h')):
            header_hash.update((label + '/' + str(p.relative_to(directory))).encode())
            header_hash.update(p.read_bytes())
    identity = subprocess.check_output([args.cc, '--version'])
    context = identity + repr(flags).encode() + header_hash.digest()
    for helper in ('prepare.py', 'returns.py', 'addresses.py', 'mmio.py'):
        context += (ROOT / 'port/core' / helper).read_bytes()
    context += json.dumps(address_map, sort_keys=True).encode() + str(args.mmio).encode()

    def compile_one(src):
        name = str(src.relative_to(ROOT)) if src.is_relative_to(ROOT) else 'port/hw/' + src.name
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
        if args.link_game:
            ir = obj.with_suffix('.ll')
            result = subprocess.run([args.cc, *flags, '-S', '-emit-llvm', str(prepared), '-o', str(ir)], text=True, capture_output=True)
            if result.returncode == 0:
                bound, resolved, unresolved = bind_ir(ir.read_text(), address_map)
                if args.mmio and name.startswith('src/'):
                    bound, metrics = instrument_mmio(bound)
                    obj.with_suffix('.mmio.json').write_text(json.dumps(metrics))
                ir.write_text(bound)
                obj.with_suffix('.bindings.json').write_text(json.dumps({'resolved': resolved, 'unresolved': unresolved}, indent=2))
                command = [args.cc, '--target=wasm32', '-O2', '-c', str(ir), '-o', str(obj)]
                result = subprocess.run(command, text=True, capture_output=True)
        else:
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
    if args.link_game and not failed and not args.compile_only:
        excluded = {'src/fn/AgbMain.c', 'src/fn/sub_08213F80.c',
                    'src/fn/sub_082485EC.c', 'src/fn/sub_082486B4.c', 'src/fn/sub_08248778.c',
                    'src/fn/sub_082488D8.c', 'src/fn/sub_08248764.c'}
        objects = [r['object'] for r in results if r['source'] not in excluded]
        command = [find_linker(), '--no-entry', '--gc-sections', '--export-memory',
                   '--initial-memory=167772160', '--max-memory=167772160',
                   '--global-base=1048576', '-z', 'stack-size=1048576', '--error-limit=0', '--fatal-warnings',
                   '--export=port_memory_init', '--export=port_load_rom',
                   '--export=port_game_initialize', '--export=port_game_step',
                   *objects, '-o', str(out / 'game.wasm')]
        response = out / 'link.rsp'
        import shlex
        response.write_text('\n'.join(shlex.quote(part) for part in command[1:]) + '\n')
        linked = subprocess.run([command[0], '@' + str(response)], text=True, capture_output=True)
        (out / 'link.log').write_text(linked.stdout + linked.stderr)
        audit = audit_link(out / 'link.log', out / 'link-audit.json')
        print(f"link boundaries: {len(audit['signature_conflicts'])} signature conflicts, {len(audit['undefined_symbols'])} undefined symbols")
        print('game link: ' + ('PASS' if linked.returncode == 0 else 'FAIL (see ' + str(out / 'link.log') + ')'))
        return linked.returncode
    return bool(failed)


if __name__ == '__main__':
    sys.exit(main())

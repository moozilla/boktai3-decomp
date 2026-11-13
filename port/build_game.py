#!/usr/bin/env python3
"""Build the full game for the browser: every game C unit plus the port runtime,
linked into build/port/game/game.wasm (no ROM bytes inside; the page loads
baserom.gba at run time).

Stages (cached by content hash under build/port/game/):
  1. prepare + clang -emit-llvm, bind GBA data addresses, route volatile MMIO
  2. whole-program ABI pass (port/core/abi.py): call signatures, code pointers
  3. compile the rewritten IR, generate the code-address table, link with LLD
"""
import argparse
import concurrent.futures
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[1]
LLVM = Path('/opt/homebrew/opt/llvm/bin')
sys.path.insert(0, str(ROOT / 'port/core'))
from prepare import prepare_headers, prepare_source  # noqa: E402
from addresses import read_addresses, bind_ir  # noqa: E402
from mmio import instrument_mmio  # noqa: E402
from link_slice import find_linker  # noqa: E402
import abi  # noqa: E402

OUT = ROOT / 'build/port/game'
# Port runtime. "game" marks units whose indirect calls take GBA code addresses.
PORT_UNITS = {
    'port/core/memory.c': False, 'port/core/timing.c': False, 'port/core/bus.c': False,
    'port/core/devices.c': False, 'port/core/video.c': False, 'port/core/callback.c': True,
    'port/core/veneers.c': False, 'port/game/icall.c': False, 'port/game/game.c': False,
    'port/game/m4a_seq.c': True, 'port/game/libgcc.c': True, 'port/ppu/ppu.c': False,
    'port/hw/bios.c': False, 'port/hw/bios_runtime.c': False, 'port/hw/cartridge.c': False,
    'port/hw/devices.c': False, 'port/hw/mixer.c': True, 'port/hw/psg.c': False,
    'port/game/hooks.c': False,
}
# Port C functions standing in for ROM assembly without a symbol in the ELF
# (MP2K jump-table targets, see port/game/m4a_seq.c). Thumb code words.
PORT_CODE = {
    'RealClear64byte': 0x0822F5ED, 'RealClearChain': 0x0822F605, 'ply_fine': 0x0822F625,
    'ply_goto': 0x0822F69D, 'ply_patt': 0x0822F6BD, 'ply_pend': 0x0822F6D9,
    'ply_rept': 0x0822F6ED, 'ply_prio': 0x0822F71D, 'ply_tempo': 0x0822F729,
    'ply_keysh': 0x0822F73D, 'ply_voice': 0x0822F751, 'ply_vol': 0x0822F781,
    'ply_pan': 0x0822F795, 'ply_bend': 0x0822F7A9, 'ply_bendr': 0x0822F7BD,
    'ply_lfodl': 0x0822F7D1, 'ply_modt': 0x0822F7DD, 'ply_tune': 0x0822F7F5,
    'ply_port': 0x0822F809,
}
EXPORTS = ['port_trace_map', 'port_memory_init', 'port_load_rom', 'port_game_run', 'port_game_set_solar',
           'port_game_set_time', 'port_frame_count', 'port_game_save_stage', 'port_game_save_dirty',
           'port_game_save_data', 'port_game_save_size',
           # agent harness (port/agent/): hooks, calls into the game, C snippets
           'port_hook_state', 'port_hook_flags', 'port_hook_ring', 'port_icall', 'port_code_known',
           'port_bus_load_i8', 'port_bus_load_i16', 'port_bus_load_i32', 'port_bus_load_ptr',
           'port_bus_store_i8', 'port_bus_store_i16', 'port_bus_store_i32', 'port_bus_store_ptr']


def game_sources():
    return (sorted(ROOT.glob('src/*.c')) + sorted(ROOT.glob('src/fn/*.c')) +
            sorted(ROOT.glob('src/lib/*.c')))


def code_addresses():
    """Function name -> the code word the ROM build uses (Thumb bit set)."""
    out = subprocess.run(['arm-none-eabi-readelf', '-sW', str(ROOT / 'build/boktai3.elf')],
                         text=True, capture_output=True, check=True).stdout
    result = {}
    for line in out.splitlines():
        parts = line.split()
        if len(parts) < 8 or parts[1] == '00000000' or not re.fullmatch(r'[0-9a-f]{8}', parts[1]):
            continue
        value = int(parts[1], 16)
        # Linker-script code labels (e.g. MPlayMain = gUnk_0822F820 + 1) are
        # NOTYPE; a Thumb bit in ROM marks them as code.
        if parts[3] == 'FUNC' or (parts[3] == 'NOTYPE' and parts[4] == 'GLOBAL' and
                                  value & 1 and 0x08000000 <= value < 0x0a000000):
            result.setdefault(parts[7], value)
    for name, value in PORT_CODE.items():
        if result.get(name, value) != value:
            sys.exit(f'PORT_CODE {name} disagrees with the ELF')
        result[name] = value
    return result


def run(cmd):
    return subprocess.run(cmd, text=True, capture_output=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('-j', '--jobs', type=int, default=os.cpu_count() or 4)
    ap.add_argument('--cc', default=str(LLVM / 'clang') if (LLVM / 'clang').exists() else 'clang')
    ap.add_argument('--force', action='store_true')
    ap.add_argument('--trace', action='store_true',
                    help='record each GBA function\'s first call (build/port/game-trace/)')
    ap.add_argument('--watch', metavar='FUNC[,FUNC]',
                    help='log these functions\' first three arguments at every entry (build/port/game-watch/)')
    args = ap.parse_args()
    os.chdir(ROOT)
    global OUT
    if args.trace:
        OUT = ROOT / 'build/port/game-trace'
    elif args.watch:
        OUT = ROOT / 'build/port/game-watch'
    if not (ROOT / 'build/boktai3.elf').exists():
        sys.exit('build/boktai3.elf is missing: run make (or tools/worktree.sh --here) first')
    OUT.mkdir(parents=True, exist_ok=True)
    headers = OUT / 'include'
    prepare_headers(ROOT, headers, 'wasm32')
    # Stage 1 emits unoptimized IR (-disable-llvm-passes keeps -O2's attributes):
    # the hooks go in before inlining, so inlined copies keep them; stage 3 optimizes.
    flags = ['--target=wasm32', '-ffreestanding', '-std=gnu89', '-O2', '-Xclang', '-disable-llvm-passes',
             '-funsigned-char', '-fno-strict-aliasing',
             '-fwrapv', '-fno-builtin', '-Wno-everything', '-Iport/include', '-Iport/core',
             '-Iport/game', '-Iport/hw', '-I' + str(headers), '-iquote', '.',
             '-include', 'port/core/compat.h']
    if args.trace:
        flags.append('-fno-inline')
    hasher = hashlib.sha256()
    for directory in (ROOT / 'include', ROOT / 'port'):
        for p in sorted(directory.rglob('*.h')):
            hasher.update(str(p.relative_to(ROOT)).encode() + p.read_bytes())
    for helper in ('prepare.py', 'returns.py', 'addresses.py', 'mmio.py'):
        hasher.update((ROOT / 'port/core' / helper).read_bytes())
    address_map = read_addresses(ROOT)
    hasher.update(json.dumps(address_map, sort_keys=True).encode())
    hasher.update(subprocess.check_output([args.cc, '--version']) + repr(flags).encode())
    context = hasher.digest()

    units = [(str(p.relative_to(ROOT)), True) for p in game_sources()]
    doubles = {int(m, 16) for p in game_sources()
               for m in re.findall(r'extern const double gUnk_([0-9A-F]{8})\[\]', p.read_text())}
    listed = {int(m, 16) for m in re.findall(r'\{0x([0-9A-F]{8}), \d+\}', (ROOT / 'port/game/game.c').read_text())}
    if doubles != listed:
        sys.exit('ROM double tables changed; update rom_doubles in port/game/game.c: '
                 + ', '.join(f'{a:08X}' for a in sorted(doubles ^ listed)))
    units += list(PORT_UNITS.items())

    def stage1(item):
        name, _game = item
        src = ROOT / name
        ll = OUT / 'ir' / (name + '.ll')
        stamp = ll.with_suffix('.key')
        key = hashlib.sha256(context + src.read_bytes()).hexdigest()
        if not args.force and ll.exists() and stamp.exists() and stamp.read_text() == key:
            return name, None
        ll.parent.mkdir(parents=True, exist_ok=True)
        prepared = OUT / 'prepared' / name
        prepared.parent.mkdir(parents=True, exist_ok=True)
        prepared.write_text('#line 1 "' + name + '"\n' + prepare_source(src, 'wasm32'))
        r = run([args.cc, *flags, '-S', '-emit-llvm', str(prepared), '-o', str(ll)])
        if r.returncode:
            return name, r.stderr[-2000:]
        text, _resolved, _unresolved = bind_ir(ll.read_text(), address_map)
        if name.startswith('src/'):
            text, _metrics = instrument_mmio(text)
        ll.write_text(text)
        stamp.write_text(key)
        return name, None

    t0 = time.monotonic()
    failures = []
    with concurrent.futures.ThreadPoolExecutor(args.jobs) as pool:
        for i, (name, err) in enumerate(pool.map(stage1, units), 1):
            if err:
                failures.append(name)
                print('FAIL', name, '\n  ' + '\n  '.join(l for l in err.splitlines() if 'error:' in l), flush=True)
            if i % 2000 == 0:
                print(f'  stage 1: {i}/{len(units)}', flush=True)
    if failures:
        sys.exit(f'{len(failures)} units failed to compile')
    print(f'stage 1 (C -> IR): {len(units)} units, {time.monotonic() - t0:.0f}s', flush=True)

    # Whole-program view: external definitions; port definitions replace game ones.
    t0 = time.monotonic()
    texts = {name: (OUT / 'ir' / (name + '.ll')).read_text() for name, _ in units}
    defs_global, owner = {}, {}
    replaced = {}
    for name, _ in units:
        defs, _decls, _internals = abi.scan_module(texts[name])
        for fn, sig in defs.items():
            if fn in owner:
                port, game = (name, owner[fn]) if name.startswith('port/') else (owner[fn], name)
                if not port.startswith('port/'):
                    sys.exit(f'{fn} defined in both {owner[fn]} and {name}')
                replaced.setdefault(game, set()).add(fn)
                owner[fn] = port
                defs_global[fn] = defs[fn] if name == port else defs_global[fn]
            else:
                owner[fn] = name
                defs_global[fn] = sig
    addresses = code_addresses()
    # ROM symbols sharing an address are one function (#define aliases in C,
    # ram.ld aliases); resolve references to the defined name.
    by_address = {}
    for fn in defs_global:
        if fn in addresses:
            by_address.setdefault(addresses[fn] & ~1, fn)
    aliases = {}
    for fn, address in addresses.items():
        target = by_address.get(address & ~1)
        if target and fn != target and fn not in defs_global:
            aliases[fn] = target
    # Pass 1 finds functions that read incoming registers; pass 2 makes their
    # direct callers publish r0..r3 (see abi.rewrite_module).
    readers = set()
    for index, (name, game) in enumerate(units):
        text = apply_aliases(texts[name], aliases) if aliases else texts[name]
        readers |= abi.rewrite_module(text, defs_global, addresses, index, game, {})[4]
    synthetic = {}
    table = []  # (address, adapter symbol)
    rewritten = {}
    adapter_failures = []
    padded_report = []
    unknown_report = []
    for index, (name, game) in enumerate(units):
        text = texts[name]
        if aliases:
            text = apply_aliases(text, aliases)
        for fn in replaced.get(name, ()):
            text = re.sub(r'^(define [^\n]*?)@' + re.escape(fn) + r'\(',
                          lambda m: m[1] + '@' + fn + '.__replaced_by_port(', text, flags=re.M)
            if not re.search(r'^declare [^\n]*@' + re.escape(fn) + r'\(', text, re.M):
                text += abi.render_declare(fn, defs_global[fn]) + '\n'
        text, values, internals, padded, _, unknown = abi.rewrite_module(
            text, defs_global, addresses, index, game, synthetic, readers)
        for callee, given, wanted in padded:
            padded_report.append(f'{name}: {callee} called with {given} of {wanted} arguments')
        for caller, slot in unknown:
            unknown_report.append(f'{name}: {caller} passes an unknown r{slot}')
        extra = []
        defs, _decls, _ = abi.scan_module(text)
        for fn, sig in defs.items():
            if fn.endswith('.__replaced_by_port') or fn not in addresses or owner.get(fn) != name:
                continue
            symbol = '__icall.' + fn
            code = abi.adapter(fn, sig, symbol)
            if code is None:
                adapter_failures.append(fn)
                continue
            extra.append(code)
            table.append((addresses[fn] & ~1, symbol))
        for fn in internals:
            key = (index, fn)
            if key in synthetic:
                symbol = f'__icall.{index}.{fn}'
                code = abi.adapter(fn, internals[fn], symbol)
                if code is None:
                    adapter_failures.append(fn)
                    continue
                extra.append(code)
                table.append((synthetic[key], symbol))
        if args.trace and name.startswith('src/'):
            text = add_trace(text, addresses)
        if name.startswith('src/'):
            text = add_hooks(text, addresses, owner, name)
        if args.watch and name.startswith('src/'):
            text = add_watch(text, set(args.watch.split(',')), addresses)
        text = text + ''.join(extra)
        if '@port_regs' in text and not re.search(r'^@port_regs\b', text, re.M):
            text += abi.PORT_REGS_DECL + '\n'
        rewritten[name] = text
    (OUT / 'padded_calls.txt').write_text('\n'.join(padded_report) + '\n')
    (OUT / 'unknown_registers.txt').write_text('\n'.join(unknown_report) + '\n')
    print(f'register-passing calls: {len(padded_report)}, {len(readers)} functions read incoming '
          f'registers, {len(unknown_report)} arguments unknown (unknown_registers.txt)')
    if adapter_failures:
        print('no adapter (aggregate signature):', ', '.join(sorted(adapter_failures)[:20]))
    table.sort()
    lines = ['target triple = "wasm32"']
    lines += [f'declare hidden i64 @"{s}"(' + ', '.join(['i32'] * abi.CANON_SLOTS) + ')' for _, s in table]
    entries = ', '.join(f'{{ i32, ptr }} {{ i32 {a}, ptr @"{s}" }}' for a, s in table)
    lines.append(f'@port_code_table = hidden constant [{len(table)} x {{ i32, ptr }}] [{entries}]')
    lines.append(f'@port_code_count = hidden constant i32 {len(table)}')
    rewritten['port/game/code_table'] = '\n'.join(lines) + '\n'
    print(f'stage 2 (ABI pass): {len(table)} code addresses, '
          f'{sum(len(v) for v in replaced.values())} game functions replaced by port code, '
          f'{time.monotonic() - t0:.0f}s', flush=True)

    def stage3(name):
        text = rewritten[name]
        ll = OUT / 'abi' / (name + '.ll')
        obj = ll.with_suffix('.o')
        key = hashlib.sha256(text.encode()).hexdigest()
        stamp = ll.with_suffix('.key')
        if not args.force and obj.exists() and stamp.exists() and stamp.read_text() == key:
            return name, None
        ll.parent.mkdir(parents=True, exist_ok=True)
        ll.write_text(text)
        r = run([args.cc, '--target=wasm32', '-O2', *(['-fno-inline'] if args.trace else []),
                 '-Wno-override-module', '-c', str(ll), '-o', str(obj)])
        if r.returncode:
            return name, r.stderr[-3000:]
        stamp.write_text(key)
        return name, None

    t0 = time.monotonic()
    failures = []
    with concurrent.futures.ThreadPoolExecutor(args.jobs) as pool:
        for name, err in pool.map(stage3, list(rewritten)):
            if err:
                failures.append(name)
                print('FAIL', name, '\n', err, flush=True)
                if len(failures) > 10:
                    break
    if failures:
        sys.exit(f'{len(failures)} rewritten units failed to compile')
    print(f'stage 3 (IR -> wasm objects): {time.monotonic() - t0:.0f}s', flush=True)

    objects = [str(OUT / 'abi' / (n + '.o')) for n in rewritten]
    cmd = ['--no-entry', '--gc-sections', '--export-memory',
           '--initial-memory=167772160', '--max-memory=167772160',
           '-z', 'stack-size=4194304', '--stack-first', '--error-limit=0',
           *[f'--export={e}' for e in EXPORTS], *objects, '-o', str(OUT / 'game.wasm')]
    rsp = OUT / 'link.rsp'
    rsp.write_text('\n'.join(cmd) + '\n')
    r = run([find_linker(), '@' + str(rsp)])
    (OUT / 'link.log').write_text(r.stdout + r.stderr)
    undefined = sorted(set(re.findall(r'undefined symbol: (\S+)', r.stderr)))
    mismatches = sorted(set(re.findall(r'function signature mismatch: (\S+)', r.stderr)))
    print(f'link: {"OK" if r.returncode == 0 else "FAILED"}; {len(undefined)} undefined, '
          f'{len(mismatches)} signature mismatches (see {OUT / "link.log"})')
    if undefined:
        print('  undefined:', ' '.join(undefined[:40]))
    if mismatches:
        print('  mismatched:', ' '.join(mismatches[:40]))
    if r.returncode == 0:
        imports = check_imports(OUT / 'game.wasm')
        print('imports:', ', '.join(imports))
        print(f'{OUT / "game.wasm"}: {(OUT / "game.wasm").stat().st_size // 1024} KiB')
    return r.returncode


_HOOK_ARG = {'ptr': 'ptrtoint ptr {v} to i32', 'i8': 'zext i8 {v} to i32', 'i16': 'zext i16 {v} to i32',
             'i1': 'zext i1 {v} to i32', 'i64': 'trunc i64 {v} to i32', 'float': 'bitcast float {v} to i32',
             'double': 'fptosi double {v} to i32'}


def add_hooks(text, addresses, owner, unit):
    """port_hook_enter(address, r0..r3) at entry, port_hook_exit(address, r0)
    before every return, in each function with a GBA address (port/game/hooks.c)."""
    out = []
    current = None
    n = 0

    def as_i32(kind, value):
        nonlocal n
        if kind == 'i32':
            return value
        if kind not in _HOOK_ARG:
            return '0'
        n += 1
        out.append(f'  %__hook{n} = ' + _HOOK_ARG[kind].format(v=value))
        return f'%__hook{n}'

    for line in text.splitlines():
        if line.startswith('define '):
            out.append(line)
            m = re.match(r'define [^@]*@([-\w.$]+)\((.*)\)[^(]*\{$', line)
            current = None
            if m and m[1] in addresses and owner.get(m[1]) == unit and ' internal ' not in line.split('@')[0] + ' ':
                current = addresses[m[1]] & ~1
                values = []
                for param in (abi.split_top(m[2]) if m[2].strip() else []):
                    words = param.split()
                    if words and words[-1].startswith('%'):
                        values.append(as_i32(words[0], words[-1]))
                values = (values + ['0'] * 4)[:4]
                out.append(f'  call void @port_hook_enter(i32 {current}, ' +
                           ', '.join(f'i32 {v}' for v in values) + ')')
            continue
        if line == '}':
            current = None
        elif current is not None:
            m = re.match(r'\s+ret (void|\S+) ?(.*)$', line)
            if m:
                value = '0' if m[1] == 'void' else as_i32(m[1], m[2].strip())
                out.append(f'  call void @port_hook_exit(i32 {current}, i32 {value})')
        out.append(line)
    return ('\n'.join(out) + '\ndeclare void @port_hook_enter(i32, i32, i32, i32, i32)\n'
            'declare void @port_hook_exit(i32, i32)\n')


def add_watch(text, names, addresses):
    """Call port_watch(address, a0, a1, a2) at the entry of each named function."""
    out = []
    used = False
    for line in text.splitlines():
        out.append(line)
        m = re.match(r'define [^@]*@([-\w.$]+)\((.*)\)[^(]*\{$', line)
        if not m or m[1] not in names:
            continue
        values = []
        for param in abi.split_top(m[2]) if m[2].strip() else []:
            words = param.split()
            kind, reg = words[0], words[-1]
            if kind == 'ptr':
                out.append(f'  %__watch{len(values)} = ptrtoint ptr {reg} to i32')
                values.append(f'%__watch{len(values)}')
            elif kind in ('i8', 'i16'):
                out.append(f'  %__watch{len(values)} = zext {kind} {reg} to i32')
                values.append(f'%__watch{len(values)}')
            elif kind == 'i32':
                values.append(reg)
        values = (values + ['0', '0', '0'])[:3]
        address = addresses.get(m[1], 0) & ~1
        out.append(f'  call void @port_watch(i32 {address}, ' + ', '.join(f'i32 {v}' for v in values) + ')')
        used = True
    return '\n'.join(out) + ('\ndeclare void @port_watch(i32, i32, i32, i32)\n' if used else '\n')


def add_trace(text, addresses):
    out = []
    for line in text.splitlines():
        out.append(line)
        m = re.match(r'define [^@]*@([-\w.$]+)\(', line)
        if m and m[1] in addresses and 'internal' not in line.split('@')[0]:
            out.append(f'  call void @port_trace(i32 {addresses[m[1]] & ~1})')
    return '\n'.join(out) + '\ndeclare void @port_trace(i32)\n'


def apply_aliases(text, aliases):
    names = set(re.findall(r'^declare [^\n]*?@([-\w.$]+)\(', text, re.M)) & set(aliases)
    if not names:
        return text
    defined = set(re.findall(r'^declare [^\n]*?@([-\w.$]+)\(', text, re.M))
    out = []
    for line in text.splitlines():
        if line.startswith('declare '):
            m = re.search(r'@([-\w.$]+)\(', line)
            if m and m[1] in names:
                if aliases[m[1]] in defined:
                    continue  # already declared under its defined name
                defined.add(aliases[m[1]])
        out.append(re.sub(r'@([-\w.$]+)(?![-\w.$])', lambda m: '@' + aliases[m[1]] if m[1] in names else m[0], line))
    return '\n'.join(out) + '\n'


def check_imports(path):
    data = path.read_bytes()
    names = []
    i = 8
    def leb(i):
        result = shift = 0
        while True:
            b = data[i]; i += 1
            result |= (b & 0x7f) << shift; shift += 7
            if not b & 0x80:
                return result, i
    while i < len(data):
        sid = data[i]; size, j = leb(i + 1)
        if sid == 2:
            count, k = leb(j)
            for _ in range(count):
                n, k = leb(k); mod = data[k:k + n].decode(); k += n
                n, k = leb(k); field = data[k:k + n].decode(); k += n
                kind = data[k]; k += 1
                if kind == 0:
                    _, k = leb(k)
                else:
                    raise RuntimeError('unexpected import kind')
                names.append(mod + '.' + field)
            break
        i = j + size
    return names


if __name__ == '__main__':
    sys.exit(main())

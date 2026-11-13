"""Compile a C snippet into a wasm side module that runs inside the live game.

The snippet is compiled like a game unit (the prepared game headers, agbcc
struct layout, globals bound to their GBA addresses, volatile MMIO through the
port bus). Every call to a game function becomes port_icall(GBA address, ...),
the path ROM function pointers take, so a snippet can call any function with a
GBA address by name, declared or not. It imports the game's memory and keeps
its own data and stack at SNIPPET_BASE (free GBA address space).

    from snippet import compile_snippet
    wasm_bytes = compile_snippet('print("%d\\n", gMain.state);')
"""
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'port/core'))
sys.path.insert(0, str(ROOT / 'port'))
from addresses import read_addresses, bind_ir  # noqa: E402
from mmio import instrument_mmio  # noqa: E402
from link_slice import find_linker  # noqa: E402
import abi  # noqa: E402

OUT = ROOT / 'build/port/agent'
GAME = ROOT / 'build/port/game'
LLVM = Path('/opt/homebrew/opt/llvm/bin')
CC = str(LLVM / 'clang') if (LLVM / 'clang').exists() else 'clang'
SNIPPET_BASE = 0x02800000
FLAGS = ['--target=wasm32', '-ffreestanding', '-std=gnu89', '-O1', '-funsigned-char',
         '-fno-strict-aliasing', '-fwrapv', '-fno-builtin', '-Wno-everything',
         '-Wimplicit-function-declaration', '-Iport/agent', '-Iport/include', '-Iport/core',
         '-I' + str(GAME / 'include'), '-iquote', '.', '-include', 'port/core/compat.h']


class SnippetError(Exception):
    pass


def symbols():
    """{'code': name -> code word, 'data': name -> address}, cached per ELF."""
    elf = ROOT / 'build/boktai3.elf'
    cache = OUT / 'symbols.json'
    stamp = f'{elf.stat().st_mtime_ns}:{(ROOT / "symbols/ram.csv").stat().st_mtime_ns}'
    if cache.exists():
        data = json.loads(cache.read_text())
        if data.get('stamp') == stamp:
            return data
    sys.path.insert(0, str(ROOT / 'port'))
    import build_game
    data = {'stamp': stamp, 'code': build_game.code_addresses(), 'data': read_addresses(ROOT)}
    OUT.mkdir(parents=True, exist_ok=True)
    cache.write_text(json.dumps(data))
    return data


def wrap(code, includes=()):
    head = '#include "global.h"\n#include "agent.h"\n'
    head += ''.join(f'#include "{h}"\n' for h in includes)
    if re.search(r'\bsnippet_main\s*\(', code):
        return head + code + '\n'
    # Statements: top-level declarations may precede them on lines ending a
    # declaration block marked by '%%' (rarely needed: block-scope externs work).
    if '\n%%\n' in code:
        decls, body = code.split('\n%%\n', 1)
    else:
        decls, body = '', code
    return (head + decls + '\nunsigned snippet_main(void)\n{\n' + body +
            '\n;\n    return agent_result;\n}\n')


def _runtime(linker):
    obj = OUT / 'snippet_rt.o'
    src = ROOT / 'port/agent/snippet_rt.c'
    if not obj.exists() or obj.stat().st_mtime < src.stat().st_mtime:
        OUT.mkdir(parents=True, exist_ok=True)
        subprocess.run([CC, '--target=wasm32', '-ffreestanding', '-O2', '-c', str(src), '-o', str(obj)],
                       check=True)
    return obj


def compile_snippet(code, includes=()):
    if not (GAME / 'include').is_dir():
        raise SnippetError('build the game first: python3 port/build_game.py')
    syms = symbols()
    code_words = syms['code']
    source = wrap(code, includes)
    key = hashlib.sha256((source + json.dumps(FLAGS)).encode()).hexdigest()[:16]
    work = OUT / 'snippets'
    work.mkdir(parents=True, exist_ok=True)
    wasm = work / (key + '.wasm')
    if wasm.exists():
        return wasm.read_bytes()
    c = work / (key + '.c')
    c.write_text(source)
    ll = work / (key + '.ll')
    r = subprocess.run([CC, *FLAGS, '-S', '-emit-llvm', str(c), '-o', str(ll)],
                       cwd=ROOT, text=True, capture_output=True)
    if r.returncode:
        raise SnippetError(_clean(r.stderr, c))
    text, _, unresolved = bind_ir(ll.read_text(), syms['data'])
    text, _ = instrument_mmio(text)
    defs, decls, internals = abi.scan_module(text)
    missing = [n for n in unresolved if n not in ('agent_result',)]
    if missing:
        raise SnippetError('unknown data symbols: ' + ', '.join(missing))
    # Game functions: every reference becomes its GBA code address.
    runtime = {'print', 'dump', 'call_address', 'agent_print', 'port_icall'}
    external = [n for n in decls if n not in defs and n not in runtime and not n.startswith('llvm.')
                and not n.startswith('port_bus_')]
    unknown = [n for n in external if n not in code_words]
    if unknown:
        raise SnippetError('unknown functions: ' + ', '.join(unknown))
    for name in external:
        word = code_words[name]
        text = re.sub(r'^declare [^\n]*@' + re.escape(name) + r'\([^\n]*\n', '', text, flags=re.M)
        text = re.sub(r'@' + re.escape(name) + r'(?![-\w.$])', f'inttoptr (i32 {word} to ptr)', text)
    text = _route_indirect(text)
    if '@port_icall' in text and not re.search(r'^declare [^\n]*@port_icall\(', text, re.M):
        text += 'declare i64 @port_icall(ptr, ' + ', '.join(['i32'] * abi.CANON_SLOTS) + ')\n'
    ll.write_text(text)
    obj = work / (key + '.o')
    r = subprocess.run([CC, '--target=wasm32', '-O1', '-Wno-override-module', '-c', str(ll), '-o', str(obj)],
                       text=True, capture_output=True)
    if r.returncode:
        raise SnippetError('IR compile failed:\n' + r.stderr[-2000:])
    linker = find_linker()
    r = subprocess.run([linker, '--no-entry', '--import-memory', '--allow-undefined',
                        '--initial-memory=167772160', '--max-memory=167772160',
                        f'--global-base={SNIPPET_BASE}', '-z', 'stack-size=262144',
                        '--export=snippet_main', '--export=agent_result',
                        str(obj), str(_runtime(linker)), '-o', str(wasm)],
                       text=True, capture_output=True)
    if r.returncode:
        raise SnippetError('link failed:\n' + r.stderr[-2000:])
    return wasm.read_bytes()


def _route_indirect(text):
    """Every call through a pointer (game addresses included) goes to port_icall."""
    fresh = abi.Namer()
    out = []
    for line in text.splitlines():
        call = abi.parse_call(line) if ' call ' in line else None
        if call and not call['callee'].startswith('@'):
            out += abi.adapt_indirect(call, fresh)
        else:
            out.append(line)
    return '\n'.join(out) + '\n'


def _clean(stderr, path):
    lines = []
    for line in stderr.splitlines():
        if 'error' in line or 'warning: call to undeclared' in line or lines and line.startswith(' '):
            lines.append(line.replace(str(path), 'snippet.c'))
    return '\n'.join(lines[:30]) or stderr[-2000:]


if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser(description='compile a C snippet (from the argument or stdin)')
    ap.add_argument('code', nargs='?')
    ap.add_argument('-i', '--include', action='append', default=[])
    ap.add_argument('--path', action='store_true', help='print the cached .wasm path (gameapi g.c uses this)')
    a = ap.parse_args()
    code = a.code if a.code is not None else sys.stdin.read()
    try:
        data = compile_snippet(code, a.include)
    except SnippetError as e:
        sys.exit(str(e))
    if a.path:
        source = wrap(code, a.include)
        print(OUT / 'snippets' / (hashlib.sha256((source + json.dumps(FLAGS)).encode()).hexdigest()[:16] + '.wasm'))
    else:
        print(len(data), 'bytes')

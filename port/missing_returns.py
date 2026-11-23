#!/usr/bin/env python3
"""List non-void game functions that fall off the end (clang -Wreturn-type over the
port's prepared sources), and the direct callers that use the value.

On the GBA the caller reads whatever r0 held; compiled for wasm the value is
undefined. Fix each like a short call (port/README.md, "Undefined behavior the
GBA forgives"): an explicit return when that still matches, else UBFIX.

    python3 port/build_game.py            # prepares build/port/game/prepared
    python3 port/missing_returns.py       # -> build/port/game/missing_returns.txt

Callers reached through function pointers (task callbacks, script commands, whose
result stops a script when it is 1) are not found by the caller scan.
"""
import concurrent.futures as cf
import os
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GAME = ROOT / 'build/port/game'
CLANG = '/opt/homebrew/opt/llvm/bin/clang'
FLAGS = ['--target=wasm32', '-ffreestanding', '-std=gnu89', '-funsigned-char', '-fsyntax-only',
         '-Wno-everything', '-Wreturn-type', '-DUBFIX', '-Iport/include', '-Iport/core', '-Iport/game',
         '-Iport/hw', '-I' + str(GAME / 'include'), '-iquote', '.', '-include', 'port/core/compat.h']


def scan(path):
    import subprocess
    r = subprocess.run([CLANG if os.path.exists(CLANG) else 'clang'] + FLAGS + [str(path)],
                       capture_output=True, text=True, cwd=ROOT)
    out = []
    lines = path.read_text().split('\n')
    for m in re.finditer(r'^\S+?:(\d+):\d+: warning: non-void function does not return a value( in all control paths)?',
                         r.stderr, re.M):
        ln = int(m[1])
        name = None
        for i in range(ln - 1, -1, -1):
            d = re.match(r'^[A-Za-z_].*?\b([A-Za-z_]\w*)\s*\(', lines[i])
            if d and not lines[i].rstrip().endswith(';') and not lines[i].startswith(('if', 'while', 'for', 'return')):
                name = d[1]
                break
        out.append((str(path.relative_to(GAME / 'prepared')), name, 'some paths' if m[2] else 'never'))
    return out


def main():
    files = sorted((GAME / 'prepared/src').rglob('*.c'))
    found = []
    with cf.ThreadPoolExecutor(8) as ex:
        for r in ex.map(scan, files):
            found += r
    sources = {p: p.read_text() for p in (ROOT / 'src').rglob('*.c')}
    lines = []
    for f, name, kind in found:
        uses = []
        if name:
            pat = re.compile(r'([=!<>(,&|?:]|return)\s*(\([^()]*\)\s*)?\b' + re.escape(name) + r'\s*\(')
            for p, text in sources.items():
                if name not in text:
                    continue
                for m in pat.finditer(text):
                    line = text.count('\n', 0, m.start()) + 1
                    uses.append(f'{p.relative_to(ROOT)}:{line}')
        lines.append(f'{f}: {name} ({kind}); value used at {len(uses)} direct sites' +
                     ''.join(f'\n    {u}' for u in uses[:8]))
    (GAME / 'missing_returns.txt').write_text('\n'.join(lines) + '\n')
    print(f'{len(found)} functions fall off the end; {sum(1 for x in lines if "used at 0 " not in x)} '
          f'have direct callers using the value -> {GAME / "missing_returns.txt"}')


if __name__ == '__main__':
    main()

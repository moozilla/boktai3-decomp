#!/usr/bin/env python3
"""Prove a discarded-return declaration failure, then run the original C call chain."""
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'port/core'))
from prepare import prepare_headers, prepare_source
from link_slice import find_linker


def main():
    cc = '/opt/homebrew/opt/llvm/bin/clang'
    out = ROOT / 'build/port/returns-slice'
    out.mkdir(parents=True, exist_ok=True)
    headers = out / 'include'
    prepare_headers(ROOT, headers, 'wasm32')
    flags = ['-O2', '-std=gnu89', '-Wno-everything', '-I' + str(headers),
             '-include', str(ROOT / 'port/core/compat.h')]
    sources = ['src/fn/sub_08163E68.c', 'src/fn/sub_0821A0C0.c', 'port/tests/returns_runtime.c']
    for target in ('host', 'wasm32'):
        arch = ['--target=wasm32', '-ffreestanding'] if target == 'wasm32' else []
        objects = []
        for source in sources:
            prepared = out / (target + '-' + Path(source).name)
            prepared.write_text(prepare_source(ROOT / source, target))
            obj = prepared.with_suffix('.o')
            subprocess.run([cc, *arch, *flags, '-c', str(prepared), '-o', str(obj)], check=True)
            objects.append(str(obj))
        if target == 'host':
            binary = out / 'returns-test'
            subprocess.run([cc, *objects, '-o', str(binary)], check=True)
            subprocess.run([str(binary)], check=True)
        else:
            link = [find_linker(), '--no-entry', '--fatal-warnings', '--export=port_test_discarded_return']
            wasm = out / 'returns.wasm'
            # Reproduce the actual mismatch with the original caller declaration.
            baseline = out / 'baseline.c'
            baseline.write_text((ROOT / sources[0]).read_text())
            baseline_obj = out / 'baseline.o'
            subprocess.run([cc, *arch, *flags, '-c', str(baseline), '-o', str(baseline_obj)], check=True)
            result = subprocess.run([*link, str(baseline_obj), *objects[1:], '-o', str(out / 'baseline.wasm')],
                                    text=True, capture_output=True)
            assert result.returncode and 'function signature mismatch: Task_Kill' in result.stderr
            (out / 'baseline-link.log').write_text(result.stderr)
            subprocess.run([*link, *objects, '-o', str(wasm)], check=True)
            subprocess.run(['node', '-e', '''
const fs = require('fs');
(async () => {
 const m = await WebAssembly.compile(fs.readFileSync(process.argv[1]));
 if (WebAssembly.Module.imports(m).length) throw Error('unexpected imports');
 const e = (await WebAssembly.instantiate(m, {})).exports;
 if (e.port_test_discarded_return()) throw Error('Task_Kill flag mismatch');
 console.log('wasm: original ParticlePool_Destroy -> Task_Kill PASS; baseline signature failure reproduced');
})().catch(e => { console.error(e); process.exitCode = 1; });
''', str(wasm)], check=True)


if __name__ == '__main__': main()

#!/usr/bin/env python3
"""Verify old ARM mixed-word double adaptations using the original math C."""
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'port/core'))
from prepare import prepare_headers, prepare_source
from addresses import read_addresses, bind_ir
from link_slice import find_linker


def main():
    os.chdir(ROOT)
    cc = os.environ.get('PORT_CC', '/opt/homebrew/opt/llvm/bin/clang')
    addresses = read_addresses(ROOT)
    units = ['src/lib/libgcc_dp.c', 'src/lib/libgcc_fp.c', 'src/lib/libm_k_sin.c', 'src/lib/libm_e_sqrt.c', 'port/tests/math_test.c']
    for target in ('host', 'wasm32'):
        out = ROOT / 'build/port/math' / target
        out.mkdir(parents=True, exist_ok=True)
        headers = out / 'include'
        prepare_headers(ROOT, headers, target)
        arch = ['--target=wasm32', '-ffreestanding'] if target == 'wasm32' else []
        objects = []
        for name in units:
            source = out / Path(name).name
            source.write_text(prepare_source(ROOT / name, target))
            ir = source.with_suffix('.ll')
            subprocess.run([cc, *arch, '-std=gnu89', '-O2', '-Wno-everything', '-fno-builtin',
                            '-I' + str(headers), '-include', 'port/core/compat.h',
                            '-S', '-emit-llvm', str(source), '-o', str(ir)], check=True)
            bound, _, _ = bind_ir(ir.read_text(), addresses)
            ir.write_text(bound)
            obj = source.with_suffix('.o')
            subprocess.run([cc, *arch, '-O2', '-c', str(ir), '-o', str(obj)], check=True)
            objects.append(str(obj))
        if target == 'wasm32':
            wasm = out / 'math.wasm'
            subprocess.run([find_linker(), '--no-entry', '--export=math_test_run', *objects, '-o', str(wasm)], check=True)
            subprocess.run(['node', '-e', 'const fs=require("fs");WebAssembly.instantiate(fs.readFileSync(process.argv[1]),{}).then(({instance})=>{const r=instance.exports.math_test_run();if(r)throw Error("math case "+r);console.log("wasm math: 8 cases PASS")})', str(wasm)], check=True)
        else:
            binary = out / 'math_test'
            subprocess.run([cc, *objects, '-o', str(binary)], check=True)
            subprocess.run([str(binary)], check=True)
            print('host math: 8 cases PASS', flush=True)


if __name__ == '__main__':
    main()

#!/usr/bin/env python3
"""Check that the ABI pass no longer guesses leftover registers.

A call passing fewer arguments than the callee takes read leftover registers on
the GBA (Time_LoadRegionList once called Script_ReadValue() with r0 still
holding Script_GetPc's result). The source now states such values (plainly, or
under UBFIX: port/README.md); a call still short is padded with zero and
reported, and port/build_game.py refuses to link the game.
"""
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'port/core'))
sys.path.insert(0, str(ROOT))
import abi
from port.hw.build import find_wasm_ld

IR = '''define void @Loader(i32 %0) {
  %2 = call ptr @Script_GetPc()
  %3 = call i32 @Script_ReadValue()
  ret void
}
declare ptr @Script_GetPc()
declare i32 @Script_ReadValue(...)
'''

sig = abi.Sig
defs = {
    'Script_GetPc': sig('ptr', []),
    'Script_ReadValue': sig('i32', [('ptr', '')]),
    'Loader': sig('void', [('i32', '')]),
}
text, _values, _internals, padded, readers, unknown = abi.rewrite_module(IR, defs, {}, 0, False, {})
assert padded == [('Script_ReadValue', 0, 1)], padded
assert not readers and not unknown, (readers, unknown)
assert '@Script_ReadValue(ptr null)' in text.replace('noundef ', ''), text
assert '@port_regs' not in text, text

# Model a rewritten call to a glyph selector and an indirect signed argument.
# Compile separately so optimization does not inline away the ABI boundary.
callee = '''define i32 @glyph(i16 zeroext %index) {
  %w = zext i16 %index to i32
  ret i32 %w
}
define i32 @offset(i8 signext %value) {
  %w = sext i8 %value to i32
  ret i32 %w
}
'''
caller = '''declare i32 @glyph(i32)
define i32 @select_glyph(i32 %index) {
  %r = call i32 @glyph(i32 %index)
  ret i32 %r
}
'''
functions, _, _ = abi.scan_module(callee)
direct, *_ = abi.rewrite_module(caller, functions, {}, 0, False, {})
assert 'call i32 @glyph(i16 zeroext ' in direct, direct
thunk = abi.adapter('offset', functions['offset'], 'select_offset')
assert 'call i32 @offset(i8 signext ' in thunk, thunk

llvm = Path('/opt/homebrew/opt/llvm/bin/clang')  # as port/build_game.py
cc = str(llvm) if llvm.exists() else shutil.which('clang')
if cc:
    with tempfile.TemporaryDirectory() as tmp_dir:
        tmp = Path(tmp_dir)
        for name, ir in {'abi': text, 'callee': callee,
                         'caller': direct + 'declare i32 @offset(i8 signext)\n' + thunk}.items():
            ll = tmp / f'{name}.ll'
            ll.write_text(ir)
            subprocess.run([cc, '--target=wasm32', '-Wno-override-module', '-O2',
                            '-c', ll, '-o', ll.with_suffix('.o')], check=True)
        wasm = tmp / 'abi.wasm'
        subprocess.run([find_wasm_ld(), '--no-entry', '--export=select_glyph',
                        '--export=select_offset', tmp / 'callee.o', tmp / 'caller.o',
                        '-o', wasm], check=True)
        subprocess.run(['node', '-e', '''
const assert = require('node:assert/strict');
const bytes = require('node:fs').readFileSync(process.argv[1]);
const ex = new WebAssembly.Instance(new WebAssembly.Module(bytes)).exports;
assert.equal(ex.select_glyph(0x10004), 4);
assert.equal(ex.select_offset(0x1fe, ...Array(11).fill(0)), 0xfffffffen);
''', wasm], check=True)
print('ABI pass: a short call is padded with zero and reported, no register guessing')
print('ABI pass: narrow unsigned and signed arguments preserve the defining signature')

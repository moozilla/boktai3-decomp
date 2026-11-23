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
import abi

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
llvm = Path('/opt/homebrew/opt/llvm/bin/clang')  # as port/build_game.py
cc = str(llvm) if llvm.exists() else shutil.which('clang')
if cc:
    with tempfile.TemporaryDirectory() as tmp:
        ll = Path(tmp) / 'abi.ll'
        ll.write_text(text)
        r = subprocess.run([cc, '--target=wasm32', '-Wno-override-module', '-c', str(ll), '-o', str(ll.with_suffix('.o'))],
                           capture_output=True, text=True)
        assert r.returncode == 0, r.stderr
print('ABI pass: a short call is padded with zero and reported, no register guessing')

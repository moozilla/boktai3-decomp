#!/usr/bin/env python3
"""Check the ABI pass's r0 tracking for calls that pass fewer arguments.

Time_LoadRegionList calls Script_ReadValue() with no argument inside
`for (...; k < count && Script_GetPc(); ...)`: on the GBA r0 still holds
Script_GetPc's result. The loop body's only predecessor is the merge block of
the `&&`, whose r0 is mixed (the short-circuit edge made no call), but the edge
taken on true comes only from the Script_GetPc block.
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
  br label %2

2:                                                ; preds = %9, %1
  %3 = icmp slt i32 %0, 4
  br i1 %3, label %4, label %6

4:                                                ; preds = %2
  %5 = call ptr @Script_GetPc()
  %w = icmp ne ptr %5, null
  br label %6

6:                                                ; preds = %4, %2
  %7 = phi i1 [ false, %2 ], [ %w, %4 ]
  br i1 %7, label %9, label %8

8:                                                ; preds = %6
  %u = call i32 @Script_ReadValue()
  ret void

9:                                                ; preds = %6
  %10 = call i32 @Script_ReadValue()
  br label %2
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
text, _values, _internals, padded, _readers, unknown = abi.rewrite_module(IR, defs, {}, 0, False, {})
merge = text[text.index('\n6:'):text.index('\n8:')]
assert 'phi ptr [ %5, %4 ], [ poison, %2 ]' in merge, merge
body = text[text.index('\n9:'):]
assert '@Script_ReadValue(ptr %__abi.' in body.replace('noundef ', ''), body
# On the false edge (block 8) r0 may be Script_GetPc's or the loop's: unknown.
assert unknown == [('Loader', 0)], unknown
assert len(padded) == 2, padded
# The result must be valid IR (the value used in block 9 has to dominate it).
llvm = Path('/opt/homebrew/opt/llvm/bin/clang')  # as port/build_game.py
cc = str(llvm) if llvm.exists() else shutil.which('clang')
if cc:
    with tempfile.TemporaryDirectory() as tmp:
        ll = Path(tmp) / 'abi.ll'
        ll.write_text(text)
        r = subprocess.run([cc, '--target=wasm32', '-Wno-override-module', '-c', str(ll), '-o', str(ll.with_suffix('.o'))],
                           capture_output=True, text=True)
        assert r.returncode == 0, r.stderr
print('ABI pass: r0 follows the true edge of `a && f()` into the loop body; the false edge stays unknown')

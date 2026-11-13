#!/usr/bin/env python3
"""Check signature inventory against LLVM attributes containing parentheses."""
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'core'))
from signatures import signature, split_parameters

name, sig = signature('define dso_local i32 @sample(ptr captures(none) %p, i32 noundef %n) #0 {')
assert name == 'sample' and sig == {'return': 'i32', 'arguments': ['i32', 'i32']}
name, sig = signature('declare void @other(ptr noundef byval(%struct.Pair) align 4, double, ...)')
assert sig == {'return': 'void', 'arguments': ['i32', 'f64', 'varargs']}
assert signature('declare void @broken(ptr captures(none)') is None
assert split_parameters('ptr byval({i32, i32}), i32') == ['ptr byval({i32, i32})', 'i32']
print('LLVM signature parser: nested parameter attributes and varargs PASS')

#!/usr/bin/env python3
"""Inventory real wasm ABI conflicts using compiled definitions as the baseline."""
import argparse
import collections
import json
from pathlib import Path
import re


def split_parameters(text):
    result, start, depth = [], 0, 0
    for i, char in enumerate(text):
        if char in '([{<': depth += 1
        elif char in ')]}>': depth -= 1
        elif char == ',' and not depth:
            result.append(text[start:i].strip())
            start = i + 1
    if text[start:].strip(): result.append(text[start:].strip())
    return result


def abi_type(text):
    match = re.match(r'(ptr|void|i\d+|float|double|\.\.\.)\b', text.strip())
    if not match: return 'unmodeled:' + text.strip().split()[0]
    kind = match[1]
    if kind == 'ptr' or kind in {'i1', 'i8', 'i16', 'i32'}: return 'i32'
    if kind == 'float': return 'f32'
    if kind == 'double': return 'f64'
    return kind


def signature(line):
    match = re.match(r'(?:define|declare)\s+(.*?)@([\w.$]+)\((.*?)\)', line)
    if not match: return None
    before, name, args = match.groups()
    ret = before.strip().split()[-1]
    params = [abi_type(p) for p in split_parameters(args)]
    return name, {'return': abi_type(ret), 'arguments': params}


def inventory(directory):
    files = sorted(Path(directory).rglob('*.c.ll'))
    definitions = {}
    declarations = []
    for path in files:
        for line in path.read_text().splitlines():
            if not (line.startswith('define ') or line.startswith('declare ')): continue
            parsed = signature(line)
            if not parsed: continue
            name, sig = parsed
            if name.startswith('llvm.') or 'unmodeled' in str(sig): continue
            if line.startswith('define ') and not re.search(r'\b(?:internal|private)\b', line.split('@')[0]):
                definitions[name] = {'signature': sig, 'source': str(path)}
            elif line.startswith('declare '):
                declarations.append((name, sig, str(path)))
    conflicts = []
    counts = collections.Counter()
    for name, declared, path in declarations:
        actual = definitions.get(name)
        if not actual or actual['signature'] == declared: continue
        expected = actual['signature']
        if len(declared['arguments']) != len(expected['arguments']): kind = 'argument_count'
        elif declared['arguments'] != expected['arguments']: kind = 'argument_type'
        else: kind = 'return_type'
        counts[kind] += 1
        conflicts.append({'function': name, 'kind': kind, 'declaration': declared,
                          'definition': expected, 'caller': path, 'callee': actual['source']})
    return {'files': len(files), 'defined_functions': len(definitions),
            'conflicts_by_kind': dict(counts), 'conflicts': conflicts}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory', nargs='?', default='build/port/wasm32-bound')
    parser.add_argument('--out', default='build/port/signatures.json')
    args = parser.parse_args()
    result = inventory(args.directory)
    Path(args.out).write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k != 'conflicts'}))


if __name__ == '__main__': main()

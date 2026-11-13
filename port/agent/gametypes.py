"""The game's types and globals as JSON, for typed access from JavaScript.

Each prepared game header (build/port/game/include, the layouts the port
compiles with) is compiled alone with debug info, together with one
`__typeof__(g) *__agent_var_g;` per global it declares that has a GBA address;
llvm-dwarfdump's view of each object is folded into one table:

    {"types": {"struct Player": {"k": "struct", "size": 3244, "fields": [...]},
               "u8": {"k": "int", "size": 1, "signed": false}, ...},
     "globals": {"gPlayers": {"addr": 33599012, "type": {"k": "array", "of": ..., "n": 4}}, ...},
     "functions": {"Script_Call": 136424713, ...},
     "params": {"Time_HandleRegionKeypad": "struct RegionMenu", ...}}

"params" names the struct a function's first parameter points to (from the
definitions and prototypes in src/ and include/): a task's update function
takes its own object, so it types the memory behind a task.

A type reference is a name in "types" or an inline object: {"k": "ptr", "to":
T}, {"k": "array", "of": T, "n": N}, {"k": "fn"}, an anonymous struct/union.
Struct fields: {"name", "off", "type", "bits"?, "bit"?}. Headers disagree in
places (the decomp is in progress); a tag defined twice keeps the definition
with the most fields.

    python3 port/agent/gametypes.py          # -> build/port/agent/types.json (cached)
"""
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'port/core'))
sys.path.insert(0, str(ROOT / 'port/agent'))
from snippet import CC, LLVM, OUT, GAME, symbols  # noqa: E402

DWARFDUMP = str(LLVM / 'llvm-dwarfdump') if (LLVM / 'llvm-dwarfdump').exists() else 'llvm-dwarfdump'
FLAGS = ['--target=wasm32', '-ffreestanding', '-std=gnu89', '-funsigned-char', '-fno-builtin',
         '-Wno-everything', '-Iport/include', '-Iport/core', '-I' + str(GAME / 'include'),
         '-iquote', '.', '-include', 'port/core/compat.h', '-g', '-fno-eliminate-unused-debug-types']
VERSION = '3:'  # bump when the output format changes
EXTERN = re.compile(r'^\s*extern\b([^;(]*?)\b([A-Za-z_]\w*)\s*(?:\[[^\]]*\]\s*)*;', re.M)
ENTRY = re.compile(r'^0x([0-9a-f]+):( +)(DW_TAG_\w+|NULL)')
ATTR = re.compile(r'^\s+DW_AT_(\w+)\t\((.*)\)$')


def header_globals(text, known):
    return [m[2] for m in EXTERN.finditer(text) if m[2] in known]


def dwarf_entries(obj):
    out = subprocess.run([DWARFDUMP, '--debug-info', str(obj)], text=True, capture_output=True, check=True).stdout
    entries, stack = {}, []
    for line in out.splitlines():
        m = ENTRY.match(line)
        if m:
            depth = (len(m[2]) - 1) // 2
            del stack[depth:]
            if m[3] == 'NULL':
                continue
            e = {'tag': m[3][7:], 'children': [], 'at': {}}
            entries[int(m[1], 16)] = e
            if stack:
                stack[-1]['children'].append(e)
            stack.append(e)
            continue
        m = ATTR.match(line)
        if m and stack:
            stack[-1]['at'][m[1]] = m[2]
    return entries


def _ref(v):
    m = re.match(r'0x([0-9a-f]+)', v or '')
    return int(m[1], 16) if m else None


def _num(v):
    if v is None:
        return None
    v = v.strip('"')
    return int(v, 16) if v.startswith('0x') else int(v)


def _name(v):
    m = re.match(r'"(.*)"', v or '')
    return m[1] if m else None


class Folder:
    """Turns DWARF entries of many objects into one type table."""

    def __init__(self):
        self.types = {}

    def fold(self, entries, offset, seen=None):
        if offset is None:
            return 'void'
        e = entries[offset]
        tag, at = e['tag'], e['at']
        name = _name(at.get('name'))
        if tag == 'base_type':
            enc = at.get('encoding', '')
            size = _num(at.get('byte_size'))
            if 'float' in enc:
                t = {'k': 'float', 'size': size}
            elif 'boolean' in enc:
                t = {'k': 'int', 'size': size, 'signed': False}
            else:
                t = {'k': 'int', 'size': size, 'signed': 'unsigned' not in enc}
            self.types.setdefault(name, t)
            return name
        if tag in ('const_type', 'volatile_type', 'restrict_type', 'atomic_type'):
            return self.fold(entries, _ref(at.get('type')), seen)
        if tag == 'typedef':
            to = self.fold(entries, _ref(at.get('type')), seen)
            if name and name not in self.types and name != to:
                self.types[name] = {'k': 'typedef', 'to': to}
            return name or to
        if tag == 'pointer_type':
            to = _ref(at.get('type'))
            if to is not None and entries[to]['tag'] == 'subroutine_type':
                return {'k': 'ptr', 'to': {'k': 'fn'}}
            return {'k': 'ptr', 'to': self.fold(entries, to, seen)}
        if tag == 'array_type':
            of = self.fold(entries, _ref(at.get('type')), seen)
            dims = []
            for c in e['children']:
                if c['tag'] == 'subrange_type':
                    n = _num(c['at'].get('count'))
                    if n is None and c['at'].get('upper_bound') is not None:
                        n = _num(c['at']['upper_bound']) + 1
                    dims.append(n or 0)
            for n in reversed(dims or [0]):
                of = {'k': 'array', 'of': of, 'n': n}
            return of
        if tag == 'subroutine_type':
            return {'k': 'fn'}
        if tag == 'enumeration_type':
            key = 'enum ' + name if name else None
            t = {'k': 'enum', 'size': _num(at.get('byte_size')) or 4,
                 'values': {c['at'].get('name', '').strip('"'): _num(c['at'].get('const_value'))
                            for c in e['children'] if c['tag'] == 'enumerator'}}
            under = _ref(at.get('type'))
            if under is not None:
                base = self.fold(entries, under, seen)
                t['signed'] = self.types.get(base, {}).get('signed', True)
            if not key:
                return t
            self._keep(key, t)
            return key
        if tag in ('structure_type', 'union_type'):
            kind = 'struct' if tag == 'structure_type' else 'union'
            key = f'{kind} {name}' if name else None
            if at.get('declaration') == 'true':
                return key or {'k': kind, 'size': 0, 'fields': []}
            seen = seen or set()
            if key and key in seen:
                return key  # recursion: the outer fold defines it
            if key:
                seen = seen | {key}
            fields = []
            for c in e['children']:
                if c['tag'] != 'member':
                    continue
                f = {'name': _name(c['at'].get('name')) or '',
                     'off': _num(c['at'].get('data_member_location')) or 0,
                     'type': self.fold(entries, _ref(c['at'].get('type')), seen)}
                if c['at'].get('bit_size'):
                    f['bits'] = _num(c['at']['bit_size'])
                    bit = _num(c['at'].get('data_bit_offset')) or 0
                    f['off'], f['bit'] = bit // 8 // 4 * 4, bit - bit // 8 // 4 * 32
                fields.append(f)
            t = {'k': kind, 'size': _num(at.get('byte_size')) or 0, 'fields': fields}
            if not key:
                return t
            self._keep(key, t)
            return key
        return 'void'

    def _keep(self, key, t):
        old = self.types.get(key)
        if old is None or len(t.get('fields', t.get('values', ()))) > len(old.get('fields', old.get('values', ()))):
            self.types[key] = t


def compile_header(header, names, work):
    src = work / (header.stem + '.c')
    obj = work / (header.stem + '.o')
    for _ in range(3):
        text = '#include "global.h"\n#include "%s"\n' % header.name
        text += ''.join(f'__typeof__({n}) *__agent_var_{n};\n' for n in names)
        src.write_text(text)
        r = subprocess.run([CC, *FLAGS, '-c', str(src), '-o', str(obj)], cwd=ROOT, text=True, capture_output=True)
        if r.returncode == 0:
            return obj, names
        bad = {n for n in names if re.search(r'\b%s\b' % re.escape(n), r.stderr)}
        if not bad:
            return None, []
        names = [n for n in names if n not in bad]
    return None, []


FIRST_PARAM = re.compile(r'^[A-Za-z_][\w \t*]*?\b([A-Za-z_]\w*)\(\s*(?:const\s+)?struct\s+(\w+)\s*\*', re.M)


def first_params(code, types):
    """{function: 'struct X'} for functions whose first parameter is a struct
    pointer of a known type; a definition in src/ wins over a prototype."""
    out = {}
    files = sorted(ROOT.glob('src/**/*.c')) + sorted((ROOT / 'include').glob('*.h'))
    for path in files:
        text = path.read_text(errors='replace')
        if 'struct' not in text:
            continue
        for m in FIRST_PARAM.finditer(text):
            name, tag = m[1], 'struct ' + m[2]
            if name in code and tag in types and name not in out:
                out[name] = tag
    return out


def build(force=False):
    include = GAME / 'include'
    headers = sorted(include.glob('*.h'))
    syms = symbols()
    data = syms['data']
    sources = sorted(ROOT.glob('src/**/*.c'))
    stamp = VERSION + hashlib.sha256(''.join(f'{h.name}:{h.stat().st_mtime_ns};' for h in headers + sources).encode()
                           + syms['stamp'].encode()).hexdigest()[:16]
    out = OUT / 'types.json'
    if out.exists() and not force:
        try:
            if json.loads(out.read_text()).get('stamp') == stamp:
                return out
        except ValueError:
            pass
    work = OUT / 'types'
    work.mkdir(parents=True, exist_ok=True)
    # The header that declares each global: ram.csv's own choice first, then any.
    owner = {}
    import csv
    with open(ROOT / 'symbols/ram.csv') as f:
        for row in csv.DictReader(f):
            if row['header'].startswith('include/'):
                owner[row['name']] = row['header'][len('include/'):]
    plan = {h.name: [] for h in headers}
    texts = {h.name: h.read_text(errors='replace') for h in headers}
    for h in headers:
        for n in header_globals(texts[h.name], data):
            owner.setdefault(n, h.name)
    for n, h in owner.items():
        if h in plan and n in data:
            plan[h].append(n)
    folder = Folder()
    globals_ = {}
    with ThreadPoolExecutor(os.cpu_count() or 4) as pool:
        results = list(pool.map(lambda h: (h, *compile_header(h, sorted(set(plan[h.name])), work)), headers))
    failed = []
    for h, obj, names in results:
        if obj is None:
            failed.append(h.name)
            continue
        entries = dwarf_entries(obj)
        for off, e in entries.items():
            if e['tag'] in ('structure_type', 'union_type', 'enumeration_type', 'typedef') and _name(e['at'].get('name')):
                folder.fold(entries, off)
            if e['tag'] == 'variable':
                n = (_name(e['at'].get('name')) or '')
                if n.startswith('__agent_var_'):
                    t = folder.fold(entries, _ref(e['at'].get('type')))
                    gname = n[len('__agent_var_'):]
                    globals_[gname] = {'addr': data[gname], 'type': t['to'] if isinstance(t, dict) else t}
    result = {'stamp': stamp, 'types': folder.types, 'globals': globals_,
              'functions': syms['code'],
              'params': first_params(syms['code'], folder.types),
              'data': {k: v for k, v in data.items()
                       if k not in globals_ and k not in syms['code'] and not k.startswith(('_0', '$', '.'))
                       and not k.endswith('__addr')},
              'failed': failed}
    out.write_text(json.dumps(result, separators=(',', ':')))
    return out


if __name__ == '__main__':
    path = build(force='--force' in sys.argv)
    d = json.loads(path.read_text())
    print(f"{path}: {len(d['types'])} types, {len(d['globals'])} typed globals, "
          f"{len(d['functions'])} functions ({len(d['params'])} with a struct parameter), "
          f"{len(d['failed'])} headers failed")

"""Typed names for memory: an address at a frame -> "Skeleton#12.chase.base.health".

Types come from build/port/agent/types.json (port/agent/gametypes.py: the prepared
headers compiled with debug info): every RAM global with its declared type, every
struct layout, and `params`, the struct each function's first parameter points to.

Static data is named by the global that holds it. Heap memory is named by what
points at it, found by walking typed memory at that frame:
  * tasks: every task in gTaskLists; its class is the struct its update function
    takes (a task's update gets its own object): "Player#3" (task id 3);
  * roots: a global or a typed member holding a pointer to a struct names the struct
    at that address: "*gGameState", "Player#3.drawable->", followed a few levels deep.
An address no typed object covers keeps its region and offset ("EWRAM+0x1F2C0").
"""
import bisect
import functools
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TYPES = ROOT / 'build/port/agent/types.json'
RAM_RANGES = ((0x02000000, 0x02040000, 'EWRAM'), (0x03000000, 0x03008000, 'IWRAM'), (0x04000000, 0x04000400, 'IO'))


def region_name(addr):
    for lo, hi, name in RAM_RANGES:
        if lo <= addr < hi:
            return f'{name}+0x{addr - lo:X}'
    return f'0x{addr:08X}'


class Types:
    def __init__(self, path=TYPES):
        data = json.loads(Path(path).read_text())
        self.types = data['types']
        self.globals = data['globals']
        self.functions = data.get('functions', {})
        self.params = data.get('params', {})
        self.fn_by_addr = {v & ~1: k for k, v in self.functions.items()}
        rows = []
        for name, g in self.globals.items():
            a = g['addr']
            if any(lo <= a < hi for lo, hi, _ in RAM_RANGES):
                rows.append((a, name, g['type']))
        rows.sort()
        # sizes: declared, else up to the next global (incomplete arrays)
        self.static = []
        for i, (a, name, t) in enumerate(rows):
            size = self.sizeof(t)
            if not size:
                nxt = rows[i + 1][0] if i + 1 < len(rows) else a + 4
                size = max(1, nxt - a)
            self.static.append((a, size, name, t))
        self.static_starts = [r[0] for r in self.static]

    def resolve(self, t):
        seen = 0
        while isinstance(t, str):
            d = self.types.get(t)
            if d is None:
                return {'k': 'int', 'size': 4, 'name': t}
            if d.get('k') == 'typedef':
                t = d.get('to')
                seen += 1
                if seen > 20:
                    break
                continue
            return d
        return t

    def name(self, t):
        if isinstance(t, str):
            return t
        k = t.get('k')
        if k == 'ptr':
            return self.name(t['to']) + ' *'
        if k == 'array':
            return f"{self.name(t['of'])}[{t.get('n', '')}]"
        if k == 'fn':
            return 'function'
        return t.get('name') or k or '?'

    @functools.lru_cache(maxsize=None)
    def _sizeof_named(self, t):
        return self._sizeof(self.resolve(t))

    def sizeof(self, t):
        if isinstance(t, str):
            return self._sizeof_named(t)
        return self._sizeof(t)

    def _sizeof(self, d):
        if d is None:
            return 0
        k = d.get('k')
        if k in ('struct', 'union', 'int', 'enum', 'float'):
            return d.get('size', 0)
        if k == 'ptr' or k == 'fn':
            return 4
        if k == 'array':
            return self.sizeof(d['of']) * d.get('n', 0)
        return d.get('size', 0)

    def struct_name(self, t):
        d = self.resolve(t)
        if d and d.get('k') in ('struct', 'union'):
            return t if isinstance(t, str) else None
        return None

    def path(self, t, off, width=1):
        """('.a.b[3].c', leaf type, leaf offset) for byte OFF of a T."""
        out = ''
        for _ in range(32):
            d = self.resolve(t)
            k = d.get('k') if d else None
            if k == 'array':
                es = self.sizeof(d['of']) or 1
                i = off // es
                out += f'[{i}]'
                off -= i * es
                t = d['of']
                continue
            if k in ('struct', 'union'):
                best = None
                for f in d.get('fields', []):
                    fs = self.sizeof(f['type']) or 1
                    if f['off'] <= off < f['off'] + fs:
                        if 'bits' in f:
                            best = f
                            break
                        best = f
                        break
                if best is None:
                    return out + f'+0x{off:X}', None, off
                out += '.' + best['name']
                off -= best['off']
                t = best['type']
                continue
            return out + (f'+{off}' if off else ''), t, off
        return out, t, off

    def static_at(self, addr):
        i = bisect.bisect_right(self.static_starts, addr) - 1
        if i >= 0:
            a, size, name, t = self.static[i]
            if a <= addr < a + size:
                return a, name, t
        return None


class ObjectMap:
    """Typed objects in RAM at one frame (see the module docstring)."""

    def __init__(self, types, ram, depth=3):
        self.t = types
        self.ram = ram
        self.objects = []   # (addr, size, type name, label)
        self._seen = set()
        self._walk_tasks()
        self._walk_roots(depth)
        self.objects.sort()
        self.starts = [o[0] for o in self.objects]

    def _add(self, addr, tname, label):
        size = self.t.sizeof(tname)
        if not size or (addr, tname) in self._seen:
            return False
        if not any(lo <= addr < hi for lo, hi, _ in RAM_RANGES[:2]):
            return False
        self._seen.add((addr, tname))
        self.objects.append((addr, size, tname, label))
        return True

    def _walk_tasks(self):
        g = self.t.globals.get('gTaskLists')
        if not g:
            return
        base = g['addr']
        lists = g['type'].get('n', 14) if isinstance(g['type'], dict) else 14
        lsize = self.t.sizeof('struct TaskList') or 8
        head_off = 0
        tl = self.t.resolve('struct TaskList')
        if tl:
            for f in tl.get('fields', []):
                if f['name'] == 'head':
                    head_off = f['off']
        for i in range(lists):
            t = self.ram.read(base + i * lsize + head_off, 4)
            n = 0
            while t and n < 1024:
                upd = self.ram.read(t + 8, 4)
                tid = self.ram.read(t + 16, 2)
                fn = self.t.fn_by_addr.get((upd or 0) & ~1)
                cls = self.t.params.get(fn) if fn else None
                if cls and self.t.sizeof(cls):
                    self._add(t, cls, f"{cls.replace('struct ', '')}#{tid}")
                else:
                    self._add(t, 'struct Task', f"Task#{tid}" + (f"({fn})" if fn else ''))
                t = self.ram.read(t + 4, 4)
                n += 1

    def _walk_roots(self, depth):
        frontier = []
        for a, size, name, t in self.t.static:
            frontier.append((a, t, name))
        for level in range(depth):
            nxt = []
            for a, t, label in frontier:
                for off, target, sub in self._pointers(t):
                    p = self.ram.read(a + off, 4)
                    if not p:
                        continue
                    sname = self.t.struct_name(target)
                    if not sname:
                        continue
                    lab = f'{label}{sub}->'
                    if self._add(p, sname, lab):
                        nxt.append((p, sname, lab))
            frontier = nxt

    @functools.lru_cache(maxsize=None)
    def _pointer_fields(self, tkey):
        return list(self._pointers_uncached(json.loads(tkey)))

    def _pointers(self, t):
        return self._pointer_fields(json.dumps(t, sort_keys=True))

    def _pointers_uncached(self, t, prefix='', base=0, budget=[0]):
        d = self.t.resolve(t)
        k = d.get('k') if d else None
        if k == 'ptr':
            yield base, d['to'], prefix
        elif k in ('struct', 'union'):
            for f in d.get('fields', []):
                if 'bits' in f:
                    continue
                yield from self._pointers_uncached(f['type'], f"{prefix}.{f['name']}", base + f['off'])
        elif k == 'array':
            n = d.get('n', 0)
            es = self.t.sizeof(d['of'])
            if 0 < n <= 64 and es:
                for i in range(n):
                    yield from self._pointers_uncached(d['of'], f'{prefix}[{i}]', base + i * es)

    def at(self, addr, width=1):
        """(label, field path, leaf type, object base) for ADDR."""
        best = None
        i = bisect.bisect_right(self.starts, addr) - 1
        # the innermost (smallest) object containing addr
        j = i
        while j >= 0 and j > i - 64:
            a, size, tname, label = self.objects[j]
            if a <= addr < a + size and (best is None or size < best[1]):
                best = self.objects[j]
            j -= 1
        st = self.t.static_at(addr)
        if st and (best is None or st[0] >= best[0]):
            a, name, t = st
            p, leaf, _ = self.t.path(t, addr - a, width)
            return name, p, leaf, a
        if best:
            a, size, tname, label = best
            p, leaf, _ = self.t.path(tname, addr - a, width)
            return label, p, leaf, a
        return region_name(addr), '', None, None


def join(label, path):
    if label.endswith('->'):
        return label + path.lstrip('.')
    return label + path


def describe(objmap, addr, width=1):
    label, p, leaf, _ = objmap.at(addr, width)
    return join(label, p)

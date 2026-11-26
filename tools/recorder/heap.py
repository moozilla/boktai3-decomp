"""Name heap memory by its allocation: which code allocated the block, when, and with what type.

The game's allocator (src/engine/heap_memory.c) keeps HeapBlock headers {prev, next, size,
unused} in front of each block and writes `block->unused = 0` exactly once per
allocation, so the last such write in the write log dates the allocation. That frame is
re-traced to find the allocator's invocation and its callers (Heap_Alloc <- Task_Create
<- Player_Create). The type is the one the allocating code uses: the struct of the first
parameter of the first function the new pointer is passed to (types.json `params`).
"""
import re

import numpy as np

from facts import Function, Block, Inst

HEAP_END = 0x0201F400
ALLOCATORS = ('Heap_Alloc', 'Heap_AllocFromEnd')
PASS_THROUGH = {'Heap_Alloc', 'Heap_AllocFromEnd', 'Task_Create', 'Heap_AllocZeroed', 'Memory_Alloc'}


class HeapNamer:
    def __init__(self, session, facts, types, slicer):
        self.s, self.facts, self.types, self.sl = session, facts, types, slicer
        g = types.globals.get('gHeapHead')
        self.head = g['addr'] if g else 0x02000714
        self.cache = {}
        self.marker_sites = None

    def _markers(self):
        if self.marker_sites is None:
            s = self.s
            s.writes
            sites = set()
            for site in np.unique(s.writes['site']):
                fn = self.facts.function_of(int(site))
                if fn is not None and fn.name in ALLOCATORS:
                    x = self.facts.site(int(site))
                    if isinstance(x, Inst) and 'store i32 0' in x.text:
                        sites.add(int(site))
            self.marker_sites = sites
        return self.marker_sites

    def block_of(self, ram, addr):
        """(header address, payload size) of the allocated block holding ADDR, or None."""
        if not (self.head <= addr < HEAP_END):
            return None
        b, n = self.head, 0
        while b and n < 4096:
            size = ram.read(b + 8, 4)
            if size is None:
                return None
            length = size & 0xFFFFF
            if b <= addr < b + max(length, 16):
                return None if size & 0x80000000 or addr < b + 16 else (b, length - 16)
            b = ram.read(b + 4, 4)
            n += 1
        return None

    def allocation(self, header, frame):
        """{'frame', 'chain': [function names, allocator first], 'type'} of the block at HEADER."""
        key = (header, frame)
        if key in self.cache:
            return self.cache[key]
        s = self.s
        s.writes
        sl = s.writes_in(0, frame)
        rows = np.nonzero((s.addr[sl] == header + 12) & np.isin(s.writes['site'][sl], list(self._markers())))[0]
        info = None
        if len(rows):
            r = rows[-1] + sl.start
            af = int(s.writes['frame'][r])
            info = {'frame': af, 'chain': [], 'type': None}
            ft = self.sl.trace(af) if af > 0 else None
            if ft is not None:
                st = np.nonzero((ft.st_pos >= 0) & (ft.st_start == header + 12))[0]
                pos = [int(ft.st_pos[i]) for i in st if int(ft.rows['site'][ft.st_pos[i]]) in self.marker_sites]
                if pos:
                    inv = int(ft.rows['inv'][pos[-1]])
                    info['chain'], info['type'] = self._callers(ft, inv)
        self.cache[key] = info
        return info

    def _callers(self, ft, inv):
        chain, typ = [], None
        child = inv
        for _ in range(6):
            ent = ft.enter.get(child)
            if not ent:
                break
            fn = self.facts.function_of(ent[1])
            chain.append(fn.name if fn else '?')
            parent = ent[2]
            if parent >= 0x80000000 or parent not in ft.enter:
                break
            pfn = self.facts.function_of(ft.enter[parent][1])
            if typ is None and pfn is not None and fn is not None and pfn.name not in PASS_THROUGH:
                typ = self._type_used(pfn, fn.name)
            if pfn is not None and pfn.name not in PASS_THROUGH and fn is not None and fn.name not in PASS_THROUGH:
                chain.append(pfn.name)
                break
            child = parent
        return chain, typ

    def _type_used(self, fn, callee):
        """The struct the result of `call @callee` in FN is used as: the update function a task
        gets (Task_SetCallbacks), else the first-parameter type of a function with FN's prefix,
        else of any non-generic function the pointer is passed to first."""
        names = set()
        for b in fn.blocks:
            for i in b.insts:
                if i.dest and re.search(r'@' + re.escape(callee) + r'\(', i.text):
                    names.add(i.dest)
        if not names:
            return None
        for b in fn.blocks:   # casts and zero-offset GEPs are the same pointer
            for j in b.insts:
                body = j.text.split(' = ', 1)[-1]
                if j.dest and any(re.search(re.escape(n) + r'\b', body) for n in names) and (
                        re.match(r'(bitcast|ptrtoint|inttoptr|freeze)\b', body) or re.search(r', i32 0$', body)):
                    names.add(j.dest)
        prefix = fn.name.split('_')[0] + '_'
        update, same, other = None, None, None
        for b in fn.blocks:
            for j in b.insts:
                m = re.search(r'call [^@]*@([-\w.$]+)\((.*)\)', j.text)
                if not m:
                    continue
                callee2, args = m[1], m[2]
                first = re.match(r'ptr [^%,]*(%[-\w.$]+)', args)
                if not first or first[1] not in names:
                    continue
                if callee2 == 'Task_SetCallbacks':
                    u = re.search(r'ptr [^,@]*@([-\w.$]+)', args.split(',', 1)[1]) if ',' in args else None
                    if u and u[1] in self.types.params and update is None:
                        update = self.types.params[u[1]]
                    continue
                if re.match(r'(Task|Heap|Memory|Mem|port)_', callee2) or callee2 not in self.types.params:
                    continue
                if callee2.startswith(prefix) and same is None:
                    same = self.types.params[callee2]
                elif other is None:
                    other = self.types.params[callee2]
        return update or same or other

    def name(self, ram, addr, frame):
        """(label, path) for a heap address, or None."""
        blk = self.block_of(ram, addr)
        if blk is None:
            return None
        header, size = blk
        info = self.allocation(header, frame)
        base = header + 16
        if info is None:
            return f'heap@{base:08X}[{size} B, allocated before the session]', f'+0x{addr - base:X}' if addr > base else ''
        chain = info['chain']
        real = [c for c in chain if c not in PASS_THROUGH]
        by = real[0] if real else (chain[-1] if chain else '?')
        k = chain.index(by) if by in chain else 0
        via = f' via {chain[k - 1]}' if k >= 1 and chain[k - 1] not in ALLOCATORS else ''
        t = info['type']
        if t:
            path, _, _ = self.types.path(t, addr - base)
            return f'{t.replace("struct ", "")}(heap, by {by}{via} @{info["frame"]})', path
        return f'heap(by {by}{via} @{info["frame"]}, {size} B)', f'+0x{addr - base:X}' if addr > base else ''


class Namer:
    """Names for memory at one frame: typed objects (lift.ObjectMap), then heap blocks by
    allocation. describe(addr, width) -> 'Player#77.position.x', 'heap(by Hud_Create ...)+0x40'."""

    def __init__(self, session, facts, types, frame, ram=None, slicer=None, heap=None):
        from lift import ObjectMap
        self.s, self.frame = session, frame
        self.ram = ram if ram is not None else session.ram_at(frame)
        self.om = ObjectMap(types, self.ram)
        self.heapbase = session.meta.get('heapBase', 0)
        self._slicer = slicer
        self.facts, self.types = facts, types
        self._heap = heap

    @property
    def heap(self):
        if self._heap is None:
            if self._slicer is None:
                from slicer import Slicer
                self._slicer = Slicer(self.s, self.facts)
            self._heap = HeapNamer(self.s, self.facts, self.types, self._slicer)
        return self._heap

    @property
    def objects(self):
        return self.om.objects

    def at(self, addr, width=1):
        label, path, leaf, base = self.om.at(addr, width)
        if label.startswith('EWRAM+'):
            h = self.heap.name(self.ram, addr, self.frame)
            if h:
                return h[0], h[1], None, None
        return label, path, leaf, base

    def describe(self, addr, width=1):
        from lift import join
        if addr is None:
            return '?'
        if addr < self.heapbase:
            return f'stack/port 0x{addr:X}'
        label, path, _, _ = self.at(addr, width)
        return join(label, path)

    def close(self):
        if self._slicer is not None:
            self._slicer.close()

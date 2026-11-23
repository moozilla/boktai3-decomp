"""Name placeholder fields from recorded behavior: python -m tools.recorder infer SESSION...

For every field or global with a placeholder name (`unk34`, `field_10`, `f5A4`,
`unknown0`, `gUnk_...`), across the given sessions:

  writes     the statements that write it (write log), its values: range, distinct values,
             the pattern of successive values per instance (flag, counter, timer, constant,
             pointer, code pointer), how often it changes;
  sources    where the stored value comes from: the C variable or parameter it was in
             (DWARF names in the facts), the named field it was loaded from;
  reads      the statements that read it (sampled traced frames), and where the loaded
             value goes (forward over the function's SSA): a named field, a named parameter
             of a call, a branch condition, a return value;
  co-writes  the named fields its writers also write.

A name is proposed from the strongest consistent evidence (a parameter or variable name
it flows through, the field it copies, a recognized value pattern plus the subsystem
stem); confidence counts independent signals that agree. Writes
build/recorder/infer/proposals.tsv, proposals.md and evidence.json. It never edits
include/ or symbols/.
"""
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

import numpy as np

from facts import Facts, Function, Block, Inst
from lift import Types, ObjectMap
from session import Session

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'build/recorder/infer'
PLACEHOLDER = re.compile(r'^(?:unk|unknown|field_?|pad|filler)_?[0-9A-Fa-f]+[a-z]?$|^[fu]_?(?=[0-9A-Fa-f]*\d)[0-9A-Fa-f]+$'
                         r'|^unk[_A-Z0-9]\w*$|^unknown\w*$')
GLOBAL_PLACEHOLDER = re.compile(r'^g?Unk_?[0-9A-Fa-f]+|^gUnk\w*')
VAGUE = {'v', 'val', 'value', 'x', 'y', 'z', 'i', 'j', 'k', 'n', 'p', 'q', 'r', 's', 't', 'a', 'b', 'c', 'd', 'e', 'w',
         'tmp', 'temp', 'ret', 'result', 'arg', 'obj', 'self', 'this', 'ptr', 'data', 'base', 'off', 'idx', 'out', 'in',
         'h', 'm', 'o', 'u', 'f', 'g', 'l', 'lo', 'hi', 'src', 'dst', 'flags_', 'unused', 'pad', 'actor', 'task', 'entry'}
CODE = (0x08000000, 0x08400000)


def is_placeholder(name):
    return bool(PLACEHOLDER.match(name))


# ---- candidate leaves -------------------------------------------------------
class Leaves:
    """Flattened scalar leaves of a type: (offset, size, path, placeholder?)."""

    def __init__(self, types):
        self.t = types
        self.cache = {}

    def of(self, t):
        key = json.dumps(t, sort_keys=True) if not isinstance(t, str) else t
        r = self.cache.get(key)
        if r is None:
            r = self.cache[key] = list(self._walk(t, 0, '', None, 0))
        return r

    def _walk(self, t, off, path, decl, depth):
        """Yields (offset, size, path, decl, kind); DECL names the placeholder member by the
        struct that declares it ('Sprite.unknown12[1]'), None for named leaves."""
        d = self.t.resolve(t)
        if d is None or depth > 6:
            return
        k = d.get('k')
        if k in ('struct', 'union'):
            tname = (t if isinstance(t, str) else '?').replace('struct ', '').replace('union ', '')
            for f in d.get('fields', []):
                if 'bits' in f:
                    continue
                sub = f"{path}.{f['name']}"
                if decl is not None:
                    nd = decl + '.' + f['name']
                elif is_placeholder(f['name']):
                    nd = f"{tname}.{f['name']}"
                else:
                    nd = None
                yield from self._walk(f['type'], off + f['off'], sub, nd, depth + 1)
        elif k == 'array':
            n, es = d.get('n', 0), self.t.sizeof(d['of'])
            if es and 0 < n <= 16:
                for i in range(n):
                    yield from self._walk(d['of'], off + i * es, f'{path}[{i}]', None if decl is None else f'{decl}[{i}]', depth + 1)
            elif es and n > 16:
                yield from self._walk(d['of'], off, f'{path}[0]', None if decl is None else f'{decl}[0]', depth + 1)
        else:
            size = self.t.sizeof(t) or 4
            if size <= 8:
                yield off, size, path, decl, k


# ---- evidence ----------------------------------------------------------------
class Field:
    def __init__(self, owner, path, size, kind):
        self.owner, self.path, self.size, self.kind = owner, path, size, kind
        self.writers = Counter()        # site -> count
        self.readers = Counter()
        self.values = Counter()
        self.deltas = Counter()         # successive-value relation per instance
        self.instances = set()
        self.frames_written = set()
        self.sessions = set()
        self.flows = Counter()          # ('field', name) / ('param', fn, name) / ('branch', text) / ('ret', fn)
        self.sources = Counter()        # ('var', name) / ('field', name) / ('param', fn, name)
        self.cowrites = Counter()
        self.last = {}

    @property
    def key(self):
        return f'{self.owner}{self.path}'

    def add_value(self, inst, v, frame):
        self.values[v] += 1
        self.frames_written.add(frame)
        prev = self.last.get(inst)
        if prev is not None:
            d = v - prev
            self.deltas['same' if d == 0 else '+1' if d == 1 else '-1' if d == -1 else '+' if d > 0 else '-'] += 1
        self.last[inst] = v
        self.instances.add(inst)


class Inferer:
    def __init__(self, sessions, trace_frames=6, log=print):
        self.sessions = [Session(s) if not isinstance(s, Session) else s for s in sessions]
        self.facts, self.types = Facts(), Types()
        self.leaves = Leaves(self.types)
        self.fields = {}
        self.trace_frames = trace_frames
        self.log = log
        self.fn_addr = {v & ~1: k for k, v in self.types.functions.items()}
        self.site_text = {}

    def field(self, key, size, kind):
        f = self.fields.get(key)
        if f is None:
            owner, dot, path = key.partition('.')
            f = self.fields[key] = Field(owner, '.' + path if path else '', size, kind)
        return f

    # objects of a frame -> intervals of leaves (placeholder ones flagged)
    def intervals(self, ram):
        """Sorted leaves of every typed object at a frame: (addr, size, owner, path, key, kind, base);
        KEY is the placeholder field's declaring-struct name (None for named leaves)."""
        om = ObjectMap(self.types, ram)
        rows = []
        for a, size, name, t in self.types.static:
            if GLOBAL_PLACEHOLDER.match(name):
                for off, sz, path, decl, k in self.leaves.of(t) or [(0, min(size, 4), '', None, 'int')]:
                    rows.append((a + off, sz, name, path, (name + path) if path else name, k, a))
            else:
                for off, sz, path, decl, k in self.leaves.of(t):
                    rows.append((a + off, sz, name, path, decl, k, a))
        for a, size, tname, label in om.objects:
            owner = tname.replace('struct ', '')
            for off, sz, path, decl, k in self.leaves.of(tname):
                rows.append((a + off, sz, owner, path, decl, k, a))
        rows.sort(key=lambda r: (r[0], r[1]))
        return rows

    def alloc_markers(self, s):
        """Write sites of the allocator's once-per-allocation `block->unused = 0` (heap.py)."""
        if getattr(self, '_markers', None) is None:
            from heap import ALLOCATORS
            sites = set()
            for site in np.unique(s.writes['site']):
                fn = self.facts.function_of(int(site))
                x = self.facts.site(int(site))
                if fn is not None and fn.name in ALLOCATORS and isinstance(x, Inst) and 'store i32 0' in x.text:
                    sites.add(int(site))
            self._markers = sites
        return self._markers

    def run(self):
        for s in self.sessions:
            self.log(f'{s.name}: {len(s)} frames')
            self.scan_writes(s)
            self.scan_reads(s)
        return self

    def scan_writes(self, s):
        s.writes
        snaps = s.snapshots
        for wi, f0 in enumerate(snaps):
            f1 = snaps[wi + 1] - 1 if wi + 1 < len(snaps) else len(s) - 1
            sl = s.writes_in(f0 + 1, f1) if f1 > f0 else slice(0, 0)
            if sl.stop <= sl.start:
                continue
            # heap blocks the allocator handed out again inside this window: their objects at
            # the snapshot are stale for part of it, so they are left out of this window
            marks = np.isin(s.writes['site'][sl], list(self.alloc_markers(s)))
            realloc = {int(a) - 12 for a in s.addr[sl][marks]}
            ram = s.ram_snapshot(f0)
            iv = [r for r in self.intervals(ram) if r[6] - 16 not in realloc]
            addr = s.addr[sl].astype(np.int64)
            order = np.argsort(addr, kind='stable')
            sa = addr[order]
            width = s.width[sl][order]
            vals = s.writes['value'][sl][order]
            sites = s.writes['site'][sl][order]
            frames = s.writes['frame'][sl][order]
            named_by_site = defaultdict(set)
            for (a, sz, owner, path, ph, k, base) in iv:
                lo = np.searchsorted(sa, a - 7)
                hi = np.searchsorted(sa, a + sz)
                if hi <= lo:
                    continue
                for j in range(lo, hi):
                    ra, rw = int(sa[j]), int(width[j])
                    if ra + rw <= a or ra >= a + sz:
                        continue
                    if not ph:
                        if len(named_by_site[int(sites[j])]) < 12:
                            named_by_site[int(sites[j])].add(owner + path)
                        continue
                    fld = self.field(ph, sz, k)
                    fld.sessions.add(s.name)
                    fld.writers[int(sites[j])] += 1
                    # the field's value from this write's bytes (only when the write covers it)
                    if ra <= a and ra + rw >= a + sz:
                        v = (int(vals[j]) >> (8 * (a - ra))) & ((1 << (8 * sz)) - 1)
                        fld.add_value((s.name, base), v, int(frames[j]))
            for fld in self.fields.values():
                for site in fld.writers:
                    for n in named_by_site.get(site, ()):
                        fld.cowrites[n] += 1

    # reads and forward flow, from a few traced frames
    def scan_reads(self, s):
        from slicer import Slicer
        start = max(1, len(s) // 3)
        frames = sorted({int(x) for x in np.linspace(start, len(s) - 2, self.trace_frames)})
        sl = Slicer(s, self.facts, max_frames=len(frames) + 2)
        try:
            for f in frames:
                ft = sl.trace(f)
                if ft is None:
                    continue
                ram = s.ram_at(f - 1)
                iv = self.intervals(ram)
                starts = np.array([x[0] for x in iv], np.int64)
                self.reads_in(ft, iv, starts, s)
                self.sources_in(ft, iv, starts, s)
        finally:
            sl.close()

    def leaf_at(self, iv, starts, addr):
        i = int(np.searchsorted(starts, addr, side='right')) - 1
        # leaves can nest (a struct inside a static object); pick the smallest covering one
        best = None
        j = i
        while j >= 0 and j > i - 40:
            a, sz, owner, path, ph, k, base = iv[j]
            if a <= addr < a + sz and (best is None or sz < best[1]):
                best = iv[j]
            j -= 1
        return best

    def name_of(self, iv, starts, addr):
        lf = self.leaf_at(iv, starts, addr)
        return (lf[2] + lf[3], lf[4]) if lf else (None, False)

    def reads_in(self, ft, iv, starts, s):
        kind = ft.kind
        loads = np.nonzero((kind == 1) | (kind == 3) & ((ft.rows['a'] >> 31) == 0))[0]
        seen = set()
        for p in loads:
            row = ft.rows[p]
            addr = int(row['a']) & 0x0FFFFFFF
            lf = self.leaf_at(iv, starts, addr)
            if not lf or not lf[4]:
                continue
            fld = self.field(lf[4], lf[1], lf[5])
            site = int(row['site'])
            fld.readers[site] += 1
            fld.sessions.add(s.name)
            if (site, fld.key) in seen:
                continue
            seen.add((site, fld.key))
            x = self.facts.site(site)
            if isinstance(x, Inst) and x.dest:
                for flow in self.forward(ft, x, int(row['inv']), p, iv, starts):
                    fld.flows[flow] += 1

    def forward(self, ft, load, inv, pos, iv, starts, depth=6):
        """Where the loaded value goes inside its function: stores (named by their trace
        event), call arguments (named parameters), branches, returns."""
        fn = load.fn
        uses = getattr(fn, '_uses', None)
        if uses is None:
            uses = defaultdict(list)
            for b in fn.blocks:
                for i in b.insts:
                    for n in set(re.findall(r'%[-\w.$]+', i.text.split(' = ', 1)[1] if i.dest else i.text)):
                        uses[n].append(i)
            fn._uses = uses
        out, frontier, seen = [], [load.dest], {load.dest}
        for _ in range(depth):
            nxt = []
            for name in frontier:
                for u in uses.get(name, ()):
                    body = u.text.split(' = ', 1)[1] if u.dest else u.text
                    op = body.split(' ', 1)[0]
                    if op == 'store':
                        ops = re.match(r'store \S+ (\S+?),', body)
                        if ops and ops[1] == name and u.site is not None:
                            # where it was stored: the store's event after the load, same invocation
                            qs = [int(q) for q in ft.st_pos if q > pos and int(ft.rows['site'][q]) == u.site and int(ft.rows['inv'][q]) == inv][:1]
                            if qs:
                                a = int(ft.st_start[np.searchsorted(ft.st_pos, qs[0])])
                                nm, ph = self.name_of(iv, starts, a)
                                if nm and not ph:
                                    out.append(('field', nm))
                    elif op in ('br', 'switch'):
                        line = self.line_text(u)
                        if line:
                            out.append(('branch', line))
                    elif op == 'ret':
                        out.append(('ret', fn.name))
                    elif op == 'call' or ' call ' in body:
                        m = re.search(r'@([-\w.$]+)\((.*)\)', body)
                        if m and not m[1].startswith(('llvm.', 'port_')):
                            args = re.findall(r'(%[-\w.$]+|\b-?\d+\b|@[-\w.$]+|null)(?=\s*(?:,|$))', m[2])
                            callee = self.callee(m[1])
                            for k, a in enumerate(args):
                                if a == name and callee is not None and k < len(callee.params):
                                    pn = callee.vars.get(callee.params[k][1])
                                    if pn and pn not in VAGUE:
                                        out.append(('param', callee.name, pn))
                    elif u.dest and u.dest not in seen and op not in ('getelementptr', 'icmp'):
                        seen.add(u.dest)
                        nxt.append(u.dest)
                    elif op == 'icmp' and u.dest not in seen:
                        seen.add(u.dest)
                        nxt.append(u.dest)
            frontier = nxt
        return out

    def callee(self, name):
        # build a name index once (all units' facts: ~6000 small JSON files)
        if not hasattr(self, '_fn_index'):
            self._fn_index = {}
            for i, u in enumerate(self.facts.units):
                if not u.startswith('src/'):
                    continue
                for fn in self.facts.unit(i).functions:
                    self._fn_index[fn.name] = fn
        return self._fn_index.get(name)

    def line_text(self, inst):
        from slicer import c_line
        k = id(inst)
        if k not in self.site_text:
            unit, line, text = c_line(self.facts, inst)
            self.site_text[k] = text.strip() if text else None
        return self.site_text[k]

    def sources_in(self, ft, iv, starts, s):
        """For stores to placeholder fields in this frame: the C variable or field the value came from."""
        for q in ft.st_pos:
            k = int(ft.kind[q])
            if k != 2:
                continue
            a = int(ft.st_start[np.searchsorted(ft.st_pos, q)])
            lf = self.leaf_at(iv, starts, a)
            if not lf or not lf[4]:
                continue
            fld = self.fields.get(lf[4])
            if fld is None:
                continue
            x = self.facts.site(int(ft.rows['site'][q]))
            if not isinstance(x, Inst):
                continue
            m = re.match(r'store \S+ (%[-\w.$]+),', x.text)
            if not m:
                continue
            v = m[1]
            fn = x.fn
            for _ in range(4):   # through casts
                if v in fn.vars and fn.vars[v] not in VAGUE:
                    fld.sources[('var', fn.vars[v])] += 1
                    break
                d = fn.defs.get(v)
                if d is None:
                    break
                body = d.text.split(' = ', 1)[1]
                if d.site is not None and body.startswith('load'):
                    p = ft.event_in(int(ft.rows['inv'][q]), d.site, ft.block_at(int(ft.rows['inv'][q]), q)[0]) if ft.block_at(int(ft.rows['inv'][q]), q) else None
                    if p is not None:
                        nm, ph = self.name_of(iv, starts, int(ft.rows['a'][p]) & 0x0FFFFFFF)
                        if nm and not ph:
                            fld.sources[('field', nm)] += 1
                    break
                cm = re.match(r'(?:zext|sext|trunc)\s+\S+\s+(%[-\w.$]+)', body)
                if not cm:
                    break
                v = cm[1]

    def text_evidence(self, fld):
        """Names from the C text of the statements that write and read the field:
        `p->unk34 = timer;` -> timer, `speed = p->unk34;` -> speed."""
        member = re.escape(re.sub(r'\[\d+\]', '', fld.path.split('.')[-1]))
        votes = Counter()
        for sites, role in ((fld.writers, 'w'), (fld.readers, 'r')):
            for site in sites:
                x = self.facts.site(site)
                if not isinstance(x, Inst) or not x.fn.unit.name.startswith('src/'):
                    continue
                text = self.line_text(x) or ''
                m = re.match(r'^(.*?)\s*=\s*(.*?);?\s*$', text)
                if not m or '==' in text[:len(m[1]) + 2]:
                    continue
                lhs, rhs = m[1], m[2]
                if role == 'w' and re.search(member + r'(\[\w+\])?$', lhs):
                    t = re.fullmatch(r'\(?(?:\(\w+\s*\*?\))?\s*([A-Za-z_][\w.>\-]*?)\)?', rhs.strip())
                    if t:
                        tail = re.split(r'\.|->', t[1])[-1]
                        if tail not in VAGUE and not is_placeholder(tail) and not tail.isdigit():
                            votes[tail] += 1
                elif role == 'r' and re.search(member, rhs) and re.fullmatch(r'(?:\w+\s+)?\*?([A-Za-z_]\w*)', lhs.strip()):
                    name = re.fullmatch(r'(?:\w+\s+)?\*?([A-Za-z_]\w*)', lhs.strip())[1]
                    if name not in VAGUE and not is_placeholder(name) and re.fullmatch(member + r'(\[\w+\])?', rhs.split('->')[-1].split('.')[-1].strip().rstrip(';')):
                        votes[name] += 1
        return votes

    def alias_evidence(self, fld):
        """Member names other code uses for the same bytes: the member accessed at each game-side
        writer's and reader's source column (`anim->sequenceId = sequence;` -> sequenceId)."""
        from facts import lvalue_text
        own = re.sub(r'\[\d+\]', '', fld.path.split('.')[-1])
        votes = Counter()
        for sites, role in ((fld.writers, 'w'), (fld.readers, 'r')):
            for site in sites:
                x = self.facts.site(site)
                if not isinstance(x, Inst) or not x.fn.unit.name.startswith('src/'):
                    continue
                text = self.line_text(x)
                if not text:
                    continue
                if role == 'w':
                    unit, line, raw = self.facts.source(x)
                    lv = lvalue_text(raw, x.col) if raw and x.col else None
                    m = re.search(r'(?:->|\.)\s*([A-Za-z_]\w*)\s*(?:\[[^\]]*\])?\s*$', lv or '')
                    name = m[1] if m else None
                else:
                    unit, line, raw = self.facts.source(x)
                    name = None
                    if raw and x.col:
                        # the access expression that starts at the load's column: `anim->durationScale`
                        m = re.match(r'[A-Za-z_]\w*(?:\s*(?:->|\.)\s*[A-Za-z_]\w*(?:\[[^\]]*\])?)+', raw[x.col - 1:])
                        if m:
                            name = re.findall(r'(?:->|\.)\s*([A-Za-z_]\w*)', m[0])[-1]
                if name and name != own and not is_placeholder(name) and name not in VAGUE and len(name) > 2 \
                        and not re.match(r'^(?:value|word|byte|half|data|field|entry|elem|arg|param|var|item|slot|reg|tmp)_?\d+$', name):
                    votes[name] += 1
        return votes

    # ---- proposals ----
    def stem(self, fld):
        """The subsystem word from the writers' and readers' function names."""
        words = Counter()
        for sites in (fld.writers, fld.readers):
            for site in sites:
                fn = self.facts.function_of(site)
                if fn and not fn.name.startswith('sub_'):
                    parts = fn.name.split('_')
                    if len(parts) > 1:
                        for w in re.findall(r'[A-Z][a-z]+', parts[1]):
                            if w not in ('Update', 'Set', 'Get', 'Init', 'Create', 'Is', 'Has', 'On', 'Apply', 'Step', 'Tick',
                                         'Run', 'Do', 'Handle', 'Process', 'Refresh', 'Clear', 'Reset', 'Destroy', 'Begin', 'End',
                                         'Start', 'Stop', 'Load', 'Read', 'Write', 'From', 'To', 'And', 'With', 'For', 'Task'):
                                words[w] += 1
        return words.most_common(1)[0][0] if words else None

    def pattern(self, fld):
        vals = fld.values
        n = sum(vals.values())
        if not n:
            return None, {}
        distinct = len(vals)
        lo, hi = min(vals), max(vals)
        d = fld.deltas
        nd = sum(d.values()) or 1
        info = {'writes': n, 'distinct': distinct, 'min': lo, 'max': hi, 'instances': len(fld.instances),
                'frames': len(fld.frames_written)}
        code = [v for v in vals if CODE[0] <= (v & ~1) < CODE[1] and (v & ~1) in self.fn_addr]
        ptr = [v for v in vals if 0x02000000 <= v < 0x02040000 or 0x03000000 <= v < 0x03008000]
        if fld.size == 4 and code and len(code) >= 0.8 * distinct:
            info['functions'] = sorted({self.fn_addr[v & ~1] for v in code})[:6]
            return 'callback', info
        if fld.size == 4 and ptr and len(ptr) >= 0.8 * distinct and lo >= 0x02000000:
            return 'pointer', info
        if set(vals) <= {0, 1} and distinct == 2:
            return 'flag', info
        if distinct == 1:
            return 'constant', info
        if d['-1'] / nd > 0.6 and lo >= 0:
            return 'timer', info
        if d['+1'] / nd > 0.6:
            return 'counter', info
        if distinct <= 8 and hi < 64:
            info['set'] = sorted(vals)
            return 'state', info
        if fld.size == 1 and hi > 200 and distinct > 32:
            return 'angle?', info
        return 'value', info

    def propose(self, fld):
        """(name, confidence 0..1, reasons)."""
        reasons = []
        votes = Counter()
        # strongest: named parameters it is passed as, variables it is stored from
        def tail_of(name):
            t = re.sub(r'\[\d+\]$', '', re.split(r'\.|->', name)[-1])
            return t[1].lower() + t[2:] if re.match(r'g[A-Z]', t) else t

        bonus = lambda c: 1 if c >= 10 else 0  # noqa: E731  repeated, consistent observations
        for (kind, *rest), c in fld.flows.items():
            if kind == 'param':
                votes[rest[1]] += 3 + bonus(c)
                reasons.append(f'passed as `{rest[1]}` to {rest[0]} (x{c})')
            elif kind == 'field':
                tail = tail_of(rest[0])
                if tail and not is_placeholder(tail):
                    votes[tail] += 2 + bonus(c)
                    reasons.append(f'copied into {rest[0]} (x{c})')
        for (kind, *rest), c in fld.sources.items():
            if kind == 'var':
                votes[rest[0]] += 3 + bonus(c)
                reasons.append(f'stored from C variable `{rest[0]}` (x{c})')
            elif kind == 'field':
                tail = tail_of(rest[0])
                if tail and not is_placeholder(tail):
                    votes[tail] += 2 + bonus(c)
                    reasons.append(f'loaded from {rest[0]} (x{c})')
        for nm, c in self.alias_evidence(fld).items():
            votes[nm] += 4 + (1 if c >= 2 else 0)   # a name another author already gave these bytes
            fld.sources[('alias', nm)] += c
            reasons.append(f'other code names these bytes `{nm}` ({c} statement(s))')
        for nm, c in self.text_evidence(fld).items():
            if re.match(r'g[A-Z]', nm):
                nm = nm[1].lower() + nm[2:]
            votes[nm] += 2
            fld.sources[('text', nm)] += c
            reasons.append(f'C text: assigned to/from `{nm}` in {c} statement(s)')
        pat, info = self.pattern(fld)
        stem = self.stem(fld)
        name = None
        if votes:
            name, score = votes.most_common(1)[0]
            total = sum(votes.values())
            agree = score / total
        else:
            score, agree = 0, 0
        # a value pattern names (or qualifies) it when no flow does
        if pat in ('timer', 'counter', 'flag', 'callback', 'pointer', 'state') and stem:
            base = stem[0].lower() + stem[1:]
            pname = {'timer': f'{base}Timer', 'counter': f'{base}Counter', 'flag': f'is{stem}',
                     'callback': f'{base}Callback', 'pointer': f'{base}Ptr', 'state': f'{base}State'}[pat]
            if name is None:
                name, score, agree = pname, 1, 0.5
            reasons.append(f'value pattern: {pat}' + (f' ({", ".join(info.get("functions", []))})' if info.get('functions') else '')
                           + f'; subsystem `{stem}`')
        if name is None:
            return None, 0.0, reasons, pat, info
        if re.match(r'g[A-Z]', name):
            name = name[1].lower() + name[2:]
        # confidence: independent signals that agree with the name, how often they were seen,
        # how many sessions; a single observation stays below 0.5
        support = [(k, c) for (k, *rest), c in list(fld.flows.items()) + list(fld.sources.items())
                   if k in ('param', 'var', 'field', 'text', 'alias') and name.lower() in ' '.join(map(str, rest)).lower()]
        seen = sum(c for _, c in support)
        conf = 0.2 * min(len(support), 3) + 0.3 * agree + 0.1 * min(seen, 3) / 3 + (0.15 if len(fld.sessions) > 1 else 0) \
            + (0.1 if pat in ('timer', 'counter', 'flag', 'callback') else 0)
        if seen <= 1:
            conf = min(conf, 0.45)
        conf = min(conf, 1.0)
        return name, round(conf, 2), reasons, pat, info

    @staticmethod
    def merge_ranges(rows):
        """Consecutive elements of one placeholder array with the same proposal become one row
        (`unknown24[0..3] -> previous`: a pointer inside a byte array)."""
        out, groups = [], defaultdict(list)
        for r in rows:
            m = re.match(r'^(.*)\[(\d+)\]$', r['field'])
            if m and r['proposal']:
                groups[(r['owner'], m[1])].append((int(m[2]), r))
            else:
                out.append(r)
        for (owner, base), items in groups.items():
            items.sort(key=lambda x: x[0])
            run = [items[0]]
            for i, r in items[1:] + [(None, None)]:
                if r is not None and i == run[-1][0] + 1 and r['proposal'] == run[-1][1]['proposal']:
                    run.append((i, r))
                    continue
                if len(run) == 1:
                    out.append(run[0][1])
                else:
                    first = dict(run[0][1])
                    first['field'] = f'{base}[{run[0][0]}..{run[-1][0]}]'
                    first['size'] = sum(x[1]['size'] for x in run)
                    first['confidence'] = max(x[1]['confidence'] for x in run)
                    first['reasons'] = run[0][1]['reasons'] + [f'{len(run)} consecutive elements agree']
                    out.append(first)
                run = [(i, r)] if r is not None else []
        return out

    def describe_sites(self, sites, n=3):
        out = []
        for site, c in sites.most_common(n):
            x = self.facts.site(site)
            if isinstance(x, Inst):
                text = self.line_text(x)
                out.append(f'{x.fn.name}: `{text}`' if text else x.fn.name)
        return out

    def report(self, out=OUT, top=None):
        out.mkdir(parents=True, exist_ok=True)
        rows, evidence = [], {}
        for key, fld in sorted(self.fields.items()):
            if not fld.writers and not fld.readers:
                continue
            name, conf, reasons, pat, info = self.propose(fld)
            ev = {'owner': fld.owner, 'field': fld.path.lstrip('.'), 'size': fld.size, 'proposal': name, 'confidence': conf,
                  'pattern': pat, 'values': info, 'reasons': reasons,
                  'writers': self.describe_sites(fld.writers, 4), 'readers': self.describe_sites(fld.readers, 4),
                  'flows': [list(map(str, k)) + [c] for k, c in fld.flows.most_common(6)],
                  'sources': [list(map(str, k)) + [c] for k, c in fld.sources.most_common(6)],
                  'cowrites': [k for k, _ in fld.cowrites.most_common(6)], 'sessions': sorted(fld.sessions)}
            evidence[key] = ev
            rows.append(ev)
        rows = self.merge_ranges(rows)
        rows.sort(key=lambda r: (-(r['confidence'] or 0), r['owner'], r['field']))
        with open(out / 'proposals.tsv', 'w') as f:
            f.write('owner\tfield\tsize\tproposal\tconfidence\tpattern\twriters\treaders\treasons\n')
            for r in rows:
                f.write('\t'.join(str(x) for x in (r['owner'], r['field'], r['size'], r['proposal'] or '', r['confidence'], r['pattern'] or '',
                                                   ' | '.join(r['writers']), ' | '.join(r['readers']), ' | '.join(r['reasons']))) + '\n')
        (out / 'evidence.json').write_text(json.dumps(evidence, indent=1, default=str))
        md = ['# Field name proposals from recorded sessions', '',
              f'Sessions: {", ".join(s.name for s in self.sessions)}. Fields with placeholder names observed: {len(rows)}; '
              f'with a proposal: {sum(1 for r in rows if r["proposal"])}. Generated by `python -m tools.recorder infer`; '
              'review before renaming (nothing in include/ or symbols/ was changed).', '',
              '| Field | Proposal | Conf. | Pattern | Evidence |', '|---|---|---|---|---|']
        for r in rows:
            if not r['proposal']:
                continue
            ev = '; '.join(r['reasons'][:3]) + ('<br>writers: ' + '; '.join(r['writers'][:2]) if r['writers'] else '')
            md.append(f"| `{r['owner']}{'.' + r['field'] if r['field'] else ''}` | `{r['proposal']}` | {r['confidence']} | {r['pattern'] or ''} | {ev.replace('|', '/')} |")
        (out / 'proposals.md').write_text('\n'.join(md) + '\n')
        return rows

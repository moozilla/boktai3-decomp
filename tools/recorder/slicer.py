"""Why does this memory hold this value? A dynamic backward slice over the frame traces.

A frame's trace (runner.mjs trace mode, emu.load_trace) has one row per event of the
game-side code: [site, a, b, c, invocation]. With the static facts (facts.py: every
instruction of every function, with its C line) the slice follows, from the store that
last wrote the target bytes:

  data      an SSA operand to its defining instruction in the same invocation;
            a phi to the value from the predecessor block that actually ran;
            a parameter to the caller's call operand; a call result to the callee's ret;
  memory    a load to the last store to the bytes it read: in the frame's trace, or in
            an earlier frame (the write log finds it, that frame is re-traced);
            copies to their source bytes;
  control   a block to the last executed branch it is control dependent on
            (postdominators of the function's CFG), and that branch's condition.

Terminals: constants, ROM data, hardware registers (the solar sensor GPIO, keys),
memory nobody wrote since the session began, and arguments of code entered from the
port (interrupt handlers). A store that keeps updating the same address from its own
old value (a counter) is followed once; earlier instances are counted through the
write log and the slice continues from the store that started the run.
"""
import re
from collections import defaultdict
from pathlib import Path

import numpy as np

from facts import Facts, Function, Block, Inst, lvalue_text
from emu import Replay, load_trace

ROOT = Path(__file__).resolve().parents[2]
TRACES = ROOT / 'build/recorder/tmp/traces'
K_BB, K_LOAD, K_STORE, K_WIDE, K_CPY, K_SET, K_AGG, K_FN = range(8)
SSA = re.compile(r'%[-\w.$]+')
LABEL = re.compile(r'label %([-\w.$]+)')


def hw_name(addr):
    if 0x080000C4 <= addr < 0x080000CA:
        return 'cartridge GPIO (solar sensor / RTC)'
    if 0x04000130 <= addr < 0x04000132:
        return 'KEYINPUT (buttons)'
    if 0x04000000 <= addr < 0x04000400:
        return f'IO register 0x{addr:08X}'
    if 0x08000000 <= addr < 0x0A000000:
        return 'ROM'
    return f'0x{addr:08X}'


# ---- one frame's trace, indexed ----------------------------------------------
class FrameTrace:
    def __init__(self, frame, rows, facts=None):
        self.frame = frame
        self.rows = rows
        n = len(rows)
        site = rows['site']
        self.kind = (site >> 29).astype(np.uint8)
        inv = rows['inv']
        self.block_ev = defaultdict(list)      # inv -> [(pos, site)] (FN enter counts as the entry block)
        self.site_pos = defaultdict(list)      # (inv, site) -> [pos]
        self.enter = {}                         # inv -> (pos, fn site, parent)
        self.children = defaultdict(list)       # parent inv -> [(pos, child inv, fn site)]
        self.exit = {}
        kind = self.kind
        for p in range(n):
            k = int(kind[p])
            s = int(site[p])
            i = int(inv[p])
            if k == K_BB:
                self.block_ev[i].append((p, s))
            elif k == K_FN:
                if rows['a'][p] == 0:
                    parent = int(rows['b'][p])
                    self.enter[i] = (p, s, parent)
                    self.block_ev[i].append((p, s))
                    self.children[parent].append((p, i, s))
                else:
                    self.exit[i] = p
            else:
                self.site_pos[(i, s)].append(p)
        # invocations already running when the frame began (the main loop, resumed
        # callers): their first block instance has no event; start it at their first row
        if facts is not None:
            first = {}
            for p in range(n):
                i = int(inv[p])
                if i not in first:
                    first[i] = p
            for i, p in first.items():
                if i in self.enter or int(kind[p]) == K_BB:
                    continue
                x = facts.site(int(site[p]))
                blk = x.block if isinstance(x, Inst) else None
                if blk is not None:
                    self.block_ev[i].insert(0, (p, blk.site if blk.index else blk.fn.site))
        # stores as byte intervals
        st = (kind == K_STORE) | ((kind == K_WIDE) & ((rows['a'] >> 31) == 1)) | (kind == K_CPY) | (kind == K_SET) \
            | ((kind == K_AGG) & ((rows['b'] >> 31) == 1))
        self.st_pos = np.nonzero(st)[0]
        a = rows['a'][self.st_pos].astype(np.int64)
        k = kind[self.st_pos]
        start = np.where((k == K_STORE) | (k == K_WIDE), a & 0x0FFFFFFF, a).astype(np.int64)
        length = np.select([k == K_STORE, k == K_WIDE, k == K_CPY, k == K_SET],
                           [1 << ((a >> 28) & 3), np.full_like(a, 8), rows['c'][self.st_pos].astype(np.int64),
                            rows['c'][self.st_pos].astype(np.int64)],
                           rows['b'][self.st_pos].astype(np.int64) & 0x7FFFFFFF)
        self.st_start, self.st_end = start, start + length

    def last_store(self, addr, before):
        """Position of the last store covering byte ADDR before position BEFORE, or None."""
        m = (self.st_start <= addr) & (self.st_end > addr) & (self.st_pos < before)
        idx = np.nonzero(m)[0]
        return int(self.st_pos[idx[-1]]) if len(idx) else None

    def block_at(self, inv, pos):
        """(pos, site) of the block instance of INV running at POS."""
        ev = self.block_ev.get(inv, [])
        lo, hi = 0, len(ev)
        while lo < hi:
            mid = (lo + hi) // 2
            if ev[mid][0] <= pos:
                lo = mid + 1
            else:
                hi = mid
        return ev[lo - 1] if lo else None

    def prev_block(self, inv, bpos):
        ev = self.block_ev.get(inv, [])
        prev = None
        for p, s in ev:
            if p >= bpos:
                break
            prev = (p, s)
        return prev

    def last_instance(self, inv, block_site, before, is_entry):
        """Position of the latest instance of a block in INV at or before BEFORE."""
        best = None
        for p, s in self.block_ev.get(inv, []):
            if p > before:
                break
            if s == block_site:
                best = p
        return best

    def event_in(self, inv, site, bpos):
        """Position of SITE's event in the block instance of INV starting at BPOS."""
        lst = self.site_pos.get((inv, site), [])
        for p in lst:
            if p >= bpos:
                nb = self.block_at(inv, p)
                return p if nb and nb[0] == bpos else None
        return None


# ---- control dependence --------------------------------------------------------
def cfg(fn):
    succ = {}
    for b in fn.blocks:
        term = b.insts[-1].text if b.insts else ''
        succ[b.label] = LABEL.findall(term) if term.lstrip().startswith(('br ', 'switch ')) or ' br ' in term else []
    return succ


def control_deps(fn):
    """block label -> set of block labels whose branch decides whether it runs."""
    cd = getattr(fn, '_cd', None)
    if cd is not None:
        return cd
    succ = cfg(fn)
    labels = [b.label for b in fn.blocks]
    exits = [l for l in labels if not succ[l]]
    pdom = {l: set(labels) for l in labels}
    for e in exits:
        pdom[e] = {e}
    changed = True
    while changed:
        changed = False
        for l in labels:
            if not succ[l]:
                continue
            new = set.intersection(*(pdom[s] for s in succ[l] if s in pdom)) if any(s in pdom for s in succ[l]) else set()
            new = new | {l}
            if new != pdom[l]:
                pdom[l] = new
                changed = True
    cd = defaultdict(set)
    for p in labels:
        if len(set(succ[p])) < 2:
            continue
        for s in succ[p]:
            if s not in pdom:
                continue
            # blocks postdominating s but not strictly postdominating p
            for b in pdom[s]:
                if b == p or b not in pdom[p]:
                    cd[b].add(p)
    fn._cd = cd
    return cd


# ---- the slicer ---------------------------------------------------------------
class Node:
    __slots__ = ('frame', 'pos', 'inv', 'fn', 'inst', 'role', 'value', 'note', 'addr', 'width')

    def __init__(self, frame, pos, inv, fn, inst, role, value=None, note=None, addr=None, width=None):
        self.frame, self.pos, self.inv, self.fn, self.inst = frame, pos, inv, fn, inst
        self.role, self.value, self.note, self.addr, self.width = role, value, note, addr, width


class Slicer:
    def __init__(self, session, facts=None, replay=None, max_nodes=400, max_frames=24, log=None):
        self.session = session
        self.facts = facts or Facts()
        self.replay = replay
        self.max_nodes, self.max_frames = max_nodes, max_frames
        self.traces = {}
        self.nodes = []
        self.terminals = []
        self.summaries = []
        self.seen = set()
        self.expanded_sites = set()     # (store site, address) already followed
        self.site_count = defaultdict(int)
        self.cycles = []                # (frame, site, addr): a statement met again, not followed
        self.log = log or (lambda *a: None)
        self.truncated = False

    # traces
    def trace(self, frame):
        ft = self.traces.get(frame)
        if ft is None:
            if len(self.traces) >= self.max_frames:
                self.truncated = True
                return None
            d = TRACES / self.session.name
            path = d / f'f{frame:08d}.bin'
            if not path.exists():
                if self.replay is None:
                    self.replay = Replay(self.session.path)
                self.replay.seek(frame - 1)
                self.replay.run(1, trace=d)
            self.log(f'  traced frame {frame}')
            ft = self.traces[frame] = FrameTrace(frame, load_trace(path), self.facts)
        return ft

    def close(self):
        if self.replay:
            self.replay.close()

    # ---- entry point ----
    def why(self, addr, width, frame):
        """Slice back from the bytes [addr, addr + width) as they are at the end of FRAME."""
        self.target = (addr, width, frame)
        self.work = []
        self.memory(frame, None, addr, width, 'target')
        while self.work and len(self.nodes) < self.max_nodes:
            item = self.work.pop()
            item[0](*item[1:])
        if self.work:
            self.truncated = True
        return self

    def why_store(self, frame, site, addr, nth):
        """Slice back from the NTH store by SITE to ADDR in FRAME (a write-log row)."""
        ft = self.trace(frame)
        hits = [int(p) for p in ft.st_pos if int(ft.rows['site'][p]) == site and ft.st_start[np.searchsorted(ft.st_pos, p)] == addr]
        if nth >= len(hits):
            raise ValueError(f'store {site:#x} to {addr:#x} ran {len(hits)} times in frame {frame}, not {nth + 1}')
        pos = hits[nth]
        width = int(ft.st_end[np.searchsorted(ft.st_pos, pos)] - addr)
        self.target = (addr, width, frame)
        self.work = []
        self.store(frame, pos, addr, width)
        while self.work and len(self.nodes) < self.max_nodes:
            item = self.work.pop()
            item[0](*item[1:])
        if self.work:
            self.truncated = True
        return self

    # ---- memory ----
    def memory(self, frame, pos, addr, width, why):
        if 0x08000000 <= addr < 0x0E000000 or (0x04000000 <= addr < 0x04000400 and not self._logged_io(addr)):
            note = ''
            if 0x080000C4 <= addr < 0x080000CA and frame < len(self.session.frames):
                # the port's sensor model answers from the solar level the host set for this frame
                prev = max(frame - 1, 0)
                note = f': host solar level {int(self.session.frames["solar"][prev])}'
            self.terminal(frame, f'{hw_name(addr)} 0x{addr:08X}{note}', addr)
            return
        if frame <= 0:
            self.terminal(0, f'0x{addr:08X} set during boot (frame 0 cannot be re-traced)', addr)
            return
        ft = self.trace(frame)
        if ft is None:
            self.terminal(frame, f'0x{addr:08X} (frame {frame} not traced: budget)', addr)
            return
        before = len(ft.rows) if pos is None else pos
        groups = defaultdict(list)
        missing = []
        for b in range(addr, addr + width):
            p = ft.last_store(b, before)
            if p is None:
                missing.append(b)
            else:
                groups[p].append(b)
        for p, bs in groups.items():
            self.work.append((self.store, frame, p, bs[0], len(bs)))
        if missing:
            self.earlier(frame, missing)

    def _logged_io(self, addr):
        return True

    def earlier(self, frame, missing):
        s = self.session
        s.writes
        by_frame = defaultdict(list)
        for b in missing:
            sl = s.writes_in(0, frame - 1) if frame > 0 else slice(0, 0)
            a, w = s.addr[sl], s.width[sl]
            idx = np.nonzero((a <= b) & (a + w > b))[0]
            if len(idx):
                row = idx[-1] + sl.start
                by_frame[int(s.writes['frame'][row])].append(b)
            elif b < self.session.meta.get('heapBase', 0):
                self.terminal(frame, f'port stack/data 0x{b:08X}, written before this frame\'s trace began', b)
            else:
                self.terminal(frame, f'0x{b:08X} not written since the session began (power-on or save state)', b)
        for f, bs in by_frame.items():
            self.work.append((self.memory, f, None, bs[0], bs[-1] - bs[0] + 1, 'earlier'))

    def store(self, frame, pos, addr, width):
        key = ('st', frame, pos)
        if key in self.seen:
            return
        self.seen.add(key)
        ft = self.traces[frame]
        row = ft.rows[pos]
        site, inv, k = int(row['site']), int(row['inv']), int(ft.kind[pos])
        x = self.facts.site(site)
        if not isinstance(x, Inst):
            self.terminal(frame, f'written by {getattr(x, "name", "?")}', addr)
            return
        fn = x.fn
        # emulated hardware (its RAM writes are traced, its code is not)
        if not self._game_side(fn):
            self.terminal(frame, f'written by the emulated hardware ({fn.name}, {fn.unit.name})', addr)
            return
        # each statement is followed once per slice: a run of the same store to the same
        # address (a counter) is counted through the write log and the slice continues
        # at the store that started the run; any other repeat is an earlier cycle
        if (site, addr) in self.expanded_sites or self.site_count[site] >= 4:
            if self.prev_store_site(frame, pos, addr) == site:
                self.collapse(frame, pos, addr, width, site)
            else:
                self.cycles.append((frame, site, addr))
            return
        self.expanded_sites.add((site, addr))
        self.site_count[site] += 1
        if k == K_CPY:
            node = self.add(frame, pos, inv, fn, x, 'copy', note=f'copy of {int(row["c"])} bytes')
            src = int(row['b']) + (addr - int(row['a']))
            self.work.append((self.memory, frame, pos, src, width, 'copy'))
        elif k == K_SET:
            self.add(frame, pos, inv, fn, x, 'fill', value=int(row['b']))
        else:
            value = int(row['b']) if k in (K_STORE, K_WIDE) else None
            if k == K_WIDE:
                value |= int(row['c']) << 32
            self.add(frame, pos, inv, fn, x, 'store', value=value, addr=addr, width=width)
            bpos = ft.block_at(inv, pos)[0]
            val = store_value(x.text)
            if val:
                # only the bits of the stored value that became the target bytes matter
                start = int(row['a']) & 0x0FFFFFFF if k != K_AGG else int(row['a'])
                off = max(0, addr - start)
                mask = ((1 << (8 * width)) - 1) << (8 * off)
                self.work.append((self.value, frame, inv, bpos, fn, val, x.block, mask))
        self.control(frame, inv, ft.block_at(inv, pos)[0], fn, x.block)

    def _game_side(self, fn):
        u = fn.unit.name
        return not (u.startswith('port/hw/') or u.startswith('port/core/'))

    def prev_store_site(self, frame, pos, addr):
        """Site of the store to ADDR just before the one at POS (this frame, else the write log)."""
        ft = self.traces[frame]
        q = ft.last_store(addr, pos)
        if q is not None:
            return int(ft.rows['site'][q])
        s = self.session
        s.writes
        sl = s.writes_in(0, frame - 1) if frame > 0 else slice(0, 0)
        a, w = s.addr[sl], s.width[sl]
        idx = np.nonzero((a <= addr) & (a + w > addr))[0]
        return int(s.writes['site'][idx[-1] + sl.start]) if len(idx) else None

    def collapse(self, frame, pos, addr, width, site):
        """Count the run of stores by SITE to ADDR ending at POS; continue at the store before it."""
        s = self.session
        ft = self.traces[frame]
        count, first, p = 1, frame, pos
        while True:
            q = ft.last_store(addr, p)
            if q is None:
                break
            if int(ft.rows['site'][q]) != site:
                self.summaries.append((addr, site, count, first, frame))
                self.work.append((self.store, frame, q, addr, width))
                return
            count += 1
            p = q
        s.writes
        sl = s.writes_in(0, frame - 1) if frame > 0 else slice(0, 0)
        a, w = s.addr[sl], s.width[sl]
        idx = np.nonzero((a <= addr) & (a + w > addr))[0]
        for j in idx[::-1]:
            r = j + sl.start
            f = int(s.writes['frame'][r])
            if int(s.writes['site'][r]) == site:
                count += 1
                first = f
                continue
            self.summaries.append((addr, site, count, first, frame))
            self.work.append((self.memory, f, None, addr, width, 'before the run'))
            return
        self.summaries.append((addr, site, count, first, frame))
        self.terminal(frame, f'0x{addr:08X}: not written before the run (power-on or save state)', addr)

    # ---- values ----
    def value(self, frame, inv, bpos, fn, name, use_block, mask=None):
        """Follow SSA value NAME; MASK is the bits of it that matter (None: all)."""
        if not name.startswith('%') or mask == 0:
            return
        key = ('v', frame, inv, bpos, name, mask)
        if key in self.seen:
            return
        self.seen.add(key)
        ft = self.traces[frame]
        params = [p[1] for p in fn.params]
        if name in params:
            self.param(frame, inv, fn, params.index(name), mask)
            return
        d = fn.defs.get(name)
        if d is None:
            return
        # the instance of the defining block
        if d.block is use_block:
            dpos = bpos
        else:
            dpos = ft.last_instance(inv, d.block.site if d.block.index else fn.site, bpos, d.block.index == 0)
            if dpos is None:
                return
        op = d.opcode
        if d.site is not None:
            p = ft.event_in(inv, d.site, dpos)
            if p is None:
                return
            row = ft.rows[p]
            k = int(ft.kind[p])
            if k == K_LOAD:
                addr, width = int(row['a']) & 0x0FFFFFFF, 1 << ((int(row['a']) >> 28) & 3)
                v = int(row['b'])
            elif k == K_WIDE:
                addr, width, v = int(row['a']) & 0x0FFFFFFF, 8, int(row['b']) | int(row['c']) << 32
            else:
                addr, width, v = int(row['a']), int(row['b']) & 0x7FFFFFFF, None
            runs = byte_runs(mask, width)
            first = runs[0] if runs else (0, width)
            self.add(frame, p, inv, fn, d, 'load', value=v, addr=addr + first[0], width=first[1])
            for o, n in runs:
                self.work.append((self.memory, frame, p, addr + o, n, 'load'))
        elif op == 'phi':
            prev = ft.prev_block(inv, dpos)
            pred = self.label_of(fn, prev[1]) if prev else None
            for val, lab in re.findall(r'\[\s*([^,\]]+),\s*%([-\w.$]+)\s*\]', d.text):
                if lab == pred:
                    self.work.append((self.value, frame, inv, prev[0] if prev else dpos, fn, val.strip(), fn.by_label.get(lab), mask))
            return
        elif op == 'call':
            self.add(frame, dpos, inv, fn, d, 'call')
            child = self.child_for(ft, inv, dpos, d)
            if child is not None:
                cinv, cfn = child
                end = ft.exit.get(cinv, len(ft.rows))
                last = ft.block_at(cinv, end)
                if last:
                    blk = self.facts.site(last[1])
                    blk = blk if isinstance(blk, Block) else cfn.blocks[0]
                    ret = blk.insts[-1].text
                    m = re.match(r'\s*ret \S+ (.*)$', ret)
                    if m:
                        self.work.append((self.value, frame, cinv, last[0], cfn, m[1].split(',')[0].strip(), blk, mask))
                return
            for n in operands(d.text, d.dest):
                self.work.append((self.value, frame, inv, dpos, fn, n, d.block))
        else:
            self.add(frame, dpos, inv, fn, d, 'compute')
            for n, m in demand(d.text, d.dest, mask):
                self.work.append((self.value, frame, inv, dpos, fn, n, d.block, m))
        self.control(frame, inv, dpos, fn, d.block)

    def label_of(self, fn, site):
        if site == fn.site:
            return fn.blocks[0].label
        x = self.facts.site(site)
        return x.label if isinstance(x, Block) else None

    def child_for(self, ft, inv, bpos, call):
        """(child inv, Function) entered by CALL in the block instance at BPOS."""
        m = re.search(r'@([-\w.$]+)\(', call.text)
        callee = m[1] if m else None
        calls = [i for i in call.block.insts if i.opcode == 'call' or ' call ' in i.text]
        nxt = None
        ev = ft.block_ev.get(inv, [])
        for p, s in ev:
            if p > bpos:
                nxt = p
                break
        kids = [(p, c, s) for p, c, s in ft.children.get(inv, []) if p > bpos and (nxt is None or p < nxt)]
        ki = 0
        for ci in calls:
            m2 = re.search(r'@([-\w.$]+)\(', ci.text)
            name = m2[1] if m2 else None
            if ki >= len(kids):
                break
            kfn = self.facts.site(kids[ki][2])
            if isinstance(kfn, Function) and (kfn.name == name or name is None):
                if ci is call:
                    return kids[ki][1], kfn
                ki += 1
            elif ci is call:
                return None
        return None

    def param(self, frame, inv, fn, k, mask=None):
        ft = self.traces[frame]
        ent = ft.enter.get(inv)
        if not ent:
            self.terminal(frame, f'argument {k + 1} of {fn.name} (entered before this frame)', None)
            return
        epos, _, parent = ent
        if parent >= 0x80000000 or parent not in ft.enter:
            self.terminal(frame, f'argument {k + 1} of {fn.name} (called from the port: interrupt or host)', None)
            return
        pblock = ft.block_at(parent, epos)
        pfn = self.facts.function_of(ft.enter[parent][1])
        blk = self.facts.site(pblock[1])
        blk = pfn.blocks[0] if isinstance(blk, Function) else blk
        for ci in blk.insts:
            if re.search(r'@' + re.escape(fn.name) + r'\(', ci.text):
                child = self.child_for(ft, parent, pblock[0], ci)
                if child and child[0] == inv:
                    args = call_operands(ci.text)
                    if k < len(args):
                        self.add(frame, pblock[0], parent, pfn, ci, 'call')
                        self.work.append((self.value, frame, parent, pblock[0], pfn, args[k], blk, mask))
                    return
        self.terminal(frame, f'argument {k + 1} of {fn.name} (indirect call)', None)

    def control(self, frame, inv, bpos, fn, block):
        key = ('c', frame, inv, bpos)
        if key in self.seen:
            return
        self.seen.add(key)
        deps = control_deps(fn).get(block.label)
        if not deps:
            return
        ft = self.traces[frame]
        best = None
        for p, s in ft.block_ev.get(inv, []):
            if p >= bpos:
                break
            lab = self.label_of(fn, s)
            if lab in deps:
                best = (p, lab)
        if not best:
            return
        b = fn.by_label[best[1]]
        term = b.insts[-1]
        self.add(frame, best[0], inv, fn, term, 'decides')
        cond = branch_condition(term.text)
        d = fn.defs.get(cond) if cond else None
        if d is not None and re.search(r'icmp (?:eq|ne) ptr \S+, null', d.text):
            return   # a null check: where the pointer came from does not explain the value
        if cond:
            self.work.append((self.value, frame, inv, best[0], fn, cond, b))

    # ---- results ----
    def add(self, frame, pos, inv, fn, inst, role, value=None, note=None, addr=None, width=None):
        n = Node(frame, pos, inv, fn, inst, role, value, note, addr, width)
        self.nodes.append(n)
        return n

    def terminal(self, frame, text, addr):
        self.terminals.append((frame, text, addr))


# ---- IR text helpers ---------------------------------------------------------
def operands(text, dest):
    body = text.split(' = ', 1)[1] if dest and ' = ' in text else text
    body = LABEL.sub('', body)
    return [n for n in SSA.findall(body) if n != dest]


def byte_runs(mask, width):
    """Contiguous (offset, length) runs of the bytes of a WIDTH-byte value that MASK touches."""
    if mask is None:
        return [(0, width)]
    runs, start = [], None
    for i in range(width):
        on = (mask >> (8 * i)) & 0xFF
        if on and start is None:
            start = i
        if not on and start is not None:
            runs.append((start, i - start))
            start = None
    if start is not None:
        runs.append((start, width - start))
    return runs


BINOP = re.compile(r'(add|sub|mul|and|or|xor|shl|lshr|ashr|udiv|sdiv|urem|srem)\b(?:\s+(?:nuw|nsw|exact|disjoint))*'
                   r'\s+i(\d+)\s+([^,]+),\s*(\S+)')
CAST = re.compile(r'(zext|sext|trunc)\s+(?:nneg\s+)?i(\d+)\s+(\S+)\s+to\s+i(\d+)')
SELECT = re.compile(r'select\s+(?:\w+\s+)*i1\s+([^,]+),\s*\S+\s+([^,]+),\s*\S+\s+(\S+)')


def demand(text, dest, mask):
    """[(operand, mask)]: which bits of each operand decide the MASK bits of DEST (demanded bits).
    A None mask means all bits; operands whose mask becomes 0 are dropped."""
    body = text.split(' = ', 1)[1] if dest and ' = ' in text else text

    def full(bits):
        return (1 << bits) - 1

    def low(m, bits):
        # carries move up: every bit at or below the highest demanded one
        return full(bits) if m is None else full(min(bits, m.bit_length()))

    out = None
    m = BINOP.match(body)
    if m:
        op, bits, a, b = m[1], int(m[2]), m[3].strip(), m[4].strip().rstrip(',')
        mk = full(bits) if mask is None else mask & full(bits)
        const = lambda v: re.fullmatch(r'-?\d+', v) is not None
        if op == 'and':
            if const(b):
                out = [(a, mk & (int(b) & full(bits)))]
            elif const(a):
                out = [(b, mk & (int(a) & full(bits)))]
            else:
                out = [(a, mk), (b, mk)]
        elif op in ('or', 'xor'):
            out = [(a, mk), (b, mk)]
        elif op == 'shl':
            out = [(a, mk >> int(b))] if const(b) else [(a, low(mk, bits)), (b, None)]
        elif op in ('lshr', 'ashr'):
            if const(b):
                k = int(b)
                am = (mk << k) & full(bits)
                if op == 'ashr' and mk >> (bits - k):
                    am |= 1 << (bits - 1)
                out = [(a, am)]
            else:
                out = [(a, None), (b, None)]
        elif op in ('add', 'sub', 'mul'):
            lm = low(mk, bits)
            out = [(a, lm), (b, lm)]
        else:
            out = [(a, None), (b, None)]
    if out is None:
        m = CAST.match(body)
        if m:
            op, src, v, dst = m[1], int(m[2]), m[3], int(m[4])
            mk = full(dst) if mask is None else mask
            if op == 'trunc':
                out = [(v, mk & full(dst))]
            elif op == 'zext':
                out = [(v, mk & full(src))]
            else:
                sm = mk & full(src)
                if mk >> src:
                    sm |= 1 << (src - 1)
                out = [(v, sm)]
    if out is None:
        m = SELECT.match(body)
        if m:
            out = [(m[1].strip(), None), (m[2].strip(), mask), (m[3].strip(), mask)]
    if out is None:
        return [(n, None) for n in operands(text, dest)]
    return [(n, mk) for n, mk in out if n.startswith('%') and mk != 0]


def store_value(text):
    from instrument import parse_store
    m = parse_store('  ' + text.strip())   # facts keep instructions without their indent
    return m[2] if m else None


def call_operands(text):
    from instrument import call_args
    m = re.search(r'@([-\w.$]+)\(', text)
    r = call_args('  ' + text.strip(), m[1]) if m else None
    return [v for _, v in r[2]] if r else []


def branch_condition(text):
    m = re.match(r'\s*br i1 ([^,]+),', text)
    if m:
        return m[1].strip()
    m = re.match(r'\s*switch \S+ ([^,]+),', text)
    return m[1].strip() if m else None


# ---- printing -----------------------------------------------------------------
def c_line(facts, inst):
    """(file, line, text) of an instruction; bus calls without a line borrow their first use's."""
    if not inst.line:
        for n in operands(inst.text, inst.dest):
            d = inst.fn.defs.get(n)
            if d is not None and d.line:
                return facts.source(d)
        blk = inst.block
        for j in blk.insts[inst.index + 1:]:
            if j.line and (not inst.dest or inst.dest in j.text):
                inst = j
                break
        else:
            for j in blk.insts[inst.index + 1:]:
                if j.line:
                    inst = j
                    break
    return facts.source(inst)


def fmt(v, width=4):
    if v is None:
        return '?'
    if width and width <= 4 and v >> (8 * width - 1) & 1:
        return f'{v - (1 << (8 * width))} (0x{v:X})'
    return str(v) if v < 10 else f'{v} (0x{v:X})'


def render(sl, describe=None):
    facts = sl.facts
    describe = describe or (lambda a, w: f'0x{a:08X}')
    out = []
    addr, width, frame = sl.target
    groups = {}
    for n in sl.nodes:
        unit, line, text = c_line(facts, n.inst)
        key = (n.frame, n.inv, unit, line)
        g = groups.get(key)
        if g is None:
            g = groups[key] = {'frame': n.frame, 'pos': n.pos, 'fn': n.fn.name, 'unit': unit, 'line': line,
                               'text': (text or '').strip(), 'loads': [], 'stores': [], 'roles': set(), 'inv': n.inv}
        g['pos'] = min(g['pos'], n.pos)
        g['roles'].add(n.role)
        if n.role == 'load':
            g['loads'].append((n.addr, n.width, n.value))
        elif n.role == 'store':
            g['stores'].append((n.addr, n.width, n.value))
    order = sorted(groups.values(), key=lambda g: (g['frame'], g['pos']), reverse=True)
    for g in order:
        mark = ' (decides)' if g['roles'] == {'decides'} else ''
        out.append(f"frame {g['frame']:6d}  {g['fn']}  {g['unit']}:{g['line']}{mark}")
        if g['text']:
            out.append(f"    {g['text']}")
        vals = []
        for a, w, v in g['stores']:
            vals.append(f"{describe(a, w)} := {fmt(v, w)}")
        for a, w, v in g['loads']:
            vals.append(f"{describe(a, w)} = {fmt(v, w)}")
        if vals:
            out.append('      ' + '; '.join(dict.fromkeys(vals)))
    if sl.summaries:
        out.append('')
        runs = {}
        for a, site, count, first, last in sl.summaries:
            k = (a, site, first)
            if k not in runs or runs[k][2] < count:
                runs[k] = (a, site, count, first, last)
        for a, site, count, first, last in sorted(runs.values(), key=lambda r: -r[4]):
            x = facts.site(site)
            unit, line, text = c_line(facts, x) if isinstance(x, Inst) else ('?', '?', '')
            span = f'frame {first}' if first == last else f'frames {first}..{last}'
            out.append(f"run: `{(text or '').strip()}` ({unit}:{line}) wrote {describe(a, 4)} {count} times in a row"
                       f" ({span}); the slice continues at the store before the run")
    if sl.cycles:
        out.append('')
        seen = {}
        for f, site, a in sl.cycles:
            seen.setdefault(site, []).append(f)
        for site, frames in seen.items():
            x = facts.site(site)
            unit, line, text = c_line(facts, x) if isinstance(x, Inst) else ('?', '?', '')
            out.append(f"earlier cycle: `{(text or '').strip()}` ({unit}:{line}) again at frame "
                       f"{', '.join(map(str, sorted(set(frames))))}: already explained above, not followed")
    if sl.terminals:
        out.append('')
        out.append('inputs:')
        for f, text, a in dict.fromkeys(sl.terminals):
            out.append(f'  frame {f}: {text}')
    if sl.truncated:
        out.append(f'(slice cut at {sl.max_nodes} nodes / {sl.max_frames} traced frames)')
    return '\n'.join(out)

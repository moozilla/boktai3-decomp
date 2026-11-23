"""Flight recorder for the source port: python -m tools.recorder COMMAND ...

  record NAME [runner options]     record a session (tools/recorder/runner.mjs record;
                                   --script F.js | --code JS, --frames N, --save F.sav, --solar N)
  verify NAME                      write log completeness: snapshot k + logged writes = snapshot k+1
  history NAME WHERE [--from F] [--to F] [--limit N]
                                   every write to WHERE: frame, value, the C statement that wrote it
  who-wrote NAME WHERE FRAME       the last write to WHERE at or before FRAME
  at NAME ADDR [FRAME]             what typed object ADDR is at FRAME
  diff RUN_A RUN_B [--both]        first typed divergence of two sessions of one movie, and its slice
  infer SESSION... [--trace-frames N]
                                   evidence and name proposals for placeholder fields
                                   (build/recorder/infer/: proposals.tsv, proposals.md, evidence.json)
  export NAME [--db F] [--sqlite]  typed history to DuckDB (history.duckdb, typed_writes view) or SQLite
  serve NAME [--port 8765]         the Recorder page: screens timeline, typed writes, history, why
  why NAME WHERE FRAME [--nodes N] [--frames N] [--check]
                                   dynamic backward slice: the C statements, with their runtime
                                   values, that produced WHERE's value at the end of FRAME

WHERE is an address (0x0300425C), a global with a field path (gGameState.timer,
gTaskLists[3].head) or an object label at that frame (Player#3.hp). Sessions live in
build/recorder/sessions/NAME (a path works too).
"""
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

import numpy as np  # noqa: E402

from facts import Facts, Function, Block, KIND_NAMES, kind_of, lvalue_text  # noqa: E402
from lift import Types, ObjectMap, join  # noqa: E402
from heap import Namer  # noqa: E402
from session import Session, SESSIONS, ROOT  # noqa: E402


# ---- places ---------------------------------------------------------------
class Place:
    def __init__(self, addr, width, name, leaf=None, signed=False):
        self.addr, self.width, self.name, self.leaf, self.signed = addr, width, name, leaf, signed

    def __str__(self):
        return f'{self.name} (0x{self.addr:08X}, {self.width} byte{"s" if self.width > 1 else ""})'


def leaf_info(types, leaf):
    d = types.resolve(leaf) if leaf is not None else None
    if not d:
        return 4, False
    size = types.sizeof(leaf) or 4
    return min(size, 8) if d.get('k') in ('int', 'enum', 'ptr', 'float') else size, bool(d.get('signed'))


def walk_path(types, t, rest, base=0, ram=None):
    """(address, leaf type) of a C path like .a.b[3]->c from a T at BASE (RAM follows `->`)."""
    off = base
    for m in re.finditer(r'\.(\w+)|\[(\d+)\]|->(\w+)', rest):
        d = types.resolve(t)
        if m[3] is not None:
            if not d or d.get('k') != 'ptr' or ram is None:
                raise ValueError(f'{types.name(t)} is not a pointer')
            p = ram.read(off, 4)
            if not p:
                raise ValueError(f'null pointer before ->{m[3]}')
            off, t = p, d['to']
            d = types.resolve(t)
        if m[2] is not None:
            if d.get('k') != 'array':
                raise ValueError(f'{types.name(t)} is not an array')
            es = types.sizeof(d['of'])
            off += int(m[2]) * es
            t = d['of']
            continue
        field = m[1] or m[3]
        for f in d.get('fields', []) if d else []:
            if f['name'] == field:
                off += f['off']
                t = f['type']
                break
        else:
            raise ValueError(f'no field {field} in {types.name(t)}')
    return off, t


def resolve_place(where, types, objmap_at):
    try:
        return _resolve_place(where, types, objmap_at)
    except ValueError as e:
        raise SystemExit(f'cannot resolve {where!r}: {e}')


def _resolve_place(where, types, objmap_at):
    m = re.fullmatch(r'(0x[0-9A-Fa-f]+|\d+)(?::(\d+))?', where)
    if m:
        addr = int(m[1], 0)
        width = int(m[2]) if m[2] else None
        om = objmap_at()
        label, path, leaf, _ = om.at(addr)
        w, signed = leaf_info(types, leaf)
        return Place(addr, width or w if width or leaf is not None else 1, join(label, path), leaf, signed)
    m = re.match(r'([A-Za-z_]\w*(?:#\d+)?)(.*)$', where)
    name, rest = m[1], m[2]
    if name in types.globals:
        g = types.globals[name]
        om = objmap_at() if '->' in rest else None
        addr, leaf = walk_path(types, g['type'], rest, g['addr'], om.ram if om else None)
        w, signed = leaf_info(types, leaf)
        return Place(addr, w, where, leaf, signed)
    om = objmap_at()
    for a, size, tname, label in om.objects:
        if label == name or label.split('(')[0] == name:
            addr, leaf = walk_path(types, tname, rest, a, om.ram)
            w, signed = leaf_info(types, leaf)
            return Place(addr, w, where, leaf, signed)
    raise SystemExit(f'cannot resolve {where!r}: not an address, a RAM global or an object label at that frame')


# ---- statements ---------------------------------------------------------------
def statement(facts, site):
    """(function, 'file:line', source text, lvalue) of a write site."""
    x = facts.site(int(site))
    if x is None:
        return '?', '?', None, None
    inst = x if not isinstance(x, (Function, Block)) else None
    fn = x if isinstance(x, Function) else x.fn
    if inst is None:
        return fn.name, fn.unit.name, None, None
    unit, line, text = facts.source(inst)
    lv = lvalue_text(text, inst.col) if text else None
    return fn.name, f'{unit}:{line or "?"}', text.strip() if text else None, lv


def fmt_value(v, width, signed):
    if signed and v >> (8 * width - 1):
        sv = v - (1 << (8 * width))
        return f'{sv} (0x{v:0{2 * width}X})'
    return f'{v} (0x{v:0{2 * width}X})' if v > 9 else str(v)


def writes_to(sess, place, first=0, last=None):
    """Write rows (indices) that touch [addr, addr + width) in frames first..last."""
    sess.writes
    last = len(sess) - 1 if last is None else last
    sl = sess.writes_in(first, last)
    a, w = sess.addr[sl], sess.width[sl]
    hit = (a < place.addr + place.width) & (a + w > place.addr)
    return np.nonzero(hit)[0] + sl.start


def value_after(sess, ram, place):
    v = ram.read(place.addr, min(place.width, 8))
    return v


# ---- commands ---------------------------------------------------------------
def open_session(name):
    p = Path(name)
    if not p.exists():
        p = SESSIONS / name
    if not (p / 'meta.json').exists():
        raise SystemExit(f'no session at {p}')
    return Session(p)


def cmd_record(args, extra):
    out = SESSIONS / args.name if '/' not in args.name else Path(args.name)
    r = subprocess.run(['node', str(HERE / 'runner.mjs'), 'record', str(out), *extra], cwd=ROOT)
    return r.returncode


def cmd_verify(args):
    s = open_session(args.name)
    bad = s.verify()
    print(f'{s.name}: {len(s)} frames, {len(s.writes)} write rows, {len(s.snapshots)} snapshots: '
          + ('write log complete' if not bad else f'{bad} interval(s) differ'))
    return 1 if bad else 0


def cmd_history(args):
    s, facts, types = open_session(args.name), Facts(), Types()
    last = args.to if args.to is not None else len(s) - 1
    place = resolve_place(args.where, types, lambda: Namer(s, facts, types, last))
    print(f'{place}  frames {args.frm}..{last}')
    rows = writes_to(s, place, args.frm, last)
    ram = s.ram_at(args.frm - 1) if args.frm > 0 else None
    prev = value_after(s, ram, place) if ram else None
    shown = 0
    frames = s.writes['frame']
    # replay the rows on a copy of the place's bytes to show the value after each write
    buf = bytearray(ram.bytes(place.addr, place.width)) if ram else bytearray(place.width)
    if prev is not None:
        print(f'  before frame {args.frm}: {fmt_value(prev, place.width, place.signed)}')
    same = 0
    for i in rows:
        a, w, v = int(s.addr[i]), int(s.width[i]), int(s.writes['value'][i])
        for k in range(w):
            o = a + k - place.addr
            if 0 <= o < place.width:
                buf[o] = (v >> (8 * k)) & 0xFF
        now = int.from_bytes(buf, 'little')
        if args.changes and now == prev:
            same += 1
            continue
        fn, loc, text, lv = statement(facts, s.writes['site'][i])
        partial = '' if (a == place.addr and w == place.width) else f' [{w}B at +{a - place.addr}]'
        print(f'  frame {int(frames[i]):6d}  = {fmt_value(now, place.width, place.signed):18s}{partial}  {fn}  {loc}')
        if text:
            print(f'                {text}')
        prev = now
        shown += 1
        if shown >= args.limit:
            print(f'  ... (limit {args.limit}; {len(rows)} writes in all)')
            break
    if args.changes and same:
        print(f'  ({same} writes left the value unchanged)')
    if not len(rows):
        print('  no writes')


def cmd_who_wrote(args):
    s, facts, types = open_session(args.name), Facts(), Types()
    place = resolve_place(args.where, types, lambda: Namer(s, facts, types, args.frame))
    rows = writes_to(s, place, 0, args.frame)
    ram = s.ram_at(args.frame)
    v = value_after(s, ram, place)
    print(f'{place} at the end of frame {args.frame} = {fmt_value(v, place.width, place.signed)}')
    if not len(rows):
        print(f'  not written since frame 0 (value from the first snapshot or the ROM)')
        return
    # the last write covering each byte
    owners = {}
    for i in rows[::-1]:
        a, w = int(s.addr[i]), int(s.width[i])
        for k in range(w):
            o = a + k - place.addr
            if 0 <= o < place.width and o not in owners:
                owners[o] = i
        if len(owners) == place.width:
            break
    for i in sorted(set(owners.values()), reverse=True):
        fn, loc, text, lv = statement(facts, s.writes['site'][i])
        nbytes = sum(1 for x in owners.values() if x == i)
        print(f'  frame {int(s.writes["frame"][i])}: {fn}  {loc}' + (f'  ({nbytes} of {place.width} bytes)' if nbytes < place.width else ''))
        if text:
            print(f'      {text}')
        if lv:
            print(f'      lvalue: {lv}')


def cmd_at(args):
    s, types = open_session(args.name), Types()
    frame = args.frame if args.frame is not None else len(s) - 1
    om = Namer(s, Facts(), types, frame)
    label, path, leaf, base = om.at(int(args.addr, 0))
    om.close()
    print(join(label, path), f'({types.name(leaf)})' if leaf is not None else '', f'object at 0x{base:08X}' if base else '')


def cmd_why(args):
    from slicer import Slicer, render
    s, facts, types = open_session(args.name), Facts(), Types()
    sl = Slicer(s, facts, max_nodes=args.nodes, max_frames=args.frames,
                log=(lambda m: print(m, file=sys.stderr)) if args.verbose else None)
    om = Namer(s, facts, types, args.frame, slicer=sl)
    ram = om.ram
    place = resolve_place(args.where, types, lambda: om)
    v = ram.read(place.addr, min(place.width, 8))
    print(f'why {place} = {fmt_value(v, place.width, place.signed)} at the end of frame {args.frame}')
    print()
    try:
        sl.why(place.addr, place.width, args.frame)
        describe = lambda a, w: om.describe(a, w or 1)  # noqa: E731
        print(render(sl, describe))
    finally:
        sl.close()
    if args.check:
        print()
        check_slice(s, sl, place, args.frame, describe)


def cmd_diff(args):
    """First typed divergence between two sessions of the same movie, and its slice in each."""
    from slicer import Slicer, render
    from session import KEYS
    a, b = open_session(args.a), open_session(args.b)
    facts, types = Facts(), Types()
    n = min(len(a), len(b))
    fa, fb = a.frames[:n], b.frames[:n]
    print(f'diff {a.name} ({len(a)} frames) vs {b.name} ({len(b)} frames)')
    if a.meta.get('wasmSize') != b.meta.get('wasmSize'):
        print(f"  builds differ: wasm {a.meta.get('wasmSize')} vs {b.meta.get('wasmSize')} bytes")
    for k in ('save', 'time', 'script'):
        if a.meta.get(k) != b.meta.get(k):
            print(f'  {k} differs: {a.meta.get(k)!r} vs {b.meta.get(k)!r}')
    inputs = np.nonzero((fa['keys'] != fb['keys']) | (fa['solar'] != fb['solar']))[0]
    if len(inputs):
        f = int(inputs[0])
        print(f"  inputs first differ for frame {f + 1}: keys {fa['keys'][f]:#x} vs {fb['keys'][f]:#x}, "
              f"solar level {fa['solar'][f]} vs {fb['solar'][f]} ({len(inputs)} frames differ in all)")
    hashes = np.nonzero((fa['lo'] != fb['lo']) | (fa['hi'] != fb['hi']))[0]
    if not len(hashes):
        print(f'  RAM identical for all {n} common frames')
        return 0
    F = int(hashes[0])
    print(f'  RAM first differs at the end of frame {F}')
    # the write streams of frame F, aligned row by row
    a.writes, b.writes
    sa, sb = a.writes_in(F, F), b.writes_in(F, F)
    ra = np.stack([a.writes['site'][sa], a.addr[sa], a.writes['value'][sa]], 1)
    rb = np.stack([b.writes['site'][sb], b.addr[sb], b.writes['value'][sb]], 1)
    m = min(len(ra), len(rb))
    neq = np.nonzero((ra[:m] != rb[:m]).any(1))[0]
    i = int(neq[0]) if len(neq) else m
    na = Namer(a, facts, types, F)
    nb = Namer(b, facts, types, F)
    if i < m or len(ra) != len(rb):
        print(f'  write {i} of frame {F} is the first that differs ({len(ra)} vs {len(rb)} writes):')
        for name, rows, sess, nm in (('A', ra, a, na), ('B', rb, b, nb)):
            if i < len(rows):
                site, addr, val = (int(x) for x in rows[i])
                fn, loc, text, lv = statement(facts, site)
                print(f'    {name}: {nm.describe(addr, 1)} := {val}   {fn}  {loc}' + (f'   `{text}`' if text else ''))
            else:
                print(f'    {name}: (no more writes)')
        same_site = i < m and ra[i][0] == rb[i][0] and ra[i][1] == rb[i][1]
        print('    -> ' + ('same statement, different value: the data diverged'
                           if same_site else 'different statements: control flow diverged'))
    # typed fields that differ at the end of frame F
    ram_a, ram_b = na.ram, nb.ram
    diffs = []
    for mem_a, mem_b, base in ((ram_a.ewram, ram_b.ewram, 0x02000000), (ram_a.iwram, ram_b.iwram, 0x03000000)):
        for off in np.nonzero(mem_a != mem_b)[0][:4000]:
            diffs.append(base + int(off))
    fields = {}
    for addr in diffs:
        label, path, _, _ = nb.at(addr, 1)
        fields.setdefault(join(label, path), []).append(addr)
    print(f'  {len(diffs)} bytes in {len(fields)} fields differ at the end of frame {F}:')
    for k, (fld, addrs) in enumerate(fields.items()):
        if k >= args.fields:
            print(f'    ... ({len(fields) - args.fields} more)')
            break
        w = min(len(addrs), 4)
        print(f'    {fld}: {ram_a.read(addrs[0], w)} vs {ram_b.read(addrs[0], w)}')
    # slice the first divergent write in both runs
    if i < m or len(ra) != len(rb):
        for name, rows, sess, nm, sidx in (('B', rb, b, nb, sb), ('A', ra, a, na, sa)):
            if i >= len(rows):
                continue
            site, addr, val = (int(x) for x in rows[i])
            nth = int(np.count_nonzero((rows[:i, 0] == site) & (rows[:i, 1] == addr)))
            sl = Slicer(sess, facts, max_nodes=args.nodes)
            try:
                sl.why_store(F, site, addr, nth)
                print()
                print(f'why, in {name} ({sess.name}): {nm.describe(addr, 1)} := {val} in frame {F}')
                print(render(sl, lambda x, w, nm=nm: nm.describe(x, w or 1)))
            except ValueError as e:
                print(f'  ({name}: {e})')
            finally:
                sl.close()
            if not args.both:
                break
    na.close(); nb.close()
    return 0


def cmd_infer(args):
    from infer import Inferer, OUT
    inf = Inferer([open_session(n) for n in args.names], trace_frames=args.trace_frames,
                  log=lambda m: print(m, file=sys.stderr))
    inf.run()
    rows = inf.report(Path(args.out) if args.out else OUT)
    named = [r for r in rows if r['proposal']]
    print(f'{len(rows)} placeholder fields observed, {len(named)} with proposals -> {args.out or OUT}')
    for r in named[:args.show]:
        print(f"  {r['owner']}.{r['field']} -> {r['proposal']}  ({r['confidence']}, {r['pattern']})")
        for why in r['reasons'][:4]:
            print(f'      {why}')
    return 0


def cmd_export(args):
    from export import export
    export(open_session(args.name), args.db, args.sqlite, log=lambda m: print(m, file=sys.stderr))
    return 0


def check_slice(s, sl, place, frame, describe):
    """Perturb the slice's inputs and cross-frame links in a replay; does the target change?"""
    from emu import Replay

    def target(r):
        return r.value(place.addr, min(place.width, 8))

    print('check (replay with one cause perturbed; the target must change):')
    with Replay(s.path) as r:
        r.seek(frame)
        base = target(r)
        # host solar level behind sensor reads
        solar_frames = [f for f, text, a in sl.terminals if 'solar level' in text]
        if solar_frames:
            f0 = min(solar_frames)
            level = int(s.frames['solar'][max(f0 - 1, 0)])
            other = 40 if level > 128 else 230
            r.seek(max(f0 - 1, 0))
            r.solar(other)
            r.run(frame - max(f0 - 1, 0))
            v = target(r)
            r.solar(None)
            print(f"  host solar level {level} -> {other} from frame {f0}: target {base} -> {v}  "
                  + ('confirmed' if v != base else 'NOT confirmed'))
        # stores in earlier frames whose value reaches the target through memory
        seen, checked = set(), 0
        for n in sorted(sl.nodes, key=lambda n: -n.frame):
            if n.role != 'store' or n.frame >= frame or n.addr is None or n.value is None:
                continue
            if not (0x02000000 <= n.addr < 0x02040000 or 0x03000000 <= n.addr < 0x03008000):
                continue   # the port's stack and data: not meaningful at a frame boundary
            key = (n.frame, n.addr)
            if key in seen or checked >= 10:
                continue
            seen.add(key)
            w = min(n.width or 4, 4)
            r.seek(n.frame)
            now = r.value(n.addr, w)
            results = []
            for flip in (1, 0x80 if w == 1 else 0x8000 if w == 2 else 0x80000000):
                r.seek(n.frame)
                r.poke(n.addr, w, now ^ flip)
                r.run(frame - n.frame)
                v = target(r)
                results.append(v)
                if v != base:
                    break
            ok = any(v != base for v in results)
            checked += 1
            print(f"  {describe(n.addr, w)} at the end of frame {n.frame} ({now} -> {now ^ flip}): target {base} -> {results[-1]}  "
                  + ('confirmed' if ok else 'NOT confirmed (masked or overwritten)'))
        # runs (counters, positions): the value the run had at a frame boundary before the target
        seen_runs = set()
        for a, site, count, first, last in sorted(set(sl.summaries), key=lambda x: -x[4]):
            f = min(last, frame - 1)
            if (a, f) in seen_runs:
                continue
            seen_runs.add((a, f))
            if not (0x02000000 <= a < 0x02040000 or 0x03000000 <= a < 0x03008000) or f < 0:
                continue
            w = min(place.width, 4) if a == place.addr else 4
            r.seek(f)
            now = r.value(a, w)
            r.poke(a, w, now ^ 1)
            r.run(frame - f)
            v = target(r)
            print(f"  {describe(a, w)} (the run) at the end of frame {f} ({now} -> {now ^ 1}): target {base} -> {v}  "
                  + ('confirmed' if v != base else 'NOT confirmed (masked or overwritten)'))
        # stores inside the target frame: replay the frame with the value stored by that very
        # execution of the statement flipped (rec_rt.c perturb), and compare
        ft = sl.traces.get(frame)
        done, inframe = set(), 0
        stores = [n for n in sl.nodes if n.frame == frame and n.role == 'store' and n.inst.site is not None]
        final = max((n.pos for n in stores if n.addr is not None and n.addr <= place.addr < n.addr + (n.width or 1)), default=None)
        for n in sorted(stores, key=lambda n: -n.pos):
            if n.pos == final or n.pos in done or inframe >= 12 or ft is None:
                continue
            done.add(n.pos)
            site = n.inst.site
            nth = int(np.count_nonzero(ft.rows['site'][:n.pos + 1] == site))
            w = min(n.width or 4, 8)
            results = []
            for flip in (1, 1 << (8 * w - 1)):
                r.seek(frame - 1)
                r.perturb(site, nth, flip)
                r.run(1)
                left = r.perturb_left()
                v = target(r)
                results.append(v)
                if v != base or left:
                    break
            inframe += 1
            fn, loc, text, lv = statement(sl.facts, site)
            if left:
                verdict = 'not reached (execution count differs)'
            else:
                verdict = 'confirmed' if any(v != base for v in results) else 'NOT confirmed (masked)'
            print(f"  in frame {frame}: `{text or loc}` (execution {nth}) stored value ^ 0x{flip:X}: target {base} -> {results[-1]}  {verdict}")
        rest = sum(1 for n in sl.nodes if n.frame == frame and n.role != 'store')
        print(f"  ({rest} loads, computations and branches inside frame {frame} are covered through the stores they feed)")


def main(argv=None):
    ap = argparse.ArgumentParser(prog='python -m tools.recorder', description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('record'); p.add_argument('name')
    p = sub.add_parser('verify'); p.add_argument('name')
    p = sub.add_parser('history'); p.add_argument('name'); p.add_argument('where')
    p.add_argument('--from', dest='frm', type=int, default=0); p.add_argument('--to', type=int)
    p.add_argument('--limit', type=int, default=60)
    p.add_argument('--changes', action='store_true', help='only writes that changed the value')
    p = sub.add_parser('who-wrote'); p.add_argument('name'); p.add_argument('where'); p.add_argument('frame', type=int)
    p = sub.add_parser('at'); p.add_argument('name'); p.add_argument('addr'); p.add_argument('frame', type=int, nargs='?')
    p = sub.add_parser('diff'); p.add_argument('a'); p.add_argument('b')
    p.add_argument('--fields', type=int, default=20); p.add_argument('--nodes', type=int, default=200)
    p.add_argument('--both', action='store_true', help='slice the divergent write in both runs')
    p = sub.add_parser('infer'); p.add_argument('names', nargs='+')
    p.add_argument('--trace-frames', type=int, default=6); p.add_argument('--out'); p.add_argument('--show', type=int, default=20)
    p = sub.add_parser('export'); p.add_argument('name'); p.add_argument('--db'); p.add_argument('--sqlite', action='store_true')
    p = sub.add_parser('serve'); p.add_argument('name'); p.add_argument('--port', type=int, default=8765)
    p = sub.add_parser('why'); p.add_argument('name'); p.add_argument('where'); p.add_argument('frame', type=int)
    p.add_argument('--nodes', type=int, default=400); p.add_argument('--frames', type=int, default=24)
    p.add_argument('-v', '--verbose', action='store_true')
    p.add_argument('--check', action='store_true', help='validate the slice by perturbing its causes in a replay')
    args, extra = ap.parse_known_args(argv)
    if args.cmd == 'record':
        return cmd_record(args, extra)
    if args.cmd == 'serve':
        from server import serve
        return serve(args.name, args.port)
    if extra:
        ap.error('unrecognized arguments: ' + ' '.join(extra))
    return {'verify': cmd_verify, 'history': cmd_history, 'who-wrote': cmd_who_wrote, 'at': cmd_at, 'why': cmd_why, 'diff': cmd_diff, 'infer': cmd_infer, 'export': cmd_export}[args.cmd](args) or 0


if __name__ == '__main__':
    sys.exit(main())

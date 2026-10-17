#!/usr/bin/env python3
"""Trace every sprite palette selection back to the code and data that set it.

  paltrace.py                 summary: how each AnimData_SelectPaletteBank call resolves
  paltrace.py sites           one line per call site with its pool entries and evidence
  paltrace.py check           compare the traced (sprite, entry) pairs with the runtime logs

AnimData_SelectPaletteBank(anim, entry) (08215284) is the only way a large
sprite's palette changes from its default. tools/assets/symeval.py gives the
`entry` argument of each call as an expression over the function's incoming
registers, its loads and its call results. This resolves those expressions
across functions:

* an incoming register is replaced by the value each `bl` caller passes
  (symbolically, so a descriptor pointer a caller passes becomes a ROM
  address whose fields are then read from the ROM);
* a load from a ROM address is read from the ROM;
* a load from an actor field (base + offset not in ROM) is the set of
  values the actor's own code stores at that offset: the stores in the
  functions reachable from the same script entry point (its "family": the
  registry routine or native command and everything it calls or installs as a
  callback, not following functions shared by many families);
* Script_GetKeywordValue(L) (sub_0821ABA8) is every value the scripts pass
  as option -L to that entry point (bigpalettes.script_option_values).

The sprite (hash) a call colors is resolved the same way: the
AnimData_Load call on the same anim in the same function, else the loads in
the family on an anim at the same offset. Everything is code or ROM data; a
call whose value depends on something else (game state, RAM tables, script
positional values) is reported as unresolved with the expression it needs.
"""
import bisect
import collections
import functools
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, ".."))
import symeval  # noqa: E402
from symeval import C, show  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(HERE))
KEYWORD = "sub_0821ABA8"
NAMEHASH = "sub_08219BD8"  # the 16-bit hash of a name string (sprite and actor names)
MAXSET = 256
SHARED = 12  # functions reached from more families than this are not followed


@functools.lru_cache(None)
def rom():
    from romlib import load_rom
    return load_rom()


@functools.lru_cache(None)
def program():
    """{func name: {calls, stores, loads, pals, ptrs, addr}} for every Thumb function."""
    out = {}
    for fn in symeval.parse(os.path.join(ROOT, "gen", "code.s")):
        rec = {"calls": [], "stores": [], "loads": [], "pals": [], "ptrs": set(), "tables": set()}
        loads = []

        def on_call(t, st, l, rec=rec, loads=loads):
            rec["calls"].append((t, st))
            if t in symeval.LOAD:
                loads.append((st.get("r0"), st.get("r1")))
                rec["loads"].append((st.get("r0"), st.get("r1")))
            elif t in symeval.SELECT:
                a = st.get("r0")
                h = next((h for x, h in reversed(loads) if repr(x) == repr(a)), None)
                if h is None and loads:
                    h = loads[-1][1]  # the sprite this function loaded last
                rec["pals"].append((a, st.get("r1"), h))

        def on_store(addr, v, size, rec=rec):
            rec["stores"].append((addr, v, size))

        symeval.run(fn, on_call, on_store)
        for ln in fn["lines"]:
            m = re.search(r"@ =(sub_[0-9A-F]{8}|[A-Za-z]\w*)$", ln)
            if m and not m.group(1).startswith("0x"):
                rec["ptrs"].add(m.group(1))
            m = re.search(r"@ =0x(08[0-9A-F]{6})$", ln)
            if m:
                a = int(m.group(1), 16)
                if a & 1:
                    rec["ptrs"].add(f"sub_{a & ~1:08X}")
                else:
                    rec["tables"].add(a)
        out[fn["name"]] = rec
    an = addr_names()
    for rec in out.values():
        # a ROM table the code addresses (state tables, callback lists): the
        # Thumb function pointers at its start
        for a in rec.pop("tables"):
            for o in range(a - 0x08000000, min(a - 0x08000000 + 256, len(rom()) - 4), 4):
                w = struct.unpack_from("<I", rom(), o)[0]
                if not (w & 1 and (w & ~1) in an):
                    break
                rec["ptrs"].add(an[w & ~1])
    names = set(out)
    for rec in out.values():
        rec["ptrs"] &= names
    return out


@functools.lru_cache(None)
def callers():
    """{callee: [(caller, state at the bl)]}."""
    out = collections.defaultdict(list)
    for f, rec in program().items():
        for t, st in rec["calls"]:
            out[t].append((f, st))
    return out


@functools.lru_cache(None)
def addr_names():
    """{address: function name in gen/code.s}."""
    out = {}
    for ln in open(os.path.join(ROOT, "gen", "code.s")):
        m = re.match(r"(\w+): @ 0x([0-9A-F]{8})", ln)
        if m:
            out[int(m.group(2), 16)] = m.group(1)
    return out


@functools.lru_cache(None)
def entries():
    """{func name: (kind, key)} script entry points (tools/assets/script_targets.py)."""
    import script_targets
    an = addr_names()
    return {an[a]: k for a, k in script_targets.roots().items() if a in an}


def _succ(f):
    rec = program().get(f)
    if not rec:
        return set()
    return {t for t, _ in rec["calls"] if t in program()} | rec["ptrs"]


@functools.lru_cache(None)
def families():
    """{entry: set of functions} reachable from each script entry point,
    not passing through functions that many families reach (task system,
    shared helpers); those are members but are not followed."""
    raw = {}
    for e in entries():
        seen, todo = {e}, [e]
        while todo and len(seen) < 4000:
            for s in _succ(todo.pop()):
                if s not in seen:
                    seen.add(s)
                    todo.append(s)
        raw[e] = seen
    count = collections.Counter(f for s in raw.values() for f in s)
    shared = {f for f, n in count.items() if n > SHARED}
    out = {}
    for e in entries():
        seen, todo = {e}, [e]
        while todo:
            f = todo.pop()
            if f in shared and f != e:
                continue
            for s in _succ(f):
                if s not in seen:
                    seen.add(s)
                    todo.append(s)
        out[e] = seen
    return out


@functools.lru_cache(None)
def family_of():
    out = collections.defaultdict(set)
    for e, fs in families().items():
        for f in fs:
            out[f].add(e)
    return out


@functools.lru_cache(None)
def option_values(letter, index=0):
    import bigpalettes
    return bigpalettes.script_option_values(letter, index)


def split(addr):
    """(root, constant offset) of an address expression."""
    if addr and addr[0] == "add" and addr[2] and addr[2][0] == "c":
        return addr[1], addr[2][1]
    if addr and addr[0] == "c":
        return None, addr[1]
    return addr, 0


def subst(v, st):
    """v with incoming registers replaced by the caller's values."""
    if v is None:
        return None
    k = v[0]
    if k == "arg":
        x = st.get(v[1])
        return x if x is not None else ("free",)
    if k == "free":
        return v
    if k == "c" or k == "sym" or k == "sp":
        return v if k != "sp" else None
    if k == "f":
        b = subst(v[1], st)
        return ("f", b, v[2], v[3]) if b is not None else None
    if k == "call":
        return v
    if k == "add":
        return symeval.add(subst(v[1], st), subst(v[2], st))
    if k == "shl":
        return symeval.shl(subst(v[1], st), v[2])
    if k == "shr":
        return symeval.shr(subst(v[1], st), v[2])
    if k == "any":
        return symeval.any_of([subst(x, st) for x in v[1]])
    return None


def namehash(a):
    """sub_08219BD8 on the string at ROM address a."""
    r, o, h = rom(), a - 0x08000000, 0
    while r[o]:
        h = (((h << 5) | (h >> 11)) & 0xFFFF) + r[o] & 0xFFFF
        o += 1
    return h


def has_free(v):
    if not isinstance(v, tuple):
        return False
    if v[0] == "free":
        return True
    return any(has_free(x) for x in v[1:] if isinstance(x, tuple))


def fill(v, i):
    if not isinstance(v, tuple):
        return v
    if v[0] == "free":
        return C(i)
    return tuple(fill(x, i) for x in v)


def has_arg(v):
    if not isinstance(v, tuple):
        return False
    if v[0] == "arg":
        return True
    return any(has_arg(x) for x in v[1:] if isinstance(x, tuple))


class Why(Exception):
    pass


def readrom(a, size):
    r = rom()
    o = a - 0x08000000
    if not 0 <= o < len(r) - 4:
        raise Why(f"load from {a:#x}")
    n = abs(size)
    v = int.from_bytes(r[o:o + n], "little")
    if size < 0 and v & (1 << (8 * n - 1)):
        v -= 1 << (8 * n)
    return v & 0xFFFFFFFF


class Tracer:
    """Resolve expressions in one family's context."""

    def __init__(self, family):
        self.family = family
        self.members = families().get(family, set()) if family else set()
        self.evidence = set()

    def joint(self, exprs, fn, depth=0):
        """[(values tuple, path)] for expressions that may share incoming
        registers: each caller binds them together."""
        if not any(has_arg(e) for e in exprs) and any(has_free(e) for e in exprs):
            return self.table(exprs, fn, depth)
        if not any(has_arg(e) for e in exprs):
            sets = [self.vals(e, fn, depth) for e in exprs]
            out = [()]
            for s in sets:
                out = [o + (x,) for o in out for x in s]
                if len(out) > MAXSET * 4:
                    raise Why("too many combinations")
            return [(o, (fn,)) for o in out]
        cs = [(c, st) for c, st in callers().get(fn, []) if not self.members or c in self.members]
        out, errs = [], []
        if depth > 6:
            errs.append("caller chain too deep")
            cs = []
        for c, st in cs:
            try:
                out += [(v, p + (fn,)) for v, p in self.joint([subst(e, st) for e in exprs], c, depth + 1)]
            except Why as w:
                errs.append(str(w))
        if out:
            return out
        # a callback (no caller passes the registers) or callers that don't
        # bind them: fields of an incoming actor pointer are the family's stores
        try:
            sets = [self.vals(e, fn, depth) for e in exprs]
        except Why as w:
            errs.append(str(w) if cs else f"{w} ({fn} has no caller in the family)")
            raise Why(errs[0] if cs and errs[0] != str(w) else errs[-1])
        res = [()]
        for st in sets:
            res = [o + (x,) for o in res for x in st]
        return [(o, (fn,)) for o in res]

    def table(self, exprs, fn, depth):
        """A sprite table indexed by a value no caller fixes: every entry,
        index 0 up, while the first expression (the hash) names a sprite."""
        import bigpalettes
        known = set(bigpalettes.defaults())
        out = []
        for i in range(256):
            try:
                sets = [self.vals(fill(e, i), fn, depth) for e in exprs]
            except Why:
                break
            if not all(len(x) == 1 for x in sets) or next(iter(sets[0])) & 0xFFFF not in known:
                break
            out.append((tuple(next(iter(x)) for x in sets), (fn,)))
        if not out:
            raise Why("index no caller passes")
        self.evidence.add(f"table entries 0-{len(out) - 1} in {fn}")
        return out

    def vals(self, v, fn, depth=0):
        """Set of concrete values of v (no incoming registers) in fn."""
        if v is None:
            raise Why("value lost in control flow")
        k = v[0]
        if k == "c":
            return {v[1]}
        if k == "any":
            out = set()
            for x in v[1]:
                out |= self.vals(x, fn, depth)
            return self.cap(out)
        if k in ("add", "shl", "shr"):
            a = self.vals(v[1], fn, depth)
            if k == "add":
                b = self.vals(v[2], fn, depth)
                return self.cap({(x + y) & 0xFFFFFFFF for x in a for y in b})
            return self.cap({(x << v[2]) & 0xFFFFFFFF if k == "shl" else x >> v[2] for x in a})
        if k == "free":
            raise Why("value lost in control flow")
        if k == "call":
            if v[1] == NAMEHASH and len(v) > 2 and 0x08000000 <= v[2] < 0x09000000:
                return {namehash(v[2])}
            if v[1] == KEYWORD and len(v) > 2:
                return self.keyword(chr(v[2]), v[3] if len(v) > 3 else 0)
            raise Why(f"result of {v[1]}()")
        if k == "arg":
            raise Why(f"incoming {v[1]}")
        if k == "f":
            base, off = split(v[1])
            if base is None:
                return {readrom(off + v[2], v[3])}
            try:
                bs = self.vals(v[1], fn, depth)
            except Why:
                bs = None
            if bs and all(0x08000000 <= b < 0x09000000 for b in bs):
                return self.cap({readrom(b + v[2], v[3]) for b in bs})
            return self.field(off + v[2], abs(v[3]), depth)
        raise Why(f"{show(v)}")

    def keyword(self, letter, index=0):
        if not self.family:
            raise Why(f"option -{letter} outside a script entry point")
        kind, key = entries()[self.family]
        if kind != "registry":
            raise Why(f"option -{letter} of command {key}")
        vs = option_values(letter, index).get(int(key, 16))
        if not vs:
            raise Why(f"no script passes -{letter} to {key}")
        self.evidence.add(f"-{letter} of {key}")
        return set(vs)

    def field(self, off, size, depth):
        """Values the family's code stores at actor offset off."""
        if not self.members:
            raise Why(f"field +{off:#x} outside a script entry point")
        if depth > 4:
            raise Why("field chain too deep")
        out, errs, n = set(), [], 0
        for f in sorted(self.members):
            for addr, val, sz in program()[f]["stores"]:
                root, o = split(addr)
                if o != off or sz != size or root is None or root[0] == "sp":
                    continue
                n += 1
                try:
                    for vs, _ in self.joint([val], f, depth + 1):
                        out.add(vs[0] & ((1 << (8 * size)) - 1))
                    self.evidence.add(f"+{off:#x} set by {f}")
                except Why as w:
                    errs.append(f"+{off:#x} set by {f}: {w}")
        if errs:
            raise Why(errs[0])
        if not n:
            raise Why(f"nothing in the family stores +{off:#x}")
        return self.cap(out)

    def cap(self, s):
        if len(s) > MAXSET:
            raise Why(f"{len(s)} values")
        return s


def anim_hashes(tr, anim, fn):
    """Hashes loaded on an anim at the same offset as `anim` in the family,
    else every hash the family loads if that is a single sprite."""
    _, off = split(anim) if anim else (None, None)
    same, every, errs = set(), set(), []
    for f in sorted(tr.members) or [fn]:
        for a, h in program()[f]["loads"]:
            try:
                vs = {v[0] & 0xFFFF for v, _ in tr.joint([h], f)}
            except Why as w:
                errs.append(f"sprite loaded by {f}: {w}")
                continue
            every |= vs
            if a is not None and split(a)[1] == off:
                same |= vs
    if same:
        return same
    if len(every) == 1:
        return every
    if every:
        raise Why(f"anim not matched among {len(every)} sprites the family loads")
    raise Why(errs[0] if errs else "no sprite loaded in the family")


def trace():
    """[{func, entry expr, families: {family: {(hash, pool)}}, why}] per call site."""
    import bigpalettes
    known = set(bigpalettes.defaults())
    out = []
    for f, rec in program().items():
        for anim, pal, h in rec["pals"]:
            site = {"func": f, "expr": show(pal), "hash": show(h), "pairs": set(), "why": [], "via": set()}
            fams = sorted(family_of().get(f, ())) or [None]
            for fam in fams:
                tr = Tracer(fam)
                try:
                    if h is not None:
                        res = tr.joint([h, pal], f)
                        pairs = {(x & 0xFFFF, p & 0xFFFF) for (x, p), _ in res}
                    else:
                        ps = {vs[0] & 0xFFFF for vs, _ in tr.joint([pal], f)}
                        hs = anim_hashes(tr, anim, f)
                        pairs = {(x, p) for x in hs for p in ps}
                    pairs = {(x, p) for x, p in pairs if x in known}
                    if not pairs:
                        raise Why("no known sprite")
                    site["pairs"] |= pairs
                    site["via"] |= {(fam, e) for e in tr.evidence} if tr.evidence else {(fam, "code")}
                except Why as w:
                    site["why"].append((fam, str(w)))
            out.append(site)
    return out


def closure(sites=None):
    """(closed, open, unresolved loads): sprites whose every palette is
    accounted for. A sprite is closed when each family that loads it has
    every palette call resolved (so the traced pairs are all of its
    variants, none meaning default only), and it is not loaded by code
    outside the script entry points. A load whose sprite can't be resolved
    could be any sprite; those are counted, not assumed away."""
    sites = sites or trace()
    bad = collections.defaultdict(bool)
    for s in sites:
        for fam in family_of().get(s["func"], {None}):
            if not s["pairs"] or any(f == fam for f, _ in s["why"]):
                bad[fam] = True
    loaded = collections.defaultdict(set)
    unresolved = []
    for f, rec in program().items():
        for a, h in rec["loads"]:
            for fam in sorted(family_of().get(f, ())) or [None]:
                try:
                    for v, _ in Tracer(fam).joint([h], f):
                        loaded[v[0] & 0xFFFF].add(fam)
                except Why:
                    unresolved.append((f, fam))
    import bigpalettes
    closed, opened = set(), set()
    for h in bigpalettes.defaults():
        fams = loaded.get(h)
        if fams and None not in fams and not any(bad[x] for x in fams):
            closed.add(h)
        else:
            opened.add(h)
    return closed, opened, unresolved


def main():
    sites = trace()
    arg = sys.argv[1] if len(sys.argv) > 1 else ""
    if arg == "closure":
        closed, opened, unresolved = closure(sites)
        pairs = collections.defaultdict(set)
        for s in sites:
            for h, p in s["pairs"]:
                pairs[h].add(p)
        import bigpalettes
        d = bigpalettes.defaults()
        only = sorted(h for h in closed if not pairs[h] - {d[h]})
        print(f"{len(closed)} sprites fully accounted for ({len(only)} default only), {len(opened)} not;"
              f" {len(unresolved)} sprite loads unresolved in {len({f for f, _ in unresolved})} functions")
        return
    if arg == "sites":
        for s in sites:
            st = f"{len(s['pairs'])} pairs" if s["pairs"] else "unresolved"
            print(f"{s['func']}  {s['expr']}  hash {s['hash']}  {st}")
            if s["pairs"]:
                print("    " + " ".join(f"{h:04X}:{p}" for h, p in sorted(s["pairs"])[:24]))
            if not s["pairs"]:
                for fam, w in s["why"][:2]:
                    print(f"    {fam or '-'}: {w}")
        return
    if arg == "check":
        import bigpalettes
        rt = bigpalettes.runtime()
        rtp = {(h, p) for h, ps in rt.items() for p in ps}
        tp = {pr for s in sites for pr in s["pairs"]}
        d = bigpalettes.defaults()
        tpv = {(h, p) for h, p in tp if d.get(h) != p}
        rtv = {(h, p) for h, p in rtp if d.get(h) != p}
        print(f"traced variant pairs {len(tpv)}, runtime variant pairs {len(rtv)}, both {len(tpv & rtv)}")
        print("runtime only:", " ".join(f"{h:04X}:{p}" for h, p in sorted(rtv - tpv)))
        return
    n = len(sites)
    res = sum(1 for s in sites if s["pairs"])
    print(f"{n} palette calls, {res} resolved to (sprite, entry) pairs")
    c = collections.Counter()
    for s in sites:
        if not s["pairs"]:
            c[re.sub(r"sub_[0-9A-F]{8}|[0-9A-F]{4}\b|0x[0-9a-f]+", "N", s["why"][0][1] if s["why"] else "?")] += 1
    for k, v in c.most_common():
        print(f"  {v:3}  {k}")


if __name__ == "__main__":
    main()

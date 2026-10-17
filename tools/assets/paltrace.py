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


def _cached(name, build):
    """build() pickled under build/cache/, keyed by gen/code.s and these tools."""
    import hashlib
    import pickle
    key = hashlib.sha1()
    for f in (os.path.join(ROOT, "gen", "code.s"), __file__, symeval.__file__):
        st = os.stat(f)
        key.update(f"{f}:{st.st_size}:{st.st_mtime_ns}".encode())
    path = os.path.join(ROOT, "build", "cache", f"paltrace_{name}_{key.hexdigest()[:12]}.pkl")
    if os.path.exists(path):
        with open(path, "rb") as f:
            return pickle.load(f)
    out = build()
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path + ".tmp", "wb") as f:
        pickle.dump(out, f)
    os.replace(path + ".tmp", path)
    return out


@functools.lru_cache(None)
def program():
    """{func name: {calls, stores, loads, pals, ptrs}} for every Thumb function."""
    return _cached("program", _program)


def _program():
    """Two passes: the second applies what each callee stores through its
    pointer arguments (from the first) to structures callers fill on the stack."""
    funcs = symeval.parse(os.path.join(ROOT, "gen", "code.s"))
    first = _evaluate(funcs, None)
    summ = {f: symeval.arg_stores(r["stores"]) for f, r in first.items()}
    return _evaluate(funcs, {f: v for f, v in summ.items() if v})


def _evaluate(funcs, summaries):
    out = {}
    for fn in funcs:
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

        symeval.run(fn, on_call, on_store, summaries)
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
    return _cached("families", _families)


def _families():
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
        return v
    if k == "f":
        b = subst(v[1], st)
        if b is not None and b[0] == "sp":
            # a structure in the caller's frame: what the caller stored there
            x = st.get("stack", {}).get(b[1] + v[2])
            return x if x is not None else ("free",)
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


def options(exprs):
    """{(letter, index)} of the script options the expressions read."""
    out = set()

    def walk(v):
        if isinstance(v, tuple):
            if v and v[0] == "call" and v[1] == KEYWORD and len(v) > 2:
                out.add((v[2], v[3] if len(v) > 3 else 0))
            for x in v[1:]:
                walk(x)
    for e in exprs:
        walk(e)
    return out


def put(v, c):
    """v with option reads replaced by the constants in c ({repr(option): value})."""
    if not isinstance(v, tuple):
        return v
    if v and v[0] == "call" and v[1] == KEYWORD and len(v) > 2:
        k = repr((v[2], v[3] if len(v) > 3 else 0))
        if k in c:
            return C(c[k])
    return tuple(put(x, c) for x in v)


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


@functools.lru_cache(None)
def level(root):
    """How many pointer loads lead to an object: 0 for an incoming pointer or
    a call result (a new object), 1 for a pointer read from a field, ..."""
    n = 0
    while root is not None and root[0] == "f":
        n += 1
        root = split(root[1])[0]
    return n if root is not None and root[0] in ("arg", "call") else None


@functools.lru_cache(None)
def stores_at():
    """{(offset, size, level): [(function, value)]} of stores through an object pointer."""
    out = collections.defaultdict(list)
    for f, rec in program().items():
        for addr, val, sz in rec["stores"]:
            root, o = split(addr)
            lv = level(root)
            if lv is not None:
                out[o, sz, lv].append((f, val))
    return out


@functools.lru_cache(None)
def loads_in():
    """[(function, anim offset, hash expr)] of every AnimData_Load."""
    return [(f, split(a)[1] if a is not None else None, h)
            for f, rec in program().items() for a, h in rec["loads"]]


class Tracer:
    """Resolve expressions in one family's context."""
    _memo = {}

    def __new__(cls, family):
        t = cls._memo.get(family)
        if t is None:
            t = super().__new__(cls)
            t.family = family
            t.members = families().get(family, set()) if family else set()
            t.fields, t.hashes = {}, None
            cls._memo[family] = t
        t.evidence = set()
        return t

    def __init__(self, family):
        pass

    def joint(self, exprs, fn, depth=0):
        """[(values tuple, path)] for expressions that may share incoming
        registers: each caller binds them together."""
        if not any(has_arg(e) for e in exprs) and any(has_free(e) for e in exprs):
            return self.table(exprs, fn, depth)
        if not any(has_arg(e) for e in exprs) and len(exprs) > 1 and len(options(exprs)) > 1:
            return self.together(exprs, fn, depth)
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

    def together(self, exprs, fn, depth):
        """Expressions reading several script options: the values each
        script call passes together (a sprite -t with its palette -c)."""
        import bigpalettes
        if not self.family or entries()[self.family][0] != "registry":
            raise Why("options outside a registry entry point")
        key = int(entries()[self.family][1], 16)
        opts = sorted(options(exprs))
        out = set()
        for call in bigpalettes.script_option_calls().get(key, []):
            combos = [{}]
            for o in opts:
                vs = (set().union(*[v for (lt, _), v in call.items() if lt == chr(o[0])]) or {0}
                      if o[1] == "*" else call.get((chr(o[0]), o[1]), {0}))
                combos = [dict(c, **{repr(o): v}) for c in combos for v in vs]
            for c in combos:
                es = [put(e, c) for e in exprs]
                sets = [self.vals(e, fn, depth) for e in es]
                prod = [()]
                for st in sets:
                    prod = [p + (x,) for p in prod for x in st]
                out |= set(prod)
        if not out:
            raise Why(f"no script calls {entries()[self.family][1]}")
        self.evidence.add(f"options {' '.join('-' + chr(o[0]) for o in opts)} of {entries()[self.family][1]} per call")
        return [(o, (fn,)) for o in sorted(out)]

    def table(self, exprs, fn, depth):
        """A sprite table indexed by a value no caller fixes: every entry,
        index 0 up, while the first expression (the hash) names a sprite."""
        import bigpalettes
        known = set(bigpalettes.defaults())
        if not has_free(exprs[0]):
            raise Why("index no caller passes")  # the sprite doesn't bound it
        out = []
        for i in range(257):
            try:
                sets = [self.vals(fill(e, i), fn, depth) for e in exprs]
            except Why:
                break
            if not all(len(x) == 1 for x in sets) or next(iter(sets[0])) & 0xFFFF not in known:
                break
            out.append((tuple(next(iter(x)) for x in sets), (fn,)))
        if not out or len(out) > 256:
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
            lv = level(base)
            if lv is None:
                raise Why(f"load from {show(v)}")
            return self.field(off + v[2], abs(v[3]), depth, lv)
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

    def field(self, off, size, depth, lv=0):
        """Values the family's code stores at offset off of an object reached
        through lv pointer loads."""
        if not self.members:
            raise Why(f"field +{off:#x} outside a script entry point")
        if depth > 4:
            raise Why("field chain too deep")
        k = (off, size, lv)
        if k not in self.fields:
            out, errs, ev, n = set(), [], set(), 0
            for f, val in stores_at().get(k, ()):
                if f not in self.members:
                    continue
                n += 1
                try:
                    for vs, _ in self.joint([val], f, depth + 1):
                        out.add(vs[0] & ((1 << (8 * size)) - 1))
                    ev.add(f"+{off:#x} set by {f}")
                except Why as w:
                    errs.append(f"+{off:#x} set by {f}: {w}")
            if errs and any("too deep" in e for e in errs):
                raise Why(errs[0])  # depends on where it was asked from: not kept
            if errs:
                self.fields[k] = Why(errs[0])
            elif not n:
                self.fields[k] = Why(f"nothing in the family stores +{off:#x}")
            elif len(out) > MAXSET:
                self.fields[k] = Why(f"{len(out)} values")
            else:
                self.fields[k] = (out, ev)
        r = self.fields[k]
        if isinstance(r, Why):
            raise r
        self.evidence |= r[1]
        return r[0]

    def cap(self, s):
        if len(s) > MAXSET:
            raise Why(f"{len(s)} values")
        return s


def anim_hashes(tr, anim, fn):
    """Hashes loaded on an anim at the same offset as `anim` in the family,
    else every hash the family loads if that is a single sprite."""
    _, off = split(anim) if anim else (None, None)
    if tr.hashes is None:
        tr.hashes, tr.hash_errs = [], []
        for f, o, h in loads_in():
            if f not in tr.members and not (not tr.members and f == fn):
                continue
            try:
                tr.hashes.append((o, {v[0] & 0xFFFF for v, _ in tr.joint([h], f)}))
            except Why as w:
                tr.hash_errs.append(f"sprite loaded by {f}: {w}")
        if not tr.members:
            hs, tr.hashes = tr.hashes, None  # outside a family: per function
        else:
            hs = tr.hashes
    else:
        hs = tr.hashes
    same, every = set(), set()
    for o, vs in hs:
        every |= vs
        if o == off:
            same |= vs
    if same:
        return same
    if len(every) == 1:
        return every
    if every:
        raise Why(f"anim not matched among {len(every)} sprites the family loads")
    raise Why(tr.hash_errs[0] if tr.hash_errs else "no sprite loaded in the family")


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
        # sprite loads: the hashes each function loaded at runtime against
        # the hashes traced for its AnimData_Load calls
        import csv
        import glob
        an = addr_names()
        starts = sorted(an)
        traced = collections.defaultdict(set)
        for f, _, h in loads_in():
            for fam in sorted(family_of().get(f, ())) or [None]:
                try:
                    traced[f] |= {v[0] & 0xFFFF for v, _ in Tracer(fam).joint([h], f)}
                except Why:
                    pass
        seen = collections.defaultdict(set)
        for path in glob.glob(os.path.join(ROOT, "build", "centaur", "**", "sprite_loads.tsv"), recursive=True):
            for r in csv.DictReader(open(path), delimiter="\t"):
                a = int(r["caller"], 16)
                seen[an[starts[bisect.bisect_right(starts, a) - 1]]].add(int(r["hash"], 16))
        ok = sum(1 for f in seen if seen[f] <= traced.get(f, set()))
        print(f"sprite loads: {sum(len(v) for v in seen.values())} (function, sprite) pairs at runtime in {len(seen)} functions;"
              f" all traced in {ok}")
        for f in sorted(seen):
            miss = seen[f] - traced.get(f, set())
            if miss:
                print(f"  {f}: not traced " + " ".join(f"{h:04X}" for h in sorted(miss)[:12]))
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

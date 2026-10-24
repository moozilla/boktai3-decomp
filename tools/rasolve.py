#!/usr/bin/env python3
"""Solve a register-allocation mismatch inside the compiler, then name the C change.

  rasolve.py FILE.c [FUNC] [--depth N] [--beam N]

FILE.c may be a multi-function unit: the target is isolated automatically
(tools/isolate.py ensure) and build/agbcc-instr is built on first use. Uses an instrumented copy
of agbcc (build/agbcc-instr, made by tools/agbcc-instr.sh) that can add to a
pseudo's allocation priority (AGBCC_PRI), pin a pseudo to a hard register
(AGBCC_FORCE) and dump every pseudo's priority, lifetime and register before
reload (AGBCC_DUMPPRI). With none of those set it emits the same code as
agbcc. The real build never uses it.

1. Force the registers the ROM uses (from tools/regdiag.py) onto the
   pseudos that differ. If the bytes then match, the C is right except for
   allocation, and the problem is only which pseudo outranks which.
   If they do, try a register preference on each global pseudo
   (AGBCC_PREF): a match names a missing copy or a value computed into
   another variable (agbccsrc.py inline-result), not a priority.
2. Search priority adjustments, inside the compiler, that make the bytes
   match: each step moves one pseudo just above or below another pseudo it
   competes with. Every try is one real compile, so the answer is exact.
3. Print the adjustments as inequalities between named source values, with
   the uses and lifetime that decide them, so the C edit is one deliberate
   change instead of a blind search.
"""
import argparse
import collections
import contextlib
import io
import os
import re
import subprocess
import sys
import tempfile
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402
import check  # noqa: E402
import permute  # noqa: E402
import regdiag  # noqa: E402
import agbserve  # noqa: E402
from romlib import ROOT, load_rom, off  # noqa: E402

INSTR = os.path.join(ROOT, "build", "agbcc-instr", "gcc", "agbcc")


class Pseudo:
    def __init__(self, kind, reg, pri, refs, got, block=None, birth=None, death=None, sugg=0, live=None,
                 calls=0):
        self.kind, self.reg, self.pri, self.refs, self.got = kind, reg, pri, refs, got
        self.block, self.birth, self.death, self.sugg = block, birth, death, sugg
        self.live, self.calls = live, calls

    def life(self):
        return self.death - self.birth if self.kind == "L" else self.live


def _cleanup(path):
    """Remove a work directory when the process exits (it holds only
    candidate sources, objects and dumps; set RASOLVE_KEEP=1 to keep it)."""
    import atexit
    import shutil
    if not os.environ.get("RASOLVE_KEEP"):
        atexit.register(shutil.rmtree, path, True)


class Compiler:
    """Compiles variants of one function through the agbcc fork server
    (tools/agbserve.py) and counts the bytes that differ from the ROM."""

    def __init__(self, src, func, lines=True):
        self.func = func
        rel = os.path.relpath(src, ROOT)
        permute.prepare_source(rel, func)  # requires one ROM function
        # With line markers and -g, reload choices carry source lines
        # (-g does not change the code).
        self.pre = build.sh(build.CPP + ["-nostdinc", "-undef"] + build.DEFINES +
                            ["-I", "include", "-iquote", ".", rel]) if lines else None
        self.symbols = check.elf_symbols()
        self.address = self.symbols[func][0] & ~1
        by_label, addrs, sizes = check.inventory()
        key = by_label.get(func, func)
        self.size = sizes.get(key)
        self.rom = load_rom(verify=False)
        self.work = tempfile.mkdtemp(prefix="rasolve-", dir=os.path.join(ROOT, "build"))
        _cleanup(self.work)
        self.flags = build.cflags_for(rel) + (["-g"] if lines else [])
        self.resolver = agbserve.Resolver(self.symbols, self.address)
        self.server = None
        self.cache = {}
        self.compiles = 0
        self.n = 0

    def __getstate__(self):  # workers make their own servers
        d = dict(self.__dict__)
        d["server"] = None
        d["servers"] = {}
        return d

    def _asm(self, text, env):
        parts = agbserve.split_source(text, self.func)
        if parts is None:
            return agbserve.compile_plain(text, self.flags, INSTR, dict(os.environ, **env))
        prefix, suffix = parts
        # a few servers, one per declarations prefix (edits to inline helpers
        # change it); the least recently used one is closed
        servers = self.__dict__.setdefault("servers", {})
        srv = servers.pop(prefix, None)
        if srv is None:
            srv = agbserve.Server(prefix, self.flags, INSTR, self.work)
            while len(servers) >= 3:
                old = next(iter(servers))
                servers.pop(old).close()
        servers[prefix] = srv
        self.server = srv
        return srv.compile(suffix, env)

    def run(self, pri=None, force=None, dump=False, spill=None, env=None):
        """(distance from the ROM in halfwords, dump or None) for one compile."""
        key = (self.pre, tuple(sorted((pri or {}).items())), tuple(sorted((force or {}).items())), dump,
               tuple(sorted((spill or {}).items())), tuple(sorted((env or {}).items())))
        if key in self.cache:
            self.last_bytes = self.__dict__.setdefault("bytes_cache", {}).get(key)
            return self.cache[key]
        self.last_bytes = None
        env = dict(env or {})
        if spill:
            env["AGBCC_SPILL"] = ",".join(f"{k}:{d}" for k, d in spill.items() if d)
        if pri:
            env["AGBCC_PRI"] = ",".join(f"{r}:{d}" for r, d in pri.items() if d)
        if force:
            env["AGBCC_FORCE"] = ",".join(f"{r}:{h}" for r, h in force.items())
        self.n += 1
        dump_path = os.path.join(self.work, f"dump{self.n & 3}.txt")
        if dump:
            if os.path.exists(dump_path):
                os.remove(dump_path)
            env["AGBCC_DUMPPRI"] = dump_path
        asm = self._asm(self.pre, env)
        self.last_asm = asm
        self.compiles += 1
        result = (None, None)
        if asm is not None:
            obj = self.resolver.assemble(asm, self.work)
            mine, missing = self.resolver.bytes_of(obj, self.func) if obj else (None, ["as"])
            if mine is not None and not missing:
                self.last_bytes = bytes(mine)  # the function's code, for tools/semcheck.py
                n = self.size or len(mine)
                orig = self.rom[off(self.address):off(self.address) + n]
                diff = distance(orig, mine)
                dumped = parse_dump(open(dump_path).read(), self.func) if dump and os.path.exists(dump_path) else None
                result = (diff, dumped)
        if len(self.cache) < 20000:
            self.cache[key] = result
            self.__dict__.setdefault("bytes_cache", {})[key] = self.last_bytes
        return result


def distance(orig, mine):
    """Halfwords that differ once the two are aligned (an inserted or
    deleted instruction counts once, not as a shift of everything after it);
    0 exactly when the bytes are equal."""
    if orig == mine:
        return 0
    import difflib
    a = [orig[i:i + 2] for i in range(0, len(orig), 2)]
    b = [mine[i:i + 2] for i in range(0, len(mine), 2)]
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    d = sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != "equal")
    return max(d, 1)


class Dump(dict):
    """Pseudos by number, plus reloads: [(k, uid, hard reg, line, file)]."""
    reloads = ()


def parse_dump(text, func):
    out = Dump()
    reloads = []
    for ln in text.splitlines():
        p = ln.split()
        if len(p) < 2 or p[1] != func:
            continue
        if p[0] == "L":
            b, reg, q, pri, birth, death, refs, sugg, phys = map(int, p[2:11])
            out[reg] = Pseudo("L", reg, pri, refs, phys, block=b, birth=birth, death=death, sugg=sugg)
            out[reg].name = p[11] if len(p) > 11 else "-"
        elif p[0] == "G":
            reg, pri, refs, live, calls, got = map(int, p[2:8])
            if reg not in out:  # local-alloc already gave it a register
                out[reg] = Pseudo("G", reg, pri, refs, got, live=live, calls=calls)
                out[reg].name = p[8] if len(p) > 8 else "-"
        elif p[0] == "R":
            k, uid, hard, line = map(int, p[2:6])
            reloads.append((k, uid, hard, line, p[6] if len(p) > 6 else "-", p[7] if len(p) > 7 else "-"))
    out.reloads = reloads
    return out


def competitors(ps, p):
    """Pseudos whose relative priority to P decides P's register."""
    if p.kind == "L":
        return [o for o in ps.values() if o.kind == "L" and o.block == p.block and o.reg != p.reg]
    return [o for o in ps.values() if o.kind == "G" and o.reg != p.reg]


def ensure_instr():
    """Build build/agbcc-instr on first use."""
    if not os.path.exists(INSTR):
        print("building build/agbcc-instr (first use, a few minutes)...", file=sys.stderr, flush=True)
        subprocess.run([os.path.join(ROOT, "tools", "agbcc-instr.sh")], check=False)
    if not os.path.exists(INSTR):
        sys.exit(f"{INSTR} missing and tools/agbcc-instr.sh failed (run tools/setup.sh first?)")


def floor_log2(n):
    return n.bit_length() - 1 if n > 0 else -1


def main():
    if len(sys.argv) == 4 and sys.argv[1] == "--worker":
        worker_main(sys.argv[2], int(sys.argv[3]))
        return
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--depth", type=int, default=6, help="priority changes to combine")
    ap.add_argument("--beam", type=int, default=6)
    ap.add_argument("--seconds", type=int, default=600)
    ap.add_argument("--max-compiles", type=int, default=0,
                    help="stop the search after N compiles (deterministic, unlike --seconds)")
    ap.add_argument("--solutions", type=int, default=12, help="exact solutions to collect")
    ap.add_argument("--search-c", type=int, default=0, metavar="SECONDS",
                    help="then search C (permuter passes, focused on the variables) toward the solution")
    ap.add_argument("-j", type=int, default=max(1, (os.cpu_count() or 2) // 2),
                    help="worker processes (default: half the cores; all of them saturated a 16 GB machine)")
    ap.add_argument("--json-out", help="write the compiler-space solution as targets for translate.py")
    ap.add_argument("--all", action="store_true", help="try every pseudo, not just the involved ones")
    a = ap.parse_args()
    ensure_instr()
    src = os.path.abspath(a.src)
    func = a.func or regdiag.first_function(src)
    import isolate
    src = os.path.join(ROOT, isolate.ensure(src, func))  # multi-function unit: isolated copy
    t0 = time.time()
    with contextlib.redirect_stdout(io.StringIO()):
        an = regdiag.analyze(src, func)
    cc = Compiler(src, func)
    base, ps = cc.run(dump=True)
    wants = {p: regdiag.ALIAS.get(c.most_common(1)[0][0], c.most_common(1)[0][0]) for p, c in an.wants.items()}
    wants = {p: int(r[1:]) for p, r in wants.items() if r.startswith("r")}
    print(f"{func}: {base} differing bytes; {len(an.struct_ops)} structural and "
          f"{sum(1 for x in an.listing if x.startswith('!'))} register differences in regdiag's alignment")
    if base == 0:
        print("already matches")
        return

    # 1. Force the ROM's registers.
    forced, _ = cc.run(force=wants) if wants else (None, None)
    if wants:
        print(f"forcing the ROM's registers on {len(wants)} pseudo(s): {forced} differing bytes")

    # 1b. Forcing works: is a register preference what is missing? global.c find_reg takes a pseudo's
    # preferred register (set_preference: a copy or operation with a value already in a hard register,
    # shared along copies by expand_preferences) before its first free one, so a preference on the
    # pseudo, or on a global it is copied or computed into, can change the register with no change of
    # priority. agbccsrc.py inline-result names one C cause (sub_08137E28).
    prefs = []
    if wants and forced == 0:
        for r, want in wants.items():
            for o in sorted(ps.values(), key=lambda o: (o.reg != r, o.reg)):
                if o.kind != "G" or ps[r].kind != "G":
                    continue
                sc, _ = cc.run(env={"AGBCC_PREF": f"{o.reg}:{want}"})
                if sc == 0:
                    prefs.append((r, o, want))
        if prefs:
            names = ", ".join(f"{o.name if o.name != '-' else 'pseudo'} ({o.reg})" for _, o, _ in prefs[:8])
            r, _, want = prefs[0]
            print(f"  a preference for r{want} matches on any of: {names}"
                  f"{f' and {len(prefs) - 8} more' if len(prefs) > 8 else ''}. The ROM's value shares r{want}"
                  f" with one of them: a copy to or from it, or the same variable (x = H(x, ...):"
                  f" spellings.sp_result_into_arg); AGBCC_WHYREG={r} shows what the allocator saw")
        else:
            print("  no single preference matches")

    # 2. Search priorities and reload rotation, in the compiler.
    involved = set(wants)
    for r, want in wants.items():
        p = ps.get(r)
        if p:
            involved |= {o.reg for o in competitors(ps, p) if o.got == want}
    if a.all or not involved:
        involved = set(ps)
    moves = []
    ks = [x[0] for x in ps.reloads]
    for k, uid, hard, line, file, _ in ps.reloads:
        for d in (1, -1, 2):
            moves.append(("spill", k, d, None))
            # change only this reload's register: undo the shift at the next one
            nxt = [x for x in ks if x > k]
            if nxt:
                moves.append(("spill2", k, d, nxt[0]))
    for r in sorted(involved, key=lambda r: r not in wants):
        p = ps.get(r)
        if not p:
            continue
        for o in competitors(ps, p):
            for delta in (o.pri - p.pri + 1, o.pri - p.pri - 1, o.pri - p.pri):
                if delta:
                    moves.append(("pri", r, delta, o.reg))
    moves = list(dict.fromkeys(moves))
    print(f"searching {len(moves)} moves: reload rotation at {len(ps.reloads)} reloads, "
          f"priorities of {len(involved)} pseudos")

    def evaluate(state):
        pri = {r: d for kind, r, d in state if kind == "pri"}
        spill = {}
        for kind, k, d in state:
            if kind == "spill":
                spill[k] = spill.get(k, 0) + d
            elif kind == "spill2":
                spill[k] = spill.get(k, 0) + d
                after = [x for x in ks if x > k]
                if after:
                    spill[after[0]] = spill.get(after[0], 0) - d
        return cc.run(pri=pri, spill=spill)[0]

    best = (base, ())
    solutions = []
    frontier = [((), base)]
    deadline = time.time() + a.seconds
    for depth in range(a.depth):
        nxt = []
        for state, score in frontier:
            used = {(kind, r) for kind, r, d in state}
            for kind, r, d, o in moves:
                if (kind, r) in used or time.time() > deadline or (a.max_compiles and cc.compiles >= a.max_compiles):
                    continue
                s2 = state + ((kind, r, d),)
                if kind == "spill2":
                    used_k = {x for k2, x, _ in state if k2 in ("spill", "spill2")}
                    if r in used_k:
                        continue
                sc = evaluate(s2)
                if sc is None:
                    continue
                if sc < best[0]:
                    best = (sc, s2)
                if sc < score:
                    nxt.append((s2, sc))
                if sc == 0 and not any({(k, r) for k, r, _ in x} == {(k, r) for k, r, _ in s2}
                                       for x in solutions):
                    solutions.append(s2)
                    if len(solutions) >= a.solutions:
                        break
            if best[0] == 0 and len(solutions) >= a.solutions:
                break
        frontier = sorted(nxt, key=lambda t: t[1])[:a.beam]
        print(f"  depth {depth + 1}: best {best[0]} differing bytes ({cc.compiles} compiles, "
              f"{time.time() - t0:.0f}s)", flush=True)
        if best[0] == 0 or not frontier:
            break

    src_text = open(src).read().splitlines()

    def where(r):
        ls = sorted(an.pseudo_lines.get(r, ()))[:3]
        return "; ".join(f"{x}: {src_text[x - 1].strip()[:60]}" for x in ls if 0 < x <= len(src_text))

    score, state = best
    if len(solutions) > 1:
        print(f"\n{len(solutions)} exact solutions at this depth; the simplest is explained below. Others:")
        for sol in solutions[1:]:
            print("  " + ", ".join(f"{'reload #' if k == 'spill' else 'pseudo '}{r} "
                                   f"{'+' if d > 0 else ''}{d if k == 'spill' else round(d / 10000, 3)}"
                                   + (f" ({ps[r].name})" if k == "pri" and ps[r].name != "-" else "")
                                   for k, r, d in sol))
    if not state:
        if wants and forced == 0 and prefs:
            print("no priority or reload-rotation change helps: a preference does (above)")
        else:
            print("no priority or reload-rotation change helps: instruction selection or CSE differs, "
                  "not allocation (see regdiag's listing)")
        return
    # Drop changes that do not matter.
    for item in list(state):
        trial = tuple(x for x in state if x != item)
        sc = evaluate(trial)
        if sc is not None and sc <= score:
            state = trial
    if a.json_out:
        import json
        targets = []
        for kind, r, d in state:
            if kind == "pri":
                p = ps[r]
                targets.append({"kind": "pri", "label": p.name, "raise": d > 0, "goal": p.pri + d,
                                "base": p.pri, "pseudo": r})
            else:
                k, uid, hard, line, file = next(x for x in ps.reloads if x[0] == r)[:5]
                lab = [x for x in ps.reloads if x[0] == r][0]
                same = [x for x in ps.reloads if len(x) > 5 and x[5] == lab[5]] if len(lab) > 5 else []
                targets.append({"kind": "reload", "label": lab[5] if len(lab) > 5 else "-",
                                "occurrence": [x[0] for x in same].index(r) if same else 0,
                                "index": r, "shift": d, "line": line})
        json.dump({"func": func, "base_bytes": base, "score": score, "targets": targets},
                  open(a.json_out, "w"), indent=1)
    print(f"\n{'EXACT MATCH' if score == 0 else f'{score} differing bytes'} in compiler space with "
          f"{len(state)} change(s); now write C that does the same:")
    for kind, r, d in state:
        if kind in ("spill", "spill2"):
            k, uid, hard, line, file = next(x for x in ps.reloads if x[0] == r)[:5]
            text = ""
            if os.path.abspath(file) == src and 0 < line <= len(src_text):
                text = src_text[line - 1].strip()[:70]
            print(f"  reload #{r} (insn {uid}, {os.path.relpath(file, ROOT) if file != '-' else '?'}:{line}"
                  f" {text}) should come {abs(d)} spill register(s) "
                  f"{'later' if d > 0 else 'earlier'} in the rotation: "
                  f"{'add' if d > 0 else 'remove'} {abs(d)} reload(s) before it (a literal RAM address,"
                  " a large constant offset, a constant folded into an address), or the reverse after it")
            continue
        p = ps[r]
        new_pri = p.pri + d
        crossed = [o for o in competitors(ps, p) if min(p.pri, new_pri) <= o.pri <= max(p.pri, new_pri)]
        crossed = sorted(crossed, key=lambda o: abs(o.pri - new_pri))[:3]
        kind_s = f"local, block {p.block}, life {p.life()}" if p.kind == "L" \
            else f"global, live {p.live}, crosses {p.calls} calls"
        print(f"  {p.name if p.name != '-' else 'pseudo ' + str(r)} (pseudo {r}): "
              f"{'raise' if d > 0 else 'lower'} priority {p.pri / 10000:.3f} -> {new_pri / 10000:.3f} "
              f"({kind_s}, {p.refs} uses; now r{p.got})")
        for o in crossed:
            name = o.name if o.name != "-" else f"pseudo {o.reg}"
            print(f"    {'above' if d > 0 else 'below'} {name} (pseudo {o.reg}: priority {o.pri / 10000:.3f}, "
                  f"{o.refs} uses, life {o.life()})" + (f" at {where(o.reg)}" if where(o.reg) else ""))
        lg = floor_log2(p.refs) * p.refs * 10000
        if lg > 0 and new_pri > 0:
            print(f"    -> at {p.refs} uses that is a life of about {lg / new_pri:.1f} (now {p.life()});"
                  f" at life {p.life()} it needs {'more' if d > 0 else 'fewer'} uses")
        if any(o.pri == new_pri - d // abs(d) or o.pri == p.pri for o in crossed):
            print("    -> equal priorities are broken by birth order (local) or pseudo number (global):"
                  " the one defined first wins; reorder the definitions")
        if where(r):
            print(f"    source: {where(r)}")
    print(f"\n{cc.compiles} compiles in {time.time() - t0:.0f}s")
    if a.search_c:
        targets, src_lines = [], set()
        for kind, r, d in state:
            if kind == "pri" and ps[r].name != "-":
                targets.append((ps[r].name, d > 0, ps[r].pri + d))
            elif kind == "pri":
                src_lines |= an.pseudo_lines.get(r, set())
            else:  # a reload shift: change the code just before that reload
                k, uid, hard, line, file = next(x for x in ps.reloads if x[0] == r)[:5]
                if os.path.abspath(file) == src:
                    src_lines |= set(range(line - 3, line + 1))
        guided_search(src, func, cc, targets, a.search_c, a.j, src_lines)


def deficit(dump, targets):
    """How far the named pseudos' priorities are from the targets (0 = met)."""
    total = 0.0
    for name, raise_, goal in targets:
        pris = [p.pri for p in dump.values() if getattr(p, "name", "-") == name]
        if not pris:
            total += 1.0
            continue
        got = max(pris) if raise_ else min(pris)
        miss = (goal - got) if raise_ else (got - goal)
        total += max(0.0, miss / max(abs(goal), 1))
    return total


def guided_search(src, func, cc, targets, seconds, jobs, src_lines=()):
    import json
    pre, line_map = permute.prepare_source(os.path.relpath(src, ROOT), func)
    names = [t[0] for t in targets]
    lines = pre.splitlines()
    focus = {i + 1 + k for i, ln in enumerate(lines) for k in (-1, 0, 1)
             if any(re.search(rf"\b{re.escape(n)}\b", ln) for n in names)}
    focus |= {b for b, l in line_map.items() if l in set(src_lines)}
    focus = [] if os.environ.get("RASOLVE_FOCUS_ALL") else sorted(focus)
    print(f"\nguided C search: targets {[(n, 'raise' if r else 'lower', round(g / 10000, 3)) for n, r, g in targets]}"
          f", {len(focus)} focus lines, {jobs} workers, {seconds}s", flush=True)
    job = os.path.join(cc.work, "job.json")
    json.dump({"src": src, "func": func, "pre": pre, "targets": targets, "focus": focus,
               "seconds": seconds}, open(job, "w"))
    procs = [subprocess.Popen([sys.executable, os.path.abspath(__file__), "--worker", job, str(seed)])
             for seed in range(jobs)]
    for p in procs:
        p.wait()
    results = []
    for seed in range(jobs):
        path = f"{job}.{seed}"
        if os.path.exists(path):
            results.append(json.load(open(path)))
    if not results:
        print("  no worker finished")
        return
    best = min(results, key=lambda r: (r["bytes"], r["deficit"]))
    tried = sum(r["n"] for r in results)
    print(f"  {tried} candidates; best {best['bytes']} differing bytes, priority deficit {best['deficit']:.3f}")
    print("  progress (s, bytes, deficit): " + " ".join(f"{t}:{b}:{d:.2f}" for t, b, d in best["hist"][-12:]))
    out = os.path.join(cc.work, "guided_best.c")
    open(out, "w").write(best["src"])
    print(f"  {out}")


def worker_main(job, seed):
    import json
    j = json.load(open(job))
    cc = Compiler(j["src"], j["func"])
    b, d, text, n, hist = guided_worker((j["pre"], j["func"], cc, [tuple(t) for t in j["targets"]],
                                         j["focus"], seed, j["seconds"]))
    json.dump({"bytes": b, "deficit": d, "src": text, "n": n, "hist": hist}, open(f"{job}.{seed}", "w"))


def guided_worker(args):
    pre, func, cc, targets, focus, seed, seconds = args
    if permute.PERMUTER not in sys.path:
        sys.path.insert(0, permute.PERMUTER)
    import copy
    import random
    from src import ast_util
    from src.randomizer import Focus, Randomizer
    from src.helpers import get_default_randomization_weights
    random.seed(seed)
    cc.work = tempfile.mkdtemp(prefix=f"w{seed}-", dir=cc.work)
    cc.cache = {}
    weights = get_default_randomization_weights("gcc")

    def evaluate(text):
        cc.pre = text
        b, dump = cc.run(dump=True)
        if b is None:
            return None
        return b, deficit(dump, targets)

    cur_src = best_src = pre
    cur = best = evaluate(pre)
    t0, n, hist = time.time(), 0, []
    cur_ast = ast_util.parse_c(pre)
    while time.time() - t0 < seconds and best[0] != 0:
        base_ast = cur_ast if random.random() < 0.8 else ast_util.parse_c(best_src)
        ast = copy.deepcopy(base_ast)
        fn, _ = ast_util.extract_fn(ast, func)
        ast_util.normalize_ast(fn, ast)
        try:
            Randomizer(weights, random.randrange(1 << 30), Focus(set(focus)) if focus else None).randomize(ast, func)
            text = ast_util.to_c(ast)
        except Exception:
            continue
        n += 1
        got = evaluate(text)
        if got is None:
            continue
        if got <= cur or random.random() < 0.02:
            cur, cur_ast, cur_src = got, ast, text
        if got < best:
            best, best_src = got, text
            hist.append((round(time.time() - t0), got[0], got[1]))
    return best[0], best[1], best_src, n, hist


if __name__ == "__main__":
    main()

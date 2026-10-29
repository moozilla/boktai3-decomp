#!/usr/bin/env python3
"""seek: a stateful search for the C that compiles to the ROM, with you as the policy.

  seek.py open FILE.c [FUNC]          start (or resume) FUNC's session from a draft; prints the facts
  seek.py add FILE.c [-p N] [-m NOTE] [--no-expand]   your edit as a child of node N (default: best),
                                      evaluated, semantics checked, and its neighborhood expanded
                                      (several files: spellings of one hypothesis, scored side by side)
  seek.py expand N [--fam F,..] [--lines A-B] [--top K]   every move at node N, compiled and ranked
  seek.py run [N..] [--budget C] [--seconds S] [--fam F,..] [--lines A-B]   best-first search from N
                                      (default: the best nodes), until MATCH or the budget
  seek.py show N [--diff]             a node: layers, skeleton gaps, block map, C diff from its parent
                                      (--diff: scope's full listing)
  seek.py values N                    allocation of every named value: draft register, the ROM's, priority
  seek.py facts [N]                   the determined part: the ROM's calls, accesses, literals, frame,
                                      and which ones node N misses
  seek.py twins [N] [--blocks 3,5]     matched functions with the same ROM code as N's differing blocks,
                                      and the C lines that produced it there (block-level prior art)
  seek.py tree [--all]                the session: nodes by key, the frontier, what was expanded
  seek.py diff A B                    C diff between two nodes and their layers
  seek.py file N                      write node N's whole file (build/seek/FUNC/nN.c) and print its path
  seek.py note N TEXT                 attach a hypothesis or finding to a node (shared with other workers)
  seek.py moves                       the move families
  seek.py land N                      a MATCH node: hand it to `campaign.py land` (unwraps NONMATCHING)

SEEK_JOBS=4 compiles expansions in four processes (each with its own agbcc fork server).

Options: -f FUNC picks the session (default: the last one opened in this tree).
Sessions are shared across worktrees ($SEEK_HOME, default: the main checkout's build/seek).
Every node file is a normal draft: run scope.py on it (why, probe, life, ...) for the compiler's reasons.

Keys are (skeleton, shape, regs): determined facts missing, register-blind distance, differing insns.
Block map characters, one per ROM basic block: = exact, r same code up to renaming registers inside the
block, s same shape, x different.
"""
import argparse
import collections
import copy
import difflib
import os
import re
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import engine as E  # noqa: E402
from engine import ROOT, key_of, sha  # noqa: E402
import moves as M  # noqa: E402
import rewrites as R  # noqa: E402
from perm_pycparser import c_ast as ca  # noqa: E402
from src import ast_util  # noqa: E402

CURRENT = os.path.join(E.LOCAL, "current")
SEM_FAMILIES = {"width", "cast", "explicit_narrow", "cse_temp", "inline_var", "reuse_stored", "related_const"}
AGBCC_SRC = os.path.join(ROOT, "build", "scope", "agbcc", "gcc")


# ------------------------------------------------------------------ parse

def parse_function(pre, names, src_text=None):
    """{name: FuncDef} of FUNC (first) and the unit's inline helpers it calls, transitively, from the
    preprocessed unit PRE. Moves apply to all of them: much of a function's code is often in a helper."""
    span = None
    for n in names:
        span = E.find_def(pre, n)
        if span:
            break
    if not span:
        raise SystemExit("cannot find the function in the preprocessed unit")
    head = pre[:span[0]]
    head = re.sub(r'^[ \t]*asm\s*\(\s*"\.include (?:\\.|[^"\\\n])*"\s*\)\s*;[ \t]*$', "", head, flags=re.M)
    body = pre[span[0]:span[1]]
    try:
        ast = ast_util.parse_c(head + body)
    except Exception:  # noqa: BLE001  other functions' bodies with syntax pycparser lacks: stub them
        ast = ast_util.parse_c(stub_bodies(head) + body)
    fn = next(e for e in reversed(ast.ext) if isinstance(e, ca.FuncDef) and e.decl.name in names)
    defs = {e.decl.name: e for e in ast.ext if isinstance(e, ca.FuncDef) and e is not fn
            and "inline" in (e.decl.funcspec or [])}
    out = {fn.decl.name: fn}
    todo = [fn]
    while todo:
        f = todo.pop()
        for n, _, _ in R.walk(f):
            if isinstance(n, ca.FuncCall) and isinstance(n.name, ca.ID) and n.name.name in defs \
                    and n.name.name not in out:
                out[n.name.name] = defs[n.name.name]
                todo.append(defs[n.name.name])
    if src_text is not None:  # helpers from headers stay as they are: a node rewrites only its own file
        out = {k: v for k, v in out.items() if k == fn.decl.name or E.find_def(src_text, k)}
    return out


def stub_bodies(text):
    """TEXT with top-level non-inline function bodies replaced by `;` (prototypes)."""
    out, i = [], 0
    for m in re.finditer(r"^[^\n;#{}]*\b(\w+)\s*\([^;{]*\)\s*\{", text, re.M):
        if m.start() < i:
            continue
        end = E.fast.body_end(text, m.end() - 1)
        if re.search(r"\binline\b", m.group(0)):
            continue
        out.append(text[i:m.end() - 1] + ";\n" + "\n" * text[m.end() - 1:end].count("\n"))
        i = end
    out.append(text[i:])
    return "".join(out)


def changed_line(fn, f2):
    """The source line of the first place F2 differs from FN (preorder walk, nearest coord)."""
    a = list(R.walk(fn))
    b = list(R.walk(f2))
    last = None
    for i in range(min(len(a), len(b))):
        x, y = a[i][0], b[i][0]
        if getattr(y, "coord", None) is not None:
            last = y.coord
        if type(x) is not type(y) or shallow(x) != shallow(y):
            c = getattr(y, "coord", None) or getattr(x, "coord", None) or last
            return c.line if c else None
    return last.line if last and len(a) != len(b) else None


def shallow(n):
    return tuple((k, getattr(n, k)) for k in n.attr_names if not isinstance(getattr(n, k), (list, dict)))


# ------------------------------------------------------------------ the engine's state in one process

class Work:
    def __init__(self, func=None, src=None):
        if src:
            self.target = E.Target(src, func)
            func = self.target.func
        else:
            func = func or current()
            if not func:
                sys.exit("no session: seek.py open FILE.c first (or -f FUNC)")
            self.target = None
        alias = os.path.join(E.HOME, func or "", "alias")
        if func and os.path.exists(alias):  # a ROM name (sub_XXXXXXXX) for a draft with a C name
            func = open(alias).read().strip()
        self.sess = E.Session(func)
        self.sess.fold_updates()
        meta = os.path.join(self.sess.dir, "meta.txt")
        if self.target is None:
            if not os.path.exists(meta):
                sys.exit(f"{func}: no session (seek.py open FILE.c)")
            src, f = open(meta).read().split()[:2]
            self.target = E.Target(src, f)
        else:
            open(meta, "w").write(f"{self.target.src} {self.target.func}\n")
            if self.target.rom_name != self.target.func:
                d = os.path.join(E.HOME, self.target.rom_name)
                os.makedirs(d, exist_ok=True)
                open(os.path.join(d, "alias"), "w").write(self.target.func + "\n")
        set_current(self.target.func)
        self.ast = {}  # node id -> (pre, {name: FuncDef})
        self.jobs = int(os.environ.get("SEEK_JOBS", "1"))
        self.hint = None  # the expanded node's rotation [0, K, D]: children try shifts near it first
        self.rot_max = None  # rotation objective: check spill shifts for children with at most this diff
        self.pool = None

    @property
    def func(self):
        return self.target.func

    def node(self, ref):
        if ref in (None, "best"):
            return self.best()
        n = int(str(ref).lstrip("n"))
        if n not in self.sess.nodes:
            sys.exit(f"no node n{n}")
        return self.sess.nodes[n]

    def best(self, k=1):
        nodes = [n for n in self.sess.nodes.values() if n.get("ok")]
        nodes.sort(key=lambda n: (rank(n), n["id"]))
        if not nodes:
            sys.exit("no evaluated node")
        return nodes[0] if k == 1 else nodes[:k]

    def pre_of(self, node):
        """(compile text, FuncDef) of a node, cached."""
        if node["id"] in self.ast:
            return self.ast[node["id"]]
        path = self.sess.materialize(node)
        pre = self.target.preprocess_file(path)
        defs = parse_function(pre, self.target.names, open(os.path.join(ROOT, path)).read())
        self.ast[node["id"]] = (pre, defs)
        return pre, defs

    def compile_all(self, base_pre, tasks):
        """[(output hash, alloc, evaluation) or None] for each (dname, ..., txt, line) task, spliced into
        BASE_PRE: in this process, or across SEEK_JOBS worker processes (each its own fork server)."""
        jobs = self.jobs
        rot = self.rot_max
        if jobs <= 1 or len(tasks) < 16:
            return [compile_one(self.target, base_pre, t[0], t[4], rot, self.hint) for t in tasks]
        if self.pool is None:
            import multiprocessing as mp
            self.pool = mp.get_context("spawn").Pool(jobs, _winit, (self.target.src, self.func, self.target.block))
        h = sha(base_pre)
        path = os.path.join(E.LOCAL, "cache", f"base-{h}.i")
        if not os.path.exists(path):
            open(path, "w").write(base_pre)
        t0 = time.time()
        out = self.pool.map(_wcompile, [(h, path, t[0], t[4], rot, self.hint) for t in tasks], chunksize=4)
        self.target.compiles += len(tasks)
        self.target.compile_time += time.time() - t0
        return out

    def set_block(self, node, b):
        """Restrict the objective to ROM block B of NODE's block map; returns the C lines behind it."""
        import json
        path = self.sess.materialize(node)
        pre = self.target.preprocess_file(path)
        code, _ = self.target.compile_pre(pre)
        _, _, regs = self.target.detail(code)
        if not 0 <= b < len(regs):
            sys.exit(f"no block {b} (the map has {len(regs)})")
        lo, hi = regs[b][0], regs[b][1]
        self.target.block = (lo, hi)
        sc = E.scope
        (mine, addr, info), _ = E.capture(sc.compile_draft, path, self.func)
        E.capture(sc.report, path, self.func, self.target.rom_code, addr, mine, info, 0)
        rows = json.load(open(os.path.join(info["wd"], "diff.json")))
        offs = {self.target.rom_rows[i][0] for i in range(lo, hi)}
        lines, prev_in = set(), False
        for r in rows:
            inside = bool(r.get("rom_addr")) and int(r["rom_addr"], 16) - self.target.addr in offs
            if (inside or (not r.get("rom_addr") and prev_in)) and str(r.get("line") or "").isdigit():
                lines.add(int(r["line"]))
            if r.get("rom_addr"):
                prev_in = inside
        return sorted(lines)

    def want_of(self, node):
        """The ROM's register per value, from the nearest file node above NODE."""
        p = node
        while p is not None:
            if p.get("want") is not None:
                return p["want"]
            p = self.sess.nodes.get(p.get("parent"))
        return {}

    # -- adding nodes
    def add_file(self, path, parent=None, note=None, who="model"):
        text = open(path).read()
        if not E.find_def(text, self.func) and not E.find_def(text, self.target.rom_name):
            sys.exit(f"{path}: no definition of {self.func}")
        fh = self.sess.put_blob(text)
        for n in self.sess.nodes.values():
            if n.get("file") == fh and not n.get("defs") and n.get("kind") == "file":
                if note and not n.get("note"):
                    self.sess.update(n["id"], note=note)
                return n  # this exact file is already a node
        d = os.path.join(E.LOCAL, self.func)
        os.makedirs(d, exist_ok=True)
        tmp = os.path.join(d, f"add{os.getpid()}.c")
        open(tmp, "w").write(text)
        pre = self.target.preprocess_file(tmp)
        code, err, alloc = self.target.compile_pre(pre, trace=True)
        nrel = alloc.pop("__reloads__", 0)
        ev = self.target.evaluate(code)
        if code and not ev.get("match") and ev["key"][2] <= 12:
            ev["rot"] = self.target.rotation(pre, nrel) or [ev["key"][2], -1, 99]
        out = sha(code) if code else None
        rec = dict(parent=parent["id"] if parent else None, kind="file", who=who, file=fh, defs={},
                   out=out, move=os.path.relpath(os.path.abspath(path), ROOT), note=note, depth=0, **ev)
        if not code:
            rec["error"] = err
        else:
            rec["sem"] = self.target.semantic(code, pre)
            rec["alloc"] = {k: v[0] for k, v in alloc.items()}
            if not ev.get("match"):
                rec["want"] = self.target.wanted(os.path.relpath(tmp, ROOT))
                rec["awrong"] = len(E.alloc_wrong(rec["alloc"], rec["want"]))
            if parent and parent.get("alloc"):
                rec["moved"] = E.alloc_changes(parent["alloc"], rec["alloc"])[:8]
        same = self.sess.by_out.get(out) if out else None
        rec = self.sess.add(rec)
        if same and same != rec["id"]:
            self.sess.update(rec["id"], same_as=same)
        try:
            self.ast[rec["id"]] = (pre, parse_function(pre, self.target.names, text))
        except Exception as e:  # noqa: BLE001
            print(f"(n{rec['id']} cannot be parsed for moves: {str(e).splitlines()[0][:200]})")
        return rec

    # -- expanding
    def expand(self, node, families=None, lines=None, quiet=False, budget=None, sem=True, deadline=None):
        """Every move at NODE: compile, evaluate, add the distinct outputs as children. Returns
        (children, stats)."""
        pre, defs = self.pre_of(node)
        t0 = time.time()
        base_pre = pre
        for name, f in defs.items():
            base_pre = E.splice(base_pre, [name], R.text(f))
        base_code, _, base_alloc = self.target.compile_pre(base_pre, trace=True)
        base_alloc.pop("__reloads__", None)
        base_alloc = {k: v[0] for k, v in base_alloc.items()}
        want = self.want_of(node)
        own_out = node.get("out")
        if base_code is not None and sha(base_code) != own_out and node.get("kind") == "file":
            print(f"note: n{node['id']} regenerated by pycparser compiles differently from the file "
                  f"(moves start from the regenerated form: {self.target.evaluate(base_code)['counts']})")
            own_out = sha(base_code)
        seen_src = set()
        stats = {"variants": 0, "neutral": 0, "known": 0, "failed": 0, "new": 0, "fam": {}}
        children = []
        file_hash = node["file"]
        depth = node.get("depth", 0) + 1
        main = next(iter(defs))
        tasks = []
        lineset = set(lines or ())
        r = node.get("rot")
        self.hint = r[1:] if r and r[0] == 0 else None
        for dname, fn in defs.items():
            seen_src.add(sha(R.text(fn)))
        for dname, fn in defs.items():
            for fam, desc, f2 in M.all_moves(fn, families):
                if (budget is not None and len(tasks) >= budget) or (deadline is not None and time.time() >= deadline):
                    break
                line = changed_line(fn, f2)
                if lines and line not in lineset:
                    continue
                try:
                    txt = R.text(f2)
                except Exception:  # noqa: BLE001
                    continue
                h = sha(txt)
                if h in seen_src:
                    continue
                seen_src.add(h)
                tasks.append((dname, fam, desc, f2, txt, line))
        stats["variants"] = len(tasks)
        for (dname, fam, desc, f2, txt, line), res in zip(tasks, self.compile_all(base_pre, tasks)):
            fs = stats["fam"].setdefault(fam, [0, 0, 0])  # tried, changed code, better
            fs[0] += 1
            if res is None:
                stats["failed"] += 1
                continue
            out, alloc, ev = res
            if out == own_out:
                stats["neutral"] += 1
                continue
            fs[1] += 1
            if out in self.sess.by_out:
                stats["known"] += 1
                continue
            vpre = E.splice(base_pre, [dname], txt)
            nd = dict(node.get("defs") or {})
            nd[dname] = self.sess.put_blob(txt)
            rec = dict(parent=node["id"], kind="move", who="auto", file=file_hash, defs=nd, out=out,
                       family=fam, move=desc if dname == main else f"{desc} (in {dname})", line=line,
                       depth=depth, alloc=alloc, moved=E.alloc_changes(base_alloc, alloc)[:8], **ev)
            if want and not ev.get("match"):
                rec["awrong"] = len(E.alloc_wrong(alloc, want))
            if sem and (desc.startswith("[sem]") or fam in SEM_FAMILIES) and rank(dict(ev, ok=True)) <= rank(node):
                code, _ = self.target.compile_pre(vpre)
                rec["sem"] = self.target.semantic(code, vpre)
            rec = self.sess.add(rec)
            if rank(rec) < rank(node):
                fs[2] += 1
            kid_defs = dict(defs)
            kid_defs[dname] = f2
            self.ast[rec["id"]] = (vpre, kid_defs)
            children.append(rec)
            stats["new"] += 1
        stats["seconds"] = round(time.time() - t0, 2)
        self.sess.event(kind="expand", node=node["id"], families=sorted(families) if families else None,
                        lines=lines, **{k: v for k, v in stats.items() if k != "fam"})
        if not quiet:
            print_expand(self, node, children, stats)
        return children, stats

    def search(self, starts, budget=2000, seconds=120, families=None, lines=None, quiet=False, slack=None,
               max_depth=6):
        """Best-first over (key, depth): expand the best node not yet expanded with these moves.
        New outputs that tie their parent stay on the frontier (a plateau is walked, not abandoned);
        a node is never expanded twice with the same move set."""
        import heapq
        t0 = time.time()
        start_compiles = self.target.compiles
        heap, tick = [], 0
        sig = (tuple(sorted(families)) if families else None, tuple(lines) if lines else None)
        done = {e["node"] for e in self.sess.events if e.get("kind") == "expand"
                and (tuple(e["families"]) if e.get("families") else None, tuple(e["lines"]) if e.get("lines") else None) == sig}
        for n in starts:
            heapq.heappush(heap, (rank(n), n.get("awrong", 0), n.get("depth", 0), tick, n["id"]))
            tick += 1
        best = min(starts, key=rank)
        if slack is None:
            slack = max(4, key_of(best)[2] // 8)
        cost = lambda n: rank(n)[1]
        rounds = 0
        while heap and self.target.compiles - start_compiles < budget and time.time() - t0 < seconds:
            nid = heapq.heappop(heap)[-1]
            if nid in done:
                # already expanded: its children are on the tree; queue them instead
                for c in self.sess.nodes.values():
                    if c.get("parent") == nid and c.get("ok") and c["id"] not in done:
                        heapq.heappush(heap, (rank(c), c.get("awrong", 0), c.get("depth", 0), tick, c["id"]))
                        tick += 1
                continue
            done.add(nid)
            node = self.sess.nodes[nid]
            try:
                kids, st = self.expand(node, families, lines, quiet=True,
                                       budget=budget - (self.target.compiles - start_compiles),
                                       deadline=t0 + seconds)
            except SystemExit as e:
                print(f"  n{nid}: {e}")
                continue
            rounds += 1
            for c in kids:
                if c.get("sem", "").startswith("DIFFERENT"):
                    continue
                ck, bk = rank(c), rank(best)
                # better, a lateral step on the plateau, or a bounded step back (moves that pay off only
                # together: the first can cost a few insns); the heap still expands the best first
                if ck <= rank(node) or (ck[0] <= bk[0] and cost(c) <= cost(best) + slack
                                        and c.get("depth", 0) <= max_depth):
                    heapq.heappush(heap, (rank(c), c.get("awrong", 0), c.get("depth", 0), tick, c["id"]))
                    tick += 1
                if rank(c) < rank(best):
                    best = c
                    if not quiet:
                        print(f"  {fmt_node(c)}   [{self.target.compiles - start_compiles} compiles, "
                              f"{time.time() - t0:.0f}s]")
                if c.get("match"):
                    break
            if best.get("match"):
                break
        el = time.time() - t0
        n = self.target.compiles - start_compiles
        print(f"run: {rounds} expansions, {n} compiles in {el:.1f}s ({1000 * el / max(1, n):.1f} ms each, "
              f"{self.jobs} job{'s' if self.jobs > 1 else ''}); "
              f"best {fmt_node(best)}")
        self.sess.event(kind="run", rounds=rounds, compiles=n, seconds=round(el, 1), best=best["id"])
        return best


def compile_one(target, base_pre, dname, txt, rot_max=None, hint=None):
    vpre = E.splice(base_pre, [dname], txt)
    code, err, alloc = target.compile_pre(vpre, trace=True)
    if code is None:
        return None
    nrel = alloc.pop("__reloads__", 0)
    ev = target.evaluate(code)
    if rot_max is not None and not ev.get("match") and ev["key"][2] <= rot_max:
        ev["rot"] = target.rotation(vpre, nrel, hint=hint) or [ev["key"][2], -1, 99]
    return sha(code), {k: v[0] for k, v in alloc.items()}, ev


_WT = None
_WBASE = {}


def _winit(src, func, block=None):
    global _WT
    import signal
    signal.signal(signal.SIGINT, signal.SIG_IGN)
    _WT = E.Target(src, func)
    _WT.block = block


def _wcompile(task):
    h, path, dname, txt, rot, hint = task
    if h not in _WBASE:
        _WBASE.clear()
        _WBASE[h] = open(path).read()
    return compile_one(_WT, _WBASE[h], dname, txt, rot, hint)


# ------------------------------------------------------------------ ranking

OBJECTIVE = {"mode": None}


def rank(n):
    """The order the search and every listing use. Structure first (determined facts missed), then
    shape + differing insns together, so a node never ranks above another with both more shape and
    more register differences (key (skel, shape, diff) is what is stored and printed).
    Objective "rotation": among nodes whose code is right up to reload's spill rotation, the one
    needing the smallest shift first (a C change that adds or removes one reload choice before the
    differing one moves the shift by one: a gradient where the insn count shows none).
    Objective "block": the cost inside one ROM block first, the rest of the function second."""
    if not n.get("ok"):
        return (9999, 9999, 9999, 9999)
    k = key_of(n)
    cost = k[1] + k[2]
    if n.get("match"):
        return (0, 0, 0, 0)
    mode = OBJECTIVE["mode"]
    if mode == "rotation" and n.get("rot"):
        r = n["rot"]  # [diff after the best single shift, K, D]
        return (k[0], r[0], r[2] if r[0] == 0 else 99, cost)
    if mode == "block" and n.get("blk") is not None:
        return (k[0], n["blk"], cost, k[1])
    return (k[0], cost, k[1], k[2])


# ------------------------------------------------------------------ printing

def fmt_key(k):
    return f"skel {k[0]} shape {k[1]} diff {k[2]}" if k[0] < 9999 else "COMPILE FAILED"


def fmt_node(n, parent=None):
    k = key_of(n)
    head = f"n{n['id']:<4} " + ("MATCH" if n.get("match") else f"{fmt_key(k):<26} {n.get('counts', ''):<22}")
    delta = ""
    if parent is not None and k[0] < 9999:
        pk = key_of(parent)
        delta = " (" + ",".join(f"{x - y:+d}" for x, y in zip(k, pk)) + ")"
    what = n.get("move") or ""
    if n.get("family"):
        what = f"{n['family']}: {what}"
    if n.get("line"):
        what += f" @{n['line']}"
    sem = n.get("sem")
    semtxt = "" if not sem or sem == "unchecked" else ("  [sem ok]" if sem.startswith("EQUIV") else
                                                       "  [sem: " + sem.split(":")[0] + "]")
    al = ""
    if OBJECTIVE["mode"] == "block" and n.get("blk") is not None:
        al += f"  [block {n['blk']}]"
    if n.get("rot"):
        r = n["rot"]
        al += f"  [rotation: AGBCC_SPILL={r[1]}:{r[2]} matches]" if r[0] == 0 else "  [not rotation-only]"
    if n.get("awrong") is not None and not n.get("match"):
        al += f"  [alloc {n['awrong']} off]"
    if n.get("moved") and parent is not None:
        al += "  {" + ", ".join(n["moved"][:4]) + ("..." if len(n["moved"]) > 4 else "") + "}"
    return f"{head}{delta}  {what}{semtxt}{al}"


def print_expand(w, node, children, st):
    print(f"expand n{node['id']} ({fmt_key(key_of(node))}): {st['variants']} variants in {st['seconds']}s -> "
          f"{st['new']} new outputs, {st['neutral']} no code change, {st['known']} already in the tree, "
          f"{st['failed']} compile errors")
    moving = [f"{f} {v[1]}/{v[0]}" + (f" ({v[2]} better)" if v[2] else "") for f, v in sorted(st["fam"].items())
              if v[1]]
    still = [f for f, v in sorted(st["fam"].items()) if not v[1]]
    if moving:
        print("  families that change the code (changed/tried): " + ", ".join(moving))
    if still:
        print("  no effect here: " + ", ".join(still))
    top = sorted(children, key=lambda c: (rank(c), c["id"]))[:getattr(w, "top", 8)]
    for c in top:
        print("  " + fmt_node(c, node))


def c_diff(a, b, la="a", lb="b", n=2):
    return "".join(difflib.unified_diff(a.splitlines(True), b.splitlines(True), la, lb, n=n))


def func_text(w, node):
    t = w.sess.text(node)
    for n in w.target.names:
        sp = E.find_def(t, n)
        if sp:
            return t[sp[0]:sp[1]]
    return t


def src_ref(fname, func):
    """'global.c:123' for FUNC's definition in agbcc's source (scope's instrumented copy)."""
    p = os.path.join(AGBCC_SRC, fname)
    try:
        for i, ln in enumerate(open(p, errors="replace"), 1):
            if re.match(re.escape(func) + r"\s*\(", ln):
                return f"{fname}:{i}"
    except OSError:
        pass
    return fname


# ------------------------------------------------------------------ commands

def current():
    try:
        return open(CURRENT).read().strip()
    except OSError:
        return None


def set_current(func):
    os.makedirs(E.LOCAL, exist_ok=True)
    open(CURRENT, "w").write(func + "\n")


def cmd_open(a):
    w = Work(a.func, a.file)
    roots = [n for n in w.sess.nodes.values() if n.get("kind") == "file" and n.get("parent") is None]
    text = open(a.file).read()
    same = [n for n in roots if n["file"] == sha(text)]
    if same:
        node = same[0]
        print(f"{w.func}: session resumed ({len(w.sess.nodes)} nodes); this file is n{node['id']}")
    else:
        node = w.add_file(a.file, None, "opened", who="model")
        print(f"{w.func}: {'session resumed, new root' if roots else 'new session'} n{node['id']}")
    print(f"  ROM {w.target.size} B at 0x{w.target.addr:08X}, {len(E.blocks_of(w.target.rom_rows))} blocks;"
          f" session in {w.sess.dir.replace(os.path.expanduser('~'), '~')}")
    print("  " + fmt_node(node))
    show_layers(w, node)
    b = w.best()
    if b["id"] != node["id"]:
        print("  best in the session: " + fmt_node(b))
    print(f"next: seek.py expand n{node['id']} | run | facts | values n{node['id']} | "
          f"scope.py {w.sess.materialize(node)} why all")


def show_layers(w, node):
    if node.get("skeleton"):
        print("  structural observations (calls gate search; other differences can be code generation):")
        for ln in node["skeleton"][:10]:
            print("    " + ln)
    if node.get("regions"):
        print(f"  blocks: {node['regions']}   (= exact, r renamed regs, s same shape, x differs)")
    if node.get("sem") and node["sem"] != "unchecked":
        print(f"  semantics: {node['sem']}")
    if node.get("error"):
        print(f"  error: {node['error']}")


def cmd_add(a):
    w = Work(a.f)
    parent = w.node(a.parent) if a.parent else w.best()
    if len(a.file) > 1:  # several spellings of one hypothesis: score them all, best first, no expansion
        recs = [w.add_file(f, parent, a.m) for f in a.file]
        for r in sorted(recs, key=lambda r: (rank(r), r["id"])):
            print(fmt_node(r, parent) + (f"  (same code as n{r['same_as']})" if r.get("same_as") else ""))
        return
    node = w.add_file(a.file[0], parent, a.m)
    print(fmt_node(node, parent))
    show_layers(w, node)
    if node.get("same_as"):
        print(f"  same code as n{node['same_as']}")
    if node.get("ok") and not node.get("match") and not a.no_expand and node["id"] in w.ast:
        w.top = 5
        w.expand(node)
    if node.get("match"):
        print(f"MATCH. Land it: seek.py land n{node['id']}")


def parse_lines(s):
    if not s:
        return None
    out = set()
    for part in s.split(","):
        lo, _, hi = part.partition("-")
        out.update(range(int(lo), int(hi or lo) + 1))
    return sorted(out)


def parse_fams(s):
    if not s:
        return None
    fams = set(x.strip() for x in s.split(",") if x.strip())
    bad = fams - set(M.FAMILIES)
    if bad:
        sys.exit(f"unknown families: {', '.join(sorted(bad))} (seek.py moves)")
    return fams


def cmd_expand(a):
    w = Work(a.f)
    if a.objective == "rotation":
        OBJECTIVE["mode"] = "rotation"
        w.rot_max = 99
    w.top = a.top
    node = w.node(a.node)
    w.expand(node, parse_fams(a.fam), parse_lines(a.lines))


def cmd_run(a):
    w = Work(a.f)
    starts = [w.node(n) for n in a.nodes] if a.nodes else w.best(a.width)
    if isinstance(starts, dict):
        starts = [starts]
    if a.block is not None:
        a.objective = "block"
        lines = w.set_block(starts[0], a.block)
        a.lines = ",".join(map(str, lines)) if not a.lines else a.lines
        print(f"block {a.block}: ROM +0x{w.target.rom_rows[w.target.block[0]][0]:X}-+0x"
              f"{w.target.rom_rows[w.target.block[1] - 1][0]:X}; moves limited to C lines {a.lines}")
        for n in starts:  # the start nodes' own block cost
            pre, _ = w.pre_of(n)
            code, _ = w.target.compile_pre(pre)
            n["blk"] = w.target.evaluate(code).get("blk")
    if a.objective:
        OBJECTIVE["mode"] = a.objective
    if a.objective == "rotation":
        w.rot_max = a.rot_max if a.rot_max is not None else max(8, min(key_of(n)[2] for n in starts) + 6)
    best = w.search(starts, a.budget, a.seconds, parse_fams(a.fam), parse_lines(a.lines), slack=a.slack,
                    max_depth=a.depth)
    if best.get("match"):
        print(f"MATCH: seek.py show n{best['id']}; land with seek.py land n{best['id']}")
    else:
        show_layers(w, best)


def cmd_show(a):
    w = Work(a.f)
    node = w.node(a.node)
    parent = w.sess.nodes.get(node.get("parent")) if node.get("parent") else None
    print(fmt_node(node, parent))
    if node.get("note"):
        print(f"  note: {node['note']}")
    chain, p = [], node
    while p.get("parent"):
        p = w.sess.nodes[p["parent"]]
        chain.append(f"n{p['id']}")
    if chain:
        print("  path: " + " <- ".join(chain))
    show_layers(w, node)
    if parent:
        d = c_diff(func_text(w, parent), func_text(w, node), f"n{parent['id']}", f"n{node['id']}")
        print(d if len(d) < 6000 else d[:6000] + "\n...")
    path = w.sess.materialize(node)
    print(f"  file: {path}")
    if a.diff:
        os.execvp(sys.executable, [sys.executable, os.path.join(ROOT, "tools", "scope", "scope.py"), path, w.func])


def cmd_values(a):
    import scope
    w = Work(a.f)
    node = w.node(a.node)
    path = w.sess.materialize(node)
    (mine, addr, info), _ = E.capture(scope.compile_draft, path, w.func)
    wd = info["wd"]
    E.capture(scope.report, path, w.func, w.target.rom_code, addr, mine, info, 0)
    tab = scope.alloc_table(wd, w.func)
    rom = scope.rom_hard(wd)
    print(f"n{node['id']}: named values, in allocation order (global alloc sorts by priority: "
          f"{src_ref('global.c', 'allocno_compare')}; registers chosen in {src_ref('global.c', 'find_reg')};"
          f" local alloc {src_ref('local-alloc.c', 'qty_compare_1')})")
    print(f"  {'value':<22} {'pseudo':>6} {'kind':<4} {'draft':<6} {'ROM':<6} {'pri':>8} {'refs':>5} {'live':>5} "
          f"{'rank':>4}")
    wrong = 0
    rows = sorted(tab.items(), key=lambda kv: (kv[1]["kind"] != "G", kv[1]["rank"] if kv[1]["rank"] is not None
                                                else 999, kv[1]["pseudo"]))
    for (name, line, k), r in rows:
        want = rom.get(r["pseudo"])
        got = r["hard"]
        rn = lambda x: scope.rname(x) if isinstance(x, int) and x >= 0 else ("mem" if x is not None else "?")
        mark = "!" if want is not None and got != want else " "
        wrong += mark == "!"
        label = name if name != "-" else f"(line {line})"
        if k > 1:
            label += f"#{k}"
        pri = f"{r['pri']:.1f}" if isinstance(r["pri"], (int, float)) else str(r["pri"])
        print(f"{mark} {label:<22} {r['pseudo']:>6} {r['kind']:<4} {rn(got):<6} {rn(want) if want is not None else '-':<6} "
              f"{pri:>8} {r['refs']!s:>5} {r['live']!s:>5} {r['rank'] if r['rank'] is not None else '':>4}")
    print(f"  {wrong} value(s) in another register than the ROM's ('!'). ROM registers are voted from aligned "
          "insns. Raise or lower a value's priority (refs, live length) to reorder;"
          f" confirm with scope.py {path} AGBCC_PRI=NAME:+N or AGBCC_FORCE=NAME:rN before changing C.")


def cmd_facts(a):
    w = Work(a.f)
    t = w.target
    ev = t.rom_skel
    calls = [e[1] for e in ev if e[0] == "call"]
    mem = [e for e in ev if e[0] == "mem"]
    lits = [e[1] for e in ev if e[0] == "lit"]
    frame = [" ".join(e[1:]) for e in ev if e[0] == "frame"]
    loops = sum(1 for r in t.rom_rows if r[4] is not None and r[4] < r[0] and r[2].startswith("b"))
    print(f"{w.func} (ROM {t.size} B): the determined part")
    later_entries = [r[0] for i, r in enumerate(t.rom_rows) if i and
                     r[2].startswith("push ") and "lr" in r[2] and
                     any(p[2].startswith("bx ") for p in t.rom_rows[max(0, i - 6):i])]
    if later_entries:
        print("  check ROM extent: possible additional routine entries after a return: "
              + ", ".join(f"+0x{x:X} ({t.addr + x:08X})" for x in later_entries))
        print("    Missing calls/accesses may belong to an undrafted suffix; inspect ROM before searching.")
    print(f"  frame: {'; '.join(frame) or 'leaf, no frame'}")
    print(f"  calls in order ({len(calls)}): " + (", ".join(calls) or "none"))
    names = {}
    for e in lits:
        v = int(e, 16)
        names[e] = t.symnames.get(v & ~1, "")
    print(f"  literals ({len(lits)}): " + ", ".join(f"{e}{' ' + names[e] if names[e] else ''}" for e in lits))
    c = collections_counter(f"{e[1]} {e[2]}" for e in mem)
    print(f"  memory accesses ({len(mem)}): " + ", ".join(f"{k}" + (f" x{n}" if n > 1 else "") for k, n in c))
    print(f"  backward branches (loops): {loops}; basic blocks: {len(E.blocks_of(t.rom_rows))}")
    if a.node or w.sess.nodes:
        node = w.node(a.node)
        print(f"n{node['id']} against them:")
        if node.get("skeleton"):
            for ln in node["skeleton"]:
                print("    " + ln)
        else:
            print("    all met" if node.get("ok") else "    (no code)")


def collections_counter(it):
    import collections
    return sorted(collections.Counter(it).items())


def block_tokens(w, lo, hi, ctx=0):
    import structure
    names = {}
    for n, v in E.scope.elf_syms().items():
        a_ = v[0] if isinstance(v, tuple) else v
        if isinstance(a_, int):
            names.setdefault(a_ & ~1, n)
    rows = [r for r in w.target.rom_rows[max(0, lo - ctx):hi + ctx] if not r[2].startswith(".")]
    return [structure.norm_insn(r[2], names) for r in rows]


def loose(tok):
    return re.sub(r"#-?(?:0x[0-9a-fA-F]+|\d+)", "#", tok)


def matched_c_lines(src, func, lo_off, hi_off):
    """{C line: text} of matched FUNC in SRC that produced its ROM bytes [lo_off, hi_off)."""
    import json
    sc = E.scope
    try:
        (mine, addr, info), _ = E.capture(sc.compile_draft, src, func)
        _, _, sizes = sc.inventory()
        rom_code = sc.rom_bytes()[E.off(addr):E.off(addr) + sizes[sc.rom_name(src, func)]]
        E.capture(sc.report, src, func, rom_code, addr, mine, info, 0)
        rows = json.load(open(os.path.join(info["wd"], "diff.json")))
    except BaseException:  # noqa: BLE001
        return {}
    lines = sorted({int(r["line"]) for r in rows if r.get("line") and r.get("rom_addr")
                    and lo_off <= int(r["rom_addr"], 16) - addr < hi_off and str(r["line"]).isdigit()})
    text = open(os.path.join(ROOT, src)).read().split("\n")
    return {ln: text[ln - 1] for ln in lines if 0 < ln <= len(text)}


def cmd_twins(a):
    """Matched functions whose ROM has the same code as this node's differing blocks, with the C lines
    that produced it there: prior art at the level of a block, not a whole function."""
    import difflib
    sc = E.scope
    w = Work(a.f)
    node = w.node(a.node)
    code = None
    path = w.sess.materialize(node)
    pre = w.target.preprocess_file(path)
    code, _ = w.target.compile_pre(pre)
    if code is None:
        sys.exit("node does not compile")
    _, _, regs = w.target.detail(code)
    want = [int(x) for x in a.blocks.split(",")] if a.blocks else None
    index = sc.like_index()
    status, files = sc.code_status()
    shing = {}
    for n, toks in index.items():
        if status.get(n) != "matched" or n == w.target.rom_name:
            continue
        lt = [loose(t) for t in toks]
        for i in range(len(lt) - 2):
            shing.setdefault("|".join(lt[i:i + 3]), set()).add(n)
    shown = 0
    for bi, (lo, hi, verdict, detail) in enumerate(regs):
        if (want is not None and bi not in want) or (want is None and verdict == "="):
            continue
        toks = block_tokens(w, lo, hi, ctx=a.context)
        lt = [loose(t) for t in toks]
        if len(lt) < 3:
            continue
        cand = collections.Counter()
        for i in range(len(lt) - 2):
            hits = shing.get("|".join(lt[i:i + 3]), ())
            if len(hits) <= 300:
                for n in hits:
                    cand[n] += 1
        scored = []
        for n, _ in cand.most_common(60):
            other = [loose(t) for t in index[n]]
            m = difflib.SequenceMatcher(None, lt, other, autojunk=False).find_longest_match(0, len(lt), 0, len(other))
            exact = sum(1 for i in range(m.size) if toks[m.a + i] == index[n][m.b + i])
            scored.append((m.size, exact, n, m.b))
        scored.sort(reverse=True)
        off0 = w.target.rom_rows[lo][0]
        print(f"block {bi} [{verdict}] ROM +0x{off0:X}, {len(toks)} insns{' with context' if a.context else ''}: "
              + (" | ".join(toks[:6]) + (" ..." if len(toks) > 6 else "")))
        if not scored or scored[0][0] < min(4, len(lt)):
            print("    no matched function shares a run of 4+ of these insns")
            continue
        for size, exact, n, b in scored[:a.show]:
            src = files.get(n, "?")
            print(f"    {size}/{len(lt)} insns in a row ({exact} exact) in {n} ({src})")
        size, exact, n, b = scored[0]
        if files.get(n) and size >= 4:
            rows = [r for r in sc.decode(sc.rom_bytes()[E.off(sc.inventory()[1][n]):E.off(sc.inventory()[1][n]) +
                                                       sc.inventory()[2][n]], sc.inventory()[1][n],
                                         sc.rom_pool_ranges(sc.rom_bytes()[E.off(sc.inventory()[1][n]):E.off(sc.inventory()[1][n]) + sc.inventory()[2][n]], sc.inventory()[1][n]))
                    if not r[2].startswith(".")]
            lo_off, hi_off = rows[b][0], rows[min(len(rows) - 1, b + size - 1)][0] + 2
            cl = matched_c_lines(files[n], n, lo_off, hi_off)
            if cl:
                print(f"    its C for those insns ({files[n]}):")
                for ln, t in list(cl.items())[:14]:
                    print(f"      {ln:5}  {t}")
        shown += 1
    if not shown:
        print("no differing blocks (or none given with --blocks)")


def cmd_tree(a):
    w = Work(a.f)
    nodes = sorted(w.sess.nodes.values(), key=lambda n: (rank(n), n["id"]))
    exp = {}
    for e in w.sess.events:
        if e.get("kind") == "expand":
            exp[e["node"]] = exp.get(e["node"], 0) + 1
    kids = {}
    for n in w.sess.nodes.values():
        kids[n.get("parent")] = kids.get(n.get("parent"), 0) + 1
    runs = [e for e in w.sess.events if e.get("kind") == "run"]
    total = sum(e.get("variants", 0) for e in w.sess.events if e.get("kind") == "expand")
    print(f"{w.func}: {len(nodes)} nodes, {sum(exp.values())} expansions ({total} variants compiled), "
          f"{len(runs)} runs; best first:")
    show = nodes if a.all else [n for n in nodes if n.get("who") == "model" or n.get("note")][:15] + nodes[:15]
    seen = set()
    for n in show:
        if n["id"] in seen:
            continue
        seen.add(n["id"])
        tag = ("E" if n["id"] in exp else " ") + ("M" if n.get("who") == "model" else " ")
        p = f"<-n{n['parent']}" if n.get("parent") else "root"
        print(f"  {tag} {fmt_node(n):<110.110} {p}" + (f"  # {n['note']}" if n.get("note") else ""))
    print("  (E expanded, M your node)")


def cmd_diff(a):
    w = Work(a.f)
    x, y = w.node(a.a), w.node(a.b)
    print(fmt_node(x))
    print(fmt_node(y, x))
    print(c_diff(func_text(w, x), func_text(w, y), f"n{x['id']}", f"n{y['id']}"))


def cmd_file(a):
    w = Work(a.f)
    print(w.sess.materialize(w.node(a.node)))


def cmd_note(a):
    w = Work(a.f)
    node = w.node(a.node)
    w.sess.update(node["id"], note=" ".join(a.text))
    print(fmt_node(w.sess.nodes[node["id"]]) + f"  # {w.sess.nodes[node['id']]['note']}")


def cmd_moves(a):
    for name, g in sorted(M.FAMILIES.items()):
        doc = (g.__doc__ or "").strip().split("\n")[0]
        print(f"  {name:<16} {doc}")


def cmd_land(a):
    import json
    import subprocess
    w = Work(a.f)
    node = w.node(a.node)
    if not node.get("match"):
        sys.exit(f"n{node['id']} does not match")
    camp = os.path.join(ROOT, "build", "scope", "campaign")
    d = os.path.join(camp, w.target.rom_name)
    os.makedirs(d, exist_ok=True)
    text = w.sess.text(node)
    if "#ifdef NONMATCHING" not in text:
        # a whole unit written clean (wrapper and INCLUDE_ASM already gone): it is the file to land
        src = os.path.join(ROOT, w.target.src)
        open(src, "w").write(text)
        fmt = os.path.join(ROOT, "build", "venv", "bin", "clang-format")
        subprocess.run([fmt if os.path.exists(fmt) else "clang-format", "-i", w.target.src], cwd=ROOT)
        r = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "check.py"), w.target.src], cwd=ROOT)
        w.sess.event(kind="land", node=node["id"], rc=r.returncode)
        print(f"n{node['id']} -> {w.target.src} (whole unit)")
        sys.exit(r.returncode)
    open(os.path.join(d, "best_match.c"), "w").write(text)
    orig = open(os.path.join(ROOT, w.target.src)).read()
    open(os.path.join(d, "base.c"), "w").write(orig)
    q = os.path.join(camp, "queue.json")
    rows = json.load(open(q)) if os.path.exists(q) else []
    rows = [r for r in rows if not (r.get("func") == w.target.rom_name and not isinstance(r.get("addr"), str))]
    if not any(r.get("func") == w.target.rom_name for r in rows):
        rows.append({"func": w.target.rom_name, "src": w.target.src, "addr": f"{w.target.addr:08X}",
                     "size": w.target.size, "kind": "nonmatching"})
        json.dump(rows, open(q, "w"), indent=1)
    rev = os.path.join(d, ".main_rev")
    if not os.path.exists(rev):
        open(rev, "w").write(subprocess.run(["git", "-C", ROOT, "rev-parse", "HEAD"], capture_output=True,
                                            text=True).stdout.strip())
    print(f"n{node['id']} -> {os.path.relpath(d, ROOT)}/best_match.c")
    r = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "scope", "campaign.py"), "land",
                        w.target.rom_name, ROOT], cwd=ROOT)
    w.sess.event(kind="land", node=node["id"], rc=r.returncode)
    sys.exit(r.returncode)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-f", help="session (function name)")
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("open")
    p.add_argument("file")
    p.add_argument("func", nargs="?")
    p.set_defaults(go=cmd_open)
    p = sub.add_parser("add")
    p.add_argument("file", nargs="+")
    p.add_argument("-p", "--parent")
    p.add_argument("-m", help="the hypothesis this edit tests")
    p.add_argument("--no-expand", action="store_true")
    p.set_defaults(go=cmd_add)
    p = sub.add_parser("expand")
    p.add_argument("node", nargs="?")
    p.add_argument("--fam")
    p.add_argument("--lines")
    p.add_argument("--top", type=int, default=10)
    p.add_argument("--objective", choices=["rotation"])
    p.set_defaults(go=cmd_expand)
    p = sub.add_parser("run")
    p.add_argument("nodes", nargs="*")
    p.add_argument("--budget", type=int, default=3000, help="compiles")
    p.add_argument("--seconds", type=float, default=120)
    p.add_argument("--width", type=int, default=3, help="start from the best N nodes (no nodes given)")
    p.add_argument("--slack", type=int, help="explore children up to this much worse (shape+diff) than the "
                   "best (default max(4, diff/8))")
    p.add_argument("--depth", type=int, default=6, help="moves stacked on a node at most")
    p.add_argument("--objective", choices=["rotation", "block"], help="rotation: rank by the reload spill "
                   "shift still needed (AGBCC_SPILL=K:D); block: by one ROM block's cost (--block)")
    p.add_argument("--rot-max", type=int, help="rotation: test shifts on children with at most this diff")
    p.add_argument("--block", type=int, help="work on one ROM block (its number in the block map): moves only on "
                   "the C lines that produce it, ranked by its own cost first")
    p.add_argument("--fam")
    p.add_argument("--lines")
    p.set_defaults(go=cmd_run)
    for name, fn_ in (("show", cmd_show), ("values", cmd_values), ("file", cmd_file), ("facts", cmd_facts)):
        p = sub.add_parser(name)
        p.add_argument("node", nargs="?")
        if name == "show":
            p.add_argument("--diff", action="store_true")
        p.set_defaults(go=fn_)
    p = sub.add_parser("twins")
    p.add_argument("node", nargs="?")
    p.add_argument("--blocks", help="block numbers (default: every block not exact)")
    p.add_argument("--context", type=int, default=2, help="insns of context on each side")
    p.add_argument("--show", type=int, default=3)
    p.set_defaults(go=cmd_twins)
    p = sub.add_parser("tree")
    p.add_argument("--all", action="store_true")
    p.set_defaults(go=cmd_tree)
    p = sub.add_parser("diff")
    p.add_argument("a")
    p.add_argument("b")
    p.set_defaults(go=cmd_diff)
    p = sub.add_parser("note")
    p.add_argument("node")
    p.add_argument("text", nargs="+")
    p.set_defaults(go=cmd_note)
    p = sub.add_parser("moves")
    p.set_defaults(go=cmd_moves)
    p = sub.add_parser("land")
    p.add_argument("node")
    p.set_defaults(go=cmd_land)
    a = ap.parse_args()
    if a.cmd == "open":
        a.f = None
    a.go(a)


if __name__ == "__main__":
    main()

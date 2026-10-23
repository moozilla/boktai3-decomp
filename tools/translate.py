#!/usr/bin/env python3
"""Turn a compiler-space solution into C: targeted, exhaustive, measured.

  translate.py FILE.c [FUNC] [--seconds N] [--beam N] [-j N] [--targets JSON]

tools/rasolve.py finds what agbcc would have to do differently for the
bytes to match: give a named value (a C variable, or a constant such as
`=65535`) a higher or lower allocation priority, or have one more or one
fewer reload before a given reload. This tool searches C for a change that
does that.

* Neighbors are enumerated, not sampled. Every decomp-permuter pass (the
  project fork's agbcc passes and upstream's) runs under a recording random
  source, which walks all of its choices like an odometer, restricted to the
  lines the targets point at. A step therefore tries every single edit of
  those lines that a pass can make.
* Each candidate is compiled by the agbcc fork server with the instrumented
  dump, so it is scored on what the target measures: the named value's
  priority, or the number of reloads before the target reload, then on the
  differing bytes. A candidate that moves toward the target counts even
  while the bytes stay the same.
* A beam keeps the best states; every state's own dump tells where its
  targets now are, so the focus follows the code as it changes.

FILE.c may be a multi-function unit (isolated automatically, tools/isolate.py). The result is written to
build/translate/FUNC/; review it like any permuter output (upstream passes
do not always keep the program's meaning) and confirm with tools/check.py.
"""
import argparse
import concurrent.futures
import copy
import hashlib
import json
import multiprocessing
import os
import random
import re
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import permute  # noqa: E402
import rasolve  # noqa: E402
import regdiag  # noqa: E402
from romlib import ROOT  # noqa: E402

sys.path.insert(0, permute.PERMUTER)
from src import ast_util, agbcc_passes, randomizer  # noqa: E402
from src.randomizer import FocusRegion, RandomizationFailure  # noqa: E402

# Passes that keep the program's meaning (the default). The new agbcc
# passes check their own safety; the upstream ones listed here only re-spell
# expressions, add braces or empty loops, or reorder declarations.
SAFE_PASSES = [
    "perm_move_to_use", "perm_split_var", "perm_const_var", "perm_literal_address",
    "perm_addr_deref", "perm_loop_form", "perm_call_order", "perm_swap_stmts",
    "perm_reorder_decls", "perm_ins_block", "perm_struct_ref", "perm_compound_assignment",
    "perm_commutative", "perm_add_sub", "perm_inequalities", "perm_condition",
    "perm_reverse_cond", "perm_chain_assignment", "perm_split_assignment",
    "perm_param_copy", "perm_early_return", "perm_reset_const",
]
# These can change the meaning (move a statement past a write or into dead
# code, merge variables, narrow a type, hoist an expression past a call):
# A4FC's best result with them read an uninitialized pointer. --unsafe.
UNSAFE_PASSES = [
    "perm_temp_for_expr", "perm_expand_expr", "perm_reorder_stmts", "perm_remove_var",
    "perm_cast_simple", "perm_randomize_internal_type", "perm_inline",
]
# Edits a programmer plausibly wrote (no `*(&x)` re-reads, no literal
# addresses; a do/while(0) block stands for a statement macro, see natfarm.py): --natural searches only these, so that a
# match reads like original source rather than a compiler-steering trick.
NATURAL_PASSES = [
    "perm_param_copy", "perm_early_return", "perm_reset_const", "perm_move_to_use",
    "perm_split_var", "perm_const_var", "perm_call_order", "perm_swap_stmts", "perm_reorder_decls",
    "perm_struct_ref", "perm_compound_assignment", "perm_commutative", "perm_add_sub",
    "perm_inequalities", "perm_condition", "perm_reverse_cond", "perm_chain_assignment",
    "perm_split_assignment", "perm_loop_form",
    # do { } while (0) around statements: a statement macro's expansion
    "perm_ins_block",
    # unsafe in general, but a natural search keeps only exact matches, and
    # identical bytes cannot mean anything else: retyping a local, a new or
    # removed temporary, a cast, an inline helper, reordered statements
] + UNSAFE_PASSES
PASSES = SAFE_PASSES
CAP_CHOICES = 48   # options tried at one choice point
CAP_RUNS = 400     # runs of one pass per state
# --lazy tries passes in this order: the ones whose edits most often ended in
# a natfarm best state on a 48-function sample (2026-10-23; notes/depin-search-2026-10-23.md)
LEARNED_ORDER = [
    "perm_expand_expr", "perm_ins_block", "perm_cast_simple", "perm_randomize_internal_type",
    "perm_const_var", "perm_split_assignment", "perm_reorder_stmts", "perm_move_to_use",
    "perm_split_var", "perm_commutative",
]
PASS_ORDER = None


class Enumerate(random.Random):
    """A random source that replays a prefix of choices and records the
    number of options at every choice point, so all sequences can be walked."""

    def __init__(self, prefix):
        super().__init__(0)
        self.prefix = prefix
        self.trace = []

    def choose(self, n):
        n = max(1, min(n, CAP_CHOICES))
        i = len(self.trace)
        v = self.prefix[i] if i < len(self.prefix) else 0
        self.trace.append((v, n))
        return min(v, n - 1)

    def choice(self, seq):
        seq = list(seq)
        if not seq:
            raise IndexError
        k = self.choose(len(seq))
        return seq[k * (len(seq) - 1) // max(1, min(len(seq), CAP_CHOICES) - 1)] if len(seq) > CAP_CHOICES else seq[k]

    def randint(self, a, b):
        n = b - a + 1
        k = self.choose(n)
        return a + (k * (n - 1) // max(1, min(n, CAP_CHOICES) - 1) if n > CAP_CHOICES else k)

    def randrange(self, start, stop=None, step=1):
        if stop is None:
            start, stop = 0, start
        return self.randint(start, stop - 1)

    def random(self):
        return (0.0, 0.999)[self.choose(2)]

    def uniform(self, a, b):
        return a + (self.choose(4) + 0.5) / 4 * (b - a)

    def sample(self, population, k):
        pop = list(population)
        out = []
        for _ in range(k):
            out.append(pop.pop(self.choose(len(pop))))
        return out

    def shuffle(self, x):
        pass


def enum_weighted(rnd, values):
    vals = [v for v, p in values if p > 0]
    if not vals:
        raise RandomizationFailure
    return rnd.choice(vals)


randomizer.random_weighted = enum_weighted
agbcc_passes.random_weighted = enum_weighted


def inline_helpers(ast, func):
    """Names of the static inline functions FUNC calls (directly or through
    other helpers): their code is compiled into FUNC."""
    from perm_pycparser import c_ast as ca
    defs = {e.decl.name: e for e in ast.ext if isinstance(e, ca.FuncDef)}
    out, todo = [], [func]
    while todo:
        f = defs.get(todo.pop())
        if f is None:
            continue

        class V(ca.NodeVisitor):
            def visit_FuncCall(self, n):
                if isinstance(n.name, ca.ID) and n.name.name in defs and n.name.name != func \
                        and n.name.name not in out and "inline" in (defs[n.name.name].decl.funcspec or []):
                    out.append(n.name.name)
                    todo.append(n.name.name)
                self.generic_visit(n)

        V().visit(f.body)
    return out


def neighbors(text, func, focus_lines, passes=PASSES, helpers=True):
    """Every distinct source one pass makes from TEXT, changing only
    FOCUS_LINES (lines of TEXT) in FUNC, or anything in the inline helpers
    FUNC calls; yields (pass name, source)."""
    base_ast = ast_util.parse_c(text)
    seen = set()
    for name in passes:
        for target in [func] + (inline_helpers(base_ast, func) if helpers else []):
            focus = focus_lines if target == func else []
            outs = _neighbors_of(base_ast, target, focus, name, seen, shallow=True)
            if outs is None:  # the pass changed something outside the function: copy it all
                base_ast = ast_util.parse_c(text)
                outs = _neighbors_of(base_ast, target, focus, name, seen, shallow=False)
            yield from outs


def clone(n):
    """Deep copy of a pycparser tree, several times faster than copy.deepcopy
    (nodes are __slots__ objects; coordinates are shared, not copied)."""
    if isinstance(n, list):
        return [clone(x) for x in n]
    if not isinstance(n, ast_util.ca.Node):
        return n
    cls = type(n)
    new = cls.__new__(cls)
    for slot in cls.__slots__:
        if slot == "__weakref__" or not hasattr(n, slot):
            continue
        v = getattr(n, slot)
        setattr(new, slot, v if slot == "coord" else clone(v))
    return new


def to_c_cached(ast, cache, base_ids):
    """ast_util.to_c(ast), reusing the text of top-level items that are the
    base's own objects (BASE_IDS; passes do not change them, which
    _neighbors_of checks); CACHE: id -> (node, text). The generator's FileAST layout."""
    parts = []
    for ext in ast.ext:
        hit = cache.get(id(ext))
        if hit is None or hit[0] is not ext:
            text = ast_util.PatchedCGenerator().visit(ext)
            if not isinstance(ext, ast_util.ca.FuncDef):
                text += "\n" if isinstance(ext, ast_util.ca.Pragma) else ";\n"
            hit = (ext, text)
            if id(ext) in base_ids:
                cache[id(ext)] = hit
        parts.append(hit[1])
    return ast_util.process_pragmas("".join(parts))


def _copy_for(base_ast, func, shallow):
    """A copy of BASE_AST a pass can change. Shallow: only FUNC's definition is
    copied (deep-copying the whole preprocessed file took 90% of the time);
    the top-level list is new, so added declarations stay out of the base."""
    if not shallow:
        return copy.deepcopy(base_ast)
    ast = copy.copy(base_ast)
    ast.ext = [clone(n) if isinstance(n, ast_util.ca.FuncDef) and n.decl.name == func else n
               for n in base_ast.ext]
    return ast


def _neighbors_of(base_ast, func, focus_lines, name, seen, shallow=False):
    if shallow:
        # extract_fn drops `static` from function declarations in place: settle
        # that once, then any other change to the base means a pass wrote to it
        ast_util.extract_fn(_copy_for(base_ast, func, True), func)
        before = ast_util.to_c(base_ast)
        cache = {}
        # top-level items and the declarations extract_fn puts in place of definitions
        base_ids = {id(n) for n in base_ast.ext}
        base_ids |= {id(n.decl) for n in base_ast.ext if isinstance(n, ast_util.ca.FuncDef)}
    found = []
    for name in [name]:
        fn_pass = getattr(randomizer, name, None) or getattr(agbcc_passes, name, None)
        if fn_pass is None:
            continue
        prefix, runs = [], 0
        while runs < CAP_RUNS:
            runs += 1
            ast = _copy_for(base_ast, func, shallow)
            fn, _ = ast_util.extract_fn(ast, func)
            ast_util.normalize_ast(fn, ast)
            indices = ast_util.compute_node_indices(fn)
            region = FocusRegion(fn, indices, set(focus_lines)) if focus_lines else randomizer.Region.unbounded()
            rnd = Enumerate(prefix)
            ok = True
            try:
                fn_pass(fn, ast, indices, region, rnd)
                ast_util.compute_node_indices(fn)
                out = to_c_cached(ast, cache, base_ids) if shallow else ast_util.to_c(ast)
            except Exception:
                ok = False
            if ok:
                h = hashlib.sha1(out.encode()).digest()
                if h not in seen:
                    found.append((h, name, out))
            # next choice sequence (odometer over the recorded options)
            trace = rnd.trace
            j = len(trace) - 1
            while j >= 0 and trace[j][0] + 1 >= trace[j][1]:
                j -= 1
            if j < 0:
                break
            prefix = [v for v, _ in trace[:j]] + [trace[j][0] + 1]
    if shallow and ast_util.to_c(base_ast) != before:
        return None
    outs = []
    for h, name, out in found:
        if h not in seen:
            seen.add(h)
            outs.append((name, out))
    return outs


# --- scoring ---------------------------------------------------------------

def _thumb_mask(h):
    """Mask the register fields of one Thumb halfword (allocation-blind shape)."""
    top5 = h >> 11
    if top5 <= 2:  # shift by immediate
        return h & ~0x3F
    if top5 == 3:  # add/sub register or imm3
        return h & ~(0x3F | (0 if h & 0x400 else 0x1C0))
    if top5 <= 7:  # mov/cmp/add/sub imm8
        return h & ~0x700
    if h >> 10 == 0x10:  # ALU
        return h & ~0x3F
    if h >> 10 == 0x11:  # hi-register ops / bx
        return h & ~0xFF
    if top5 == 9:  # ldr pc-relative
        return h & ~0x700
    if h >> 12 == 5:  # load/store register offset
        return h & ~0x1FF
    if h >> 13 == 3 or h >> 12 == 8:  # load/store immediate offset, halfword
        return h & ~0x3F
    if h >> 12 in (9, 10):  # sp-relative load/store, add to pc/sp
        return h & ~0x700
    if h >> 12 == 11 and (h >> 9) & 3 == 2:  # push/pop
        return h & ~0xFF
    if h >> 12 == 12:  # ldm/stm
        return h & ~0x7FF
    return h


def shape_distance(orig, mine):
    """Halfwords that differ with register fields masked: how far the code's
    shape is from the ROM's, apart from allocation."""
    a = [_thumb_mask(int.from_bytes(orig[i:i + 2], "little")) for i in range(0, len(orig) - 1, 2)]
    b = [_thumb_mask(int.from_bytes(mine[i:i + 2], "little")) for i in range(0, len(mine) - 1, 2)]
    if a == b:
        return 0
    import difflib
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    return max(1, sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != "equal"))


_last_pair = [None]
_orig_distance = rasolve.distance


def _recording_distance(orig, mine):
    _last_pair[0] = (orig, mine)
    return _orig_distance(orig, mine)


rasolve.distance = _recording_distance

_cc = None
_targets = None
_base_feat = None


def _init(src, func, targets=None, base_feat=None):
    global _cc, _targets, _base_feat
    _cc = rasolve.Compiler(src, func, lines=False)
    _cc.flags = _cc.flags + ["-g"]  # line numbers in the reload dump
    _targets, _base_feat = targets, base_feat


def evaluate(text):
    """(bytes, features, residual bytes): residual is what still differs when
    the compiler is told to close the remaining target gap; 0 means the
    candidate broke nothing on the way."""
    _cc.pre = text
    b, dump = _cc.run(dump=True)
    if b is None:
        return None
    feat = summarize(dump)
    feat["shape"] = shape_distance(*_last_pair[0]) if b and _last_pair[0] else 0
    # where each reload target's statement is in this text
    lines = [norm_line(x) for x in text.splitlines()] if _targets else []
    feat["anchors"] = {} if not _targets else {i: [n + 1 for n, x in enumerate(lines) if x == t["anchor"]]
                       for i, t in enumerate(_targets or []) if t.get("anchor")}
    # what the candidate compiles to, without debug lines: equal code, equal state
    code = re.sub(r"^\s*\.(stab|loc|file|ident)[^\n]*\n|^\.?L[MBT]?\w*:\n", "",
                  _cc.last_asm or "", flags=re.M)
    feat["code"] = hashlib.sha1(code.encode()).hexdigest()
    residual = b
    if _targets and _base_feat is not None:
        env = residual_env(feat, _targets, _base_feat)
        if env:
            r, _ = _cc.run(env=env)
            residual = r if r is not None else 10 ** 6
    return b, feat, residual


def residual_env(feat, targets, base_feat):
    pri, spill = [], []
    for ti, t in enumerate(targets):
        if t["kind"] == "pri":
            got = feat["pri"].get(t["label"])
            if got:
                v = max(got) if t["raise"] else min(got)
                d = t["goal"] - v
                if (d > 0) == t["raise"] and d:
                    pri.append(f"{t['label']}:{d}")
        else:
            k_c, k_b = reload_index(feat, t, ti), reload_index(base_feat, t, ti)
            if k_c is not None and k_b is not None:
                d = t["shift"] - (k_c - k_b)
                if d:
                    if t.get("anchor"):
                        spill.append(("index", k_c, d))
                    else:
                        spill.append(f"{t['label']}#{t['occurrence']}:{d}")
    env = {}
    if pri:
        env["AGBCC_PRI_LABEL"] = ",".join(pri)
    by_label = [x for x in spill if isinstance(x, str)]
    by_index = [x for x in spill if not isinstance(x, str)]
    if by_label:
        env["AGBCC_SPILL_LABEL"] = ",".join(by_label)
    if by_index:
        env["AGBCC_SPILL"] = ",".join(f"{k}:{d}" for _, k, d in by_index)
    return env


def norm_line(x):
    return re.sub(r"\s+", "", x)


def reload_index(feat, t, ti=None):
    """Index of the target reload among all reloads: the one on the target
    statement's line (anchor), else by label and occurrence."""
    if t.get("anchor") and ti is not None and feat.get("anchors", {}).get(ti):
        line = feat["anchors"][ti][0]
        on = [k for k, lab, ln in feat["reloads"] if ln == line]
        if len(on) > t.get("anchor_occ", 0):
            return on[t.get("anchor_occ", 0)]
        return None
    same = [k for k, lab, line in feat["reloads"] if lab == t["label"]]
    return same[t["occurrence"]] if len(same) > t["occurrence"] else None


def summarize(dump):
    """What the targets measure, from one dump: priorities by label, and
    reloads (index, label, line)."""
    pri = {}
    for p in dump.values():
        name = getattr(p, "name", "-")
        if name != "-":
            pri.setdefault(name, []).append(p.pri)
    reloads = [(r[0], r[5] if len(r) > 5 else "-", r[3]) for r in dump.reloads]
    return {"pri": pri, "reloads": reloads}


def gap(feat, targets, base_feat):
    total = 0.0
    for ti, t in enumerate(targets):
        if t["kind"] == "pri":
            got = feat["pri"].get(t["label"])
            if not got:
                total += 1.0
                continue
            v = max(got) if t["raise"] else min(got)
            miss = (t["goal"] - v) if t["raise"] else (v - t["goal"])
            total += max(0.0, miss / max(abs(t["goal"]), 1))
        else:
            k_c, k_b = reload_index(feat, t, ti), reload_index(base_feat, t, ti)
            if k_c is None or k_b is None:
                total += 1.0
                continue
            total += abs((k_c - k_b) - t["shift"])
    return total


_regdiag_cache = {}


def regdiag_lines(text, func):
    """Lines of TEXT whose code differs from the ROM (tools/regdiag.py)."""
    key = hashlib.sha1(text.encode()).digest()
    if key not in _regdiag_cache:
        import contextlib
        import io
        import tempfile
        with tempfile.TemporaryDirectory(prefix="tr-", dir=os.path.join(ROOT, "build")) as d:
            path = os.path.join(d, "state.c")
            open(path, "w").write(text)
            try:
                with contextlib.redirect_stdout(io.StringIO()):
                    lines = regdiag.analyze(path, func).focus_lines()
            except BaseException:
                lines = set()
        _regdiag_cache[key] = lines
    return _regdiag_cache[key]


def focus_for(text, feat, targets, func=None):
    """Lines of TEXT the targets point at; without targets, the lines whose
    code differs."""
    lines = text.splitlines()
    out = set()
    if not targets and func:
        return sorted(x + k for x in regdiag_lines(text, func) for k in (-1, 0, 1) if x + k > 0)
    for t in targets:
        if t["kind"] == "pri" and re.match(r"^[A-Za-z_]\w*$", t["label"]):
            pat = re.compile(rf"\b{re.escape(t['label'])}\b")
            out |= {i + 1 + k for i, ln in enumerate(lines) if pat.search(ln) for k in (-1, 0, 1)}
        elif t["kind"] == "reload":
            ti = targets.index(t)
            k = reload_index(feat, t, ti)
            same = [(kk, ln) for kk, lab, ln in feat["reloads"] if kk == k]
            if same:
                k, line = same[0]
                # a reload anywhere since the previous reload moves the rotation
                before = [ln for kk, lab, ln in feat["reloads"] if kk < k and 0 < ln <= line]
                start = max(before) if before else 1
                if not before:  # the first reload: the function's start
                    m = re.search(r"\b" + re.escape(func or "") + r"\s*\(", text) if func else None
                    start = text[:m.start()].count("\n") + 1 if m else max(1, line - 40)
                out |= set(range(start - 1, line + 2))
    return sorted(x for x in out if x > 0)


def expand(job):
    """Neighbors of one state for one pass (runs in a worker)."""
    text, func, focus, name = job
    return [out for _, out in neighbors(text, func, focus, [name])]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--targets", help="rasolve.py --json-out file (default: run rasolve); "
                    "'none': search the lines whose code differs, by bytes alone")
    ap.add_argument("--seconds", type=int, default=600)
    ap.add_argument("--steps", type=int, default=0, help="stop after N steps (0: no limit)")
    ap.add_argument("--beam", type=int, default=6)
    ap.add_argument("-j", type=int, default=max(1, (os.cpu_count() or 2) // 2),
                    help="worker processes (default: half the cores; all of them saturated a 16 GB machine)")
    ap.add_argument("--unsafe", action="store_true", help="also use passes that can change the program's meaning")
    ap.add_argument("--natural", action="store_true", help="only edits a programmer plausibly wrote (NATURAL_PASSES)")
    ap.add_argument("--lazy", action="store_true",
                    help="generate each state's neighbors one pass at a time, most productive passes first "
                         "across all states, and stop at the first match")
    ap.add_argument("--shape", action="store_true",
                    help="beam also keeps the states whose code is closest to the ROM's with registers masked")
    ap.add_argument("--pass-order", default="",
                    help="comma-separated pass priority for --lazy (default: LEARNED_ORDER)")
    a = ap.parse_args()
    global PASSES, PASS_ORDER
    PASS_ORDER = a.pass_order.split(",") if a.pass_order else LEARNED_ORDER
    if a.natural:
        PASSES = NATURAL_PASSES
    elif a.unsafe:
        PASSES = SAFE_PASSES + UNSAFE_PASSES
    src = os.path.abspath(a.src)
    func = a.func or regdiag.first_function(src)
    import isolate
    src = os.path.join(ROOT, isolate.ensure(src, func))  # multi-function unit: isolated copy
    out_dir = os.path.join(ROOT, "build", "translate", func)
    os.makedirs(out_dir, exist_ok=True)
    t0 = time.time()
    if a.targets == "none":
        sol = {"score": None, "targets": []}
    elif not a.targets:
        a.targets = os.path.join(out_dir, "targets.json")
        r = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "rasolve.py"), src, func,
                            "--seconds", "120", "--json-out", a.targets], capture_output=True, text=True)
        print(r.stdout.strip().split("\n\n")[-2] if "\n\n" in r.stdout else r.stdout.strip()[-2000:])
        if not os.path.exists(a.targets):
            sys.exit("rasolve found no compiler-space solution")
    if a.targets != "none":
        sol = json.load(open(a.targets))
    targets = sol["targets"]
    if sol["score"]:
        print(f"note: the compiler-space solution leaves {sol['score']} bytes")
    print(f"targets: {targets}")

    base, _ = permute.prepare_source(os.path.relpath(src, ROOT), func)
    base = ast_util.to_c(ast_util.parse_c(base))  # the form neighbors are printed in
    _init(src, func)
    base_b, base_feat, _ = evaluate(base)
    # anchor each reload target to its statement's text in the base
    base_lines = base.splitlines()
    for t in targets:
        if t["kind"] != "reload":
            continue
        same = [(k, ln) for k, lab, ln in base_feat["reloads"] if lab == t["label"]]
        if len(same) > t["occurrence"] and 0 < same[t["occurrence"]][1] <= len(base_lines):
            k, ln = same[t["occurrence"]]
            t["anchor"] = norm_line(base_lines[ln - 1])
            t["anchor_occ"] = [kk for kk, lab, l2 in base_feat["reloads"] if l2 == ln].index(k)
    _init(src, func, targets)
    base_b, base_feat, _ = evaluate(base)
    g0 = gap(base_feat, targets, base_feat)
    print(f"base: {base_b} differing bytes (shape {base_feat.get('shape', 0)}), target gap {g0:.3f}")
    ctx = multiprocessing.get_context("spawn")
    with concurrent.futures.ProcessPoolExecutor(a.j, mp_context=ctx, initializer=_init,
                                                initargs=(src, func, targets, base_feat)) as pool:
        # state: (residual, gap, bytes, text, features, history)
        _, _, res0 = pool.submit(evaluate, base).result()
        states = [(res0, g0, base_b, base, base_feat, [])]
        best = states[0]
        seen = {hashlib.sha1(base.encode()).digest()}
        seen_code = {base_feat.get("code")}
        step = 0
        while time.time() - t0 < a.seconds and best[2] != 0 and not (a.steps and step >= a.steps):
            step += 1
            if a.lazy:
                # (state, pass) pairs in priority order: every state's most
                # productive passes before any state's least productive ones;
                # neighbors generated only when their turn comes
                order = PASS_ORDER or PASSES
                pairs = sorted(((order.index(n) if n in order else len(order), si, st, n)
                                for si, st in enumerate(states) for n in PASSES), key=lambda x: x[:2])
                jobs, results = [], []
                focus_of = {}
                hit = False
                for _, si, st, name in pairs:
                    if si not in focus_of:
                        focus_of[si] = focus_for(st[3], st[4], targets, func)
                    new_jobs = []
                    for cand in expand((st[3], func, focus_of[si], name)):
                        h = hashlib.sha1(cand.encode()).digest()
                        if h not in seen:
                            seen.add(h)
                            new_jobs.append((cand, st[5] + [name]))
                    for i in range(0, len(new_jobs), 16):
                        part = new_jobs[i:i + 16]
                        res = list(pool.map(evaluate, [c for c, _ in part], chunksize=2))
                        jobs += part
                        results += res
                        if any(r and r[0] == 0 for r in res) or time.time() - t0 >= a.seconds:
                            hit = True
                            break
                    if hit:
                        break
                if not jobs:
                    print("no new neighbors")
                    break
            else:
                gen_jobs, owners = [], []
                for st in states:
                    focus = focus_for(st[3], st[4], targets, func)
                    for name in PASSES:
                        gen_jobs.append((st[3], func, focus, name))
                        owners.append(st)
                jobs = []
                for st, job, outs in zip(owners, gen_jobs, pool.map(expand, gen_jobs)):
                    for cand in outs:
                        h = hashlib.sha1(cand.encode()).digest()
                        if h not in seen:
                            seen.add(h)
                            jobs.append((cand, st[5] + [job[3]]))
                if not jobs:
                    print("no new neighbors")
                    break
                # in chunks, best states' candidates first: stop at a match or when the time is up
                results, cands, chunk = [], [c for c, _ in jobs], max(16, 8 * a.j)
                for i in range(0, len(cands), chunk):
                    part = list(pool.map(evaluate, cands[i:i + chunk], chunksize=2))
                    results += part
                    if any(r and r[0] == 0 for r in part) or time.time() - t0 >= a.seconds:
                        break
            jobs = jobs[:len(results)]
            scored = []
            for (cand, hist), res in zip(jobs, results):
                if res is None:
                    continue
                b, feat, residual = res
                if feat["code"] in seen_code:  # same code as a state already tried
                    continue
                seen_code.add(feat["code"])
                scored.append((residual, gap(feat, targets, base_feat), b, cand, feat, hist))
            if not scored:
                break
            # On the path: nothing else broken (low residual), closer to the
            # target; also the best gap closers (a later edit may repair them)
            # and the fewest differing bytes.
            by_path = sorted(scored, key=lambda s: (s[0], s[1], s[2]))
            by_gap = sorted(scored, key=lambda s: (s[1], s[0], s[2]))
            by_bytes = sorted(scored, key=lambda s: (s[2], s[0]))
            keep = []
            pool_ = by_path[:a.beam] + by_gap[:max(1, a.beam // 2)] + by_bytes[:max(1, a.beam // 3)]
            if a.shape:
                # the code's shape apart from allocation: a state with the
                # ROM's instructions in other registers is one allocation
                # decision away, even when its byte count is not the lowest
                by_shape = sorted(scored, key=lambda s: (s[4].get("shape", 0), s[2]))
                pool_ = by_bytes[:max(1, a.beam // 2)] + by_shape[:a.beam]
            for s_ in pool_:
                if s_ not in keep:
                    keep.append(s_)
            states = keep
            top = min(scored, key=lambda s_: (s_[2], s_[0], s_[1]))
            if (top[2], top[0]) < (best[2], best[0]):
                best = top
            print(f"step {step}: {len(jobs)} candidates, {len(scored)} new codes; on-path: residual "
                  f"{by_path[0][0]} gap {by_path[0][1]:.3f} bytes {by_path[0][2]}; closest: gap {by_gap[0][1]:.3f} "
                  f"residual {by_gap[0][0]}; fewest bytes {by_bytes[0][2]}; "
                  f"{time.time() - t0:.0f}s; best shape {min(s[4].get('shape', 0) for s in scored)}", flush=True)
    path = os.path.join(out_dir, "best.c")
    open(path, "w").write(best[3])
    print(f"\n{'MATCH' if best[2] == 0 else f'{best[2]} differing bytes'} after "
          f"{len(best[5])} edit(s): {path}")
    if best[5]:
        print("edits: " + " ".join(best[5]))


if __name__ == "__main__":
    main()

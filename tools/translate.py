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

FILE.c holds one ROM function (tools/isolate.py). The result is written to
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

# Passes that keep the program's meaning or change only types and
# temporaries; those that delete code or add dummy statements are left out.
PASSES = [
    "perm_move_to_use", "perm_split_var", "perm_const_var", "perm_literal_address",
    "perm_addr_deref", "perm_loop_form",
    "perm_temp_for_expr", "perm_expand_expr", "perm_reorder_stmts", "perm_reorder_decls",
    "perm_struct_ref", "perm_split_assignment", "perm_compound_assignment",
    "perm_commutative", "perm_add_sub", "perm_inequalities", "perm_cast_simple",
    "perm_ins_block", "perm_remove_var", "perm_chain_assignment", "perm_condition",
    "perm_randomize_internal_type", "perm_reverse_cond", "perm_inline",
]
CAP_CHOICES = 48   # options tried at one choice point
CAP_RUNS = 400     # runs of one pass per state


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


def neighbors(text, func, focus_lines, passes=PASSES):
    """Every distinct source one pass makes from TEXT, changing only
    FOCUS_LINES (lines of TEXT); yields (pass name, source)."""
    base_ast = ast_util.parse_c(text)
    seen = set()
    for name in passes:
        fn_pass = getattr(randomizer, name, None) or getattr(agbcc_passes, name, None)
        if fn_pass is None:
            continue
        prefix, runs = [], 0
        while runs < CAP_RUNS:
            runs += 1
            ast = copy.deepcopy(base_ast)
            fn, _ = ast_util.extract_fn(ast, func)
            ast_util.normalize_ast(fn, ast)
            indices = ast_util.compute_node_indices(fn)
            region = FocusRegion(fn, indices, set(focus_lines)) if focus_lines else randomizer.Region.unbounded()
            rnd = Enumerate(prefix)
            ok = True
            try:
                fn_pass(fn, ast, indices, region, rnd)
                ast_util.compute_node_indices(fn)
                out = ast_util.to_c(ast)
            except Exception:
                ok = False
            if ok:
                h = hashlib.sha1(out.encode()).digest()
                if h not in seen:
                    seen.add(h)
                    yield name, out
            # next choice sequence (odometer over the recorded options)
            trace = rnd.trace
            j = len(trace) - 1
            while j >= 0 and trace[j][0] + 1 >= trace[j][1]:
                j -= 1
            if j < 0:
                break
            prefix = [v for v, _ in trace[:j]] + [trace[j][0] + 1]


# --- scoring ---------------------------------------------------------------

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
    for t in targets:
        if t["kind"] == "pri":
            got = feat["pri"].get(t["label"])
            if got:
                v = max(got) if t["raise"] else min(got)
                d = t["goal"] - v
                if (d > 0) == t["raise"] and d:
                    pri.append(f"{t['label']}:{d}")
        else:
            k_c, k_b = reload_index(feat, t), reload_index(base_feat, t)
            if k_c is not None and k_b is not None:
                d = t["shift"] - (k_c - k_b)
                if d:
                    spill.append(f"{t['label']}#{t['occurrence']}:{d}")
    env = {}
    if pri:
        env["AGBCC_PRI_LABEL"] = ",".join(pri)
    if spill:
        env["AGBCC_SPILL_LABEL"] = ",".join(spill)
    return env


def reload_index(feat, t):
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
    for t in targets:
        if t["kind"] == "pri":
            got = feat["pri"].get(t["label"])
            if not got:
                total += 1.0
                continue
            v = max(got) if t["raise"] else min(got)
            miss = (t["goal"] - v) if t["raise"] else (v - t["goal"])
            total += max(0.0, miss / max(abs(t["goal"]), 1))
        else:
            k_c, k_b = reload_index(feat, t), reload_index(base_feat, t)
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
        d = tempfile.mkdtemp(prefix="tr-", dir=os.path.join(ROOT, "build"))
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
            same = [(k, line) for k, lab, line in feat["reloads"] if lab == t["label"]]
            if len(same) > t["occurrence"]:
                line = same[t["occurrence"]][1]
                out |= set(range(line - 4, line + 2))
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
    ap.add_argument("--beam", type=int, default=6)
    ap.add_argument("-j", type=int, default=os.cpu_count() or 4)
    a = ap.parse_args()
    src = os.path.abspath(a.src)
    func = a.func or regdiag.first_function(src)
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
    g0 = gap(base_feat, targets, base_feat)
    print(f"base: {base_b} differing bytes, target gap {g0:.3f}")
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
        while time.time() - t0 < a.seconds and best[2] != 0:
            step += 1
            gen_jobs, owners = [], []
            for st in states:
                focus = focus_for(st[3], st[4], targets, func)
                for name in PASSES:
                    gen_jobs.append((st[3], func, focus, name))
                    owners.append(st)
            jobs = []
            for st, outs in zip(owners, pool.map(expand, gen_jobs)):
                for cand in outs:
                    h = hashlib.sha1(cand.encode()).digest()
                    if h not in seen:
                        seen.add(h)
                        jobs.append((cand, st[5]))
            if not jobs:
                print("no new neighbors")
                break
            results = pool.map(evaluate, [c for c, _ in jobs], chunksize=4)
            scored = []
            for (cand, hist), res in zip(jobs, results):
                if res is None:
                    continue
                b, feat, residual = res
                if feat["code"] in seen_code:  # same code as a state already tried
                    continue
                seen_code.add(feat["code"])
                scored.append((residual, gap(feat, targets, base_feat), b, cand, feat, hist + ["edit"]))
            if not scored:
                break
            # On the path: nothing else broken (low residual), closer to the
            # target; also the best gap closers (a later edit may repair them)
            # and the fewest differing bytes.
            by_path = sorted(scored, key=lambda s: (s[0], s[1], s[2]))
            by_gap = sorted(scored, key=lambda s: (s[1], s[0], s[2]))
            by_bytes = sorted(scored, key=lambda s: (s[2], s[0]))
            keep = []
            for s_ in by_path[:a.beam] + by_gap[:max(1, a.beam // 2)] + by_bytes[:max(1, a.beam // 3)]:
                if s_ not in keep:
                    keep.append(s_)
            states = keep
            top = min(scored, key=lambda s_: (s_[2], s_[0], s_[1]))
            if (top[2], top[0]) < (best[2], best[0]):
                best = top
            print(f"step {step}: {len(jobs)} candidates, {len(scored)} new codes; on-path: residual "
                  f"{by_path[0][0]} gap {by_path[0][1]:.3f} bytes {by_path[0][2]}; closest: gap {by_gap[0][1]:.3f} "
                  f"residual {by_gap[0][0]}; fewest bytes {by_bytes[0][2]}; "
                  f"{time.time() - t0:.0f}s", flush=True)
    path = os.path.join(out_dir, "best.c")
    open(path, "w").write(best[3])
    print(f"\n{'MATCH' if best[2] == 0 else f'{best[2]} differing bytes'} after "
          f"{len(best[5])} edit(s): {path}")


if __name__ == "__main__":
    main()

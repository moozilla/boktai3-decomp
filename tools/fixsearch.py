#!/usr/bin/env python3
"""Find the C edit that makes a near-miss draft match: a systematic local search.

  fixsearch.py FILE.c [FUNC] [--beam 6] [--depth 4] [--seeds 40] [-j N] [--seconds S]

FILE.c holds one ROM function (tools/isolate.py). Unlike the permuter's random
walk, every round enumerates the whole neighbourhood of each kept candidate:
each rewrite (the permuter's passes plus agbcc-specific ones in this file) is
applied with many seeds and the distinct results are all compiled (agbcc fork
server, ~10 ms each) and scored. Score is structure first: instruction-shape
distance from the ROM (registers and constants abstracted), then aligned
halfword distance. The best --beam candidates go to the next round.

Prints the best candidate's function as a unified diff against the draft and
writes it to build/fixsearch/FUNC/best.c (a full file: drop it in place of the
draft). Exit 0 on an exact match.
"""
import argparse
import copy
import difflib
import hashlib
import multiprocessing
import os
import re
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import idioms  # noqa: E402
import permute  # noqa: E402
import rasolve  # noqa: E402
from romlib import ROOT, off  # noqa: E402

sys.path.insert(0, permute.PERMUTER)
from src import ast_util  # noqa: E402
from src import randomizer as R  # noqa: E402
from perm_pycparser import c_ast as ca  # noqa: E402

# --- scoring -------------------------------------------------------------------

_cc = None
_rom_shapes = None


def _init_worker(src, func):
    global _cc, _rom_shapes
    _cc = rasolve.Compiler(src, func, lines=False)
    n = _cc.size
    rom = _cc.rom[off(_cc.address):off(_cc.address) + n]
    _rom_shapes = [idioms.shape(t) for _, t in idioms.disasm_function(_cc.rom, _cc.address, n)]
    _cc.rom_code = rom


def shape_distance(a, b):
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    return sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != "equal")


def evaluate(text):
    """(shape distance, byte distance) or None for a compile error."""
    try:
        asm = _cc._asm(text, {})
    except Exception:
        return None
    if asm is None:
        return None
    obj = _cc.resolver.assemble(asm, _cc.work)
    if obj is None:
        return None
    mine, missing = _cc.resolver.bytes_of(obj, _cc.func)
    if mine is None or missing:
        return None
    rom = _cc.rom_code
    mine = mine[:max(len(rom), 2)]
    bd = rasolve.distance(rom, mine[:len(rom)]) if mine[:len(rom)] != rom else 0
    if bd == 0:
        return (0, 0, 0)
    import regdiag
    dis = regdiag.strip_pools(idioms.disasm(mine[:len(rom)], 0), len(rom), mine, _cc.address)
    prefix = next((i for i in range(0, len(rom), 2) if rom[i:i + 2] != mine[i:i + 2]), len(rom))
    return (shape_distance(_rom_shapes, [idioms.shape(t) for _, t in dis]), -prefix // 2, bd)


# --- rewrites ----------------------------------------------------------------------

import rewrites  # noqa: E402

# permuter passes worth enumerating (the rest add junk: float literals, xor 0, ...)
PERMUTER_PASSES = ["perm_temp_for_expr", "perm_reorder_stmts", "perm_chain_assignment", "perm_split_assignment",
                   "perm_compound_assignment", "perm_inline", "perm_condition", "perm_reverse_cond",
                   "perm_inequalities", "perm_add_sub", "perm_cast_simple", "perm_refer_to_var",
                   "perm_duplicate_assignment", "perm_loop_form", "perm_move_to_use", "perm_split_var",
                   "perm_const_var", "perm_call_order", "perm_struct_ref", "perm_ins_block"]


def neighbours(fn, ast, func, seeds, concessions=False):
    """{function text: (description, FuncDef)} one rewrite away from FN."""
    out = {}
    base = ast_util.to_c(fn)
    for desc, f2 in rewrites.all_rewrites(fn, concessions):
        try:
            t = ast_util.to_c(f2)
        except Exception:
            continue
        if t != base and t not in out:
            out[t] = (desc, f2)
    if seeds:
        by_name = {p.__name__: p for p in R.RANDOMIZATION_PASSES}
        for name in PERMUTER_PASSES:
            p = by_name.get(name)
            if p is None:
                continue
            for seed in range(seeds):
                f2 = copy.deepcopy(fn)
                ast.ext[-1] = f2
                try:
                    indices = ast_util.compute_node_indices(f2)
                    p(f2, ast, indices, R.Region.unbounded(), R.Random(seed * 7919 + len(name)))
                    ast_util.compute_node_indices(f2)
                    t = ast_util.to_c(f2)
                except Exception:
                    continue
                if t != base and t not in out:
                    out[t] = (name, f2)
        ast.ext[-1] = fn
    return out


# --- search -------------------------------------------------------------------------

def function_text(text, func):
    m = None
    for m in re.finditer(r"^[^\n;#]*\b" + re.escape(func) + r"\s*\([^;{]*\)\s*\{", text, re.M):
        pass
    return text[m.start():] if m else text


def prepare(src, func):
    rel = os.path.relpath(os.path.abspath(src), ROOT)
    pre, _ = permute.prepare_source(rel, func)
    ast = ast_util.parse_c(pre)
    fn, _ = ast_util.extract_fn(ast, func)
    # declarations before, the function last; FUNC's own prototypes dropped so
    # its signature may change
    ext = [e for e in ast.ext if e is not fn and not (isinstance(e, ca.Decl) and e.name == func)]
    ast.ext = ext + [fn]
    prefix = ast_util.to_c(ca.FileAST(ext))
    return rel, ast, fn, prefix


MAX_CANDIDATES = 400   # per batch: one batch must fit the time budget
MAX_QUEUE = 4000       # states kept; each holds a parse tree (a run once reached 8.4 GB)


def beam_search(pool, prefix, frontier, seen, depth, beam, seeds, ast, func, concessions, deadline, log, tag):
    """Best-first search: expand the best unexpanded state (score, then shorter
    source), BEAM states per batch, until a match, the deadline, or DEPTH*BEAM
    expansions. Returns (best, the best few states, compiles)."""
    import heapq
    heap = []
    counter = 0
    for st in frontier:
        heapq.heappush(heap, (st[0], len(ast_util.to_c(st[1])), counter, st))
        counter += 1
    best = min(frontier, key=lambda x: (x[0], len(ast_util.to_c(x[1]))))
    explored = []
    compiles = 0
    for rnd in range(depth * 3):
        batch = [heapq.heappop(heap)[3] for _ in range(min(beam, len(heap)))]
        if not batch:
            break
        explored += batch
        if time.time() > deadline:
            break
        cands = []
        for score, f, hist in batch:
            if len(hist) >= depth + 3 or len(cands) >= MAX_CANDIDATES or time.time() > deadline:
                continue
            for t, (desc, f2) in neighbours(f, ast, func, seeds, concessions).items():
                if t in seen:
                    continue
                seen.add(t)
                cands.append((t, f2, hist + [desc]))
                if len(cands) >= MAX_CANDIDATES:
                    break
        if not cands:
            continue
        scores = []                              # compile in chunks so the deadline holds mid-batch
        for i in range(0, len(cands), 32):
            scores += pool.map(evaluate, [prefix + t for t, _, _ in cands[i:i + 32]], chunksize=4)
            if time.time() > deadline:
                break
        cands = cands[:len(scores)]
        compiles += len(cands)
        for s_, (t, f2, h) in zip(scores, cands):
            if s_ is None:
                continue
            heapq.heappush(heap, (s_, len(t), counter, (s_, f2, h)))
            counter += 1
            if (s_, len(t)) < (best[0], len(ast_util.to_c(best[1]))):
                best = (s_, f2, h)
        if len(heap) > MAX_QUEUE:                # memory: keep only the best states
            heap = heapq.nsmallest(MAX_QUEUE // 2, heap)
            heapq.heapify(heap)
        log(f"  {tag} batch {rnd + 1}: {len(cands)} candidates, best {best[0]}, queue {len(heap)}")
        if best[0][0] == 0 and best[0][2] == 0 or time.time() > deadline:
            break
    rest = [heapq.heappop(heap)[3] for _ in range(min(beam, len(heap)))]
    return best, sorted(explored + rest, key=lambda x: x[0])[:beam], compiles


def cleanup(pool, prefix, best, seconds=60):
    """Simplify while the score holds: any rewrite or deletion that keeps the
    score (or improves it) and shortens the function is taken."""
    score, f, hist = best
    t_end = time.time() + seconds
    while time.time() < t_end:
        cur = len(ast_util.to_c(f))
        trials = []
        for bi, b in enumerate(rewrites.blocks(f)):
            for i in range(len(b.block_items)):
                f2 = copy.deepcopy(f)
                del rewrites.blocks(f2)[bi].block_items[i]
                trials.append(f2)
        for _, f2 in rewrites.all_rewrites(f):
            trials.append(f2)
        texts = [ast_util.to_c(t) for t in trials]
        keep = [(t, f2) for t, f2 in zip(texts, trials) if len(t) < cur]
        if not keep:
            break
        scores = pool.map(evaluate, [prefix + t for t, _ in keep], chunksize=4)
        ok = sorted(((s_, len(t), f2) for s_, (t, f2) in zip(scores, keep) if s_ is not None and s_ <= score),
                    key=lambda x: (x[0], x[1]))
        if not ok:
            break
        score, _, f = ok[0]
    return score, f, hist


def search(src, func, beam=10, depth=6, seeds=0, jobs=None, seconds=600, pins=True, log=print):
    rel, ast, fn, prefix = prepare(src, func)
    t0 = time.time()
    jobs = jobs or os.cpu_count()
    pool = multiprocessing.get_context("fork").Pool(jobs, initializer=_init_worker, initargs=(rel, func))
    base_fn_text = ast_util.to_c(fn)
    base = pool.apply(evaluate, (prefix + base_fn_text,))
    log(f"{func}: base score {base} (shape, bytes)")
    if base is None:
        pool.terminate()
        return None, None, []
    seen = {base_fn_text}
    frontier = [(base, fn, [])]
    best, frontier, n1 = beam_search(pool, prefix, frontier, seen, depth, beam, seeds, ast, func, False,
                                     t0 + seconds * (0.5 if pins else 1.0), log, "C")
    n2 = 0
    if not (best[0][0] == 0 and best[0][2] == 0) and pins:
        # second phase: register pins and asm("") barriers, from the plain-C beam
        start = sorted({id(x[1]): x for x in frontier + [best]}.values(), key=lambda x: (x[0], len(x[2])))[:beam]
        best2, _, n2 = beam_search(pool, prefix, start, seen, max(2, depth // 2), beam, 0, ast, func, True,
                                   t0 + seconds, log, "pins")
        if (best2[0], len(best2[2])) < (best[0], len(best[2])):
            best = best2
    best = cleanup(pool, prefix, best)
    pool.terminate()
    score, f, hist = best
    text = prefix + ast_util.to_c(f)
    out_dir = os.path.join(ROOT, "build", "fixsearch", func)
    os.makedirs(out_dir, exist_ok=True)
    open(os.path.join(out_dir, "best.c"), "w").write(text)
    log(f"best {score} after {n1 + n2} compiles, {time.time() - t0:.0f} s; edits: {'; '.join(hist) or 'none'}")
    log("\n".join(difflib.unified_diff(base_fn_text.splitlines(), ast_util.to_c(f).splitlines(),
                                       "draft", "best", lineterm="", n=1)))
    return score, text, hist


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--beam", type=int, default=10)
    ap.add_argument("--depth", type=int, default=6)
    ap.add_argument("--seeds", type=int, default=0, help="also enumerate permuter passes with N seeds")
    ap.add_argument("--seconds", type=int, default=600)
    ap.add_argument("-j", type=int, default=None)
    ap.add_argument("--nonmatching", action="store_true", help="search the #ifdef NONMATCHING draft")
    ap.add_argument("--no-pins", action="store_true", help="plain C only (no register pins or asm barriers)")
    a = ap.parse_args()
    import explain
    import build
    if a.nonmatching:
        build.DEFINES.append("-DNONMATCHING")
        import split
        func = a.func or split.nonmatching_functions(a.src)[0][0]
    else:
        func = a.func or explain.last_function(a.src)
    score, _, _ = search(a.src, func, a.beam, a.depth, a.seeds, a.j, a.seconds, not a.no_pins)
    score = score or (1, 1, 1)
    sys.exit(0 if score[2] == 0 else 1)


if __name__ == "__main__":
    main()

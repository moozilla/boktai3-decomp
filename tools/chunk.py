#!/usr/bin/env python3
"""Chunk-then-solve: match one region's instruction selection on its own.

  chunk.py FILE.c [FUNC] --map
  chunk.py FILE.c [FUNC] --lines A-B [--search SECONDS] [-j N] [--out DIR]

FILE.c holds one ROM function (tools/isolate.py). `--map` prints, per
top-level statement, how many of its instructions differ from the ROM in
shape (registers ignored), so the regions worth solving stand out.

`--lines A-B` takes the statements on those lines (siblings in one block),
makes them a function of their own, `void FUNC_chunk(live-ins)`, and passes
live-out values to an external sink so they are computed. It scores the
chunk against the ROM instructions aligned to those lines, by shape only:
instruction selection and order, not registers. `--search` mutates the chunk
with the decomp-permuter fork's passes for that long (a small function
compiles fast and has a small search space) and writes the best chunk and
the whole function with the chunk spliced back (DIR/spliced.c). What is left
after that is register allocation: tools/regsolve.py and
tools/permute.py --focus.
"""
import argparse
import copy
import difflib
import io
import contextlib
import multiprocessing
import os
import random
import re
import subprocess
import sys
import tempfile
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402
import check  # noqa: E402
import regdiag  # noqa: E402
import permute  # noqa: E402
from romlib import ROOT  # noqa: E402

sys.path.insert(0, permute.PERMUTER)
from perm_pycparser import c_ast as ca  # noqa: E402
from src import ast_util  # noqa: E402
from src.randomizer import Randomizer, RandomizationFailure  # noqa: E402
from src.helpers import get_default_randomization_weights  # noqa: E402

PROLOGUE = re.compile(r"^(push|pop|bx lr|bx R|add sp|sub sp|mov R, sp|add R, sp, #0)\b")


def norm(text):
    """Shape of an instruction: no registers, offsets of pc loads, branch targets;
    a low-register copy (adds rX, rY, #0) and a high one (mov) are one shape."""
    t = regdiag.shape(text)
    t = re.sub(r"^add R, R, #0$", "mov R, R", t)
    t = re.sub(r"^ldr R, \[pc, #\S+\]$", "ldr R, [pc]", t)
    t = re.sub(r"^ldr R, \[R, #X\]$", "ldr R, [pc]", t)
    t = re.sub(r"^(b\w*|bl) #\S+$", r"\1", t)
    return t


def distance(a, b):
    """Differing instructions between shape lists A and B, not counting
    unmatched instructions at either end (prologue, epilogue, sink call)."""
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    ops = sm.get_opcodes()
    while ops and ops[0][0] != "equal":
        ops = ops[1:]
    while ops and ops[-1][0] != "equal":
        ops = ops[:-1]
    return sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in ops if tag != "equal"), \
        sum(i2 - i1 for tag, i1, i2, j1, j2 in ops if tag == "equal")


def preprocess_with_lines(src):
    """(text, {text line: src line}) as tools/permute.py prepares base.c."""
    return permute.prepare_source(os.path.relpath(src, ROOT), regdiag.first_function(src))


def parse_function(text, func):
    ast = ast_util.parse_c(text)
    fn, idx = ast_util.extract_fn(ast, func)
    return ast, fn, idx


def stmt_lines(node, line_map):
    """Source lines a statement covers."""
    lines = set()

    class V(ca.NodeVisitor):
        def generic_visit(self, n):
            c = getattr(n, "coord", None)
            if c is not None and getattr(c, "line", None) in line_map:
                lines.add(line_map[c.line])
            super().generic_visit(n)

    V().visit(node)
    return lines


def blocks(fn):
    out = []

    def rec(block):
        out.append(block)
        for s in ast_util.get_block_stmts(block, False):
            ast_util.brace_nested_blocks(s)
            ast_util.for_nested_blocks(s, rec)

    rec(fn.body)
    return out


def decls_in(node):
    out = {}

    class V(ca.NodeVisitor):
        def visit_Decl(self, n):
            if n.name and not isinstance(n.type, ca.FuncDecl):
                out[n.name] = n
            if n.init is not None:
                self.visit(n.init)

    V().visit(node)
    return out


def locals_of(fn):
    out = decls_in(fn.body)
    if fn.decl.type.args:
        for p in fn.decl.type.args.params:
            if isinstance(p, ca.Decl) and p.name:
                out[p.name] = p
    return out


def ids(node):
    found = []

    class V(ca.NodeVisitor):
        def visit_ID(self, n):
            found.append(n)

        def visit_StructRef(self, n):
            self.visit(n.name)

    V().visit(node)
    return found


def first_use_is_write(stmts, name):
    """Whether NAME's first appearance in STMTS is the target of `name = ...`."""
    for s in stmts:
        result = None

        class V(ca.NodeVisitor):
            def visit_Assignment(self, n):
                nonlocal result
                if result is not None:
                    return
                self.visit(n.rvalue)
                if result is None and isinstance(n.lvalue, ca.ID) and n.lvalue.name == name:
                    result = n.op == "="
                elif result is None:
                    self.visit(n.lvalue)

            def visit_ID(self, n):
                nonlocal result
                if result is None and n.name == name:
                    result = False

            def visit_StructRef(self, n):
                self.visit(n.name)

        V().visit(s)
        if result is not None:
            return result
    return False


class Chunk:
    def __init__(self, src, func, lines):
        self.src, self.func = src, func
        text, self.line_map = preprocess_with_lines(src)
        self.ast, self.fn, self.idx = parse_function(text, func)
        ast_util.normalize_ast(self.fn, self.ast)
        rev = {}
        for b, s in self.line_map.items():
            rev.setdefault(s, b)
        target = set(lines)
        for block in blocks(self.fn):
            stmts = ast_util.get_block_stmts(block, False)
            chosen = [i for i, s in enumerate(stmts) if stmt_lines(s, self.line_map) & target
                      and not isinstance(s, ca.Decl)]
            if chosen:  # the outermost block with statements on those lines
                self.block, self.i0, self.i1 = block, chosen[0], chosen[-1] + 1
                break
        if not hasattr(self, "block"):
            sys.exit("no statements on those lines")
        stmts = ast_util.get_block_stmts(self.block, False)[self.i0:self.i1]
        self.stmts = stmts
        self.lines = set().union(*(stmt_lines(s, self.line_map) for s in stmts))
        loc = locals_of(self.fn)
        inner = set()
        for st in stmts:  # declared inside the region: stays in the region
            inner |= set(decls_in(st))
        loc = {k: v for k, v in loc.items() if k not in inner}
        used = []
        for s in stmts:
            for i in ids(s):
                if i.name in loc and i.name not in used:
                    used.append(i.name)
        self.internal = [n for n in used if first_use_is_write(stmts, n)]
        self.params = [n for n in used if n not in self.internal]
        # live-out: written in the chunk and read after it
        indices = ast_util.compute_node_indices(self.fn)
        end = indices.ends[stmts[-1]]
        written = set()
        for s in stmts:
            for n in ast_util_assigned(s):
                written.add(n)
        later = {i.name for i in ids(self.fn.body) if indices.starts.get(i, 0) > end}
        self.outs = [n for n in used if n in written and n in later]
        self.decls = loc

    def source(self, stmts=None, extra_decls=()):
        stmts = stmts if stmts is not None else self.stmts
        ext = [e for i, e in enumerate(self.ast.ext) if i != self.idx]
        head = "".join(ast_util.to_c(ca.FileAST([e])) for e in ext)
        params = ", ".join(self._decl_text(n) for n in self.params) or "void"
        body = [self._decl_text(n) + ";" for n in self.internal]
        body += [ast_util.to_c(ca.FileAST([d])).strip() for d in extra_decls]
        gen = ast_util.PatchedCGenerator()
        body += [gen.visit(s) + (";" if not isinstance(s, (ca.Compound, ca.If, ca.For, ca.While,
                                                           ca.DoWhile, ca.Switch, ca.Label)) else "")
                 for s in stmts]
        if self.outs:
            body.append(f"chunk_sink({', '.join(self.outs)});")
        return (head + "\nvoid chunk_sink();\n" + f"void {self.func}_chunk({params})\n{{\n"
                + "\n".join(body) + "\n}\n")

    def _decl_text(self, name):
        d = copy.deepcopy(self.decls[name])
        d.init = None
        d.storage = [x for x in d.storage if x != "register"]
        d.asmlabel = None
        return ast_util.to_c(ca.FileAST([d])).strip().rstrip(";")


def ast_util_assigned(node):
    out = set()

    class V(ca.NodeVisitor):
        def visit_Assignment(self, n):
            if isinstance(n.lvalue, ca.ID):
                out.add(n.lvalue.name)
            self.generic_visit(n)

        def visit_UnaryOp(self, n):
            if n.op in ("p++", "p--", "++", "--") and isinstance(n.expr, ca.ID):
                out.add(n.expr.name)
            self.generic_visit(n)

    V().visit(node)
    return out


def compile_shapes(c_source, func, workdir):
    """Instruction shapes of FUNC compiled from C_SOURCE, or None."""
    cc = os.path.join(ROOT, build.AGBCC)
    r = subprocess.run([cc, *build.CFLAGS, "-o", "-"], input=c_source, text=True, capture_output=True)
    if r.returncode or "error" in r.stderr:
        return None
    s_path = os.path.join(workdir, "c.s")
    o_path = os.path.join(workdir, "c.o")
    open(s_path, "w").write(r.stdout)
    if subprocess.run(build.AS + ["-o", o_path, s_path], capture_output=True).returncode:
        return None
    try:
        with contextlib.redirect_stdout(io.StringIO()):
            start, code = regdiag.object_function(o_path, func)
    except SystemExit:
        return None
    lines = regdiag.asm_lines(s_path, func)
    offs = {o for o, _, _ in lines}
    return [norm(t) for o, t in check.disasm(code, 0) if o in offs]


def original_slice(an, lines):
    """ROM instruction shapes aligned to candidate instructions on LINES."""
    in_region = [i for i, (o, t) in enumerate(an.cd) if an.off_line.get(o) in lines]
    if not in_region:
        return [], []
    lo, hi = min(in_region), max(in_region)
    js = []
    for tag, i1, i2, j1, j2 in an.opcodes:
        if i2 <= lo or i1 > hi:
            if not (i1 == i2 and lo <= i1 <= hi):
                continue
        if tag == "equal":
            js += [j1 + (i - i1) for i in range(max(i1, lo), min(i2, hi + 1))]
        else:
            js += list(range(j1, j2))
    if not js:
        return [], []
    orig = [norm(t) for o, t in an.od[min(js):max(js) + 1]]
    cand = [norm(t) for o, t in an.cd[lo:hi + 1]]
    return orig, cand


def per_statement_map(src, func):
    with contextlib.redirect_stdout(io.StringIO()):
        an = regdiag.analyze(src, func)
    text, line_map = preprocess_with_lines(src)
    ast, fn, idx = parse_function(text, func)
    ast_util.normalize_ast(fn, ast)
    src_text = open(src).read().splitlines()
    print(f"{func}: {an.n_insns} instructions; per top-level statement, "
          "differing shapes / instructions (registers ignored)")
    for s in ast_util.get_block_stmts(fn.body, False):
        if isinstance(s, ca.Decl):
            continue
        ls = stmt_lines(s, line_map)
        if not ls:
            continue
        orig, cand = original_slice(an, ls)
        d, same = distance(cand, orig) if cand else (0, 0)
        first = min(ls)
        print(f"  {first:4d}-{max(ls):<4d} {d:3d} / {len(cand):3d}  {src_text[first - 1].strip()[:60]}")


def worker(args):
    """Hill-climb with the permuter fork's passes; distance in shapes."""
    start_src, fname, target, seed, seconds, weights, focus = args
    random.seed(seed)
    from src.randomizer import Focus
    with tempfile.TemporaryDirectory() as wd:
        best_src, best = start_src, None
        shapes = compile_shapes(start_src, fname, wd)
        if shapes is not None:
            best = distance(shapes, target)[0]
        cur_ast = ast_util.parse_c(start_src)
        deadline = time.time() + seconds
        history = []
        n = 0
        while time.time() < deadline and best != 0:
            ast = copy.deepcopy(cur_ast if random.random() < 0.7 else ast_util.parse_c(best_src))
            fn, _ = ast_util.extract_fn(ast, fname)
            ast_util.normalize_ast(fn, ast)
            try:
                Randomizer(weights, random.randrange(1 << 30),
                           Focus(focus) if focus else None).randomize(ast, fname)
            except Exception:
                continue
            src = ast_util.to_c(ast)
            shapes = compile_shapes(src, fname, wd)
            n += 1
            if shapes is None:
                continue
            d = distance(shapes, target)[0]
            if best is None or d < best:
                best, best_src = d, src
                history.append((round(seconds - (deadline - time.time()), 1), d))
                cur_ast = ast
            elif d == best:
                cur_ast = ast
        return best, best_src, history, n


def search(start_src, fname, target, seconds, jobs, focus=None):
    weights = get_default_randomization_weights("gcc")
    args = [(start_src, fname, target, s, seconds, weights, focus) for s in range(jobs)]
    t0 = time.time()
    with multiprocessing.get_context("fork").Pool(jobs) as pool:
        results = pool.map(worker, args)
    best, best_src, history, _ = min(results, key=lambda r: (r[0] is None, r[0] if r[0] is not None else 0))
    tried = sum(r[3] for r in results)
    print(f"search: {tried} candidates in {time.time() - t0:.0f}s; best distance {best}")
    print("  improvements (seconds, distance): " + " ".join(f"{t}:{d}" for t, d in history))
    return best, best_src


def splice(ch, chunk_src):
    """The whole function's source with the chunk's statements put back."""
    cast = ast_util.parse_c(chunk_src)
    cfn, _ = ast_util.extract_fn(cast, ch.func + "_chunk")
    items = list(cfn.body.block_items or [])
    decls = [x for x in items if isinstance(x, ca.Decl)]
    stmts = [x for x in items if not isinstance(x, ca.Decl)]
    if ch.outs and stmts and isinstance(stmts[-1], ca.FuncCall) \
            and getattr(stmts[-1].name, "name", "") == "chunk_sink":
        stmts = stmts[:-1]
    new_decls = [d for d in decls if d.name not in ch.decls]
    ast = copy.deepcopy(ch.ast)
    fn, idx = ast_util.extract_fn(ast, ch.func)
    # helpers a pass added (perm_inline) go before the function
    old_names = {getattr(e, "name", None) or getattr(getattr(e, "decl", None), "name", None) for e in ast.ext}
    added = [e for e in cast.ext if isinstance(e, ca.FuncDef) and e.decl.name not in old_names
             and e.decl.name != ch.func + "_chunk"]
    ast.ext[idx:idx] = added
    ast_util.normalize_ast(fn, ast)
    # find the same block again by position
    blocks_new = blocks(fn)
    block = blocks_new[blocks(ch.fn).index(ch.block)]
    ast_util.get_block_stmts(block, True)[ch.i0:ch.i1] = stmts
    body = fn.body.block_items
    n = 0
    while n < len(body) and isinstance(body[n], ca.Decl):
        n += 1
    body[n:n] = new_decls
    return ast_util.to_c(ast)


def full_distance(c_source, func, target):
    with tempfile.TemporaryDirectory() as wd:
        shapes = compile_shapes(c_source, func, wd)
    return None if shapes is None else distance(shapes, target)[0]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--map", action="store_true")
    ap.add_argument("--lines")
    ap.add_argument("--search", type=int, default=0)
    ap.add_argument("--inplace", action="store_true",
                    help="mutate only the region, inside the whole function (no extraction)")
    ap.add_argument("-j", type=int, default=os.cpu_count() or 4)
    ap.add_argument("--out")
    a = ap.parse_args()
    src = os.path.abspath(a.src)
    func = a.func or regdiag.first_function(src)
    if a.map:
        per_statement_map(src, func)
        return
    if not a.lines:
        ap.error("--map or --lines")
    lines = permute.parse_lines(a.lines)
    ch = Chunk(src, func, lines)
    with contextlib.redirect_stdout(io.StringIO()):
        an = regdiag.analyze(src, func)
    full_target = [norm(t) for o, t in an.od]
    target, cand = original_slice(an, ch.lines)
    out = a.out or os.path.join(ROOT, "build", "chunk", func, f"{min(ch.lines)}-{max(ch.lines)}")
    os.makedirs(out, exist_ok=True)
    full_src = ast_util.to_c(ch.ast)
    d_full = full_distance(full_src, func, full_target)
    print(f"chunk lines {min(ch.lines)}-{max(ch.lines)}: live-in {ch.params}, internal {ch.internal}, "
          f"live-out {ch.outs}")
    print(f"whole function differs from the ROM in {d_full} instruction shapes; "
          f"the region's {len(target)} ROM instructions differ in {distance(cand, target)[0]} in place")
    if a.inplace:
        focus = sorted(b for b, l in ch.line_map.items() if l in ch.lines)
        if not a.search:
            return
        best, best_src = search(full_src, func, full_target, a.search, a.j, focus)
        open(os.path.join(out, "inplace_best.c"), "w").write(best_src)
        print(f"in place: whole-function distance {d_full} -> {best}; {out}/inplace_best.c")
        return
    chunk_src = ch.source()
    open(os.path.join(out, "chunk.c"), "w").write(chunk_src)
    with tempfile.TemporaryDirectory() as wd:
        shapes = compile_shapes(chunk_src, func + "_chunk", wd)
    if shapes is None:
        sys.exit(f"the chunk does not compile: {out}/chunk.c")
    d0 = distance(shapes, target)[0]
    print(f"the extracted chunk differs in {d0} (context lost by extraction: {d0 - distance(cand, target)[0]:+d})")
    if not a.search:
        return
    best, best_src = search(chunk_src, func + "_chunk", target, a.search, a.j)
    open(os.path.join(out, "best_chunk.c"), "w").write(best_src)
    spliced = splice(ch, best_src)
    open(os.path.join(out, "spliced.c"), "w").write(spliced)
    print(f"chunk distance {d0} -> {best}; spliced back, whole function {d_full} -> "
          f"{full_distance(spliced, func, full_target)}  ({out}/spliced.c)")


if __name__ == "__main__":
    main()

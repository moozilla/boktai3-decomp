#!/usr/bin/env python3
"""Fix one divergence at a time: name the decision, then try the C spellings that flip it.

  fix.py FILE.c [FUNC] [--nonmatching] [--depth N] [--show K] [--all]

Each round:
  1. tools/step.py names the first divergence and its decision (an evaluation
     order, a cse block boundary, a hoisted invariant, a narrowing, a signed
     compare, the frame...).
  2. The spellings known to flip that decision (tools/spellings.py) and the
     generic rewrites (tools/rewrites.py, the permuter's passes) are applied to
     the function and to the inline helpers it calls; every result is compiled
     through the agbcc fork server (about 10 ms each) and compared with the ROM.
  3. It prints the edits that fix the divergence or shrink the difference, the
     decision's own spellings first, as C diffs.
With --depth N (default 3) the best edit is kept and the next round starts
from it, so a draft with several independent divergences is fixed in one call.
A match is written to build/fix/FUNC.c as the whole source file, ready to copy
over the draft. Exit 0 on a match.

--all tries every rewrite site in every round; the default skips generic
rewrites when the decision's own spellings already found an improvement.
"""
import argparse
import os
import re
import sys
import tempfile
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import permute  # noqa: E402
from romlib import ROOT  # noqa: E402

sys.path.insert(0, permute.PERMUTER)
import causebook as atlas  # noqa: E402
import fixsearch  # noqa: E402
import rewrites  # noqa: E402
import spellings  # noqa: E402
from perm_pycparser import c_ast as ca  # noqa: E402
from src import ast_util  # noqa: E402

OUT = os.path.join(ROOT, "build", "fix")
TAKE = "static inline u8 TakeFlag_(u8 *flag)\n{\n    if (*flag) {\n        *flag = 0;\n        return 1;\n    }\n" \
       "    return 0;\n}\n"


def strip_draft(text):
    """The draft's C with an #ifdef NONMATCHING / #else INCLUDE_ASM / #endif wrapper removed."""
    text = re.sub(r"^#ifdef NONMATCHING\n", "", text, flags=re.M)
    return re.sub(r"^#else\n\s*INCLUDE_ASM\([^)]*\);\n#endif\n?", "", text, flags=re.M)


def find_def(text, name):
    """(start, end) of NAME's definition in TEXT (static helpers included), or None."""
    m = re.search(r"^(?:[A-Za-z_][\w \t\*]*?)?\b" + re.escape(name) + r"\s*\([^;{)]*\)\s*\{", text, re.M)
    if not m:
        return None
    start = m.start()
    if text[start:].startswith(name):  # the return type on the line before
        prev = text.rfind("\n", 0, start - 1) + 1
        if re.fullmatch(r"(static\s+|inline\s+)*[A-Za-z_][\w \t\*]*", text[prev:start - 1]):
            start = prev
    depth, i = 0, m.end() - 1
    while i < len(text):
        depth += {"{": 1, "}": -1}.get(text[i], 0)
        i += 1
        if depth == 0:
            return start, i
    return None


class Draft:
    """One function and the inline helpers it calls, as parse trees, with the ROM to compare against."""

    def __init__(self, src_text, func, workdir):
        self.func = func
        alias = re.search(r"^#define (\w+)\s+" + re.escape(func) + r"\s*$", src_text, re.M)
        # the name the source text defines the function under
        self.src_name = alias.group(1) if alias and find_def(src_text, alias.group(1)) else func
        self.text = src_text
        self.path = os.path.join(workdir, f"{func}.c")
        open(self.path, "w").write(src_text)
        rel, ast, fn, prefix = fixsearch.prepare(self.path, func)
        self.rel, self.ast, self.fn = rel, ast, fn
        self.rom = atlas.init_compiler(rel, func)
        ext = ast.ext[:-1]
        called = {n.name.name for n, _, _ in rewrites.walk(fn) if isinstance(n, ca.FuncCall)
                  and isinstance(n.name, ca.ID)}
        defs = {e.decl.name: i for i, e in enumerate(ext) if isinstance(e, ca.FuncDef)}
        # helpers called from fn, and the helpers they call
        todo, seen = [c for c in called if c in defs], set()
        while todo:
            h = todo.pop()
            if h in seen:
                continue
            seen.add(h)
            for n, _, _ in rewrites.walk(ext[defs[h]]):
                if isinstance(n, ca.FuncCall) and isinstance(n.name, ca.ID) and n.name.name in defs:
                    todo.append(n.name.name)
        self.helpers = {h: ext[defs[h]] for h in seen}
        cut = sorted(defs[h] for h in seen)
        self.segments, last = [], 0
        for i in cut:
            self.segments.append((ast_util.to_c(ca.FileAST(ext[last:i])), ext[i].decl.name))
            last = i + 1
        self.tail = ast_util.to_c(ca.FileAST(ext[last:]))
        self.take = "SceneActorControl_TakeFlag" if "SceneActorControl_TakeFlag" in defs else "TakeFlag_"
        self.need_take = self.take == "TakeFlag_" and "TakeFlag_" not in defs

    def source(self, fn=None, helpers=None):
        helpers = helpers or {}
        out = []
        for seg, h in self.segments:
            out.append(seg)
            out.append(ast_util.to_c(helpers.get(h, self.helpers[h])))
        out.append(self.tail)
        if self.need_take:
            out.append(TAKE)
        out += [ast_util.to_c(h) for k, h in helpers.items() if k not in self.helpers]
        out.append(ast_util.to_c(fn or self.fn))
        return "\n".join(out)

    def score(self, fn=None, helpers=None):
        mine = atlas.texts_of(self.source(fn, helpers))
        if mine is None:
            return None
        regs = atlas.regions(self.rom, mine)
        total = sum(max(i2 - i1, j2 - j1) for _, i1, i2, j1, j2 in regs)
        if total == 0 and mine != self.rom:
            # regions() lets branch offsets follow from the instructions; a label that moves
            # (a statement into or out of a loop) shows only there
            diff = [i for i, (a, b) in enumerate(zip(self.rom, mine)) if a != b]
            return max(1, len(diff)), (diff[0] if diff else len(self.rom)), max(1, len(diff))
        # third: differences that are not register names alone. The right shape with a register
        # cascade after it (Ghost_TurnTowardHeading's `& 7`: 37 insns, 2 of them shape) is often
        # closer than a wrong shape that happens to keep the allocation (4 insns, 4 shape)
        shape = sum(max(i2 - i1, j2 - j1) for kind, i1, i2, j1, j2 in regs if kind != "regs")
        return total, (regs[0][1] if regs else len(self.rom)), shape

    def materialize(self, fn, helpers):
        """The source file with FN and the edited HELPERS written back."""
        text = self.text
        for name, h in helpers.items():
            span = find_def(text, name) if name in self.helpers else None
            if name not in self.helpers:  # a new helper: before the function
                span = find_def(text, self.src_name)
                text = text[:span[0]] + ast_util.to_c(h).strip() + "\n\n" + text[span[0]:]
            elif span:
                text = text[:span[0]] + ast_util.to_c(h).strip() + "\n" + text[span[1]:]
            else:  # a header's helper: a local copy under another name
                new = name + "_"
                h2 = __import__("copy").deepcopy(h)
                h2.decl.name = new
                h2.decl.type.type.declname = new
                fn = __import__("copy").deepcopy(fn)
                for n, _, _ in rewrites.walk(fn):
                    if isinstance(n, ca.FuncCall) and isinstance(n.name, ca.ID) and n.name.name == name:
                        n.name.name = new
                span = find_def(text, self.src_name)
                text = text[:span[0]] + ast_util.to_c(h2).strip() + "\n\n" + text[span[0]:]
        if self.take == "TakeFlag_" and "TakeFlag_(" in ast_util.to_c(fn) and "TakeFlag_(u8" not in text:
            span = find_def(text, self.src_name)
            text = text[:span[0]] + TAKE + "\n" + text[span[0]:]
        span = find_def(text, self.src_name)
        body = ast_util.to_c(fn).strip()
        if self.src_name != self.func:
            body = re.sub(r"\b" + self.func + r"\b", self.src_name, body, count=1)
        return text[:span[0]] + body + "\n" + text[span[1]:]


def decision(path, func):
    """(entry, [decision keys]) of the first divergence, from step.py."""
    import step
    import hint
    st = step.analyze(path, func)
    if st.matched or not st.entries:
        return None, []
    e = st.entries[0]
    keys = []
    note = e["note"] or ""
    for k in ("frame", "call result", "comparison type", "order", "dead store"):
        if note.startswith(k) or e["kind"] == k:
            keys.append(k)
    keys += [t for t in hint.topics_for(st, e) if t not in keys]
    return (st, e), keys


# spelling families not tried unless asked for (--allow): Ben prefers C without
# `do { } while (0)` loop-weight wrappers
AVOID = {"sp_loop_weight"}


def candidates(d, seeds):
    """(family, description, fn, helpers) one edit away, over the function and its helpers."""
    targets = [(None, d.fn)] + list(d.helpers.items())
    for name, node in targets:
        gens = [(sp.__name__, sp) for sp in spellings.SPELLINGS] + [(rw.__name__, rw) for rw in rewrites.REWRITES]
        for fam, g in gens:
            try:
                it = g(node, d.take) if fam == "sp_flag_take" else g(node)
                for desc, n2 in it:
                    extra = []
                    if isinstance(n2, tuple):
                        n2, extra = n2
                    where = f" in {name}" if name else ""
                    if name:
                        yield fam, desc + where, d.fn, dict({name: n2}, **{e.decl.name: e for e in extra})
                    else:
                        yield fam, desc, n2, {e.decl.name: e for e in extra}
            except Exception as ex:
                if os.environ.get("FIX_DEBUG"):
                    print(f"   ({fam}: {type(ex).__name__}: {ex})")
                continue
    for sp in spellings.HELPER_SPELLINGS:
        try:
            for desc, (f2, extra) in sp(d.fn, d.helpers):
                yield sp.__name__, desc, f2, {e.decl.name: e for e in extra}
        except Exception as ex:
            if os.environ.get("FIX_DEBUG"):
                print(f"   ({sp.__name__}: {type(ex).__name__}: {ex})")
    for t, (fam, desc, f2) in atlas.edits(d.fn, d.ast, d.func, seeds).items():
        if fam not in {r.__name__ for r in rewrites.REWRITES}:
            yield fam, desc, f2, {}


def one_round(d, keys, show, try_all, seeds, log, deadline=None):
    base = d.score()
    prefer = []
    for k in keys:
        prefer += [x for x in spellings.DECISIONS.get(k, []) if x not in prefer]
    results, seen, compiles = [], set(), 0
    cands = list(candidates(d, seeds))
    cands.sort(key=lambda c: (c[0] not in prefer, prefer.index(c[0]) if c[0] in prefer else 0))
    cands = [c for c in cands if c[0] not in AVOID]
    for fam, desc, fn, helpers in cands:
        if deadline and time.time() > deadline:
            break
        if not try_all and fam not in prefer and any(r[0][0] < base[0] for r in results if r[2] in prefer):
            break
        src = ast_util.to_c(fn) + "".join(ast_util.to_c(h) for h in helpers.values())
        if src in seen:
            continue
        seen.add(src)
        s = d.score(fn, helpers)
        compiles += 1
        if s is None:
            continue
        results.append((s, desc, fam, fn, helpers))
        if s[0] == 0:
            break
    # a spelling that leaves the score as it is can enable the next one (an if/else to a ternary,
    # then the temporary inlined): kept, ranked last
    good = sorted([r for r in results if r[0][0] < base[0] or r[0][1] > base[1] or r[0][2] < base[2]
                   or r[0] == base and r[2].startswith("sp_")],
                  key=lambda r: (r[0][0], r[2] not in prefer, -r[0][1], len(r[1])))
    return base, good, compiles


def cse_probe(path):
    """The CSE decision that alone explains a near match, as a line to print, or None: a pseudo made
    canonical (tools/rtlprobe.py canon) or a jump cse follows into its target (rtlprobe.py follow)."""
    import subprocess
    if not os.path.exists(os.path.join(ROOT, "build", "agbcc-instr", "gcc", "agbcc")):
        return None
    probe = os.path.join(ROOT, "tools", "rtlprobe.py")
    r = subprocess.run([sys.executable, probe, "canon", path, "-j", "2"], capture_output=True, text=True, cwd=ROOT)
    m = re.search(r"CSE_NOCANON=(\d+):.*MATCH", r.stdout)
    if m:
        reg = m.group(1)
        return (f"CSE makes pseudo {reg} a canonical register (rtlprobe.py canon: CSE_NOCANON={reg} matches); "
                f"make the ROM-side value live longer, or keep the copy into {reg} inside one block")
    r = subprocess.run([sys.executable, probe, "alias", path, "-j", "2"], capture_output=True, text=True, cwd=ROOT)
    if "no alias sets" in r.stdout and "MATCH" in r.stdout.split("no alias sets")[1].split("\n")[0]:
        return ("a type-based alias decision (rtlprobe.py alias: AGBCC_NOSTRICT=1 matches): two accesses of "
                "different types that the original made the same type (sp_alias_read, or the field's type)")
    r = subprocess.run([sys.executable, probe, "follow", path, "-j", "2"], capture_output=True, text=True, cwd=ROOT)
    m = re.search(r"CSE_NOFOLLOW=(\d+):.*MATCH\n\s*jump at line (\d+): (.*)", r.stdout)
    if m:
        return (f"CSE carries values across the jump at line {m.group(2)} `{m.group(3).strip()}` "
                f"(rtlprobe.py follow: CSE_NOFOLLOW={m.group(1)} matches); keep the reused value from being "
                f"known there (a constant in another mode: sp_narrow_mask)")
    return None


HIGH = ("r8", "r9", "r10", "r11", "ip", "sb", "sl", "fp")


def kept_copy(rom, mine):
    """A cause line when the ROM sets a high register and copies it straight back (`mov ip, r0;
    mov r0, ip`) more often than the candidate, else None. reload, reload_cse and jump's no-op move
    deletion drop that copy back unless the set and the use differ in mode (an HImode set, an SImode
    use): agbccsrc.py reload-mode, spellings.sp_helper_ret_word."""
    def pairs(t):
        out = 0
        for a, b in zip(t, t[1:]):
            m = re.match(r"mov (\w+), (r[0-7])$", a)
            if m and m.group(1) in HIGH and b == f"mov {m.group(2)}, {m.group(1)}":
                out += 1
        return out
    n, k = pairs(rom), pairs(mine or [])
    if n > k:
        return (f"the ROM copies a high register straight back {n - k} more time(s) (`mov H, rN; mov rN, H`): "
                "the value was set in another mode than it is used in (an HImode constant used as SImode), "
                "so reload could not reuse rN")
    return None


def expand(text, func, wd, show, try_all, seeds, log, verbose, deadline=None, extra=()):
    """Children of one state: (score, new text, desc, fam), plus the compiles spent. EXTRA: decision
    keys known from a compiler probe (cse_probe), tried before step.py's."""
    path = os.path.join(wd, f"{func}.c")
    open(path, "w").write(text)
    try:
        dec, keys = decision(path, func)
    except (Exception, SystemExit) as ex:  # a state step.py cannot compile: drop it
        log(f"   (step.py failed on a state: {type(ex).__name__})")
        return [], 0
    keys = list(extra) + [k for k in keys if k not in extra]
    d = Draft(text, func, wd)
    if dec is None:
        sc = d.score()
        if not sc or sc[0] == 0:
            return "match", 0
        # only a branch offset differs: a label moved (a statement into or out of a loop, a
        # hoisted invariant), which step.py does not see as a differing instruction
        keys = list(extra) + ["loop-invariant", "order"]
        dec = (None, {"lines": [], "note": "only a branch target differs (a label moved)"})
    st, e = dec
    base, good, n = one_round(d, keys, show, try_all, seeds, log, deadline)
    if base is None:
        return [], n
    if verbose:
        log(f"   decision {', '.join(keys[:4]) or '?'} (line {', '.join(map(str, e['lines'][:2])) or '?'}): "
            f"{base[0]} differing insns as written; tried {n} edits")
        if e["note"]:
            log(f"      {e['note'][:160]}")
        for sc, desc, fam, fn, helpers in good[:show]:
            tag = "MATCH" if sc[0] == 0 else f"{sc[0]} insns"
            log(f"      {tag:9} [{fam}] {desc}")
    keep = good[:show]
    keep += [r for r in sorted(good, key=lambda r: (-r[0][1], r[0][0]))[:show] if r not in keep]
    # and the edits that fix shape at the price of registers (fewest differences beyond names)
    shaped = [r for r in sorted(good, key=lambda r: (r[0][2], r[0][0]))[:2] if r[0][2] < base[2] and r not in keep]
    if verbose:
        for sc, desc, fam, fn, helpers in shaped:
            log(f"      {sc[0]} insns, {sc[2]} beyond registers [{fam}] {desc}")
    keep += shaped
    return [(sc, d.materialize(fn, helpers), desc, fam) for sc, desc, fam, fn, helpers in keep], n


def run(src, func, nonmatching=False, depth=3, show=4, try_all=False, seeds=2, log=print, beam=4, budget=None):
    """Beam search over one-edit steps: each round expands the BEAM best states (fewest differing
    instructions, then the latest first divergence)."""
    text = open(src).read()
    if nonmatching:
        text = strip_draft(text)
    t0 = time.time()
    total = 0
    states = [((10 ** 9, 0, 10 ** 9), text, [])]
    start = None
    seen = {text}
    best = best_shape = states[0]
    extra = []
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as wd:
        # a near match: ask the instrumented compiler first whether one CSE decision explains it,
        # and try that decision's spellings first in every round
        open(os.path.join(wd, f"{func}.c"), "w").write(text)
        try:
            start = Draft(text, func, wd).score()
        except (Exception, SystemExit):
            start = None
        if start and 0 < start[0] <= 4:
            d0 = Draft(text, func, wd)
            cause = kept_copy(d0.rom, atlas.texts_of(d0.source()))
            if cause:
                extra = ["reload-mode"]
            else:
                cause = cse_probe(os.path.join(wd, f"{func}.c"))
            if cause:
                log(f"   cause: {cause}")
                if "canonical register" in cause:
                    extra = ["cse-canon"]
        for rnd in range(depth):
            log(f"\n== round {rnd + 1}")
            children = []
            for i, (sc, txt, path) in enumerate(states):
                if budget and time.time() - t0 > budget:
                    break
                kids, n = expand(txt, func, wd, show, try_all, seeds, log, verbose=(i == 0),
                                 deadline=budget and t0 + budget, extra=extra)
                total += n
                if kids == "match":
                    kids = [((0, 0, 0), txt, "as written", "")]
                for ksc, ktxt, desc, fam in kids:
                    if ktxt in seen:
                        continue
                    seen.add(ktxt)
                    children.append((ksc, ktxt, path + [f"[{fam}] {desc}"]))
            if not children:
                log("   no edit helps")
                break
            # half the beam by fewest differences, half by the latest first divergence (fixing
            # one divergence can expose more differences after it)
            children.sort(key=lambda c: (c[0][0], -c[0][1], len(c[2])))
            states = children[:(beam + 1) // 2]
            fewest_shape = min(children, key=lambda c: (c[0][2], c[0][0], len(c[2])))
            if fewest_shape not in states and fewest_shape[0][2] < states[0][0][2]:
                states.append(fewest_shape)
            for c in sorted(children, key=lambda c: (-c[0][1], c[0][0], len(c[2]))):
                if len(states) >= beam:
                    break
                if c not in states:
                    states.append(c)
            if (states[0][0][0], -states[0][0][1]) < (best[0][0], -best[0][1]):
                best = states[0]
            shaped = min(states, key=lambda c: (c[0][2], c[0][0]))
            if (shaped[0][2], shaped[0][0]) < (best_shape[0][2], best_shape[0][0]):
                best_shape = shaped
            if states[0][0][0] == 0:
                sc, txt, path = states[0]
                for ln in atlas.cdiff(text, txt, limit=24):
                    log("      " + ln)
                os.makedirs(OUT, exist_ok=True)
                out = os.path.join(OUT, func + ".c")
                open(out, "w").write(txt)
                log("\n".join("   " + p for p in path))
                log(f"\nMATCH after {len(path)} edit(s), {total} compiles, {time.time() - t0:.0f} s: "
                    f"{os.path.relpath(out, ROOT)}")
                return 0, path, total
            log(f"   beam: {', '.join(f'{c[0][0]}/{c[0][2]}' for c in states)} differing insns/not registers alone")
        if best_shape is not best and 0 < best_shape[0][2] <= 4:
            # right shape but for a few insns, the rest register names: ask the compiler why
            open(os.path.join(wd, f"{func}.c"), "w").write(best_shape[1])
            cause = cse_probe(os.path.join(wd, f"{func}.c"))
            log(f"\n   closest shape: {best_shape[0][2]} insns beyond registers ({best_shape[0][0]} in all) after "
                f"{'; '.join(best_shape[2]) or 'no edit'}" + (f"\n   cause: {cause}" if cause else ""))
    sc, txt, path = best
    log("\n".join("   " + p for p in path))
    log(f"\nno match: best {sc[0]} differing insns (from {start[0] if start else '?'}) after {len(path)} edit(s), "
        f"{total} compiles, "
        f"{time.time() - t0:.0f} s")
    os.makedirs(OUT, exist_ok=True)
    open(os.path.join(OUT, func + ".partial.c"), "w").write(txt)
    return 1, path, total


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--nonmatching", action="store_true")
    ap.add_argument("--depth", type=int, default=3)
    ap.add_argument("--show", type=int, default=4)
    ap.add_argument("--seeds", type=int, default=2)
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--beam", type=int, default=4)
    ap.add_argument("--budget", type=float, help="seconds; no new state is expanded after it")
    ap.add_argument("--allow", default="", help="comma-separated families to try although avoided by default "
                                                 f"({', '.join(sorted(AVOID))})")
    a = ap.parse_args()
    AVOID.difference_update(a.allow.split(","))
    src = os.path.abspath(a.src)
    asm = re.search(r'INCLUDE_ASM\("asm/nonmatching", (\w+)\)', open(a.src).read())
    func = a.func or (asm.group(1) if a.nonmatching and asm else None) or re.findall(r"^\w[\w \*]*?\b(\w+)\s*\([^;]*?\)\s*\{", open(src).read(), re.M)[-1]
    alias = re.search(r"^#define " + re.escape(func) + r"\s+(\w+)\s*$", open(src).read(), re.M)
    if alias:  # a draft that names the function through a #define compiles to the ROM name
        func = alias.group(1)
    os.chdir(ROOT)
    rc, _, _ = run(src, func, a.nonmatching, a.depth, a.show, a.all, a.seeds, beam=a.beam, budget=a.budget)
    return rc


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""Work backwards from the registers the ROM uses to C changes.

  regsolve.py FILE.c [FUNC] [--nonmatching] [--depth N]

tools/regdiag.py says which pseudos got the wrong register. This tool
replays agbcc's global allocator (global.c: allocno_compare, find_reg) from
the `.greg`/`.lreg` dumps, checks that the replay reproduces the registers
the compiler chose, and then searches for the allocation order that gives
the original's registers. The order is a priority ranking,

    priority = floor_log2(refs) * refs / live_length     (ties: lower pseudo)

so each change of order becomes "pseudo A must outrank pseudo B", which the
report turns into the uses or lifetime one of them needs and the C edits that
change those (move a definition next to its use, split or merge a variable,
reuse or recompute a value). Local pseudos (one basic block) and reload
registers are diagnosed from the same formula and the spill rotation, without
a replay.

The output is advice for editing C by hand or for steering
`tools/permute.py --focus`; it never writes registers into the source.
"""
import argparse
import collections
import itertools
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import regdiag  # noqa: E402
from romlib import ROOT  # noqa: E402

FIRST_PSEUDO = 17
# config/thumb.h in agbcc
CLASSES = {
    "NO_REGS": 0x00000,
    "NONARG_LO_REGS": 0x000F0,
    "LO_REGS": 0x000FF,
    "STACK_REG": 0x02000,
    "BASE_REGS": 0x020FF,
    "HI_REGS": 0x0FF00,
    "ALL_REGS": 0x1FFFF,
    "GENERAL_REGS": 0x1FFFF,
}
FIXED = {11, 13, 14, 15, 16}
CALL_USED = {0, 1, 2, 3, 11, 12, 13, 14, 15, 16}
REGNAME = {i: f"r{i}" for i in range(16)}
REGNAME.update({9: "sb", 10: "sl", 11: "fp", 12: "ip", 13: "sp", 14: "lr", 15: "pc"})


def regnum(name):
    name = regdiag.ALIAS.get(name, name)
    return {"sp": 13, "lr": 14, "pc": 15}.get(name, None) if not name.startswith("r") else int(name[1:])


def floor_log2(n):
    return n.bit_length() - 1 if n > 0 else -1


def priority(refs, live, size=1):
    if live == 0:
        live = -1
    return int(floor_log2(refs) * refs / live * 10000 * size)


class Pseudo:
    def __init__(self, n):
        self.n = n
        self.refs = 0
        self.live = 0
        self.size = 1
        self.calls = 0
        self.block = None      # set for local (single-block) pseudos
        self.pref = "ALL_REGS"
        self.alt = "ALL_REGS"
        self.conflicts = set()  # pseudos
        self.hard_conflicts = set()
        self.prefs = set()
        self.got = None
        self.to_allocate = False

    @property
    def priority(self):
        return priority(self.refs, self.live, self.size)


def parse(an):
    ps = {}

    def get(n):
        if n not in ps:
            ps[n] = Pseudo(n)
        return ps[n]

    for m in re.finditer(r"^Register (\d+) used (\d+) times across (\d+) insns(?: in block (\d+))?(.*)$",
                         an.lreg, re.M):
        p = get(int(m.group(1)))
        p.refs, p.live = int(m.group(2)), int(m.group(3))
        p.block = int(m.group(4)) if m.group(4) else None
        rest = m.group(5)
        c = re.search(r"crosses (\d+) calls?", rest)
        p.calls = int(c.group(1)) if c else 0
        c = re.search(r"; pref (\w+)(?:, else (\w+))?", rest)
        if c:
            p.pref = c.group(1)
            p.alt = c.group(2) or "ALL_REGS"
        c = re.search(r"; (\w+) or none", rest)
        if c:
            p.pref, p.alt = c.group(1), "NO_REGS"
    order = []
    sec = an.greg.split("Registers to be allocated in sorted order:", 1)
    if len(sec) == 2:
        for m in re.finditer(r"^Register (\d+), refs = (\d+), live_length = (-?\d+), size = (\d+)$",
                             sec[1].split("\n\n", 1)[0], re.M):
            p = get(int(m.group(1)))
            p.refs, p.live, p.size = int(m.group(2)), int(m.group(3)), int(m.group(4))
            order.append(p.n)
    m = re.search(r";; \d+ regs to allocate:([^\n]*)", an.greg)
    if m:
        for tok in m.group(1).split():
            if tok[0].isdigit():  # "(2)" gives the preceding allocno's size
                get(int(re.match(r"\d+", tok).group(0))).to_allocate = True
    for m in re.finditer(r"^;; (\d+) conflicts:([^\n]*)", an.greg, re.M):
        p = get(int(m.group(1)))
        for x in map(int, m.group(2).split()):
            if x >= FIRST_PSEUDO:
                if x != p.n:
                    p.conflicts.add(x)
            else:
                p.hard_conflicts.add(x)
    for m in re.finditer(r"^;; (\d+) preferences:([^\n]*)", an.greg, re.M):
        get(int(m.group(1))).prefs = set(map(int, m.group(2).split()))
    for p, h in an.disp.items():
        get(p).got = h
    # regs_ever_live before global: hard registers named in the RTL itself
    # (register pins, argument and result copies).
    rtl = an.lreg.split(";; Register dispositions")[0]
    ever_live = {int(x) for x in re.findall(r"\(reg(?:/\w+)*:\w+ (\d+) \w+\)", rtl) if int(x) < FIRST_PSEUDO}
    ever_live.discard(7)  # the frame pointer, eliminated unless needed
    return ps, order, ever_live


def bits(mask):
    return {i for i in range(17) if mask >> i & 1}


def simulate(ps, order, no_r7=False, ever_live=()):
    """Replay global allocation in ORDER; {pseudo: hard reg or None}.
    NO_R7: reload needed the frame pointer and moved r7's pseudos (agbcc
    then retries them in global order, which this replay folds in)."""
    result = {}
    # locally allocated pseudos keep their registers
    used_so_far = set(CALL_USED) | set(ever_live)
    for p in ps.values():
        if not p.to_allocate and p.got is not None:
            used_so_far.add(p.got)
    acquired = collections.defaultdict(set)
    todo = [n for n in order if ps[n].to_allocate]
    for i, n in enumerate(todo):
        p = ps[n]
        someone_prefers = set()
        for m in todo[i + 1:]:
            if m in p.conflicts or n in ps[m].conflicts:
                someone_prefers |= ps[m].prefs - p.prefs
        best = None
        for cls, accept_clobbered in ((p.pref, False), (p.alt, False)):
            if cls == "NO_REGS":
                continue
            best = find_reg(p, cls, accept_clobbered, acquired[n], used_so_far, someone_prefers, no_r7)
            if best is not None:
                break
        if best is None and p.calls and 4 * p.calls < p.refs:  # CALLER_SAVE_PROFITABLE
            best = find_reg(p, p.pref, True, acquired[n], used_so_far, someone_prefers, no_r7)
        result[n] = best
        if best is not None:
            used_so_far.add(best)
            for m in p.conflicts:
                acquired[m].add(best)
            for m in ps:
                if n in ps[m].conflicts:
                    acquired[m].add(best)
    return result


def find_reg(p, cls, accept_clobbered, acquired, used_so_far, someone_prefers, no_r7=False):
    if accept_clobbered:
        used1 = set(FIXED)
    elif p.calls == 0:
        used1 = set(FIXED)
    else:
        used1 = set(CALL_USED)
    used1 |= set(range(17)) - bits(CLASSES.get(cls, CLASSES["ALL_REGS"]))
    used1 |= p.hard_conflicts | acquired
    if no_r7:
        used1.add(7)
    nopref = used1 | (set(range(17)) - used_so_far) | someone_prefers
    best, used = None, used1
    for used in (nopref, used1):
        free = [r for r in range(16) if r not in used]
        if free:
            best = free[0]
            break
    if best is None:
        return None
    for r in sorted(p.prefs - used):
        if (r < 8) == (best < 8):  # same class (LO vs HI) as the found reg
            return r
    return best


def describe(p):
    return f"pseudo {p.n} (refs {p.refs}, live {p.live}, priority {p.priority / 10000:.2f})"


def needs_to_outrank(a, b):
    """Changes to A or B that put A ahead of B."""
    target = b.priority
    out = []
    # A: shorter life
    for live in range(a.live - 1, 0, -1):
        if priority(a.refs, live) > target or (priority(a.refs, live) == target and a.n < b.n):
            out.append(f"shorten {a.n}'s life to {live} insns (now {a.live})")
            break
    for refs in range(a.refs + 1, a.refs + 12):
        if priority(refs, a.live) > target:
            out.append(f"give {a.n} {refs - a.refs} more use(s)")
            break
    for live in range(b.live + 1, b.live * 4 + 40):
        if priority(b.refs, live) < a.priority:
            out.append(f"lengthen {b.n}'s life to {live} insns (now {b.live})")
            break
    for refs in range(b.refs - 1, 0, -1):
        if priority(refs, b.live) < a.priority:
            out.append(f"take {b.refs - refs} use(s) away from {b.n}")
            break
    return out


EDITS = {
    "shorten": "move its assignment down next to its first use, or move the last use up "
               "(perm_move_to_use); if the variable is reused for unrelated values, split it (perm_split_var)",
    "more": "use the variable where the same value is recomputed (perm_temp_for_expr), "
            "or read it once more instead of an equal expression",
    "lengthen": "compute it earlier (move the assignment up) or keep a later use of it",
    "fewer": "recompute the value instead of reusing the variable (perm_expand_expr), "
             "or merge it with another variable (perm_remove_var)",
}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--nonmatching", action="store_true")
    ap.add_argument("--depth", type=int, default=2, help="order moves to combine in the search")
    a = ap.parse_args()
    src = a.src if os.path.exists(a.src) else os.path.join(ROOT, "src/fn", a.src + ".c")
    an = regdiag.analyze(src, a.func, a.nonmatching)
    src_text = open(src).read().splitlines()
    ps, order, ever_live = parse(an)

    def lines_of(n, limit=3):
        ls = sorted(an.pseudo_lines.get(n, ()))[:limit]
        return "; ".join(f"{x}: {src_text[x - 1].strip()[:60]}" for x in ls if 0 < x <= len(src_text))

    print(f"{an.func}: {an.n_insns} instructions, {len(an.struct_ops)} structural diffs, "
          f"{sum(1 for x in an.listing if x.startswith('!'))} register diffs")
    if an.struct_ops:
        print("note: instruction selection or order differs too; the registers below may move once it matches")

    wants = {p: regnum(c.most_common(1)[0][0]) for p, c in an.wants.items()}
    globals_ = [n for n in order if ps[n].to_allocate]
    # A frame pointer shows as no pseudo at all in r7 although one was placed there.
    no_r7 = all(p.got != 7 for p in ps.values()) and 7 in simulate(ps, order, False, ever_live).values()
    sim = simulate(ps, order, no_r7, ever_live)
    agree = sum(1 for n in globals_ if sim.get(n) == ps[n].got)
    print(f"\nglobal replay reproduces {agree}/{len(globals_)} of agbcc's global registers")
    for n in globals_:
        if sim.get(n) != ps[n].got:
            print(f"  replay differs: pseudo {n} replay {sim.get(n)} agbcc {ps[n].got}")

    target = {n: ps[n].got for n in globals_}
    target.update({n: r for n, r in wants.items() if n in target})
    gwant = [n for n in wants if n in target]
    if gwant:
        cache = {}

        def score(o):
            key = tuple(o)
            if key not in cache:
                s = simulate(ps, o, no_r7, ever_live)
                cache[key] = sum(1 for n in globals_ if s.get(n) == target[n])
            return cache[key]
        base = score(order)
        best_order, best, best_hist = order, base, ()
        # Moving a pseudo only matters relative to the pseudos it conflicts with.
        moves = set()
        for n in gwant:
            for m in ps[n].conflicts | {m for m in globals_ if n in ps[m].conflicts}:
                if m in target:
                    moves.add((n, m, 0))   # just before m
                    moves.add((n, m, 1))   # just after m
        frontier = [(order, ())]
        for _ in range(a.depth):
            nxt = []
            for o, hist in frontier:
                for n, m, after in moves:
                    if n in hist or n == m:
                        continue
                    o2 = [x for x in o if x != n]
                    o2.insert(o2.index(m) + after, n)
                    sc = score(o2)
                    if sc > best:
                        best, best_order, best_hist = sc, o2, hist + (n,)
                    if sc > score(o):
                        nxt.append((o2, hist + (n,)))
            frontier = sorted(nxt, key=lambda t: -score(t[0]))[:12]
            if not frontier:
                break
        print(f"\nglobal pseudos wanting another register: {len(gwant)}; "
              f"best reordering gives {best}/{len(globals_)} (now {base})")
        if best > base:
            pos = {n: i for i, n in enumerate(best_order)}
            old = {n: i for i, n in enumerate(order)}
            print("needed priority changes (allocation order is priority order):")
            for x in best_hist:
                passed = [y for y in globals_ if old[x] > old[y] and pos[x] < pos[y]
                          and (y in ps[x].conflicts or x in ps[y].conflicts)]
                fell = [y for y in globals_ if old[x] < old[y] and pos[x] > pos[y]
                        and (y in ps[x].conflicts or x in ps[y].conflicts)]
                if passed:
                    y = max(passed, key=lambda y: (ps[y].priority, -y))
                    print(f"  raise {describe(ps[x])} above {describe(ps[y])}")
                    advice = needs_to_outrank(ps[x], ps[y])
                elif fell:
                    y = min(fell, key=lambda y: (ps[y].priority, -y))
                    print(f"  lower {describe(ps[x])} below {describe(ps[y])}")
                    advice = needs_to_outrank(ps[y], ps[x])
                else:
                    continue
                for s in advice:
                    key = s.split()[0]
                    key = {"give": "more", "take": "fewer"}.get(key, key)
                    print(f"    - {s}: {EDITS[key]}")
                for z in (x, y):
                    if lines_of(z):
                        print(f"      {z}: {lines_of(z)}")
        else:
            print("no reordering of global pseudos reaches the target: the conflicts or preferences "
                  "differ (a value lives across a call or overlaps another), not just the priorities")
            for n in gwant:
                p = ps[n]
                holder = [m for m in p.conflicts if ps[m].got == wants[n]]
                hard = wants[n] in p.hard_conflicts
                why = "the hard register is live across its lifetime" if hard else \
                    f"held by conflicting pseudo(s) {holder}" if holder else "class or call-crossing"
                print(f"  {describe(p)} got {REGNAME.get(p.got)} wants {REGNAME.get(wants[n])}: {why}")
                if lines_of(n):
                    print(f"      {lines_of(n)}")

    lwant = [n for n in wants if n not in target and n in ps]
    if lwant:
        print("\nlocal pseudos (one block, local-alloc; priority order within the block):")
        for n in lwant:
            p = ps[n]
            rivals = [m for m, q in ps.items() if q.block == p.block and q.got == wants[n] and m != n]
            print(f"  {describe(p)} got {REGNAME.get(p.got)} wants {REGNAME.get(wants[n])}"
                  f" in block {p.block}")
            for m in rivals[:3]:
                q = ps[m]
                if q.priority >= p.priority:
                    adv = needs_to_outrank(p, q)
                    print(f"    holder {describe(q)} outranks it: " + "; ".join(adv[:2]))
                else:
                    print(f"    holder {describe(q)} ranks lower: it is born first or has a suggested "
                          f"register (a copy to/from a hard register, e.g. call argument or result)")
            if lines_of(n):
                print(f"      {lines_of(n)}")

    if an.reloads:
        co, uid, ct, ot = an.reloads[0]
        cr, orr = regdiag.norm_regs(ct), regdiag.norm_regs(ot)
        print("\nreload registers: the spill rotation is off from the first differing reload on")
        print(f"  {co:04X} {ct}  | original {ot}  (line {an.off_line.get(co)})")
        print("  reload starts after the register the previous reload used: add or remove one reload "
              "before this point (a literal RAM address instead of a symbol, a large constant offset, "
              "or a constant folded into an address) to rotate it")
        diffs = [(x, y) for x, y in zip(cr, orr) if x != y]
        if diffs:
            x, y = diffs[0]
            print(f"  candidate {x}, original {y}")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Forward symbolic evaluation of Thumb functions in gen/code.s.

  symeval.py gen/code.s OUT.json     every AnimData_Load / SelectPaletteBank call site

Enough to name the arguments of a call: values are ("c", n) constants,
("arg", reg) incoming registers, ("f", base, off, size) loads,
("call", target[, r0]) call results (with the call's constant first argument,
e.g. the keyword letter of Script_GetKeywordValue), ("add", a, b), ("shl", a,
n), ("shr", a, n), ("sp", off) stack addresses, ("any", alternatives) for a
value that differs by path (up to 8), None unknown. A Script_GetValue after
Script_SeekToKeyword(L) is ("call", Script_GetKeywordValue, L, n), the
option's n-th value. Straight-line stores through registers and to the stack
are remembered until the next call or label (and reported to on_store);
labels merge the states that reach them; at a loop head the registers the
loop writes are unknown. Used by tools/assets/bigpalettes.py and
tools/assets/paltrace.py.
"""
import re, sys, collections, json

def C(n): return ("c", n & 0xFFFFFFFF)
ANY_MAX = 8
def lift(f):
    """Apply f over ("any", alternatives) operands."""
    def g(*xs):
        for i, x in enumerate(xs):
            if isinstance(x, tuple) and x[0] == "any":
                return any_of([g(*xs[:i], y, *xs[i + 1:]) for y in x[1]])
        return f(*xs)
    return g
def any_of(vs):
    """One value, ("any", alternatives) for a few, None if any is unknown."""
    out = []
    for v in vs:
        if v is None: return None
        for y in (v[1] if v[0] == "any" else (v,)):
            if y not in out: out.append(y)
    if len(out) > ANY_MAX: return None
    return out[0] if len(out) == 1 else ("any", tuple(out))
@lift
def add(a, b):
    if a is None or b is None: return None
    if a[0] == "c" and b[0] == "c": return C(a[1] + b[1])
    if b[0] == "c" and b[1] == 0: return a
    if a[0] == "c" and a[1] == 0: return b
    if a[0] == "c": a, b = b, a
    if a[0] == "add" and a[2][0] == "c" and b[0] == "c": return add(a[1], C(a[2][1] + b[1]))
    if a[0] == "sp" and b[0] == "c": return ("sp", a[1] + b[1])
    return ("add", a, b)
@lift
def shl(a, n):
    if a is None: return None
    if a[0] == "c": return C(a[1] << n)
    return ("shl", a, n)
@lift
def shr(a, n, signed=False):
    if a is None: return None
    if a[0] == "c":
        v = a[1]
        if signed and v & 0x80000000: v -= 1 << 32
        return C(v >> n)
    if a[0] == "shl" and a[2] == n and n in (16, 24): return a[1]  # zero/sign extension idiom
    return ("shr", a, n)

LOAD = {"sub_082151E4", "AnimData_Load"}
KEYWORD = "sub_0821ABA8"  # Script_GetKeywordValue(letter, default)
SEEK = {"Script_SeekToKeyword"}
GETVALUE = {"Script_GetValue", "Script_GetValueSafe"}
SELECT = {"sub_08215284", "AnimData_SelectPaletteBank"}
REGS = ["r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "sb", "sl", "ip", "fp"]

def parse(path):
    funcs, cur = [], None
    for ln in open(path):
        ln = ln.rstrip("\n")
        m = re.match(r"\s*thumb_func_start (\S+)", ln)
        if m:
            cur = {"name": m.group(1), "lines": []}; funcs.append(cur); continue
        m = re.match(r"\s*arm_func_start (\S+)", ln)
        if m: cur = None; continue
        if cur is not None: cur["lines"].append(ln.strip())
    return funcs

WRITES = re.compile(r"^(?:adds|subs|movs|mov|add|sub|lsls|lsrs|asrs|ands|orrs|eors|bics|muls|negs|rsbs|mvns|adcs|sbcs|rors|ldr\w*) (\w+)")

def run(fn, on_call, on_store=None):
    """Evaluate fn; on_call(target, state, line) at each bl, on_store(addr,
    value, size) at each store through a register with a known base."""
    init = {r: ("arg", r) for r in REGS}
    state, label_states, stack, mem = dict(init), collections.defaultdict(list), {}, {}
    live, seek = True, None
    def merge(states):
        if not states: return None
        return {r: any_of([s.get(r) for s in states]) for r in REGS}
    lines = fn["lines"]
    # loop heads (labels targeted by backward branches): registers the loop
    # body writes (or any call clobbers) are unknown there, the rest keep
    # the values that enter the loop
    pos = {l[:-1]: i for i, l in enumerate(lines) if re.match(r"^_[0-9A-F]+:$", l)}
    back = {}
    for i, l in enumerate(lines):
        m = re.match(r"^b\w* (_[0-9A-F]+)$", l)
        if m and m.group(1) in pos and pos[m.group(1)] <= i:
            body = lines[pos[m.group(1)]:i + 1]
            w = back.setdefault(m.group(1), set())
            for b in body:
                mm = WRITES.match(b)
                if mm: w.add(mm.group(1))
                if b.startswith("bl "): w |= {"r0", "r1", "r2", "r3", "ip"}
                if b.startswith(("pop", "ldm")): w |= set(REGS)
    for l in lines:
        if not l or l.startswith((".", "@")): continue
        m = re.match(r"^(_[0-9A-F]+):$", l)
        if m:
            lab = m.group(1)
            ins = label_states.get(lab, []) + ([state] if live else [])
            mem.clear()
            state = merge(ins) or {r: None for r in REGS}
            if lab in back:
                for r in back[lab]: state[r] = None
            live = True
            continue
        if not live: continue
        m = re.match(r"^(\w+)\s*(.*)$", l)
        if not m: continue
        op, a = m.group(1), [x.strip() for x in re.split(r",(?![^\[]*\])", m.group(2))] if m.group(2) else []
        g = state.get
        def val(x):
            if x.startswith("#"): return C(int(x[1:], 0))
            return g(x)
        try:
            if op == "ldr" and len(a) == 2 and "=" in l and "@ =" in l:
                v = l.split("@ =")[1].strip()
                state[a[0]] = C(int(v, 16)) if v.startswith("0x") else ("sym", v)
            elif op == "movs" and a[1].startswith("#"): state[a[0]] = C(int(a[1][1:], 0))
            elif op in ("mov", "movs") and len(a) == 2: state[a[0]] = g(a[1])
            elif op in ("adds", "add") and len(a) == 2:
                if a[0] == "sp" or a[1] == "sp": pass
                else: state[a[0]] = add(g(a[0]), val(a[1]))
            elif op in ("adds", "add") and len(a) == 3:
                if a[1] == "sp": state[a[0]] = ("sp", int(a[2][1:], 0)) if a[2].startswith("#") else None
                else: state[a[0]] = add(g(a[1]), val(a[2]))
            elif op == "subs" and len(a) == 2 and a[1].startswith("#"): state[a[0]] = add(g(a[0]), C(-int(a[1][1:], 0)))
            elif op == "subs" and len(a) == 3 and a[2].startswith("#"): state[a[0]] = add(g(a[1]), C(-int(a[2][1:], 0)))
            elif op == "lsls" and len(a) == 3: state[a[0]] = shl(g(a[1]), int(a[2][1:], 0))
            elif op == "lsrs" and len(a) == 3: state[a[0]] = shr(g(a[1]), int(a[2][1:], 0))
            elif op == "asrs" and len(a) == 3: state[a[0]] = shr(g(a[1]), int(a[2][1:], 0), True)
            elif op in ("ldr", "ldrh", "ldrb", "ldrsh", "ldrsb") and len(a) == 2 and a[1].startswith("["):
                mm = re.match(r"\[(\w+)(?:, #(0x[0-9a-f]+|\d+)|, (\w+))?\]", a[1])
                base, off, ireg = mm.group(1), int(mm.group(2), 0) if mm.group(2) else 0, mm.group(3)
                size = {"ldr": 4, "ldrh": 2, "ldrb": 1, "ldrsh": -2, "ldrsb": -1}[op]
                if base == "sp": state[a[0]] = stack.get(off)
                else:
                    b = g(base) if not ireg else add(g(base), g(ireg))
                    k = (repr(b), off)
                    state[a[0]] = mem[k] if b is not None and k in mem else (("f", b, off, size) if b is not None else None)
            elif op in ("str", "strh", "strb") and len(a) == 2 and a[1].startswith("[sp"):
                mm = re.match(r"\[sp(?:, #(0x[0-9a-f]+|\d+))?\]", a[1])
                if mm: stack[int(mm.group(1), 0) if mm.group(1) else 0] = g(a[0])
            elif op in ("str", "strh", "strb") and len(a) == 2 and a[1].startswith("["):
                mm = re.match(r"\[(\w+)(?:, #(0x[0-9a-f]+|\d+)|, (\w+))?\]", a[1])
                if mm:
                    b = g(mm.group(1)) if not mm.group(3) else add(g(mm.group(1)), g(mm.group(3)))
                    off = int(mm.group(2), 0) if mm.group(2) else 0
                    if b is not None:
                        mem[(repr(b), off)] = g(a[0])
                        if on_store: on_store(add(b, C(off)), g(a[0]), {"str": 4, "strh": 2, "strb": 1}[op])
            elif op == "bl":
                on_call(a[0], dict(state), l)
                r0 = state.get("r0")
                for r in ("r0", "r1", "r2", "r3", "ip"): state[r] = None
                mem.clear()
                if a[0] in SEEK and r0 and r0[0] == "c":
                    seek = [r0[1], 0]
                    state["r0"] = ("call", a[0], r0[1])
                elif a[0] in GETVALUE and seek:
                    # the values of the option the last seek found, in order:
                    # the same as Script_GetKeywordValue(letter) for the first
                    state["r0"] = ("call", KEYWORD, seek[0], seek[1])
                    seek[1] += 1
                else:
                    state["r0"] = ("call", a[0], r0[1]) if r0 and r0[0] == "c" else ("call", a[0])
            elif op in ("b",) and a and a[0].startswith("_"):
                label_states[a[0]].append(dict(state)); live = False
            elif re.match(r"^b(eq|ne|cs|cc|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|hs|lo)$", op):
                label_states[a[0]].append(dict(state))
            elif op in ("bx", "pop") and ("pc" in l or op == "bx"):
                live = False
            elif op in ("push", "cmp", "cmn", "tst", "str", "strh", "strb", "stm", "stmia"):
                if op == "stm" or op == "stmia":
                    mm = re.match(r"(\w+)!", a[0]) if a else None
                    if mm: state[mm.group(1)] = None
            elif a and a[0] in REGS:
                state[a[0]] = None
        except Exception:
            if a and a[0] in REGS: state[a[0]] = None

def show(v):
    if v is None: return "?"
    k = v[0]
    if k == "c": return f"{v[1]:#x}"
    if k == "arg": return v[1]
    if k == "sym": return v[1]
    if k == "f": return f"{show(v[1])}[{v[2]:#x}]{'' if abs(v[3]) == 4 else ('h' if abs(v[3]) == 2 else 'b')}"
    if k == "call" and v[1] == KEYWORD and len(v) > 2: return f"-{chr(v[2])}" + (f"[{v[3]}]" if len(v) > 3 and v[3] else "")
    if k == "call": return f"{v[1]}({v[2]:#x})" if len(v) > 2 else f"{v[1]}()"
    if k == "add": return f"({show(v[1])}+{show(v[2])})"
    if k == "shl": return f"({show(v[1])}<<{v[2]})"
    if k == "shr": return f"({show(v[1])}>>{v[2]})"
    if k == "sp": return f"sp+{v[1]:#x}"
    if k == "any": return "{" + "|".join(show(x) for x in v[1]) + "}"
    return repr(v)

def call_sites(path):
    """[{func, kind: load|pal, anim, arg, argv, hash, hashv}] for every
    AnimData_Load / AnimData_SelectPaletteBank call; a palette call is paired
    with the latest load on the same anim (else the latest load)."""
    sites = []
    for fn in parse(path):
        loads = []
        def cb(t, st, l, fn=fn, loads=loads):
            if t in LOAD:
                loads.append((repr(st.get("r0")), st.get("r1")))
                sites.append(dict(func=fn["name"], kind="load", anim=show(st.get("r0")), arg=show(st.get("r1")), argv=st.get("r1")))
            elif t in SELECT:
                h = next((h for a, h in reversed(loads) if a == repr(st.get("r0"))), None)
                h = h if h is not None else (loads[-1][1] if loads else None)
                sites.append(dict(func=fn["name"], kind="pal", anim=show(st.get("r0")), arg=show(st.get("r1")), argv=st.get("r1"), hash=show(h), hashv=h))
        run(fn, cb)
    return sites


def main():
    sites = call_sites(sys.argv[1])
    c = collections.Counter((s["kind"], "const" if s["argv"] and s["argv"][0] == "c" else ("none" if s["argv"] is None else s["argv"][0])) for s in sites)
    print(sorted(c.items()))
    json.dump([{k: show(v) if k.endswith("v") else v for k, v in s.items()} for s in sites], open(sys.argv[2], "w"), indent=0)


if __name__ == "__main__":
    main()

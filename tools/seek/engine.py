"""seek's engine: a target (one ROM function), layered evaluation, and the session store.

Evaluation is layered, from what the C cannot vary to what it can:

  skeleton  the determined part: calls (in order), memory accesses (width, signedness, offset; base
            register abstracted), literal-pool values, the frame (pushed registers, stack size).
            A draft that misses one of these computes something else or computes it another way;
            no reordering or allocation move fixes it.
  shape     register-blind instruction distance (scope's shape_distance): order and instruction choice.
  regs      differing instructions after alignment (scope's count): allocation, the last layer.
  regions   the ROM's basic blocks, each judged on its own: '=' exact, 'r' exact up to a consistent
            renaming of low registers inside the block (so a register shift from an earlier block
            does not hide that this block's code is right), 's' same shape, 'x' different, '-' missing.

The key a search sorts by is (skeleton, shape, regs): structure before allocation, so a change that
buys registers by breaking shape (a pin, a scaffold temporary) never ranks above the right shape.
"""
import collections
import contextlib
import hashlib
import io
import json
import os
import re
import subprocess
import sys
import time
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(TOOLS, "scope"))
sys.path.insert(0, TOOLS)
import scope  # noqa: E402  (re-execs under build/venv when capstone is missing)
import fast  # noqa: E402
import build  # noqa: E402
from romlib import ROOT, off  # noqa: E402
import scope as _scope  # noqa: E402,F401


_UNICORN_PROBE = """
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB
uc = Uc(UC_ARCH_ARM, UC_MODE_THUMB)
uc.mem_map(0x08000000, 0x1000)
uc.mem_write(0x08000000, b"\\x01\\x20\\x70\\x47")  # movs r0, #1; bx lr
uc.emu_start(0x08000001, 0x08000002)
"""
_unicorn_ok = None


def unicorn_usable():
    """Whether unicorn runs here. In some sandboxes (no JIT memory) it dies with SIGILL, which no
    try/except catches, so probe it once in a child process. SEEK_SEM=0 skips semantics entirely. In
    Codex's seatbelt sandbox the answer is known, and the probe's crash would raise a macOS crash dialog
    on every seek command, so it is not run there."""
    global _unicorn_ok
    if os.environ.get("SEEK_SEM") == "0" or os.environ.get("CODEX_SANDBOX") == "seatbelt":
        return False
    if _unicorn_ok is None:
        try:
            r = subprocess.run([sys.executable, "-c", _UNICORN_PROBE], capture_output=True, timeout=30)
            _unicorn_ok = r.returncode == 0
        except Exception:  # noqa: BLE001
            _unicorn_ok = False
    return _unicorn_ok


def sha(text):
    return hashlib.sha1(text.encode() if isinstance(text, str) else text).hexdigest()[:16]


def main_checkout():
    try:
        out = subprocess.run(["git", "-C", ROOT, "worktree", "list", "--porcelain"], capture_output=True,
                             text=True).stdout
        return out.split("\n", 1)[0].split(" ", 1)[1]
    except (OSError, IndexError):
        return ROOT


# Sessions live in the main checkout's build/ so workers in other worktrees share them.
HOME = os.environ.get("SEEK_HOME") or os.path.join(main_checkout(), "build", "seek")
LOCAL = os.path.join(ROOT, "build", "seek")  # materialized node files (inside this tree, for cpp paths)


# ------------------------------------------------------------------ source surgery

def find_def(text, name):
    """(start, end) of NAME's definition in TEXT (the last one), or None. Start is the line start (the
    return type may sit on the line before)."""
    m = None
    # a definition starts at column 0 with no parenthesis before the name (`if (Name(x)) {` is a call)
    for m2 in re.finditer(r"^(?![ \t])[^\n;#{}()=]*\b" + re.escape(name) + r"\s*\([^;{]*\)\s*\{", text, re.M):
        m = m2
    if not m:
        return None
    start = text.rfind("\n", 0, m.start()) + 1
    if re.match(r"\s*" + re.escape(name) + r"\b", text[start:]):  # `void\nName(...)`
        prev = text.rfind("\n", 0, start - 1) + 1
        if re.fullmatch(r"(static\s+|inline\s+)*[A-Za-z_][\w \t\*]*", text[prev:start - 1]):
            start = prev
    return start, fast.body_end(text, m.end() - 1)


def splice(text, names, new_def):
    aliases = [alias for alias, target in
               re.findall(r"^\s*#define\s+(\w+)\s+(\w+)\s*$", text, re.M)
               if target in names]
    for n in [*names, *aliases]:
        span = find_def(text, n)
        if span:
            return text[:span[0]] + new_def.rstrip() + "\n" + text[span[1]:]
        # cpp expands a file-local friendly-name alias in the AST. Preserve
        # the source spelling when putting that AST back into the source unit.
        aliases = re.findall(r"^#define[ \t]+(\w+)[ \t]+" + re.escape(n)
                             + r"[ \t]*(?://[^\n]*)?$", text, re.M)
        for alias in aliases:
            span = find_def(text, alias)
            if span:
                renamed = re.sub(r"\b" + re.escape(n) + r"(?=\s*\()", alias, new_def, count=1)
                return text[:span[0]] + renamed.rstrip() + "\n" + text[span[1]:]
    return None


# ------------------------------------------------------------------ the determined part

MEM = re.compile(r"^(ldr|ldrh|ldrsh|ldrb|ldrsb|str|strh|strb) (r\d+|sb|sl|fp|ip), \[(\w+)(?:, (#?-?(?:0x[0-9a-f]+|\d+)|\w+))?\]")


def skeleton(rows, names):
    """Ordered determined events of a decoded listing: ('call', name), ('mem', op, offset),
    ('lit', value), ('frame', ...)."""
    ev = []
    for r in rows:
        t = r[2]
        m = re.match(r"bl #(0x[0-9a-f]+)$", t)
        if m:
            a = int(m.group(1), 16)
            ev.append(("call", names.get(a & ~1, hex(a))))
            continue
        m = MEM.match(t)
        if m:
            op, base, idx = m.group(1), m.group(3), m.group(4)
            if base == "sp":
                continue  # the frame layout is allocation's business
            if base == "pc":
                continue
            o = idx if idx and idx.startswith("#") else ("#0" if not idx else "reg")
            ev.append(("mem", op, o))
            continue
        m = re.match(r"ldr (r\d+), =(0x[0-9A-Fa-f]+)$", r[3])
        if m:
            ev.append(("lit", m.group(2).lower()))
            continue
        if t.startswith("push") or (t.startswith("pop") and "pc" in t):
            ev.append(("frame", t.split()[0], re.sub(r"\s", "", t.split(None, 1)[1])))
        elif re.match(r"(sub|add) sp, #", t):
            ev.append(("frame", t))
    return ev


def ev_text(e):
    if e[0] == "call":
        return f"call {e[1]}"
    if e[0] == "mem":
        return f"{e[1]} [base, {e[2]}]"
    if e[0] == "lit":
        return f"literal {e[1]}"
    return " ".join(e[1:])


def skeleton_diff(rom_ev, mine_ev):
    """(distance, [lines]) between two skeletons: multiset differences, then call order."""
    out = []
    a, b = collections.Counter(rom_ev), collections.Counter(mine_ev)
    miss, extra = a - b, b - a
    # Instruction inventories are observations, not semantic invariants: spills,
    # pointer views and tail sharing change their widths, offsets and counts.
    # Only calls gate search; shape/diff already measure the other differences.
    dist = sum(n for e, n in miss.items() if e[0] == "call")
    dist += sum(n for e, n in extra.items() if e[0] == "call")
    for e, n in sorted(miss.items(), key=str):
        out.append(f"ROM has, draft lacks: {ev_text(e)}" + (f" x{n}" if n > 1 else ""))
    for e, n in sorted(extra.items(), key=str):
        out.append(f"draft has, ROM lacks: {ev_text(e)}" + (f" x{n}" if n > 1 else ""))
    ca_ = [e[1] for e in rom_ev if e[0] == "call"]
    cb = [e[1] for e in mine_ev if e[0] == "call"]
    if ca_ != cb and collections.Counter(ca_) == collections.Counter(cb):
        dist += 1
        out.append("calls in another order: ROM " + ", ".join(ca_[:12]) + " / draft " + ", ".join(cb[:12]))
    return dist, out


# ------------------------------------------------------------------ regions

def blocks_of(rows):
    """[(lo, hi)] row index ranges of the basic blocks of a decoded listing."""
    at = {r[0]: i for i, r in enumerate(rows)}
    leaders = {0}
    for i, r in enumerate(rows):
        t = r[2]
        if r[4] is not None and t.split()[0].startswith("b") and t.split()[0] not in ("bic", "bics", "bl"):
            if r[4] in at:
                leaders.add(at[r[4]])
            leaders.add(i + 1)
        elif t.startswith(("bx", "pop")) and ("pc" in t or t.startswith("bx")):
            leaders.add(i + 1)
        elif t.startswith(".word") or t.startswith("."):
            leaders.add(i)
            leaders.add(i + 1)
    leaders = sorted(x for x in leaders if x < len(rows))
    return [(a, b) for a, b in zip(leaders, leaders[1:] + [len(rows)])]


def regions(rom, mine, pairs):
    """[(lo, hi, verdict, detail)] per ROM block."""
    by_rom = {x: (y, t) for x, y, t in pairs if x is not None}
    extra = collections.Counter()  # draft-only insns, charged to the ROM row before them
    last = 0
    for x, y, t in pairs:
        if x is not None:
            last = x
        elif t == "+":
            extra[last] += 1
    out = []
    for lo, hi in blocks_of(rom):
        if all(rom[i][2].startswith(".") or rom[i][2] == "movs r0, r0" for i in range(lo, hi)):
            continue  # literal pool
        tags = [by_rom.get(i, (None, "-")) for i in range(lo, hi)]
        added = sum(extra[i] for i in range(lo, hi))
        if all(t == "=" for _, t in tags) and not added:
            out.append((lo, hi, "=", ""))
            continue
        if any(y is None for y, _ in tags) or added:
            out.append((lo, hi, "x", f"{sum(1 for y, _ in tags if y is None)} missing, {added} extra"))
            continue
        same_shape = all(scope.norm_insn(rom[i][3]) == scope.norm_insn(mine[y][3]) or t in "=→"
                         for i, (y, t) in zip(range(lo, hi), tags))
        if not same_shape:
            out.append((lo, hi, "x", f"{sum(1 for _, t in tags if t != '=')} differ"))
            continue
        fwd, back, ok = {}, {}, True
        for i, (y, t) in zip(range(lo, hi), tags):
            for a, b in zip(scope.reg_nums(rom[i][2]), scope.reg_nums(mine[y][2])):
                if a > 7 and b > 7:
                    ok &= a == b
                    continue
                if fwd.setdefault(b, a) != a or back.setdefault(a, b) != b:
                    ok = False
        if ok:
            ren = ", ".join(f"r{b}->r{a}" for b, a in sorted(fwd.items()) if a != b)
            out.append((lo, hi, "r", ren))
        else:
            out.append((lo, hi, "s", ""))
    return out


def block_cost(pairs, lo, hi):
    """Differing insns charged to ROM rows [lo, hi): ROM rows not exact, and draft-only rows after them."""
    n, last = 0, -1
    for x, y, t in pairs:
        if x is not None:
            last = x
            if lo <= x < hi and t != "=":
                n += 1
        elif t == "+" and lo <= last < hi:
            n += 1
    return n


def region_string(regs):
    return "".join(v for _, _, v, _ in regs)


# ------------------------------------------------------------------ allocation decisions

def parse_alloc(text, names):
    """{label: [reg, priority, refs, live, kind]} from AGBCC_DUMPPRI records of FUNC: global-alloc (G) and
    local-alloc (L) values with a C name or a constant label (=N); repeats of a label get #2, #3 in
    pseudo order. reg -1 = no register (stack or reload)."""
    recs = []
    for ln in text.splitlines():
        p = ln.split()
        if not p or p[0] not in ("G", "L") or len(p) < 3 or p[1] not in names:
            continue
        if p[0] == "G" and len(p) >= 9:
            pseudo, pri, refs, live, reg, lab = int(p[2]), int(p[3]), int(p[4]), int(p[5]), int(p[7]), p[8]
            recs.append((pseudo, lab, [reg, pri, refs, live, "G"]))
        elif p[0] == "L" and len(p) >= 12:
            pseudo, pri, refs, reg, lab = int(p[3]), int(p[5]), int(p[8]), int(p[10]), p[11]
            recs.append((pseudo, lab, [reg, pri, refs, int(p[7]) - int(p[6]), "L"]))
    out, seen = {}, collections.Counter()
    for pseudo, lab, v in sorted(recs):
        if lab == "-":
            continue
        seen[lab] += 1
        out[lab if seen[lab] == 1 else f"{lab}#{seen[lab]}"] = v
    return out


def alloc_changes(a, b):
    """[text] of labels whose register differs between allocations A (parent) and B."""
    out = []
    for lab in sorted(set(a) | set(b)):
        reg = lambda v: v[0] if isinstance(v, list) else v
        ra, rb = reg(a.get(lab)), reg(b.get(lab))
        if ra != rb:
            out.append(f"{lab} {rname(ra)}->{rname(rb)}")
    return out


def rname(r):
    if r is None:
        return "gone"
    if r < 0:
        return "mem"
    return {13: "sp", 14: "lr", 12: "ip", 11: "fp", 10: "sl", 9: "sb"}.get(r, f"r{r}")


def alloc_wrong(alloc, want):
    """[label] whose register is not the one the ROM uses for it."""
    reg = lambda v: v[0] if isinstance(v, list) else v
    return [lab for lab, r in want.items() if lab in alloc and reg(alloc[lab]) != r]


# ------------------------------------------------------------------ target

class Target:
    """One ROM function and its draft's unit: compiles variants in milliseconds and evaluates them."""

    def __init__(self, src, func=None):
        self.src = os.path.relpath(os.path.abspath(src), ROOT)
        text = open(os.path.join(ROOT, self.src)).read()
        if func and not find_def(text, func):
            aliases = re.findall(r"^#define[ \t]+(\w+)[ \t]+" + re.escape(func)
                                 + r"[ \t]*(?://[^\n]*)?$", text, re.M)
            func = next((n for n in aliases if find_def(text, n)), func)
        if func and not find_def(text, func):  # a ROM name whose draft has a C name: its block's definition
            for b in scope.nonmatching_blocks(text):
                if func in b["names"]:
                    body = text[b["body"]:b["else"][0]]
                    aliases = re.findall(r"^#define[ \t]+(\w+)[ \t]+" + re.escape(func)
                                         + r"[ \t]*(?://[^\n]*)?$", body, re.M)
                    defs = [(find_def(body, n), n) for n in b["names"] if n != func]
                    defs = sorted((span[0], n) for span, n in defs if span)
                    func = next((n for n in aliases if find_def(body, n)),
                                defs[-1][1] if defs else func)
        self.func = func or scope.nonmatching_func(self.src)
        if not self.func:
            raise SystemExit(f"{src}: no NONMATCHING function found; name it")
        self.rom_name = scope.rom_name(self.src, self.func)
        self.names = (self.func, self.rom_name)
        self.group = scope.source_group(self.src, self.func)
        _, addr, sizes = scope.inventory()
        if self.rom_name not in addr:
            raise SystemExit(f"{self.func}: not in the ROM inventory")
        self.addr, self.size = addr[self.rom_name], sizes[self.rom_name]
        self.rom_code = scope.rom_bytes()[off(self.addr):off(self.addr) + self.size]
        self.rom_rows = scope.decode(self.rom_code, self.addr, scope.rom_data(self.rom_name, self.addr,
                                                                               self.rom_code))
        self.symnames = {}
        for n, v in scope.elf_syms().items():
            a = v[0] if isinstance(v, tuple) else v
            if isinstance(a, int):
                self.symnames.setdefault(a & ~1, n)
        self.rom_skel = skeleton(self.rom_rows, self.symnames)
        flags = list(build.cflags_for(self.src))
        os.makedirs(os.path.join(LOCAL, "cache"), exist_ok=True)
        self.fast = fast.Fast(scope.INSTR, flags, build.AS, scope.elf_syms(), self.addr,
                              os.path.join(LOCAL, "cache"), fixup=scope.inline_macros)
        self.compiles = 0
        self.compile_time = 0.0
        self.block = None  # (lo, hi) ROM rows: the block objective's region
        self._oracle = None

    # -- compiling
    def preprocess_file(self, path):
        rel = os.path.relpath(os.path.abspath(path), ROOT)
        return scope.fast_preprocess(rel, {}, self.func)

    def compile_pre(self, pre, knobs=None, trace=False):
        """(code, error) for a preprocessed unit text; with TRACE, (code, error, alloc): the allocation
        agbcc made, per labeled value (AGBCC_DUMPPRI, written by the fork server's child: no -g needed)."""
        t0 = time.time()
        self.group = scope.source_group(self.src, self.func, pre)
        knobs = dict(knobs or {})
        if trace:
            path = os.path.join(LOCAL, "cache", f"pri{os.getpid()}.txt")
            open(path, "w").close()
            knobs["AGBCC_DUMPPRI"] = path
        code, err = self.fast.bytes(pre, self.group, knobs)
        self.compiles += 1
        self.compile_time += time.time() - t0
        if not trace:
            return code, err
        text = open(path).read()
        alloc = parse_alloc(text, self.names) if code else {}
        alloc["__reloads__"] = sum(1 for ln in text.splitlines() if ln.startswith("R ") and ln.split()[1] in self.names)
        return code, err, alloc

    def rotation(self, pre, n_reloads, max_d=5, hint=None):
        """Is this code right up to reload's round-robin spill choice? [0, K, D] for the smallest shift
        D (AGBCC_SPILL=K:D, reload1.c's last_spill_reg moved D places before choice K) that gives the
        ROM's bytes, else None. D counts reload choices the ROM made (or skipped) before choice K that
        this C does not: the C fix adds or removes that many."""
        if hint:  # near the parent's shift: a move changes the count of earlier choices by one or two
            k0, d0 = hint
            ks = [k for k in range(k0 - 3, k0 + 4) if 0 <= k < n_reloads]
            ds = [d for d in (d0 - 1, d0 - 2, d0, d0 + 1, d0 + 2) if 1 <= d <= max_d]
        else:
            ks, ds = range(n_reloads), range(1, max_d + 1)
        for d in sorted(ds):
            for k in ks:
                code, _ = self.compile_pre(pre, {"AGBCC_SPILL": f"{k}:{d}"})
                if code == self.rom_code:
                    return [0, k, d]
        return None

    # -- evaluation
    def evaluate(self, code):
        if code is None:
            return {"ok": False, "key": [9999, 9999, 9999], "counts": "COMPILE FAILED"}
        if code == self.rom_code:
            return {"ok": True, "match": True, "key": [0, 0, 0], "counts": "MATCH", "regions": "",
                    "size": len(code), "blk": 0}
        rows = scope.decode(code, self.addr, self.fast.data)
        pairs, _ = scope.align(self.rom_rows, rows)
        diff = sum(1 for _, _, t in pairs if t != "=")
        shape = scope.shape_distance(self.rom_rows, rows)
        sk, sk_lines = skeleton_diff(self.rom_skel, skeleton(rows, self.symnames))
        regs = regions(self.rom_rows, rows, pairs)
        counts = scope.count_text([{"tag": t} for _, _, t in pairs])
        out = {"ok": True, "match": False, "key": [sk, shape, diff], "counts": counts,
               "regions": region_string(regs), "skeleton": sk_lines[:12], "size": len(code)}
        if self.block:
            out["blk"] = block_cost(pairs, *self.block)
        return out

    def detail(self, code):
        rows = scope.decode(code, self.addr, self.fast.data)
        pairs, _ = scope.align(self.rom_rows, rows)
        return rows, pairs, regions(self.rom_rows, rows, pairs)

    def wanted(self, path):
        """{label: the ROM's register} for the values of the draft at PATH: scope's full compile, then
        votes from aligned insns (scope.rom_hard), keyed like parse_alloc's labels."""
        try:
            (mine, addr, info), _ = capture(scope.compile_draft, path, self.func)
            capture(scope.report, path, self.func, self.rom_code, addr, mine, info, 0)
            wd = info["wd"]
            rom = scope.rom_hard(wd)
            pn = scope.pseudo_names(wd, self.func)
        except BaseException as e:  # noqa: BLE001
            print(f"(no ROM register targets: {str(e)[:200]})", file=sys.stderr)
            return {}
        out, seen = {}, collections.Counter()
        for x in sorted(pn):
            lab = pn[x]
            if not lab or lab == "-":
                continue
            lab = re.sub(r"#\d+$", "", lab)
            seen[lab] += 1
            key = lab if seen[lab] == 1 else f"{lab}#{seen[lab]}"
            if x in rom:
                out[key] = rom[x]
        return out

    # -- semantics
    def oracle(self, pre):
        if self._oracle is None and not unicorn_usable():
            self._oracle = False
            print("(semcheck unavailable: unicorn cannot run here, e.g. in a sandbox without JIT; SEEK_SEM=0 skips the probe)",
                  file=sys.stderr)
        if self._oracle is None:
            try:
                import semcheck
            except BaseException:  # noqa: BLE001  semcheck exits when unicorn is missing
                self._oracle = False
                return False
            text = open(os.path.join(ROOT, self.src)).read()
            try:
                self._oracle = semcheck.Oracle(self.rom_name, 6, semcheck.returns_value(text, self.func),
                                               arity=semcheck.arities(pre),
                                               kinds=semcheck.param_kinds(pre, self.func))
            except Exception as e:  # noqa: BLE001
                self._oracle = False
                print(f"(semcheck unavailable: {e})", file=sys.stderr)
        return self._oracle

    def semantic(self, code, pre):
        o = self.oracle(pre)
        if not o or code is None:
            return "unchecked"
        try:
            verdict, detail = o.check(code)
        except Exception as e:  # noqa: BLE001
            return f"unchecked ({e})"
        return verdict if not detail else f"{verdict}: {detail}"


# ------------------------------------------------------------------ session store

class Session:
    """The search tree of one function: nodes.jsonl (append-only, one evaluated node per line) and
    content-addressed blobs. Shared by every worker on the function (SEEK_HOME)."""

    def __init__(self, func):
        self.func = func
        self.dir = os.path.join(HOME, func)
        os.makedirs(os.path.join(self.dir, "blobs"), exist_ok=True)
        self.path = os.path.join(self.dir, "nodes.jsonl")
        self.nodes = {}
        self.by_out = {}
        self.events = []
        self.offset = 0
        if os.path.exists(self.path):
            self.offset = os.path.getsize(self.path)
            for ln in open(self.path):
                if not ln.strip():
                    continue
                r = json.loads(ln)
                if r.get("type") == "event":
                    self.events.append(r)
                    continue
                self.nodes[r["id"]] = r
                if r.get("out"):
                    self.by_out.setdefault(r["out"], r["id"])

    def put_blob(self, text):
        h = sha(text)
        p = os.path.join(self.dir, "blobs", h + ".z")
        if not os.path.exists(p):
            tmp = f"{p}.{os.getpid()}"
            open(tmp, "wb").write(zlib.compress(text.encode(), 6))
            os.replace(tmp, p)
        return h

    def blob(self, h):
        return zlib.decompress(open(os.path.join(self.dir, "blobs", h + ".z"), "rb").read()).decode()

    def _append(self, rec):
        import fcntl
        with open(self.path, "a") as f:
            fcntl.flock(f, fcntl.LOCK_EX)
            f.write(json.dumps(rec) + "\n")

    def add(self, rec):
        import fcntl
        with open(self.path, "a+") as f:  # ids are allocated under the lock: other workers append too
            fcntl.flock(f, fcntl.LOCK_EX)
            f.seek(self.offset)  # only what others appended since we last looked
            for ln in f:
                try:
                    r = json.loads(ln)
                except ValueError:
                    continue
                if r.get("type") != "event":
                    self.nodes.setdefault(r["id"], r)
                    if r.get("out"):
                        self.by_out.setdefault(r["out"], r["id"])
            rec["id"] = max(self.nodes, default=0) + 1
            rec.setdefault("t", round(time.time()))
            f.write(json.dumps(rec) + "\n")
            f.flush()
            self.offset = f.tell()
        self.nodes[rec["id"]] = rec
        if rec.get("out"):
            self.by_out.setdefault(rec["out"], rec["id"])
        return rec

    def event(self, **kw):
        kw.update(type="event", t=round(time.time()))
        self._append(kw)
        self.events.append(kw)

    def update(self, nid, **kw):
        """Annotate a node (notes, semantic verdicts): appended as an event, folded in on load."""
        self.nodes[nid].update(kw)
        self.event(kind="update", node=nid, fields=kw)

    def fold_updates(self):
        for e in self.events:
            if e.get("kind") == "update" and e.get("node") in self.nodes:
                self.nodes[e["node"]].update(e["fields"])

    def text(self, node):
        """The node's whole source file."""
        text = self.blob(node["file"])
        for name, h in sorted((node.get("defs") or {}).items()):
            aliases = re.findall(r"^#define[ \t]+(\w+)[ \t]+" + re.escape(name)
                                 + r"[ \t]*(?://[^\n]*)?$", text, re.M)
            source_name = next((n for n in [name] + aliases if find_def(text, n)), name)
            definition = self.blob(h)
            if source_name != name:
                definition = re.sub(r"\b" + re.escape(name) + r"\b", source_name, definition)
            out = splice(text, [source_name], definition)
            if out is None:
                raise SystemExit(f"n{node['id']}: cannot find {name}'s definition in its base file")
            text = out
        return text

    def materialize(self, node):
        """build/seek/FUNC/nN.c in this tree (cpp resolves includes relative to the tree)."""
        d = os.path.join(LOCAL, self.func)
        os.makedirs(d, exist_ok=True)
        p = os.path.join(d, f"n{node['id']}.c")
        t = self.text(node)
        t = re.sub(r"^// scope: write \w+ here\n", "", t, flags=re.M)
        t = f"// scope: write {self.func} here\n" + t
        if not os.path.exists(p) or open(p).read() != t:
            open(p, "w").write(t)
        return os.path.relpath(p, ROOT)

    def expanded(self, nid):
        return [e for e in self.events if e.get("kind") == "expand" and e.get("node") == nid]


def key_of(node):
    return tuple(node.get("key") or (9999, 9999, 9999))


def capture(fn, *a, **kw):
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        r = fn(*a, **kw)
    return r, buf.getvalue()

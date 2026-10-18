#!/usr/bin/env python3
"""Fragment-level idiom corpus: which C produced which instruction shape.

  idioms.py --build [-j N]           compile every matched function in src/
                                     with -g and index its (C line, asm) pairs
  idioms.py ADDR|NAME [--lines A-B]  for each basic block of a function (by
                                     default an unmatched one), the matched C
                                     whose asm has the same or nearest shape
  idioms.py --shape "ldrsh R,[R,#K]; cmp R,#K"   look up a shape directly

The corpus comes from the ROM itself: a matched function's bytes are the
compiler's output, so its ROM disassembly (capstone, the same syntax every
query uses) is split by the -g line table of its C. A fragment is a run of
consecutive instructions from one source line; windows of up to three
neighbouring runs are indexed too, so a statement whose code interleaves with
its neighbour still has an entry.

Shapes abstract registers (r0-r12 -> R; sp, lr and pc stay), literal-pool
loads (ldr R,=K), branch targets (L) and callees (F), and immediates (#K)
except shift amounts and #0, which decide the idiom (lsl/asr #0x10 is an s16
cast, #0x18 an s8 one). The index is build/idioms/corpus.pkl (never commit
it); rebuild after large batches of matches.

Library use (tools/explain.py):
    lines = compile_lines(path, func)   {func: [(offset, size, file, line)]}
    shape(text), Corpus.load().lookup(tokens, k)
"""
import argparse
import collections
import difflib
import multiprocessing
import os
import pickle
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402
import check  # noqa: E402
from romlib import ROOT, load_rom, off  # noqa: E402

CORPUS = os.path.join(ROOT, "build", "idioms", "corpus.pkl")
MAX_RUNS = 3      # runs per window
MAX_INSNS = 24    # instructions per window
NGRAM = 3

# --- normalization ----------------------------------------------------------

REG = re.compile(r"\b(r1[0-2]|r[0-9]|sb|sl|fp|ip)\b")
IMM = re.compile(r"#-?(0x[0-9a-f]+|\d+)")
SHIFTS = ("lsls", "lsrs", "asrs", "rors", "lsl", "lsr", "asr")
BRANCH = re.compile(r"^b(eq|ne|cs|hs|cc|lo|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al)?(\.n|\.w)?$")


def shape(text):
    """Shape token of one capstone instruction (see the module docstring)."""
    mnem, _, ops = text.partition(" ")
    if BRANCH.match(mnem):
        return mnem.split(".")[0] + " L"
    if mnem in ("bl", "blx"):
        return "bl F"
    if mnem == "ldr" and "[pc" in ops:
        return "ldr R,=K"
    if mnem in ("push", "pop"):
        return mnem + (" {..pc}" if "pc" in ops else " {..lr}" if "lr" in ops else " {..}")
    if mnem in ("ldm", "stm", "ldmia", "stmia"):
        return mnem + " R!,{..}"
    ops = REG.sub("R", ops).replace(" ", "")
    if mnem not in SHIFTS:
        ops = IMM.sub(lambda m: m.group(0) if m.group(1) in ("0", "0x0") else "#K", ops)
    return f"{mnem} {ops}".strip()


def exact(text):
    """Instruction text with branch and call targets removed (they move with
    any size change); registers and constants kept."""
    mnem, _, ops = text.partition(" ")
    if BRANCH.match(mnem):
        return mnem.split(".")[0] + " L"
    if mnem in ("bl", "blx"):
        return text
    if mnem == "ldr" and "[pc" in ops:
        return ops.split(",")[0] + " <- pool"
    return text


def is_terminator(text):
    mnem, _, ops = text.partition(" ")
    return bool(BRANCH.match(mnem)) or mnem == "bx" or (mnem == "pop" and "pc" in ops) or \
        (mnem in ("mov", "add") and ops.startswith("pc"))


def branch_target(text):
    mnem, _, ops = text.partition(" ")
    if BRANCH.match(mnem) and ops.startswith("#"):
        return int(ops[1:], 0)
    return None


# --- compiling with line info -----------------------------------------------

def _bytes_of_directive(body, out):
    """Append the bytes of one .byte/.2byte/.4byte/.ascii line; symbolic
    .4byte operands become ('sym', name)."""
    kw, _, arg = body.partition("\t") if "\t" in body else body.partition(" ")
    arg = arg.strip()
    if kw == ".ascii":
        s = arg[1:-1]
        s = re.sub(r"\\([0-7]{1,3})", lambda m: chr(int(m.group(1), 8)), s)
        out.extend(s.encode("latin-1"))
        return
    width = {".byte": 1, ".2byte": 2, ".4byte": 4, ".word": 4}.get(kw)
    if not width:
        return
    for a in arg.split(","):
        a = a.strip()
        try:
            v = int(a, 0)
        except ValueError:
            out.append(("sym", a))
            out.extend([0] * (width - 1))
            continue
        out.extend((v & (0xFF << (8 * i))) >> (8 * i) for i in range(width))


def _uleb(data, i):
    v = shift = 0
    while True:
        b = data[i]
        i += 1
        v |= (b & 0x7F) << shift
        shift += 7
        if not b & 0x80:
            return v, i


def _sleb(data, i):
    v = shift = 0
    while True:
        b = data[i]
        i += 1
        v |= (b & 0x7F) << shift
        shift += 7
        if not b & 0x80:
            if b & 0x40:
                v -= 1 << shift
            return v, i


def line_table(asm):
    """{label: (file, line)} from the DWARF 2 .debug_line sections of agbcc
    -g output (gcc 2.95 writes the line program as data directives)."""
    rows = {}
    chunks, cur = [], None
    for ln in asm.splitlines():
        s = ln.strip()
        if s.startswith(".section"):
            cur = [] if ".debug_line" in s else None
            if cur is not None:
                chunks.append(cur)
            continue
        if s in (".text", ".data") or s.startswith((".text", ".code")):
            cur = None
        if cur is not None and s.startswith("."):
            _bytes_of_directive(s.split("@")[0].strip(), cur)
    for data in chunks:
        i = 0
        while i + 10 < len(data):
            start = i
            unit_len = data[i] | data[i + 1] << 8 | data[i + 2] << 16 | data[i + 3] << 24
            end = start + 4 + unit_len
            i += 6  # length, version
            i += 4  # header length
            min_inst, _, line_base, line_range, opcode_base = data[i:i + 5]
            line_base = line_base - 256 if line_base > 127 else line_base
            i += 5 + opcode_base - 1
            while data[i] != 0:  # include directories
                while data[i] != 0:
                    i += 1
                i += 1
            i += 1
            files = [None]
            while data[i] != 0:
                j = i
                while data[j] != 0:
                    j += 1
                files.append(bytes(data[i:j]).decode("latin-1"))
                i = j + 1
                for _ in range(3):
                    _, i = _uleb(data, i)
            i += 1
            fileno, line, addr = 1, 1, None
            while i < end and i < len(data):
                op = data[i]
                i += 1
                if op >= opcode_base:
                    line += line_base + (op - opcode_base) % line_range
                    if addr is not None:
                        rows[addr] = (files[fileno], line)
                elif op == 0:
                    n, i = _uleb(data, i)
                    sub = data[i]
                    if sub == 2:
                        a = data[i + 1]
                        addr = a[1] if isinstance(a, tuple) else None
                    elif sub == 1:
                        fileno, line, addr = 1, 1, None
                    i += n
                elif op == 1:
                    if addr is not None:
                        rows[addr] = (files[fileno], line)
                elif op == 2:
                    _, i = _uleb(data, i)
                elif op == 3:
                    d, i = _sleb(data, i)
                    line += d
                elif op == 4:
                    fileno, i = _uleb(data, i)
                elif op == 5:
                    _, i = _uleb(data, i)
                elif op == 9:
                    i += 2
            i = end
    return rows


def functions_in_asm(asm, rows=None):
    """{func: [(offset, size, file, line)]} for every instruction of every
    function in agbcc assembly; offsets count pools and alignment like as."""
    rows = line_table(asm) if rows is None else rows
    out = {}
    cur, offset, where, insns = None, 0, (None, None), None
    for ln in asm.splitlines():
        body = ln.split("@", 1)[0].strip()
        if not body:
            continue
        m = re.match(r"^([A-Za-z_]\w*):$", body)
        if m and not ln.startswith((" ", "\t")):
            cur, offset, where = m.group(1), 0, (None, None)
            insns = out.setdefault(cur, [])
            continue
        if body.endswith(":"):
            if body[:-1] in rows:
                where = rows[body[:-1]]
            continue
        if cur is None:
            continue
        if body.startswith((".include", ".section", ".size")) or body.startswith(".Lfe"):
            cur = None
            continue
        if body.startswith(".align"):
            n = 1 << int(body.split()[1].rstrip(","))
            offset = (offset + n - 1) // n * n
            continue
        if body.startswith((".word", ".4byte", ".long")):
            offset += 4
            continue
        if body.startswith((".short", ".hword", ".2byte")):
            offset += 2
            continue
        if body.startswith("."):
            continue
        size = 4 if body.split()[0] in ("bl", "blx") else 2
        insns.append((offset, size, where[0], where[1]))
        offset += size
    return {f: v for f, v in out.items() if v}


def preprocess_lines(src, defines=()):
    """Preprocessed text with line markers (so -g records real lines)."""
    return build.sh(build.CPP + ["-nostdinc", "-undef"] + build.DEFINES + list(defines) +
                    ["-I", "include", "-iquote", ".", src])


def compile_g(src, text=None, defines=()):
    """agbcc -g assembly for SRC (or its preprocessed TEXT), or None."""
    rel = os.path.relpath(os.path.join(ROOT, src), ROOT)
    text = text if text is not None else preprocess_lines(rel, defines)
    flags = build.cflags_for(rel) + ["-g"]
    with tempfile.TemporaryFile(mode="w+") as f:
        f.write(text)
        f.seek(0)
        r = subprocess.run([build.compiler_for(rel)] + flags + ["-o", "-"], stdin=f,
                           cwd=ROOT, capture_output=True, text=True)
    return r.stdout if r.returncode == 0 else None


# --- the corpus ---------------------------------------------------------------

_source_cache = {}


def source_line(path, line):
    if path not in _source_cache:
        try:
            _source_cache[path] = open(os.path.join(ROOT, path), errors="replace").read().splitlines()
        except OSError:
            _source_cache[path] = []
    lines = _source_cache[path]
    if not line or line > len(lines):
        return ""
    text = lines[line - 1].strip()
    k = line
    # join a statement continued on the next lines
    while k < len(lines) and k < line + 3 and not re.search(r"[;{}]\s*(//.*)?$", text) \
            and not text.startswith("#"):
        text += " " + lines[k].strip()
        k += 1
    text = re.sub(r"\s+", " ", re.sub(r"//.*$|/\*.*?\*/", "", text)).strip()
    return text[:110]


_md = None


def disasm(code, base=0):
    """[(offset, text)] decoding ARMv4T Thumb only: just BL pairs are 32 bits.
    (capstone's Thumb mode reads a pool word plus the next halfword as one
    Thumb-2 instruction and loses the instruction boundaries.)"""
    global _md
    if _md is None:
        import capstone
        _md = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_THUMB)
    out, i = [], 0
    while i + 1 < len(code):
        hw = code[i] | code[i + 1] << 8
        n = 4 if (hw >> 11) == 0x1E and i + 3 < len(code) and (code[i + 3] >> 3) == 0x1F else 2
        ins = next(_md.disasm(code[i:i + n], base + i), None)
        if ins is None or ins.size != n:
            out.append((base + i, f".2byte 0x{hw:04x}"))
            i += 2
            continue
        out.append((base + i, f"{ins.mnemonic} {ins.op_str}".strip()))
        i += n
    return out


def disasm_function(rom, addr, size):
    """[(offset, text)] for the code of a ROM function, literal pools dropped."""
    import regdiag
    code = rom[off(addr):off(addr) + size]
    dis = disasm(code, 0)
    return regdiag.strip_pools(dis, len(code), code, addr)


_names = None


def name_calls(ins, address):
    """Show `bl` targets of instructions disassembled at offset 0 as the
    callee's name (the function sits at ADDRESS)."""
    global _names
    if _names is None:
        import progress
        _names = {a: n for n, a in progress.load_functions()[0]}
    out = []
    for o, t in ins:
        m = re.match(r"(bl|blx) #(0x[0-9a-f]+)$", t)
        if m:
            a = (address + int(m.group(2), 16)) & 0xFFFFFFFF
            t = f"{m.group(1)} {_names.get(a, f'{a:#010x}')}"
        out.append((o, t))
    return out


def basic_blocks(ins):
    """Split [(offset, text)] into basic blocks (lists of indices)."""
    targets = {branch_target(t) for _, t in ins}
    blocks, cur = [], []
    for i, (o, t) in enumerate(ins):
        if cur and o in targets:
            blocks.append(cur)
            cur = []
        cur.append(i)
        if is_terminator(t):
            blocks.append(cur)
            cur = []
    if cur:
        blocks.append(cur)
    return blocks


def _matched_functions():
    """[(src path, [C function names])] for files the default build compiles
    with agbcc -O2."""
    from split import c_functions
    out = []
    for dirpath, _, files in os.walk(os.path.join(ROOT, "src")):
        for fn in sorted(files):
            if not fn.endswith(".c"):
                continue
            p = os.path.relpath(os.path.join(dirpath, fn), ROOT)
            if build.compiler_for(p) != build.AGBCC or build.cflags_for(p) != build.CFLAGS:
                continue
            names = c_functions(os.path.join(ROOT, p), with_asm=False)
            if names:
                out.append((p, names))
    return sorted(out)


LOOSE = {"add": "ad", "sub": "su", "neg": "rs", "swi": "sv", "ldmia": "ld", "stmia": "st",
         "mul": "mu", "lsl": "ls", "lsr": "ls", "asr": "as", "and": "an", "orr": "or",
         "eor": "eo", "bic": "bi", "mvn": "mv", "ror": "ro", "adc": "ad", "sbc": "sb", "mov": "mo"}


def _harvest(job):
    """Fragments of one source file: [(func, [(shape tokens, ctext, loc)])]."""
    path, names, addrs, sizes = job
    asm = compile_g(path)
    if asm is None:
        return path, [], "compile failed"
    rom = _harvest.rom
    funcs = functions_in_asm(asm)
    out = []
    bad = []
    for name in names:
        lab = name
        if name not in funcs:
            continue
        addr = addrs.get(name)
        if addr is None:
            bad.append(name)
            continue
        code_ins = funcs[lab]
        size = sizes.get(name) or (code_ins[-1][0] + code_ins[-1][1])
        code = rom[off(addr):off(addr) + size]
        dis = dict(disasm(code, 0))
        texts = []
        for o, sz, f, ln in code_ins:
            t = dis.get(o)
            if t is None or (sz == 4) != (t.split()[0] in ("bl", "blx")):
                texts = None
                break
            texts.append((t, f, ln))
        if not texts:
            bad.append(name)
            continue
        # runs of consecutive instructions from one source line
        runs = []
        for t, f, ln in texts:
            if runs and runs[-1][0] == (f, ln):
                runs[-1][1].append(shape(t))
            else:
                runs.append(((f, ln), [shape(t)]))
        frags = []
        for i in range(len(runs)):
            toks, keys = [], []
            for j in range(i, min(i + MAX_RUNS, len(runs))):
                toks = toks + runs[j][1]
                if len(toks) > MAX_INSNS:
                    break
                if runs[j][0] not in keys:
                    keys.append(runs[j][0])
                if len(toks) < 2 or None in (runs[j][0][1],):
                    continue
                lines = sorted(k for k in keys if k[1])
                ctext = " | ".join(dict.fromkeys(source_line(f, ln) for f, ln in lines if source_line(f, ln)))
                if not ctext:
                    continue
                f0, l0 = lines[0]
                frags.append((tuple(toks), ctext, f"{f0}:{l0}"))
        out.append((name, frags))
    return path, out, (f"{len(bad)} unresolved: {' '.join(bad[:3])}" if bad else "")


def _init_harvest():
    _harvest.rom = load_rom(verify=False)


def build_corpus(jobs):
    import progress
    funcs, sizes = progress.load_functions()
    addrs = {n: a for n, a in funcs}
    addrs.update({f"sub_{a:08X}": a for n, a in funcs})
    by_addr = {a: n for n, a in funcs}
    sizes2 = dict(sizes)
    sizes2.update({f"sub_{a:08X}": sizes[n] for n, a in funcs})
    files = _matched_functions()
    work = [(p, names, {n: addrs[n] for n in names if n in addrs},
             {n: sizes2[n] for n in names if n in sizes2}) for p, names in files]
    vocab, ids = [], {}
    merged = {}  # (token ids, ctext) -> [count, loc]
    nfun = nfail = 0
    with multiprocessing.get_context("fork").Pool(jobs, initializer=_init_harvest) as pool:
        for k, (path, out, note) in enumerate(pool.imap_unordered(_harvest, work, chunksize=8)):
            if note:
                nfail += 1
                print(f"  {path}: {note}", file=sys.stderr)
            for name, frags in out:
                nfun += 1
                for toks, ctext, loc in frags:
                    key = (tuple(ids.setdefault(t, len(ids)) for t in toks), ctext)
                    e = merged.get(key)
                    if e:
                        e[0] += 1
                    else:
                        merged[key] = [1, f"{loc} {name}"]
            if k % 500 == 0:
                print(f"  {k}/{len(work)} files, {nfun} functions, {len(merged)} fragments", file=sys.stderr)
    vocab = [None] * len(ids)
    for t, i in ids.items():
        vocab[i] = t
    frags = [(toks, ctext, count, loc) for (toks, ctext), (count, loc) in merged.items()]
    frags.sort(key=lambda f: (-f[2], len(f[0])))
    exact_ix = collections.defaultdict(list)
    grams = collections.defaultdict(list)
    for i, (toks, _, _, _) in enumerate(frags):
        exact_ix[toks].append(i)
        for g in set(_ngrams(toks)):
            grams[g].append(i)
    os.makedirs(os.path.dirname(CORPUS), exist_ok=True)
    with open(CORPUS + ".tmp", "wb") as f:
        pickle.dump({"vocab": vocab, "frags": frags, "exact": dict(exact_ix), "grams": dict(grams)}, f,
                    protocol=pickle.HIGHEST_PROTOCOL)
    os.replace(CORPUS + ".tmp", CORPUS)
    print(f"{nfun} functions from {len(work)} files ({nfail} with notes): {len(frags)} fragments, "
          f"{len(exact_ix)} distinct shapes -> {os.path.relpath(CORPUS, ROOT)}")


def _ngrams(toks):
    if len(toks) < NGRAM:
        return [tuple(toks)]
    return [tuple(toks[i:i + NGRAM]) for i in range(len(toks) - NGRAM + 1)]


class Corpus:
    _loaded = None

    def __init__(self, d):
        self.vocab = d["vocab"]
        self.ids = {t: i for i, t in enumerate(self.vocab)}
        self.frags, self.exact, self.grams = d["frags"], d["exact"], d["grams"]

    @classmethod
    def load(cls):
        if cls._loaded is None:
            if not os.path.exists(CORPUS):
                sys.exit("no idiom corpus: run `python3 tools/idioms.py --build` (about a minute)")
            with open(CORPUS, "rb") as f:
                cls._loaded = cls(pickle.load(f))
        return cls._loaded

    def encode(self, tokens):
        return tuple(self.ids.get(t, -1 - k) for k, t in enumerate(tokens))

    def lookup(self, tokens, k=3, distinct=True, min_sim=0.5):
        """[(similarity, ctext, count, loc)]: exact shapes first (most common C
        first), then the nearest by aligned token similarity."""
        q = self.encode(tokens)
        out, seen = [], set()

        def add(i, sim):
            toks, ctext, count, loc = self.frags[i]
            key = re.sub(r"\w+", "w", ctext) if distinct else ctext
            if key in seen:
                return
            seen.add(key)
            out.append((sim, ctext, count, loc))

        ex = self.exact.get(q, [])[:200]
        for i in sorted(ex, key=lambda i: 'asm("' in self.frags[i][1]):
            add(i, 1.0)
            if len(out) >= k:
                return out
        votes = collections.Counter()
        qgrams = set(_ngrams(q))
        for g in qgrams:
            post = self.grams.get(g, ())
            if len(post) > 40000:
                continue
            votes.update(post)
        cands = [i for i, _ in votes.most_common(400)]
        scored = []
        for i in cands:
            toks = self.frags[i][0]
            sm = difflib.SequenceMatcher(None, q, toks, autojunk=False)
            pinned = 'asm("' in self.frags[i][1]  # register pins: concessions, not idioms
            scored.append((round(sm.ratio(), 2), not pinned, self.frags[i][2], -len(toks), i))
        scored.sort(reverse=True)
        for sim, _, _, _, i in scored:
            if sim < min_sim or len(out) >= k:
                break
            add(i, sim)
        return out


# --- queries ------------------------------------------------------------------

def resolve_function(arg):
    import progress
    funcs, sizes = progress.load_functions()
    for n, a in funcs:
        if arg in (n, f"sub_{a:08X}") or re.fullmatch(r"(0x)?0?" + f"{a:08X}"[1:], arg, re.I):
            return n, a, sizes[n]
    sys.exit(f"unknown function {arg}")


def pieces(ins, block, max_len=10):
    """Cut a basic block after calls and at max_len so each piece is about
    one statement's worth of code."""
    out, cur = [], []
    for i in block:
        cur.append(i)
        if ins[i][1].startswith("bl ") or len(cur) >= max_len:
            out.append(cur)
            cur = []
    if cur:
        if out and len(cur) == 1:
            out[-1] += cur
        else:
            out.append(cur)
    return out


def format_hits(hits, indent="    "):
    lines = []
    for sim, ctext, count, loc in hits:
        tag = f"={count:<3}" if sim == 1.0 else f"~{sim:.2f}"
        lines.append(f"{indent}{tag} {ctext}   [{loc.split()[0]}]")
    return lines


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("func", nargs="?", help="address or name")
    ap.add_argument("--build", action="store_true")
    ap.add_argument("-j", type=int, default=os.cpu_count())
    ap.add_argument("--lines", help="instruction numbers A-B (as printed) to query")
    ap.add_argument("--shape", help="semicolon-separated shape or capstone instructions")
    ap.add_argument("-k", type=int, default=3)
    a = ap.parse_args()
    if a.build:
        build_corpus(a.j)
        return
    corpus = Corpus.load()
    if a.shape:
        toks = [t.strip() for t in a.shape.split(";") if t.strip()]
        toks = [t if (" R" in t or "=K" in t or t.endswith((" L", " F"))) else shape(t) for t in toks]
        print("; ".join(toks))
        print("\n".join(format_hits(corpus.lookup(toks, a.k)) or ["    (nothing similar)"]))
        return
    if not a.func:
        ap.error("give a function, --shape or --build")
    name, addr, size = resolve_function(a.func)
    ins = name_calls(disasm_function(load_rom(verify=False), addr, size), addr)
    lo, hi = 1, len(ins)
    if a.lines:
        lo, hi = (int(x) for x in a.lines.split("-"))
    print(f"{name} {addr:08X}: {len(ins)} instructions; '=N' same shape (N matched uses), '~s' nearest")
    for block in basic_blocks(ins):
        for piece in pieces(ins, block):
            if piece[-1] + 1 < lo or piece[0] + 1 > hi:
                continue
            toks = [shape(ins[i][1]) for i in piece]
            print(f"#{piece[0] + 1}-{piece[-1] + 1} {addr + ins[piece[0]][0]:08X}: " +
                  "; ".join(ins[i][1] for i in piece))
            print("\n".join(format_hits(corpus.lookup(toks, a.k)) or ["    (nothing similar)"]))


if __name__ == "__main__":
    main()

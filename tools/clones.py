#!/usr/bin/env python3
"""Find clone families and port matched C to unmatched siblings.

  clones.py --stats                      family statistics
  clones.py --port [--loose] [--range START END]  write candidate C for unmatched clones of
                                         matched functions, check each, keep MATCHes

Two functions are clones when their instructions are identical after symbols
(callees, data labels, literal-pool addresses) are abstracted away. A matched
member's C is copied to each unmatched sibling with every referenced symbol and
address renamed in order of appearance; tools/check.py decides. Candidates are
written to wip/clones/ and copied to src/fn/ only on MATCH.

  clones.py --graft [--loose]            the same for siblings that have a NONMATCHING draft:
                                         put the template's function (renamed) into the draft's
                                         own file, which keeps its own structs and externs (a
                                         sibling often has the same code over another layout,
                                         so field offsets differ), and promote the draft on MATCH
"""
import argparse
import collections
import os
import re
import shutil
import subprocess
import sys

from romlib import ROOT
from worklist import done_names

REGS = {f"r{i}" for i in range(13)} | {"sp", "lr", "pc", "ip", "sl", "sb", "fp"}
TOKEN = re.compile(r"\b(0x[0-9A-Fa-f]{8}|[A-Za-z_]\w*)\b")


LOOSE = False
IMM = re.compile(r"#(-?(?:0x[0-9A-Fa-f]+|\d+))")


def parse():
    """name -> (addr, [normalized lines], [symbol keys in order], [immediates])"""
    funcs = {}
    cur = None
    for ln in open(os.path.join(ROOT, "gen", "code_sym.s")):
        m = re.match(r"^\t(?:thumb|arm|non_word_aligned_thumb)_func_start (\S+)", ln)
        if m:
            cur = [m.group(1), None, [], [], []]
            funcs[cur[0]] = cur
            continue
        if cur is None:
            continue
        s = ln.split("@")[0].strip()
        if not s:
            continue
        m = re.match(r"^(\w+):$", s) or re.match(r"^(\w+):\s*(.*)$", s)
        if m and cur[1] is None and m.group(1) == cur[0]:
            a = re.search(r"0x([0-9A-F]{8})", ln)
            cur[1] = int(a.group(1), 16) if a else None
            continue
        if re.match(r"^_0[89][0-9A-F]{6}:$", s):
            cur[2].append("L:")
            continue
        if s.startswith(".incbin") or s.startswith(".byte"):
            cur[2].append("DATA")  # differing trailing data: not a safe clone
            cur[2].append(s)
            continue
        m = re.match(r"^_0[89][0-9A-F]{6}:\s*(.*)$", s)
        if m:
            s = m.group(1)
        op, _, args = s.partition(" ")

        def sub(mt):
            t = mt.group(1)
            if t in REGS or t.startswith("_0"):
                return "L" if t.startswith("_0") else t
            if t.startswith("0x") and op != ".4byte":
                return t
            cur[3].append(t)
            return "S"
        line = op + " " + TOKEN.sub(sub, args)
        if LOOSE:
            def imm(mt):
                cur[4].append(int(mt.group(1), 0))
                return "#I"
            line = IMM.sub(imm, line)
            if op == ".4byte" and re.fullmatch(r"\.4byte (0x[0-9A-Fa-f]+|\d+)", line.strip()):
                pass
        cur[2].append(line)
    return {n: (f[1], f[2], f[3], f[4]) for n, f in funcs.items() if f[1] is not None}


def key_of(sym):
    """The string to rename in C for a referenced symbol."""
    m = re.match(r"^0x([0-9A-Fa-f]{8})$", sym)
    return m.group(1).upper() if m else sym


def families(funcs):
    fam = collections.defaultdict(list)
    for n, (a, lines, *_) in funcs.items():
        if len(lines) >= 3 and not any(l == "DATA" for l in lines):
            fam["\n".join(lines)].append(n)
    return [v for v in fam.values() if len(v) > 1]


def port(src_c, src_name, dst_name, funcs):
    a_syms = [src_name] + funcs[src_name][2]
    b_syms = [dst_name] + funcs[dst_name][2]
    mapping = {}
    for x, y in zip(a_syms, b_syms):
        kx, ky = key_of(x), key_of(y)
        if mapping.get(kx, ky) != ky:
            return None  # inconsistent renaming
        mapping[kx] = ky
    text = open(src_c).read()
    # rename identifiers and 8-digit hex addresses in one pass
    hexmap = {}
    for k, v in mapping.items():
        for pat in (k, k[-8:] if re.search(r"[0-9A-F]{8}$", k) else None):
            if pat and re.fullmatch(r"[0-9A-F]{8}", pat):
                tgt = v[-8:] if re.search(r"[0-9A-F]{8}$", v) else None
                if tgt:
                    if hexmap.get(pat, tgt) != tgt:
                        return None
                    hexmap[pat] = tgt
    names = {k: v for k, v in mapping.items() if not re.fullmatch(r"[0-9A-F]{8}", k)}
    imap = {}
    if LOOSE:
        for x, y in zip(funcs[src_name][3], funcs[dst_name][3]):
            if x != y:
                if imap.get(x, y) != y:
                    return None
                imap[x] = y
        if any(x in imap.values() and x not in imap for x in imap):
            return None

    def rep(m):
        t = m.group(0)
        if t in names:
            return names[t]
        h = re.search(r"([0-9A-Fa-f]{8})$", t)
        if h and h.group(1).upper() in hexmap:
            return t[:h.start(1)] + hexmap[h.group(1).upper()]
        return t
    out = re.sub(r"\b\w+\b", rep, text)
    if imap:
        # immediates: rewrite numeric literals with the same value (keep base);
        # skip the #include line and hex address-like identifiers
        def num(m):
            t = m.group(0)
            v = int(t, 0)
            if v in imap:
                nv = imap[v]
                return hex(nv).upper().replace("0X", "0x") if t.lower().startswith("0x") else str(nv)
            return t
        out = re.sub(r"(?<![\w.])(0[xX][0-9A-Fa-f]+|[1-9]\d*)(?![\w.])", num, out)
    return out


def function_span(text, name):
    """(start, end) of NAME's definition in TEXT, or None."""
    m = re.search(r"^[A-Za-z_][\w \*]*\b" + re.escape(name) + r"\s*\([^;{]*\)\s*\{", text, re.M)
    if not m:
        return None
    depth, i = 0, m.end() - 1
    while i < len(text):
        depth += {"{": 1, "}": -1}.get(text[i], 0)
        i += 1
        if depth == 0:
            return m.start(), i
    return None


def draft_file(name):
    """The src file whose INCLUDE_ASM fallback is NAME's, and the C name of the draft."""
    pat = re.compile(r'INCLUDE_ASM\("asm/nonmatching", ' + re.escape(name) + r'\)')
    for d, _, fs in os.walk(os.path.join(ROOT, "src")):
        for f in fs:
            if f.endswith(".c"):
                p = os.path.join(d, f)
                text = open(p).read()
                if pat.search(text):
                    alias = re.search(r"^#define (\w+)\s+" + re.escape(name) + r"\s*$", text, re.M)
                    return p, alias.group(1) if alias else name
    return None, None


def graft(funcs, done, fams):
    import tempfile
    ok = 0
    for f in fams:
        tmpl = [n for n in f if n in done]
        for n in f:
            if n in done:
                continue
            path, cname = draft_file(n)
            if not path:
                continue
            text = open(path).read()
            block = re.search(r"#\s*ifdef\s+NONMATCHING\b.*?#\s*else\s*\n\s*INCLUDE_ASM\([^)]*\b" +
                              re.escape(n) + r"\b", text, re.S)
            span = block and function_span(text[block.start():block.end()], cname)
            if not span:
                continue
            a, b = block.start() + span[0], block.start() + span[1]
            for t in tmpl:
                p = os.path.join(ROOT, "src", "fn", f"sub_{funcs[t][0]:08X}.c")
                if not os.path.exists(p):
                    continue
                ported = port(p, t, n, funcs)
                s = ported and function_span(ported, n)
                if not s:
                    continue
                body = ported[s[0]:s[1]]
                if cname != n:
                    body = re.sub(r"\b" + re.escape(n) + r"\b", cname, body)
                cand = text[:a] + body + text[b:]
                fd, tmp = tempfile.mkstemp(suffix=".c", prefix=".graft_", dir=os.path.dirname(path))
                with os.fdopen(fd, "w") as fh:
                    fh.write(cand)
                try:
                    r = subprocess.run([sys.executable, "tools/check.py", tmp, "--nonmatching"], cwd=ROOT,
                                       capture_output=True, text=True)
                finally:
                    os.unlink(tmp)
                out = r.stdout + r.stderr
                m = re.search(r"fuzzy ([\d.]+)%", out)
                hit = re.search(r"^" + re.escape(cname) + r": MATCH|^" + re.escape(n) + r": MATCH", out, re.M)
                print(f"{'MATCH' if hit else '-----'} {n} from {t}: {m.group(1) + '%' if m else 'error'} "
                      f"({os.path.relpath(path, ROOT)})", flush=True)
                if hit:
                    open(path, "w").write(cand)
                    subprocess.run([sys.executable, "tools/promote_draft.py", path, n], cwd=ROOT)
                    ok += 1
                    break
    print(f"{ok} drafts grafted")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--stats", action="store_true")
    ap.add_argument("--port", action="store_true")
    ap.add_argument("--range", nargs=2)
    ap.add_argument("--loose", action="store_true", help="also allow differing immediates")
    ap.add_argument("--graft", action="store_true")
    a = ap.parse_args()
    global LOOSE
    LOOSE = a.loose
    funcs = parse()
    done = done_names()
    fams = families(funcs)
    if a.graft:
        return graft(funcs, done, fams)
    if a.stats or not a.port:
        n_un = sum(1 for f in fams for n in f if n not in done)
        port_ok = sum(1 for f in fams if any(n in done for n in f) for n in f if n not in done)
        print(f"{len(fams)} clone families covering {sum(map(len, fams))} functions; "
              f"{n_un} unmatched members, {port_ok} in families with a matched member")
        return
    lo, hi = (int(a.range[0], 16), int(a.range[1], 16)) if a.range else (0, 1 << 32)
    # one-function units are src/fn/sub_ADDR.c whatever the function is called now
    srcs = {}
    for n, v in funcs.items():
        p = os.path.join(ROOT, "src", "fn", f"sub_{v[0]:08X}.c")
        if os.path.exists(p):
            srcs[n] = p
    wip = os.path.join(ROOT, "wip", "clones")
    os.makedirs(wip, exist_ok=True)
    ok = 0
    for f in fams:
        # only exact matches are templates: a NONMATCHING draft's INCLUDE_ASM
        # fallback would pass check.py trivially
        tmpl = [n for n in f if n in srcs and n in done]
        if not tmpl:
            continue
        for n in f:
            if n in done or not (lo <= funcs[n][0] < hi):
                continue
            for t in tmpl:
                c = port(srcs[t], t, n, funcs)
                if c is None:
                    continue
                out = os.path.join(wip, f"sub_{funcs[n][0]:08X}.c")
                open(out, "w").write(c)
                r = subprocess.run([sys.executable, "tools/check.py", out, "--quiet"], cwd=ROOT,
                                   capture_output=True, text=True)
                if r.returncode == 0:
                    dst = os.path.join(ROOT, "src", "fn", f"sub_{funcs[n][0]:08X}.c")
                    if not os.path.exists(dst):
                        print(f"MATCH {n} (from {t}) in {out}: its unit is not src/fn/sub_ADDR.c, merge by hand")
                        continue
                    shutil.copy(out, dst)
                    ok += 1
                    print(f"MATCH {n} (from {t})")
                    break
    print(f"{ok} functions ported")


if __name__ == "__main__":
    main()

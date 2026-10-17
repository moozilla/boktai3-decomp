#!/usr/bin/env python3
"""A readable, Python-style view of the GCL scripts.

  gclview.py PROC...          procedures by call id (the ids calls use)

gcx.py prints GCL close to Konami's source syntax. This view is for reading:
the same bytecode, decoded into nodes (statements, values, expressions), then
rendered with English names and named arguments:

    trap(area=arg1, who=Player, event=ANY, names=[block_name]):
        run:
            if arg3 == 0xdd2:
                print("Crest on!!!!! $: P_block name", block_name)
                SceneObjects_SwitchPalette(id=block_name, mode=1, sound=0)
    spawn OrbitingParticles(name=0x355f, pos=(x=2688, y=512, z=2432), radius=128, duration=15, type=4)

Names come from gcl/names (registry keys, variables, parameters), the option
schemas of tools/assets/gcloptions.py, and gcl/prints.csv; anything unknown
stays raw (`r708[0x70]`, `d=128`, `chara_99CA`). It is a view, not source:
gcx.py stays the form that maps one-to-one onto the bytes.
"""
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.dirname(HERE))
import gclnames  # noqa: E402
import gcx  # noqa: E402

# ---------- decoding into nodes ----------
# values: ("int", v, hex?) ("proc", call id) ("str", s) ("text", id) ("var", region, off, bit, t, a, i) ("arg", n)
#   ("local", n) ("expr", node) ("block", stmts) ("end",) ("raw", text)
# expression nodes: values, ("bin", op, l, r), ("un", op, x)
# statements: ("cmd", name, cid, args, opts) with opts [(letter, [values])],
#   ("call", pid, args), ("eval", node), ("raw", text)


def value(b, p):
    op = b[p]
    fam = op & 0xF0
    if (fam & 0xC0) == 0xC0:
        return ("int", (op & 0x3F) - 1, False), p + 1
    if fam == 0:
        if op == 0:
            return ("end",), p + 1
        if op == 8:  # a procedure reference: callbacks such as on_press=, then_proc=
            return ("proc", b[p + 1] | b[p + 2] << 8), p + 3
        if op == 6:
            return ("int", b[p + 1] | b[p + 2] << 8, True), p + 3
        if op == 1:
            v = b[p + 1] | b[p + 2] << 8
            return ("int", v - 0x10000 if v & 0x8000 else v, False), p + 3
        if op in (9, 10, 13):
            return ("int", struct.unpack_from("<I", b, p + 1)[0], True), p + 5
        if op in (2, 3, 4):
            return ("int", b[p + 1], False), p + 2
        if op == 7:
            n = b[p + 1]
            return ("str", bytes(b[p + 2:p + 2 + n]).split(b"\0")[0].decode("euc_jp", "backslashreplace")), p + 2 + n
        if op == 14:
            return ("text", b[p + 1] | b[p + 2] << 8), p + 3
        return ("raw", f"?{op:02x}"), p + 1
    if fam in (0x10, 0x20):
        d = b[p] << 24 | b[p + 1] << 16 | b[p + 2] << 8 | b[p + 3]
        region = {0x800000: "gs", 0x100000: "r700"}.get(d & 0xF00000, "r708")
        t = (d >> 24) & 15
        bit = (d >> 16) & 15 if t == 4 else None
        q, a, i = p + 4, None, None
        if fam == 0x20:
            a, q = value(b, q)
            i, q = value(b, q)
        return ("var", region, d & 0xFFFF, bit, t, a, i), q
    if fam == 0x40:
        if op & 15 == 15:
            return ("arg", b[p + 1] + 15), p + 2
        return ("arg", op & 15), p + 1
    if fam == 0x90:
        return ("local", op & 15), p + 1
    start, n = gcx.length(b, p)
    if fam == 0x30:
        return ("expr", expression(b, start, start + n)), start + n
    if fam == 0x80:
        return ("block", block(b, start, start + n)), start + n
    return ("raw", f"?{op:02x}"), start + n


def expression(b, p, end):
    stack = []
    while p < end:
        op = b[p]
        if op & 0xE0 == 0xA0:
            o = op & 0x1F
            if o == 0:
                break
            r = stack.pop() if stack else ("raw", "?")
            if o in gcx.UNARY:
                stack.append(("un", o, r))
            else:
                left = stack.pop() if stack else ("raw", "?")
                stack.append(("bin", o, left, r))
            p += 1
            continue
        v, p = value(b, p)
        stack.append(v)
    return stack[0] if len(stack) == 1 else ("raw", " ".join(map(str, stack)))


def values(b, p, end):
    out = []
    while p < end:
        v, p = value(b, p)
        if v[0] == "end":
            break
        out.append(v)
    return out


def block(b, p, end):
    out = []
    while p < end:
        op = b[p]
        if op == 0:
            break
        start, n = gcx.length(b, p)
        kind = op >> 4
        if kind == 6:
            cid = b[start] | b[start + 1] << 8
            q, alen = gcx.arglist(b, start + 2)
            args = values(b, q, q + alen)
            q += alen
            opts = []
            while q < start + n and b[q] & 0xF0 == 0x50:
                s2, n2 = gcx.length(b, q)
                opts.append((chr(b[s2]), values(b, s2 + 1, s2 + n2)))
                q = s2 + n2
            out.append(("cmd", gcx.COMMANDS.get(cid, f"cmd_{cid:04X}"), cid, args, opts))
        elif kind == 7:
            out.append(("call", b[start] | b[start + 1] << 8, values(b, start + 2, start + n)))
        elif kind == 3:
            out.append(("eval", expression(b, start, start + n)))
        else:
            out.append(("raw", f".byte {bytes(b[p:start + n]).hex()}"))
        p = start + n
    return out


def procedure(img, cid):
    a = img.proc_address(cid - 1)
    if a is None:
        return None
    start, n = gcx.length(img.rom, a)
    return block(img.rom, start, start + n)


# ---------- names ----------
def ident(english):
    """'st02_27_candle shutter flag' -> st02_27_candle_shutter_flag."""
    s = re.sub(r"[^0-9A-Za-z]+", "_", english.lower()).strip("_")
    return ("_" + s) if s[:1].isdigit() else s


def pascal(english):
    """'Fan Cannon Manager' -> FanCannonManager."""
    return "".join(w[:1].upper() + w[1:] for w in re.split(r"[^0-9A-Za-z]+", english) if w)


SUFFIX = re.compile(r"_?(CreateFromScript|ScriptCreate|FromScript|Create|Script)$")


class Names:
    def __init__(self):
        import gcloptions
        self.keys = gclnames.best("chara")
        self.cbs = gcx.callback_names()
        self.reg = gcloptions.registry_schemas()
        self.cmd = gcloptions.command_schemas()
        self.vars = gclnames.var_names()
        self.params = gclnames.param_names()
        self.prints = gclnames.prints()
        self.texts = gcx.text_excerpts()
        self.procs = {}
        import csv
        try:
            with open(os.path.join(gcx.ROOT, "gcl", "names", "proc.csv"), encoding="utf-8") as f:
                self.procs = {int(r["proc"]): r for r in csv.DictReader(f)}
        except OSError:
            pass
        self.data = set()  # text-bank ids that are data records, not text
        try:
            for ln in open(os.path.join(gcx.ROOT, "build", "text", "bank.tsv"), encoding="utf-8"):
                p = ln.split("\t", 2)
                if len(p) > 1 and p[1] == "B" and p[0].isdigit():
                    self.data.add(int(p[0]))
        except OSError:
            pass
        self.labels = {}
        try:
            with open(os.path.join(gcx.ROOT, "data", "notes.csv"), encoding="utf-8") as f:
                for r in csv.DictReader(f):
                    if r["label"]:
                        self.labels.setdefault(r["kind"], {})[r["key"].upper()] = r["label"]
        except OSError:
            pass

    def callee(self, key):
        """(callable name, comment) for a registry key."""
        r = self.keys.get(key)
        if r and r["english"] and r["confidence"] in ("high", "medium"):
            return pascal(r["english"].rstrip("?")), ""
        cb = self.cbs.get(key, "")
        note = f"{r['name']}? ({r['english']})" if r else ""
        if cb and not cb.startswith("sub_"):
            return SUFFIX.sub("", cb) or cb, note
        return cb or f"key_{key:04X}", note

    def option(self, owner, letter):
        o = (owner or {}).get(letter)
        if o and (o["english"] or o["name"]):
            return ident(o["english"] or o["name"]), o
        return letter, o


ANY = {0x14C9: "ANY", 0x1516: "ALL"}  # trap events: the hashes of '？' and '＊'
PY_OPS = {20: "or", 21: "and", 22: "="}
PY_UNARY = {1: "-", 2: "not ", 3: "~"}


# {text id: English} for comments on text references when a caller asks for
# English (the Studio sets it from the fan translation); Japanese otherwise.
ENGLISH = {}


class Readable:
    def __init__(self, names, cid, inline=None, stack=(), english=False):
        """inline(pid) -> True expands a call statement to pid in place."""
        self.n, self.cid, self.lines = names, cid, []
        self.inline_ok, self.stack, self.english = inline, stack + (cid,), english

    # values
    def val(self, v, top=True):
        k = v[0]
        if k == "arg" and self.n.procs.get(self.cid):  # this procedure's own named parameters
            ps = self.n.procs[self.cid]["params"].split(",")
            if 0 < v[1] <= len(ps) and ps[v[1] - 1] and not self.n.params.get((self.cid, f"arg{v[1]}")):
                return ps[v[1] - 1]
        if k == "int":
            return f"{v[1]:#x}" if v[2] and v[1] > 9 else str(v[1])
        if k == "proc":
            r = self.n.procs.get(v[1])
            return r["name"] if r else f"proc_{v[1]}"
        if k == "str":
            return '"' + v[1].replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n") + '"'
        if k == "text":
            return f"data({v[1]})" if v[1] in self.n.data else f"text({v[1]})"
        if k == "arg":
            r = self.n.params.get((self.cid, f"arg{v[1]}"))
            return ident(r["english"]) if r and r["english"] else f"arg{v[1]}"
        if k == "local":
            r = self.n.params.get((self.cid, f"local{v[1]}"))
            return ident(r["english"]) if r and r["english"] else f"local{v[1]}"
        if k == "var":
            return self.var(v)
        if k == "expr":
            return self.expr(v[1], top)
        if k == "block":  # a block used as a value: its last statement's result
            parts = [self.inline(s) for s in v[1]]
            return parts[0] if len(parts) == 1 else "(" + "; ".join(parts) + ")"
        if k in ("bin", "un"):
            return self.expr(v, top)
        return v[1] if len(v) > 1 else "?"

    def var(self, v):
        _, region, off, bit, t, a, i = v
        key = f"{region}:{off:#x}" + (f".{bit}" if bit is not None else "") + f":t{t}"
        stride = None
        if a is not None:
            stride = self.val(a)
            key += f"({stride})"
        r = self.n.vars.get(key)
        name = ident(r["english"]) if r and r["english"] else f"{region}[{off:#x}" + (f".{bit}" if bit is not None else "") + "]"
        return name + (f"[{self.val(i)}]" if i is not None else "")

    def expr(self, e, top=True):
        if e[0] == "bin":
            o = e[1]
            op = PY_OPS.get(o) or gcx.OPS.get(o, f"op{o}")
            s = f"{self.expr(e[2], False)} {op} {self.expr(e[3], o == 22)}"
            return s if top or o == 22 else f"({s})"
        if e[0] == "un":
            return PY_UNARY.get(e[1], "?") + self.expr(e[2], False)
        return self.val(e, top)

    # statements
    def emit(self, depth, text, note=""):
        self.lines.append("    " * depth + text + (f"  # {note}" if note else ""))

    RESOURCE = {"map": "map", "tileset": "tileset", "hp": "hp", "palette": "pal"}

    def call(self, s):
        """(text, note) of a procedure call, named from gcl/names/proc.csv."""
        _, pid, args = s
        r = self.n.procs.get(pid)
        if not r:
            return f"proc_{pid}({', '.join(self.val(a) for a in args)})", ""
        params, parts, notes = r["params"].split(","), [], []
        for i, a in enumerate(args):
            p = params[i] if i < len(params) else ""
            parts.append(f"{p}={self.val(a)}" if p else self.val(a))
            kind = self.RESOURCE.get(p)
            if kind and a[0] == "int" and self.n.labels.get(kind, {}).get(f"{a[1]:04X}"):
                notes.append(f"{p}: {self.n.labels[kind][f'{a[1]:04X}']}")
        return f"{r['name']}({', '.join(parts)})", "; ".join(notes)

    def inline(self, s):
        """A statement used as a value (inside a block operand)."""
        start = len(self.lines)
        self.stmt(s, 0)
        out = self.lines[start:]
        del self.lines[start:]
        return " ".join(x.split("  # ")[0].strip() for x in out) if out else "?"

    def call_args(self, owner, args, opts, positional=()):
        """'a, b, x=1, pos=(x=.., y=.., z=..)' and the block options [(name, stmts)]."""
        parts = [f"{n}={self.val(a)}" if n else self.val(a) for n, a in zip(list(positional) + [None] * len(args), args)]
        blocks, taken = [], {}
        for letter, _ in opts:
            taken[self.n.option(owner, letter)[0]] = taken.get(self.n.option(owner, letter)[0], 0) + 1
        for letter, vs in opts:
            name, o = self.n.option(owner, letter)
            if taken[name] > 1 and name != letter:  # two letters with one generic name
                name = f"{name}_{letter}"
            if len(vs) == 1 and vs[0][0] == "block":
                blocks.append((name, vs[0][1]))
                continue
            if len(vs) == 3 and "pos" in name:
                parts.append(f"{name}=(x={self.val(vs[0])}, y={self.val(vs[1])}, z={self.val(vs[2])})")
            elif name in ("names", "ids", "scripts"):
                parts.append(f"{name}=[{', '.join(self.val(x) for x in vs)}]")
            elif len(vs) == 1:
                parts.append(f"{name}={self.val(vs[0])}")
            elif not vs:
                parts.append(f"{name}=True")
            elif o and o["kind"] == "values" and o["values"] <= 1 and len(vs) > 1 or name in ("names", "ids", "scripts"):
                parts.append(f"{name}=[{', '.join(self.val(x) for x in vs)}]")
            else:
                parts.append(f"{name}=({', '.join(self.val(x) for x in vs)})")
        return ", ".join(parts), blocks

    def blocks(self, depth, head, blocks, note=""):
        if not blocks:
            self.emit(depth, head, note)
            return
        self.emit(depth, head + ":", note)
        for name, body in blocks:
            self.emit(depth + 1, f"{name}:")
            self.body(body, depth + 2)

    def body(self, stmts, depth):
        if not stmts:
            self.emit(depth, "pass")
        for s in stmts:
            self.stmt(s, depth)

    def stmt(self, s, depth):
        k = s[0]
        if k == "eval":
            self.emit(depth, self.expr(s[1]))
        elif k == "call":
            if self.inline_ok and s[1] not in self.stack and self.inline_ok(s[1]):
                text, note = self.call(s)
                self.emit(depth, f"# inlined: {text}", note)
                sub = Readable(self.n, s[1], self.inline_ok, self.stack, english=self.english)
                sub.body(procedure(_image(), s[1]) or [], depth)
                self.lines += sub.lines
                return
            self.emit(depth, *self.call(s))
        elif k == "raw":
            self.emit(depth, s[1])
        else:
            self.command(s, depth)

    def command(self, s, depth):
        _, name, cid, args, opts = s
        if name == "if":
            self.emit(depth, f"if {self.val(args[0])}:")
            self.body(args[1][1] if len(args) > 1 and args[1][0] == "block" else [], depth + 1)
            for letter, vs in opts:
                blk = [v for v in vs if v[0] == "block"]
                if letter == "i":
                    self.emit(depth, f"elif {self.val(vs[0])}:")
                elif letter == "e":
                    self.emit(depth, "else:")
                self.body(blk[0][1] if blk else [], depth + 1)
            return
        if name == "switch":
            self.emit(depth, f"match {self.val(args[0])}:")
            for letter, vs in opts:
                blk = [v for v in vs if v[0] == "block"]
                self.emit(depth + 1, f"case {self.val(vs[0])}:" if letter == "c" else "case _:")
                self.body(blk[0][1] if blk else [], depth + 2)
            return
        if name == "return":
            self.emit(depth, "return" + (f" {self.val(args[0])}" if args else ""))
            return
        if name == "eval":
            self.emit(depth, ", ".join(self.val(a) for a in args))
            return
        if name == "print":  # does nothing in this ROM: shown as a comment
            parts = []
            for a in args:
                if a[0] == "str":
                    esc = a[1].replace("\\", "\\\\").replace("'", "\\'").replace("\n", "\\n")
                    t = self.n.prints.get(esc) or a[1]  # gcl/prints.csv keys are gcx.py's escaped text
                    t = re.sub(r"\$+[a-z]*:\s*\S.*$", "", t)  # a variable label: the value is named below
                    t = re.sub(r"([■◆●▼△▽★＊*=!！])\1+", r"\1", t).strip(" 　■◆●▼△▽★＊*=:：")
                    if t:
                        parts.append(t)
                else:
                    parts.append("{" + self.val(a) + "}")
            self.emit(depth, "# log: " + " ".join(parts) if parts else "# log")
            return
        if name in ("chara", "command") and args and args[0][0] == "int":
            key = args[0][1]
            callee, note = self.n.callee(key)
            owner = self.n.reg.get(key)
            pos = ["name"] if name == "chara" else []
            text, blocks = self.call_args(owner, args[1:], opts, pos)
            head = ("spawn " if name == "chara" else "") + f"{callee}({text})"
            self.blocks(depth, head, blocks, note + self.text_note(args, opts))
            return
        owner = self.n.cmd.get(name)
        pos = {"trap": ["area", "who"], "load": ["scene"], "call": ["proc"]}.get(name, [])
        args = list(args)
        if name == "trap" and len(args) > 1 and args[1][0] == "int":
            args[1] = ("raw", ANY.get(args[1][1]) or self.n.callee(args[1][1])[0])
        if name == "trap":
            opts = [(lt, [("raw", ANY[v[1]]) if v[0] == "int" and v[1] in ANY else v for v in vs]) for lt, vs in opts]
        text, blocks = self.call_args(owner, args, opts, pos)
        self.blocks(depth, f"{name}({text})", blocks, self.text_note(args, opts))

    def text_note(self, args, opts):
        ids = [v[1] for v in list(args) + [x for _, vs in opts for x in vs] if v[0] == "text"]
        tx = lambda i: re.sub(r"\s*\n\s*", " ", (ENGLISH.get(i) if self.english else None) or self.n.texts.get(i) or "")
        return "; ".join(f'"{tx(i)}"' for i in ids if tx(i))


_names = None


_img = None


def _image():
    global _img
    with gcx._lock:
        if _img is None:
            _img = gcx.Image(gcx.load_rom())
    return _img


def names():
    global _names
    with gcx._lock:
        if _names is None:
            _names = Names()
    return _names


def render(img, cid, inline=None, english=False):
    """Readable lines of a procedure (by call id), or None. inline(pid) -> True
    expands call statements to pid in place (see gclfiles.py)."""
    global _names
    stmts = procedure(img, cid)
    if stmts is None:
        return None
    with gcx._lock:
        if _names is None:
            _names = Names()
    r = Readable(_names, cid, inline, english=english)
    r.body(stmts, 0)
    return r.lines


def callee_names():
    """{readable callable name: registry key hex} for linking."""
    global _names
    with gcx._lock:
        if _names is None:
            _names = Names()
    return {_names.callee(k)[0]: f"{k:04X}" for k in _names.cbs}


def main():
    img = gcx.Image(gcx.load_rom())
    for a in sys.argv[1:] or sys.exit(__doc__):
        lines = render(img, int(a, 0))
        print(f"# proc {a}")
        for ln in lines or ["pass"]:
            print(ln)


if __name__ == "__main__":
    main()

"""A small GCL assembler: build procedure bytecode from Python values.

The inverse of tools/assets/gcx.py for the forms the showcase needs. An item
is a kind nibble and a length (low nibble 0-12, or 13/14/15 followed by a
1/2/3-byte length); a procedure is a block (kind 8) of statements:

    Cmd(name or id, args, kw)   kind 6: command id, argument list, keyword items
    Call(proc, *args)           kind 7: procedure id, operands, <end>
    Set(var, expr)              kind 3: an assignment expression

Operands: Python ints (encoded as small, s16 or u32 as needed), Hash(x)
(type 6), Proc(x) (type 8, a procedure reference), Text(n), Var(raw32),
Arg(n), Local(n), Expr(...), Block([...]). Keyword items are (letter, [operands]).
"""
import struct

CMD = {"if": 0x0D86, "call": 0x121F, "mesg": 0x22FF, "switch": 0x4A6F, "eval": 0x64C0,
       "chara": 0x9906, "command": 0xB745, "load": 0xC8BB, "return": 0xCD3A}
OPS = {"+": 4, "-": 5, "*": 6, "/": 7, "%": 8, "<<": 9, ">>": 10, "==": 11, "!=": 12, "<": 13,
       "<=": 14, ">": 15, ">=": 16, "|": 17, "&": 18, "^": 19, "||": 20, "&&": 21, "=": 22}


def item(kind, payload):
    n = len(payload)
    if n <= 12:
        head = bytes([kind << 4 | n])
    elif n < 0x100:
        head = bytes([kind << 4 | 13, n])
    elif n < 0x10000:
        head = bytes([kind << 4 | 14, n & 0xFF, n >> 8])
    else:
        head = bytes([kind << 4 | 15, n & 0xFF, n >> 8 & 0xFF, n >> 16])
    return head + payload


class Hash:
    def __init__(self, v):
        self.v = v

    def enc(self):
        return bytes([6]) + struct.pack("<H", self.v)


class Proc:
    def __init__(self, v):
        self.v = v

    def enc(self):
        return bytes([8]) + struct.pack("<H", self.v)


class Text:
    def __init__(self, v):
        self.v = v

    def enc(self):
        return bytes([14]) + struct.pack("<H", self.v)


class Var:
    """A variable as gcx.py prints it: raw 32-bit form, e.g. r700[0x119]:t2 = 0x12100119."""
    def __init__(self, raw):
        self.raw = raw

    def enc(self):
        return struct.pack(">I", self.raw)


def r700(off, t=2):
    return Var(0x10000000 | t << 24 | 0x100000 | off)


def r708(off, t=1):
    return Var(0x10000000 | t << 24 | off)


class Arg:
    def __init__(self, n):
        self.n = n

    def enc(self):
        return bytes([0x40 | self.n]) if self.n < 15 else bytes([0x4F, self.n - 15])


class Local:
    def __init__(self, n):
        self.n = n

    def enc(self):
        return bytes([0x90 | self.n])


class Expr:
    """Postfix: Expr(a, b, "+") or nested; operands and operator strings."""
    def __init__(self, *parts):
        self.parts = parts

    def body(self):
        out = b""
        for p in self.parts:
            if isinstance(p, str):
                out += bytes([0xA0 | OPS[p]])
            elif isinstance(p, Expr):
                out += p.body()
            else:
                out += operand(p)
        return out

    def enc(self):
        return item(3, self.body() + b"\xa0")


class Block:
    def __init__(self, stmts):
        self.stmts = stmts

    def enc(self):
        return item(8, b"".join(s.enc() for s in self.stmts) + b"\0")


def operand(v):
    if isinstance(v, int):
        if -1 <= v <= 62:
            return bytes([0xC0 | (v + 1)])
        if -0x8000 <= v < 0x8000:
            return bytes([1]) + struct.pack("<h", v)
        return bytes([9]) + struct.pack("<I", v & 0xFFFFFFFF)
    return v.enc()


class Cmd:
    def __init__(self, name, args=(), kw=()):
        self.cid = CMD[name] if isinstance(name, str) else name
        self.args, self.kw = list(args), list(kw)

    def enc(self):
        a = b"".join(operand(x) for x in self.args)
        alen = bytes([len(a)]) if len(a) < 0x80 else struct.pack(">H", 0x8000 | len(a))
        body = struct.pack("<H", self.cid) + alen + a
        for letter, vals in self.kw:
            body += item(5, letter.encode() + b"".join(operand(x) for x in vals))
        return item(6, body + b"\0")


class Call:
    def __init__(self, proc, *args):
        self.proc, self.args = proc, args

    def enc(self):
        return item(7, struct.pack("<H", self.proc) + b"".join(operand(x) for x in self.args) + b"\0")


class Set:
    def __init__(self, var, *expr):
        self.var, self.expr = var, expr

    def enc(self):
        return Expr(self.var, *self.expr, "=").enc()


def command(key, **kw):
    """`command KEY -x ...`: a registry routine with keyword options."""
    return Cmd("command", [Hash(key)], [(k, v if isinstance(v, (list, tuple)) else [v]) for k, v in kw.items()])


def chara(typ, name, **kw):
    """chara TYPE NAME ...: NAME is a hash, or any operand (an argument)."""
    return Cmd("chara", [Hash(typ), Hash(name) if isinstance(name, int) else name], [(k, v if isinstance(v, (list, tuple)) else [v]) for k, v in kw.items()])


def switch(var, cases):
    """switch (var) { case value { ... } ... }: cases is [(value, [stmts])]."""
    return Cmd("switch", [Expr(var)], [("c", [v, Block(s)]) for v, s in cases])


def proc(stmts):
    return Block(stmts).enc()


class If:
    """if (cond) { then } [else { other }]: the condition and the block form the
    argument list; else is keyword e."""
    def __init__(self, cond, then, other=None):
        self.cond, self.then, self.other = cond, then, other

    def enc(self):
        a = operand(self.cond) + Block(self.then).enc()
        alen = bytes([len(a)]) if len(a) < 0x80 else struct.pack(">H", 0x8000 | len(a))
        body = struct.pack("<H", CMD["if"]) + alen + a
        if self.other is not None:
            body += item(5, b"e" + Block(self.other).enc())
        return item(6, body + b"\0")

"""Semantics-preserving C rewrites that move agbcc's code generation.

Each rewrite is a generator: rw_X(fn) yields (description, new FuncDef) for
every site where it applies, without touching fn. They are the edits that
fixed real near misses (docs/MODEL_BENCHMARK.md): reuse a stored value, derive
a constant from another, copy a variable, a temporary for a value, goto vs
early return, a void return, a scoped constant, a local's width, operand order,
statement order, if/else order.
"""
import copy
import itertools

from perm_pycparser import c_ast as ca  # the permuter's pycparser (tools/permute.py puts it on sys.path)
from perm_pycparser import c_generator

_gen = c_generator.CGenerator()
INT_TYPES = ["u8", "s8", "u16", "s16", "u32", "s32"]


def text(node):
    return _gen.visit(node)


def walk(node, parent=None, slot=None):
    """Preorder (node, parent, slot) where slot is the child name."""
    yield node, parent, slot
    for name, child in node.children():
        yield from walk(child, node, name)


def set_child(parent, slot, new):
    if "[" in slot:
        attr, i = slot[:-1].split("[")
        getattr(parent, attr)[int(i)] = new
    else:
        setattr(parent, slot, new)


def nth(fn, pred, k):
    """The k-th (node, parent, slot) satisfying pred, in preorder."""
    return next(itertools.islice(((n, p, s) for n, p, s in walk(fn) if pred(n, p, s)), k, None))


def sites(fn, pred):
    return sum(1 for n, p, s in walk(fn) if pred(n, p, s))


def new_name(fn, base="t"):
    names = {n.name for n, _, _ in walk(fn) if isinstance(n, (ca.ID, ca.Decl)) and getattr(n, "name", None)}
    for i in itertools.count(1):
        if f"{base}{i}" not in names:
            return f"{base}{i}"


def _decl(name, tp, init):
    return ca.Decl(name=name, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[], type=tp,
                   init=init, bitsize=None, asmlabel=None)


def typeof_decl(name, expr, init=None):
    return _decl(name, ca.TypeDecl(name, [], None, ca.Typeof(copy.deepcopy(expr))), init)


def int_decl(name, tname, init=None):
    return _decl(name, ca.TypeDecl(name, [], None, ca.IdentifierType([tname])), init)


def add_local(fn, decl):
    items = fn.body.block_items or []
    i = 0
    while i < len(items) and isinstance(items[i], ca.Decl):
        i += 1
    items.insert(i, decl)
    fn.body.block_items = items


def blocks(fn):
    """Every Compound in fn."""
    return [n for n, _, _ in walk(fn) if isinstance(n, ca.Compound) and n.block_items]


TEMP = __import__("re").compile(r"^[tck]\d+$")


def is_temp(node):
    return isinstance(node, ca.ID) and bool(TEMP.match(node.name))


def is_stmt_assign(n, p, s):
    """A statement-level `L = E`, not one a rewrite produced (temporaries,
    chained assignments): rewrites do not stack on their own output."""
    return (isinstance(n, ca.Assignment) and n.op == "=" and isinstance(p, ca.Compound)
            and not is_temp(n.lvalue) and not isinstance(n.rvalue, ca.Assignment) and not is_temp(n.rvalue))


def reads_of(node, want):
    """(node, parent, slot) of subexpressions whose text is WANT, not as an
    assignment target."""
    out = []
    for n, p, s in walk(node):
        if isinstance(p, ca.Assignment) and s == "lvalue" or isinstance(p, ca.StructRef) and s == "field":
            continue
        if isinstance(n, (ca.ID, ca.UnaryOp, ca.StructRef, ca.ArrayRef, ca.Cast)) and text(n) == want:
            out.append((n, p, s))
    return out


def following(fn, stmt):
    """Statements after STMT in its enclosing block (and in enclosing blocks)."""
    out = []
    for b in blocks(fn):
        if stmt in b.block_items:
            out = b.block_items[b.block_items.index(stmt) + 1:]
    return out


def straight_line_reads(fn, stmt, want):
    """Reads before a write, call or control-flow boundary can invalidate a snapshot.

    In particular a for condition is not a read of the value entering the loop:
    the backedge can change it even when the condition precedes the step in AST order.
    """
    out = []
    for st in following(fn, stmt):
        if any(isinstance(n, (ca.For, ca.While, ca.DoWhile, ca.If, ca.Switch,
                              ca.Compound, ca.Label, ca.Goto, ca.FuncCall)) for n, _, _ in walk(st)):
            break
        if any(isinstance(n, ca.Assignment) and text(n.lvalue) == want
               or isinstance(n, ca.UnaryOp) and n.op in ("++", "--", "p++", "p--", "&")
               and text(n.expr) == want for n, _, _ in walk(st)):
            break
        out.extend(reads_of(st, want))
    return out


# --- rewrites ------------------------------------------------------------------------

def rw_reuse_stored(fn):
    """L = E; ... L ...  ->  L = t = E; ... t ...  (the ROM reuses the register)."""
    n_sites = sites(fn, is_stmt_assign)
    for k in range(n_sites):
        node, _, _ = nth(fn, is_stmt_assign, k)
        lv = text(node.lvalue)
        later = straight_line_reads(fn, node, lv)
        if not later:
            continue
        for take in ("first", "all"):
            f2 = copy.deepcopy(fn)
            a, _, _ = nth(f2, is_stmt_assign, k)
            t = new_name(f2)
            add_local(f2, typeof_decl(t, a.lvalue))
            a.rvalue = ca.Assignment("=", ca.ID(t), a.rvalue)
            reads = straight_line_reads(f2, a, lv)
            for r in reads[:1] if take == "first" else reads:
                set_child(r[1], r[2], ca.ID(t))
            yield f"reuse {lv} via {t} ({take})", f2


def _made_by_rewrite(p, s):
    """A constant a rewrite already routed through a temporary (t1 = K, t1 + d)."""
    if isinstance(p, ca.Assignment) and s == "rvalue" and isinstance(p.lvalue, ca.ID) and p.lvalue.name[:1] in "tk":
        return True
    return isinstance(p, ca.BinaryOp) and isinstance(p.left, ca.ID) and p.left.name[:1] in "tk" and s == "right"


def _consts(fn):
    return [(n, p, s) for n, p, s in walk(fn) if isinstance(n, ca.Constant) and n.type == "int"
            and not _made_by_rewrite(p, s)]


def _cval(c):
    v = c.value.rstrip("uUlL")
    try:
        return int(v, 0)
    except ValueError:
        return None


def rw_related_const(fn):
    """... = K1 ...; f(K2)  ->  ... = (t = K1) ...; f(t + (K2 - K1))."""
    cs = _consts(fn)
    vals = [_cval(c) for c, _, _ in cs]
    # only values that are stored or passed (an assignment's value or a call argument)
    passed = [isinstance(p, ca.ExprList) or (isinstance(p, ca.Assignment) and s == "rvalue") for _, p, s in cs]
    for i, j in itertools.permutations(range(len(cs)), 2):
        if i > j or vals[i] is None or vals[j] is None or not (passed[i] and passed[j]):
            continue
        d = vals[j] - vals[i]
        if vals[i] < 8 or abs(vals[j]) < 8 and d != 0 or abs(d) > 255:
            continue
        f2 = copy.deepcopy(fn)
        c2 = _consts(f2)
        t = new_name(f2)
        add_local(f2, int_decl(t, "u32"))
        ci, pi, si = c2[i]
        cj, pj, sj = c2[j]
        set_child(pi, si, ca.Assignment("=", ca.ID(t), ci))
        if d == 0:
            set_child(pj, sj, ca.ID(t))
        else:
            set_child(pj, sj, ca.BinaryOp("+" if d > 0 else "-", ca.ID(t), ca.Constant("int", str(abs(d)))))
        yield f"constant {vals[j]} from {vals[i]} via {t}", f2


def _local_names(fn):
    names = []
    for n, _, _ in walk(fn.body):
        if isinstance(n, ca.Decl) and isinstance(n.type, (ca.TypeDecl, ca.PtrDecl)):
            names.append(n.name)
    if isinstance(fn.decl.type, ca.FuncDecl) and fn.decl.type.args:
        for p in fn.decl.type.args.params:
            if isinstance(p, ca.Decl) and p.name:
                names.append(p.name)
    return names


def rw_copy_var(fn):
    """v = E; S1; S2  ->  v = E; w = v; ... (a second variable for the same value:
    one register keeps it, another is the working copy). The copy replaces v in
    each contiguous range of the later statements that read it."""
    names = set(_local_names(fn))
    pred = lambda n, p, s: is_stmt_assign(n, p, s) and isinstance(n.lvalue, ca.ID) and n.lvalue.name in names
    for k in range(sites(fn, pred)):
        node, par0, _ = nth(fn, pred, k)
        v = node.lvalue.name
        fol = following(fn, node)
        users = [i for i, st in enumerate(fol) if reads_of(st, v)]
        ranges = [(a, b) for a in range(len(users)) for b in range(a, len(users))][:12]
        for a_, b_ in ranges:
            f2 = copy.deepcopy(fn)
            a, par, slot = nth(f2, pred, k)
            w = new_name(f2, "c")
            add_local(f2, typeof_decl(w, ca.ID(v)))
            fol2 = following(f2, a)
            par.block_items.insert(par.block_items.index(a) + 1, ca.Assignment("=", ca.ID(w), ca.ID(v)))
            for i in users[a_:b_ + 1]:
                for r in reads_of(fol2[i], v):
                    set_child(r[1], r[2], ca.ID(w))
            yield f"copy {v} to {w} for uses {a_ + 1}-{b_ + 1}", f2


def rw_temp_rvalue(fn):
    """L = E  ->  t = E; L = t  (E computed into its own variable first)."""
    pred = lambda n, p, s: is_stmt_assign(n, p, s) and not isinstance(n.rvalue, (ca.ID, ca.Constant))
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        a, par, _ = nth(f2, pred, k)
        t = new_name(f2)
        add_local(f2, typeof_decl(t, a.rvalue))
        par.block_items.insert(par.block_items.index(a), ca.Assignment("=", ca.ID(t), a.rvalue))
        a.rvalue = ca.ID(t)
        yield f"temporary for {text(a.lvalue)}", f2


def rw_scoped_const(fn):
    """L = K  ->  { T v = K; L = v; }"""
    pred = lambda n, p, s: is_stmt_assign(n, p, s) and isinstance(n.rvalue, ca.Constant)
    for k in range(sites(fn, pred)):
        for tname in ("u32", "s32", None):
            f2 = copy.deepcopy(fn)
            a, par, _ = nth(f2, pred, k)
            v = new_name(f2, "k")
            d = int_decl(v, tname, a.rvalue) if tname else typeof_decl(v, a.lvalue, a.rvalue)
            blk = ca.Compound([d, ca.Assignment("=", a.lvalue, ca.ID(v))])
            par.block_items[par.block_items.index(a)] = blk
            yield f"scoped {tname or 'typeof'} constant for {text(a.lvalue)}", f2


def _returns_in_ifs(fn):
    def pred(n, p, s):
        if not isinstance(n, ca.If) or n.iffalse is not None:
            return False
        t = n.iftrue
        if isinstance(t, ca.Compound) and t.block_items and len(t.block_items) == 1:
            t = t.block_items[0]
        return isinstance(t, ca.Return)
    return pred


def rw_goto_return(fn):
    """if (c) return X;  ->  if (c) goto ret; ... ret: return X;  (block layout)."""
    pred = _returns_in_ifs(fn)
    n = sites(fn, pred)
    if not n:
        return
    vals = {}
    for k in range(n):
        node, _, _ = nth(fn, pred, k)
        t = node.iftrue.block_items[0] if isinstance(node.iftrue, ca.Compound) else node.iftrue
        vals.setdefault(text(t.expr) if t.expr else "", []).append(k)
    choices = [[k] for k in range(n)] + [ks for ks in vals.values() if len(ks) > 1]
    for ks in choices:
        f2 = copy.deepcopy(fn)
        nodes = [nth(f2, pred, k)[0] for k in ks]
        lab = new_name(f2, "ret")
        ret = None
        for node in nodes:
            t = node.iftrue.block_items[0] if isinstance(node.iftrue, ca.Compound) else node.iftrue
            ret = t
            node.iftrue = ca.Goto(lab)
        f2.body.block_items.append(ca.Label(lab, copy.deepcopy(ret)))
        yield f"goto {lab} for return {text(ret.expr) if ret.expr else ''} x{len(ks)}", f2


def rw_void(fn):
    """Return nothing (the ROM's epilogue pops r0: no value is returned)."""
    ft = fn.decl.type
    if not isinstance(ft, ca.FuncDecl) or (isinstance(ft.type, ca.TypeDecl) and
                                            getattr(ft.type.type, "names", None) == ["void"]):
        return
    f2 = copy.deepcopy(fn)
    f2.decl.type.type = ca.TypeDecl(f2.decl.name, [], None, ca.IdentifierType(["void"]))
    for n, p, s in list(walk(f2)):
        if isinstance(n, ca.Return) and n.expr is not None:
            if isinstance(n.expr, (ca.Constant, ca.ID)):
                n.expr = None
            else:
                set_child(p, s, ca.Compound([n.expr, ca.Return(None)]))
    yield "void return", f2


def rw_width(fn):
    """A local or parameter of another integer width."""
    def pred(n, p, s):
        return (isinstance(n, ca.Decl) and isinstance(n.type, ca.TypeDecl) and
                isinstance(n.type.type, ca.IdentifierType) and n.type.type.names[-1] in INT_TYPES + ["int"])
    for k in range(sites(fn, pred)):
        node, _, _ = nth(fn, pred, k)
        cur = node.type.type.names[-1]
        for t in INT_TYPES:
            if t == cur:
                continue
            f2 = copy.deepcopy(fn)
            d, _, _ = nth(f2, pred, k)
            d.type.type = ca.IdentifierType([t])
            yield f"{node.name}: {cur} -> {t}", f2


def rw_commute(fn):
    pred = lambda n, p, s: isinstance(n, ca.BinaryOp) and n.op in ("+", "*", "&", "|", "^", "==", "!=")
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        b, _, _ = nth(f2, pred, k)
        b.left, b.right = b.right, b.left
        yield f"swap operands of {text(b)}", f2


def rw_swap_stmts(fn):
    """Swap two adjacent statements (scheduling / evaluation order)."""
    bl = blocks(fn)
    for bi, b in enumerate(bl):
        for i in range(len(b.block_items) - 1):
            x, y = b.block_items[i], b.block_items[i + 1]
            if isinstance(x, (ca.Decl, ca.Label, ca.Return)) or isinstance(y, (ca.Decl, ca.Label)):
                continue
            f2 = copy.deepcopy(fn)
            b2 = blocks(f2)[bi]
            b2.block_items[i], b2.block_items[i + 1] = b2.block_items[i + 1], b2.block_items[i]
            yield f"swap statements {i}/{i + 1}", f2


def rw_if_swap(fn):
    """if (c) A else B  ->  if (!c) B else A."""
    pred = lambda n, p, s: isinstance(n, ca.If) and n.iffalse is not None
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        node, _, _ = nth(f2, pred, k)
        node.cond = ca.UnaryOp("!", node.cond)
        node.iftrue, node.iffalse = node.iffalse, node.iftrue
        yield "negate and swap if/else", f2


def rw_decl_order(fn):
    """Move one local declaration to the front (allocation priority ties)."""
    items = fn.body.block_items or []
    decls = [i for i, x in enumerate(items) if isinstance(x, ca.Decl)]
    for i in decls[1:]:
        f2 = copy.deepcopy(fn)
        it = f2.body.block_items
        it.insert(0, it.pop(i))
        yield f"declare {items[i].name} first", f2



def _calls_or_assign(node):
    return any(isinstance(n, (ca.FuncCall, ca.Assignment)) or (isinstance(n, ca.UnaryOp) and n.op in ("p++", "p--", "++", "--"))
               for n, _, _ in walk(node))


def rw_return_for_goto(fn):
    """goto L; ... L: return X;  ->  return X;  (the inverse of rw_goto_return)."""
    labels = {n.name: n for n, _, _ in walk(fn) if isinstance(n, ca.Label) and isinstance(n.stmt, ca.Return)}
    pred = lambda n, p, s: isinstance(n, ca.Goto) and n.name in labels
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        labs = {n.name: n for n, _, _ in walk(f2) if isinstance(n, ca.Label) and isinstance(n.stmt, ca.Return)}
        g, p, s = nth(f2, pred, k)
        set_child(p, s, copy.deepcopy(labs[g.name].stmt))
        yield f"return instead of goto {g.name}", f2


def rw_temp_arg(fn):
    """f(..., E, ...)  ->  t = E; f(..., t, ...)  (an argument in its own typed variable)."""
    def pred(n, p, s):
        return isinstance(p, ca.ExprList) and not isinstance(n, (ca.ID, ca.Constant))
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        arg, p, s = nth(f2, pred, k)
        stmt = _enclosing_stmt(f2, arg)
        if stmt is None:
            continue
        blk, st = stmt
        t = new_name(f2)
        add_local(f2, typeof_decl(t, arg))
        idx = blk.block_items.index(st)
        blk.block_items.insert(idx, ca.Assignment("=", ca.ID(t), arg))
        set_child(p, s, ca.ID(t))
        yield f"argument {text(arg)[:30]} in {t}", f2
    # constant arguments too
    pred2 = lambda n, p, s: isinstance(p, ca.ExprList) and isinstance(n, ca.Constant)
    for k in range(sites(fn, pred2)):
        f2 = copy.deepcopy(fn)
        arg, p, s = nth(f2, pred2, k)
        stmt = _enclosing_stmt(f2, arg)
        if stmt is None:
            continue
        blk, st = stmt
        t = new_name(f2)
        add_local(f2, int_decl(t, "s32"))
        idx = blk.block_items.index(st)
        blk.block_items.insert(idx, ca.Assignment("=", ca.ID(t), arg))
        set_child(p, s, ca.ID(t))
        yield f"constant argument {text(arg)} in {t}", f2


def _enclosing_stmt(fn, node):
    """(block, statement) for the innermost block item containing NODE."""
    found = None
    for b in blocks(fn):  # preorder: deeper blocks come later
        for st in b.block_items:
            if any(n is node for n, _, _ in walk(st)):
                found = (b, st)
    return found


def rw_share_const(fn):
    """Every occurrence of K (two or more) through one variable."""
    cs = _consts(fn)
    by = {}
    for i, (c, _, _) in enumerate(cs):
        v = _cval(c)
        if v is not None and v >= 2:
            by.setdefault(v, []).append(i)
    for v, idxs in by.items():
        if len(idxs) < 2:
            continue
        for where in ("top", "first"):
            f2 = copy.deepcopy(fn)
            c2 = _consts(f2)
            t = new_name(f2, "k")
            if where == "top":
                add_local(f2, int_decl(t, "u32", ca.Constant("int", c2[idxs[0]][0].value)))
                for i in idxs:
                    set_child(c2[i][1], c2[i][2], ca.ID(t))
            else:
                add_local(f2, int_decl(t, "u32"))
                c0, p0, s0 = c2[idxs[0]]
                for i in idxs[1:]:
                    set_child(c2[i][1], c2[i][2], ca.ID(t))
                set_child(p0, s0, ca.Assignment("=", ca.ID(t), c0))
            yield f"share constant {v} via {t} ({where})", f2


def rw_inline_var(fn):
    """v = E; ... v ...  ->  ... (E) ...  (recompute instead of keeping v)."""
    names = set(_local_names(fn))
    pred = lambda n, p, s: (is_stmt_assign(n, p, s) and isinstance(n.lvalue, ca.ID) and n.lvalue.name in names
                            and not _calls_or_assign(n.rvalue))
    for k in range(sites(fn, pred)):
        node, _, _ = nth(fn, pred, k)
        v = node.lvalue.name
        fol = following(fn, node)
        if not any(reads_of(st, v) for st in fol):
            continue
        # stop at the next assignment to v
        f2 = copy.deepcopy(fn)
        a, par, _ = nth(f2, pred, k)
        for st in following(f2, a):
            if any(isinstance(n, ca.Assignment) and text(n.lvalue) == v for n, _, _ in walk(st)):
                break
            for r in reads_of(st, v):
                set_child(r[1], r[2], copy.deepcopy(a.rvalue))
        yield f"inline {v} = {text(a.rvalue)[:30]}", f2


def _neg(c):
    if isinstance(c, ca.UnaryOp) and c.op == "!":
        return c.expr
    ops = {"==": "!=", "!=": "==", "<": ">=", ">=": "<", ">": "<=", "<=": ">"}
    if isinstance(c, ca.BinaryOp) and c.op in ops:
        return ca.BinaryOp(ops[c.op], c.left, c.right)
    return ca.UnaryOp("!", c)


def _items(st):
    return list(st.block_items or []) if isinstance(st, ca.Compound) else [st]


def rw_guard_clause(fn):
    """if (c) { B } R  (R a return)  ->  if (!c) R; B; R   (nested ifs as early returns)."""
    def pred(n, p, s):
        if not isinstance(n, ca.If) or n.iffalse is not None or not isinstance(p, ca.Compound):
            return False
        i = p.block_items.index(n)
        return i + 1 < len(p.block_items) and isinstance(p.block_items[i + 1], ca.Return)
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        node, par, _ = nth(f2, pred, k)
        i = par.block_items.index(node)
        ret = par.block_items[i + 1]
        par.block_items[i:i + 1] = [ca.If(_neg(node.cond), copy.deepcopy(ret), None)] + _items(node.iftrue)
        yield f"guard clause for if ({text(node.cond)[:30]})", f2


def rw_split_and(fn):
    """if (a && b) S  ->  if (a) if (b) S; with a following return R also
    if (!a) R; if (!b) R; S."""
    def pred(n, p, s):
        return isinstance(n, ca.If) and isinstance(n.cond, ca.BinaryOp) and n.cond.op == "&&" and n.iffalse is None
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        node, par, slot = nth(f2, pred, k)
        a, b = node.cond.left, node.cond.right
        set_child(par, slot, ca.If(a, ca.If(b, node.iftrue, None), None))
        yield f"nested ifs for {text(node.cond)[:30]}", f2
        f3 = copy.deepcopy(fn)
        node, par, slot = nth(f3, pred, k)
        if isinstance(par, ca.Compound):
            i = par.block_items.index(node)
            if i + 1 < len(par.block_items) and isinstance(par.block_items[i + 1], ca.Return):
                ret = par.block_items[i + 1]
                a, b = node.cond.left, node.cond.right
                par.block_items[i:i + 2] = ([ca.If(_neg(a), copy.deepcopy(ret), None),
                                             ca.If(_neg(b), copy.deepcopy(ret), None)] + _items(node.iftrue))
                yield f"guard clauses for {text(node.cond)[:30]}", f3


def rw_bitfield(fn):
    """(E >> s) & m  ->  ((E << 16) >> (16 + s)) & m, (E << (32-s-w)) >> (32-w), (E & (m << s)) >> s."""
    def pred(n, p, s_):
        return (isinstance(n, ca.BinaryOp) and n.op == "&" and isinstance(n.right, ca.Constant)
                and isinstance(n.left, ca.BinaryOp) and n.left.op == ">>" and isinstance(n.left.right, ca.Constant))
    for k in range(sites(fn, pred)):
        node, _, _ = nth(fn, pred, k)
        m, sh = _cval(node.right), _cval(node.left.right)
        if m is None or sh is None or m & (m + 1):
            continue
        w = m.bit_length()
        C = lambda v: ca.Constant("int", str(v))
        forms = []
        if 16 + sh < 32:
            forms.append(("u16 shift", lambda E: ca.BinaryOp("&", ca.BinaryOp(">>", ca.BinaryOp("<<", E, C(16)), C(16 + sh)), C(m))))
        if 32 - sh - w >= 0:
            forms.append(("shift pair", lambda E: ca.BinaryOp(">>", ca.BinaryOp("<<", E, C(32 - sh - w)), C(32 - w))))
        forms.append(("mask first", lambda E: ca.BinaryOp(">>", ca.BinaryOp("&", E, C(m << sh)), C(sh))))
        for name, make in forms:
            f2 = copy.deepcopy(fn)
            n2, p2, s2 = nth(f2, pred, k)
            set_child(p2, s2, make(n2.left.left))
            yield f"bitfield {name} for {text(node)[:30]}", f2
    # r = c & m  ->  shift pair too
    pred2 = lambda n, p, s_: (isinstance(n, ca.BinaryOp) and n.op == "&" and isinstance(n.right, ca.Constant)
                               and not (isinstance(n.left, ca.BinaryOp) and n.left.op == ">>"))
    for k in range(sites(fn, pred2)):
        node, _, _ = nth(fn, pred2, k)
        m = _cval(node.right)
        if m is None or m & (m + 1) or m == 0:
            continue
        w = m.bit_length()
        f2 = copy.deepcopy(fn)
        n2, p2, s2 = nth(f2, pred2, k)
        set_child(p2, s2, ca.BinaryOp(">>", ca.BinaryOp("<<", n2.left, ca.Constant("int", str(32 - w))),
                                      ca.Constant("int", str(32 - w))))
        yield f"bitfield shift pair for {text(node)[:30]}", f2


REGS = ["r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"]


def rw_pin(fn):
    """register T v asm("rN") for a local (the project's register-pinning concession)."""
    def pred(n, p, s_):
        return (isinstance(n, ca.Decl) and isinstance(p, ca.Compound) and isinstance(n.type, (ca.TypeDecl, ca.PtrDecl))
                and "register" not in (n.storage or []) and not n.asmlabel)
    for k in range(sites(fn, pred)):
        for r in REGS:
            f2 = copy.deepcopy(fn)
            d, _, _ = nth(f2, pred, k)
            d.storage = list(d.storage or []) + ["register"]
            d.asmlabel = ca.Constant("string", f'"{r}"')
            yield f"pin {d.name} to {r}", f2


def rw_barrier(fn):
    """asm("" : "+r"(v)) after an assignment to v (keeps v in a register there)."""
    names = set(_local_names(fn))
    pred = lambda n, p, s_: is_stmt_assign(n, p, s_) and isinstance(n.lvalue, ca.ID) and n.lvalue.name in names
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        a, par, _ = nth(f2, pred, k)
        v = a.lvalue.name
        par.block_items.insert(par.block_items.index(a) + 1,
                               ca.ID(f'asm("" : "+r"({v}))'))
        yield f"barrier on {v}", f2


def rw_explicit_narrow(fn):
    """v = E (v u8/s8/u16/s16)  ->  t = E << n; v = t >> n  (the shifted value in its own register)."""
    widths = {"u8": (24, ">>"), "s8": (24, ">>"), "u16": (16, ">>"), "s16": (16, ">>")}
    decls = {n.name: n.type.type.names[-1] for n, _, _ in walk(fn)
             if isinstance(n, ca.Decl) and isinstance(n.type, ca.TypeDecl)
             and isinstance(n.type.type, ca.IdentifierType) and n.type.type.names[-1] in widths}
    pred = lambda n, p, s_: is_stmt_assign(n, p, s_) and isinstance(n.lvalue, ca.ID) and n.lvalue.name in decls
    for k in range(sites(fn, pred)):
        node, _, _ = nth(fn, pred, k)
        tname = decls[node.lvalue.name]
        sh = widths[tname][0]
        for ttype in (("u32", "s32") if tname[0] == "u" else ("s32",)):
            f2 = copy.deepcopy(fn)
            a, par, _ = nth(f2, pred, k)
            t = new_name(f2)
            add_local(f2, int_decl(t, ttype))
            par.block_items.insert(par.block_items.index(a), ca.Assignment(
                "=", ca.ID(t), ca.BinaryOp("<<", a.rvalue, ca.Constant("int", str(sh)))))
            a.rvalue = ca.BinaryOp(">>", ca.ID(t), ca.Constant("int", str(sh)))
            yield f"explicit {tname} narrowing of {a.lvalue.name} via {ttype} {t}", f2


def rw_hoist_args(fn):
    """f(a, b, c)  ->  t1 = a; t2 = b; ...; f(t1, t2, ...): every non-trivial
    argument of one call in its own variable, in source order and reversed."""
    pred = lambda n, p, s_: isinstance(n, ca.FuncCall) and n.args and \
        sum(not isinstance(a, (ca.ID, ca.Constant)) for a in n.args.exprs) >= 2
    for k in range(sites(fn, pred)):
        for order in ("forward", "reverse"):
            f2 = copy.deepcopy(fn)
            call, _, _ = nth(f2, pred, k)
            found = _enclosing_stmt(f2, call)
            if not found:
                continue
            blk, st = found
            idx = blk.block_items.index(st)
            hoisted = []
            for i, a in enumerate(call.args.exprs):
                if isinstance(a, (ca.ID, ca.Constant)):
                    continue
                t = new_name(f2)
                add_local(f2, typeof_decl(t, a))
                hoisted.append(ca.Assignment("=", ca.ID(t), a))
                call.args.exprs[i] = ca.ID(t)
            if order == "reverse":
                hoisted.reverse()
            idx = blk.block_items.index(st)
            blk.block_items[idx:idx] = hoisted
            yield f"hoist arguments of {text(call.name)} ({order})", f2


REWRITES = [rw_reuse_stored, rw_related_const, rw_copy_var, rw_temp_rvalue, rw_scoped_const,
            rw_goto_return, rw_return_for_goto, rw_guard_clause, rw_split_and, rw_temp_arg, rw_hoist_args,
            rw_share_const, rw_inline_var, rw_bitfield, rw_explicit_narrow, rw_void, rw_width, rw_commute, rw_swap_stmts, rw_if_swap, rw_decl_order]


CONCESSIONS = [rw_pin, rw_barrier]


def all_rewrites(fn, concessions=False):
    for rw in REWRITES + (CONCESSIONS if concessions else []):
        try:
            for desc, f2 in rw(fn):
                yield desc, f2
        except Exception:
            continue

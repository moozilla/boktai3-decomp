"""Semantics-preserving C moves over the pycparser AST of one function.

The companion of tools/rewrites.py, in the same style: each move is a
generator mv_X(fn) yielding (description, new FuncDef) for every site where it
applies, never touching fn. Moves change the C's form (loop shape, comparison
direction, zero tests, compound assignments, declaration scope, ...) without
changing what it computes. A description that starts with "[sem] " marks a
move that is only equivalent under assumptions the AST cannot check (types,
aliasing) and needs a semantic check (compile and compare) before it is
trusted.

all_moves(fn) runs every family (these plus rewrites.REWRITES); FAMILIES maps
a family name (the generator name without rw_/mv_) to its generator.
"""
import copy
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import permute  # noqa: E402  (puts the permuter's pycparser on sys.path)
sys.path.insert(0, permute.PERMUTER)
import rewrites as R  # noqa: E402
from perm_pycparser import c_ast as ca  # noqa: E402

text, walk, set_child, new_name = R.text, R.walk, R.set_child, R.new_name

LOOPS = (ca.For, ca.While, ca.DoWhile)
JUMPS = (ca.Return, ca.Goto, ca.Break, ca.Continue)
EXPR_STMT = (ca.Assignment, ca.Cast, ca.UnaryOp, ca.BinaryOp, ca.TernaryOp, ca.FuncCall, ca.ArrayRef,
             ca.StructRef, ca.Constant, ca.ID, ca.ExprList)
STMT_SLOTS = {ca.If: ("iftrue", "iffalse"), ca.For: ("stmt",), ca.While: ("stmt",), ca.DoWhile: ("stmt",),
              ca.Label: ("stmt",)}
RELATIONAL = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}
LOGICAL = ("&&", "||")
COMPARE = tuple(RELATIONAL) + ("==", "!=")
COMPOUND_OPS = ("+", "-", "*", "/", "%", "&", "|", "^", "<<", ">>")
COMMUTATIVE = ("+", "*", "&", "|", "^")


# --- helpers ---------------------------------------------------------------------------

def _sites(fn, pred):
    """Preorder indices of the nodes satisfying pred(n, p, s)."""
    return [k for k, (n, p, s) in enumerate(walk(fn)) if pred(n, p, s)]


def _copy_at(fn, k):
    """(copy of fn, node, parent, slot) for preorder index k in the copy."""
    f2 = copy.deepcopy(fn)
    for i, (n, p, s) in enumerate(walk(f2)):
        if i == k:
            return f2, n, p, s
    raise IndexError(k)


def _each(fn, pred):
    """For every site: (copy, node, parent, slot) of a fresh copy."""
    for k in _sites(fn, pred):
        yield _copy_at(fn, k)


def _const(v):
    return ca.Constant("int", str(v))


def _cval(n):
    return R._cval(n) if isinstance(n, ca.Constant) and n.type == "int" else None


def _items(st):
    return list(st.block_items or []) if isinstance(st, ca.Compound) else [st]


def _block(stmts):
    return ca.Compound(list(stmts))


def _replace_stmt(parent, slot, stmts):
    """Put the statement list STMTS where the statement at parent.slot is."""
    if isinstance(parent, ca.Compound):
        i = int(slot[slot.index("[") + 1:-1])
        parent.block_items[i:i + 1] = stmts
    else:
        set_child(parent, slot, stmts[0] if len(stmts) == 1 else _block(stmts))


def _is_stmt(p, s):
    """Whether the child at p.s is in statement position."""
    if isinstance(p, ca.Compound):
        return True
    return type(p) in STMT_SLOTS and s in STMT_SLOTS[type(p)]


def _impure(node):
    """Calls, assignments, ++/-- anywhere in node."""
    return node is not None and R._calls_or_assign(node)


def _ids(node):
    """Identifier names in node (not struct member names)."""
    return {n.name for n, p, s in walk(node) if isinstance(n, ca.ID) and not (isinstance(p, ca.StructRef)
                                                                            and s == "field")} \
        if node is not None else set()


def _has_continue(st):
    """A continue that belongs to the loop whose body is ST."""
    def rec(n):
        if isinstance(n, ca.Continue):
            return True
        if isinstance(n, LOOPS):
            return False
        return any(rec(c) for _, c in n.children())
    return rec(st)


def _int_name(tp):
    if isinstance(tp, ca.Typename):
        tp = tp.type
    if isinstance(tp, ca.TypeDecl) and isinstance(tp.type, ca.IdentifierType) and len(tp.type.names) == 1:
        return tp.type.names[0]
    return None


def _typename(t):
    return ca.Typename(None, [], None, [], ca.TypeDecl(None, [], None, ca.IdentifierType([t])))


def fix_dangling_else(fn):
    """Brace an if/else's then-branch when it ends in an else-less if: CGenerator
    prints if/else without braces, and `if (a) if (b) x; else y;` would bind the
    else to the inner if."""
    def ends_open(st):
        if isinstance(st, ca.If):
            return st.iffalse is None or ends_open(st.iffalse)
        if isinstance(st, (ca.For, ca.While, ca.Label)):
            return ends_open(st.stmt)
        return False
    for n, _, _ in walk(fn):
        if isinstance(n, ca.If) and n.iffalse is not None and ends_open(n.iftrue):
            n.iftrue = _block([n.iftrue])
    return fn


# --- 1. loop forms ---------------------------------------------------------------------

def _init_stmts(init):
    if init is None:
        return []
    if isinstance(init, ca.DeclList):
        return list(init.decls)
    return [init]


def mv_loop_form(fn):
    """for <-> while (+init/step), while -> if+do-while and back, for(;;) <-> while(1)."""
    # for (init; cond; step) S  ->  init; while (cond) { S; step; }
    pred = lambda n, p, s: isinstance(n, ca.For) and _is_stmt(p, s) and not _has_continue(n.stmt) \
        and not (n.init is None and n.cond is None and n.next is None)
    for f2, n, p, s in _each(fn, pred):
        body = _items(n.stmt) + ([n.next] if n.next is not None else [])
        loop = ca.While(n.cond or _const(1), _block(body))
        init = _init_stmts(n.init)
        if isinstance(n.init, ca.DeclList):  # C99 for-declaration: keep the scope
            _replace_stmt(p, s, [_block(init + [loop])])
        else:
            _replace_stmt(p, s, init + [loop])
        yield f"while for for ({text(n.cond) if n.cond else ''})", f2
    # for (init; cond; step) S  ->  init; if (cond) do { S; step; } while (cond);
    pred = lambda n, p, s: isinstance(n, ca.For) and _is_stmt(p, s) and n.cond is not None \
        and not _has_continue(n.stmt) and not isinstance(n.init, ca.DeclList)
    for f2, n, p, s in _each(fn, pred):
        body = _items(n.stmt) + ([n.next] if n.next is not None else [])
        loop = ca.If(n.cond, _block([ca.DoWhile(copy.deepcopy(n.cond), _block(body))]), None)
        _replace_stmt(p, s, _init_stmts(n.init) + [loop])
        yield f"guarded do-while for for ({text(n.cond)})", f2
    # while (c) S  ->  for (; c;) S, for (; c; step) S', for (init; c; step) S'
    pred = lambda n, p, s: isinstance(n, ca.While) and _is_stmt(p, s)
    for k in _sites(fn, pred):
        n0, p0, s0 = next((n, p, s) for i, (n, p, s) in enumerate(walk(fn)) if i == k)
        f2, n, p, s = _copy_at(fn, k)
        cond = None if _cval(n.cond) not in (None, 0) else n.cond
        set_child(p, s, ca.For(None, cond, None, n.stmt))
        yield f"for (;;) for while ({text(n0.cond)})", f2
        body = _items(n0.stmt)
        last = body[-1] if body else None
        steppable = (isinstance(last, (ca.Assignment, ca.UnaryOp, ca.ExprList)) and not _has_continue(n0.stmt)
                     and isinstance(n0.stmt, ca.Compound))
        if not steppable:
            continue
        f2, n, p, s = _copy_at(fn, k)
        items = _items(n.stmt)
        step = items.pop()
        set_child(p, s, ca.For(None, n.cond if _cval(n.cond) in (None, 0) else None, step, _block(items)))
        yield f"for with step for while ({text(n0.cond)})", f2
        if isinstance(p0, ca.Compound):
            i = int(s0[s0.index("[") + 1:-1])
            if i > 0 and isinstance(p0.block_items[i - 1], (ca.Assignment, ca.ExprList)):
                f2, n, p, s = _copy_at(fn, k)
                items = _items(n.stmt)
                step = items.pop()
                init = p.block_items[i - 1]
                p.block_items[i - 1:i + 1] = [ca.For(init, n.cond, step, _block(items))]
                yield f"for with init and step for while ({text(n0.cond)})", f2
    # while (c) S  ->  if (c) do S while (c);
    pred = lambda n, p, s: isinstance(n, ca.While) and _is_stmt(p, s) and _cval(n.cond) is None
    for f2, n, p, s in _each(fn, pred):
        set_child(p, s, ca.If(n.cond, _block([ca.DoWhile(copy.deepcopy(n.cond), n.stmt)]), None))
        yield f"guarded do-while for while ({text(n.cond)})", f2

    # if (c) do S while (c);  ->  while (c) S
    def guarded(n, p, s):
        if not (isinstance(n, ca.If) and n.iffalse is None and _is_stmt(p, s)):
            return False
        t = _items(n.iftrue)
        return len(t) == 1 and isinstance(t[0], ca.DoWhile) and text(t[0].cond) == text(n.cond)
    for f2, n, p, s in _each(fn, guarded):
        dw = _items(n.iftrue)[0]
        set_child(p, s, ca.While(n.cond, dw.stmt))
        yield f"while for guarded do-while ({text(n.cond)})", f2
    # for (;;) <-> while (1)
    pred = lambda n, p, s: isinstance(n, ca.For) and n.init is None and n.cond is None and n.next is None
    for f2, n, p, s in _each(fn, pred):
        set_child(p, s, ca.While(_const(1), n.stmt))
        yield "while (1) for for (;;)", f2


# --- 2. comparison direction -----------------------------------------------------------

def mv_cmp_flip(fn):
    """a < b  ->  b > a (all four relations)."""
    pred = lambda n, p, s: isinstance(n, ca.BinaryOp) and n.op in RELATIONAL
    for f2, n, p, s in _each(fn, pred):
        old = text(n)
        n.op, n.left, n.right = RELATIONAL[n.op], n.right, n.left
        yield f"flip {old[:40]}", f2


# --- 3. zero tests ---------------------------------------------------------------------

def _cond_ctx(p, s):
    if isinstance(p, (ca.If, ca.While, ca.DoWhile, ca.For, ca.TernaryOp)) and s == "cond":
        return True
    if isinstance(p, ca.BinaryOp) and p.op in LOGICAL:
        return True
    return isinstance(p, ca.UnaryOp) and p.op == "!"


def mv_zero_test(fn):
    """In a condition: x == 0 <-> !x, x != 0 <-> x."""
    def pred(n, p, s):
        return _cond_ctx(p, s) and n is not None and not (isinstance(n, ca.Constant))
    for f2, n, p, s in _each(fn, pred):
        if isinstance(n, ca.BinaryOp) and n.op in ("==", "!=") and (_cval(n.right) == 0 or _cval(n.left) == 0):
            x = n.left if _cval(n.right) == 0 else n.right
            new = ca.UnaryOp("!", x) if n.op == "==" else x
            what = f"{'!' if n.op == '==' else ''}x for {text(n)[:40]}"
        elif isinstance(n, ca.UnaryOp) and n.op == "!":
            new, what = ca.BinaryOp("==", n.expr, _const(0)), f"== 0 for {text(n)[:40]}"
        elif isinstance(n, ca.BinaryOp) and n.op in COMPARE + LOGICAL:
            continue
        else:
            new, what = ca.BinaryOp("!=", n, _const(0)), f"!= 0 for {text(n)[:40]}"
        set_child(p, s, new)
        yield f"zero test {what}", f2


# --- 4. multiply <-> shift -------------------------------------------------------------

def _log2(v):
    return v.bit_length() - 1 if v is not None and v >= 2 and v & (v - 1) == 0 else None


def mv_mul_shift(fn):
    """x * 2^k <-> x << k (also *= / <<=)."""
    def pred(n, p, s):
        if isinstance(n, ca.BinaryOp) and n.op == "*":
            return _log2(_cval(n.right)) is not None or _log2(_cval(n.left)) is not None
        if isinstance(n, ca.Assignment) and n.op == "*=":
            return _log2(_cval(n.rvalue)) is not None
        if isinstance(n, ca.BinaryOp) and n.op == "<<":
            v = _cval(n.right)
            return v is not None and 1 <= v < 31
        return isinstance(n, ca.Assignment) and n.op == "<<=" and _cval(n.rvalue) is not None and 1 <= _cval(n.rvalue) < 31
    for f2, n, p, s in _each(fn, pred):
        old = text(n)[:40]
        if isinstance(n, ca.BinaryOp) and n.op == "*":
            if _log2(_cval(n.right)) is not None:
                set_child(p, s, ca.BinaryOp("<<", n.left, _const(_log2(_cval(n.right)))))
            else:
                set_child(p, s, ca.BinaryOp("<<", n.right, _const(_log2(_cval(n.left)))))
        elif isinstance(n, ca.Assignment) and n.op == "*=":
            n.op, n.rvalue = "<<=", _const(_log2(_cval(n.rvalue)))
        elif isinstance(n, ca.BinaryOp):
            n.op, n.right = "*", _const(1 << _cval(n.right))
        else:
            n.op, n.rvalue = "*=", _const(1 << _cval(n.rvalue))
        yield f"shift/multiply for {old}", f2


# --- 5. compound assignment and increments ---------------------------------------------

def _stmt_expr(p, s):
    return _is_stmt(p, s) or (isinstance(p, ca.For) and s == "next")


def mv_compound(fn):
    """a op= b <-> a = a op b; a++ / ++a / a += 1 interchange (statements, for steps)."""
    def pred(n, p, s):
        return _stmt_expr(p, s) and (isinstance(n, ca.Assignment) or
                                     isinstance(n, ca.UnaryOp) and n.op in ("p++", "++", "p--", "--"))
    for k in _sites(fn, pred):
        n0 = next(n for i, (n, _, _) in enumerate(walk(fn)) if i == k)
        forms = []  # functions from the node (in a copy) to its replacement
        if isinstance(n0, ca.Assignment):
            lv = text(n0.lvalue)
            if n0.op != "=" and n0.op[:-1] in COMPOUND_OPS and not _impure(n0.lvalue):
                op = n0.op[:-1]
                forms.append(("expand", lambda n, op=op: ca.Assignment("=", n.lvalue, ca.BinaryOp(
                    op, copy.deepcopy(n.lvalue), n.rvalue))))
                if op in "+-" and _cval(n0.rvalue) == 1:
                    for u in (("p" + op * 2), op * 2):
                        forms.append((u.replace("p", "post "), lambda n, u=u: ca.UnaryOp(u, n.lvalue)))
            if n0.op == "=" and isinstance(n0.rvalue, ca.BinaryOp) and n0.rvalue.op in COMPOUND_OPS:
                b = n0.rvalue
                if text(b.left) == lv:
                    forms.append(("fold", lambda n: ca.Assignment(n.rvalue.op + "=", n.lvalue, n.rvalue.right)))
                elif b.op in COMMUTATIVE and text(b.right) == lv:
                    forms.append(("fold", lambda n: ca.Assignment(n.rvalue.op + "=", n.lvalue, n.rvalue.left)))
        else:
            op = n0.op.strip("p")[0]
            for u in ("p" + op * 2, op * 2):
                if u != n0.op:
                    forms.append((u.replace("p", "post "), lambda n, u=u: ca.UnaryOp(u, n.expr)))
            forms.append((op + "= 1", lambda n, op=op: ca.Assignment(op + "=", n.expr, _const(1))))
        for name, make in forms:
            f2, n, p, s = _copy_at(fn, k)
            set_child(p, s, make(n))
            yield f"{name} {text(n0)[:40]}", f2


# --- 6. ternaries ----------------------------------------------------------------------

def _single(st):
    it = _items(st) if st is not None else []
    return it[0] if len(it) == 1 else None


def mv_ternary(fn):
    """x = c ? a : b; <-> if/else assignments; return c ? a : b; <-> if/else returns."""
    pred = lambda n, p, s: (_is_stmt(p, s) and isinstance(n, ca.Assignment) and n.op == "="
                            and isinstance(n.rvalue, ca.TernaryOp))
    for f2, n, p, s in _each(fn, pred):
        t = n.rvalue
        set_child(p, s, ca.If(t.cond, _block([ca.Assignment("=", n.lvalue, t.iftrue)]),
                              _block([ca.Assignment("=", copy.deepcopy(n.lvalue), t.iffalse)])))
        yield f"if/else for {text(n.lvalue)} = ?:", f2
    pred = lambda n, p, s: _is_stmt(p, s) and isinstance(n, ca.Return) and isinstance(n.expr, ca.TernaryOp)
    for f2, n, p, s in _each(fn, pred):
        t = n.expr
        set_child(p, s, ca.If(t.cond, _block([ca.Return(t.iftrue)]), _block([ca.Return(t.iffalse)])))
        yield "if/else returns for return ?:", f2

    def assign_pair(n, p, s):
        if not (isinstance(n, ca.If) and _is_stmt(p, s) and n.iffalse is not None):
            return False
        a, b = _single(n.iftrue), _single(n.iffalse)
        return (isinstance(a, ca.Assignment) and isinstance(b, ca.Assignment) and a.op == b.op == "="
                and text(a.lvalue) == text(b.lvalue))
    for f2, n, p, s in _each(fn, assign_pair):
        a, b = _single(n.iftrue), _single(n.iffalse)
        set_child(p, s, ca.Assignment("=", a.lvalue, ca.TernaryOp(n.cond, a.rvalue, b.rvalue)))
        yield f"{text(a.lvalue)} = ?: for if/else", f2

    def return_pair(n, p, s):
        if not (isinstance(n, ca.If) and _is_stmt(p, s) and n.iffalse is not None):
            return False
        a, b = _single(n.iftrue), _single(n.iffalse)
        return isinstance(a, ca.Return) and isinstance(b, ca.Return) and a.expr is not None and b.expr is not None
    for f2, n, p, s in _each(fn, return_pair):
        a, b = _single(n.iftrue), _single(n.iffalse)
        set_child(p, s, ca.Return(ca.TernaryOp(n.cond, a.expr, b.expr)))
        yield "return ?: for if/else returns", f2

    # if (c) return a; return b;  ->  return c ? a : b;
    def return_after(n, p, s):
        if not (isinstance(n, ca.If) and isinstance(p, ca.Compound) and n.iffalse is None):
            return False
        a = _single(n.iftrue)
        i = p.block_items.index(n)
        b = p.block_items[i + 1] if i + 1 < len(p.block_items) else None
        return (isinstance(a, ca.Return) and isinstance(b, ca.Return) and a.expr is not None
                and b.expr is not None)
    for f2, n, p, s in _each(fn, return_after):
        i = p.block_items.index(n)
        a, b = _single(n.iftrue), p.block_items[i + 1]
        p.block_items[i:i + 2] = [ca.Return(ca.TernaryOp(n.cond, a.expr, b.expr))]
        yield "return ?: for if-return/return", f2


# --- 7. declaration scope --------------------------------------------------------------

def _local_decl(d):
    return (isinstance(d, ca.Decl) and isinstance(d.type, (ca.TypeDecl, ca.PtrDecl, ca.ArrayDecl))
            and not set(d.storage or []) - {"register"} and not d.asmlabel)


def _n_decls(items):
    i = 0
    while i < len(items) and isinstance(items[i], ca.Decl):
        i += 1
    return i


def _uses(root, name, skip=None):
    """ID nodes named NAME under root (not under SKIP)."""
    out = []

    def rec(n):
        if n is skip:
            return
        if isinstance(n, ca.ID) and n.name == name:
            out.append(n)
        for slot, c in n.children():
            if not (isinstance(n, ca.StructRef) and slot == "field"):
                rec(c)
    rec(root)
    return out


def _contains_all(node, targets):
    ids = {id(n) for n, _, _ in walk(node)}
    return all(id(t) in ids for t in targets)


def _compound_path(fn, target):
    """The chain of nodes from fn.body down to target (inclusive), or None."""
    def rec(n, path):
        path.append(n)
        if n is target:
            return True
        for _, c in n.children():
            if rec(c, path):
                return True
        path.pop()
        return False
    path = []
    return path if rec(fn.body, path) else None


def _top_names(fn):
    """Parameters and the function block's own locals."""
    names = {d.name for d in fn.body.block_items or [] if isinstance(d, ca.Decl)}
    if isinstance(fn.decl.type, ca.FuncDecl) and fn.decl.type.args:
        names |= {q.name for q in fn.decl.type.args.params if isinstance(q, ca.Decl) and q.name}
    return names


def _declared_names(fn):
    names = [n.name for n, _, _ in walk(fn) if isinstance(n, ca.Decl) and n.name]
    return names


def mv_decl_scope(fn):
    """Sink a top-level local into the innermost block using it; hoist a nested one to the top."""
    top = fn.body.block_items or []
    for i in range(_n_decls(top)):
        d = top[i]
        if not _local_decl(d) or d.init is not None and not isinstance(d.init, ca.Constant):
            continue
        uses = _uses(fn.body, d.name, skip=d)
        if not uses:
            continue
        inner = [b for b in R.blocks(fn) if b is not fn.body and _contains_all(b, uses)]
        if not inner:
            continue
        tgt = inner[-1]  # preorder: the deepest
        path = _compound_path(fn, tgt)
        if any(isinstance(x, LOOPS) for x in path) or any(isinstance(x, ca.Decl) and x.name == d.name
                                                            for x in tgt.block_items):
            continue  # into a loop body: the value would not survive an iteration
        bi = R.blocks(fn).index(tgt)
        f2 = copy.deepcopy(fn)
        t2 = R.blocks(f2)[bi]
        d2 = f2.body.block_items.pop(i)
        nd = _n_decls(t2.block_items)
        at = next((j for j in range(nd) if _uses(t2.block_items[j], d.name)), nd)
        t2.block_items.insert(at, d2)
        yield f"declare {d.name} in its inner block", f2

    # hoist a nested declaration to the top
    for bi, b in enumerate(R.blocks(fn)):
        if b is fn.body:
            continue
        items = b.block_items
        nd = _n_decls(items)
        for j in range(nd):
            d = items[j]
            if not _local_decl(d) or _declared_names(fn).count(d.name) != 1 or d.name in top_names:
                continue
            if _uses(fn.body, d.name) and not _contains_all(b, _uses(fn.body, d.name)):
                continue  # the name means something else (a global) outside the block
            local_inner = {x.name for blk in R.blocks(fn) if blk is not fn.body
                           for x in blk.block_items if isinstance(x, ca.Decl)}
            if _ids(d.type) & local_inner:
                continue  # its type names an inner local (typeof)
            init = d.init
            if init is not None:
                if isinstance(init, ca.InitList) or "const" in (d.quals or []) or isinstance(d.type, ca.ArrayDecl):
                    continue
                later = items[j + 1:nd]
                if any(x.init is not None and not isinstance(x.init, ca.Constant) for x in later):
                    continue  # the initializer would move past others' evaluation
            f2 = copy.deepcopy(fn)
            b2 = R.blocks(f2)[bi]
            d2 = b2.block_items.pop(j)
            if init is not None:
                e = d2.init
                d2.init = None
                b2.block_items.insert(nd - 1, ca.Assignment("=", ca.ID(d2.name), e))
            R.add_local(f2, d2)
            yield f"declare {d.name} at the top{' (split)' if init is not None else ''}", f2


# --- 8. initializer merge / split ------------------------------------------------------

def mv_init_merge(fn):
    """T x; x = E; (first statement) -> T x = E; and the reverse split."""
    for bi, b in enumerate(R.blocks(fn)):
        items = b.block_items
        nd = _n_decls(items)
        # merge
        if nd < len(items):
            st = items[nd]
            if isinstance(st, ca.Assignment) and st.op == "=" and isinstance(st.lvalue, ca.ID):
                x = st.lvalue.name
                j = next((j for j in range(nd) if items[j].name == x), None)
                if j is not None and _local_decl(items[j]) and items[j].init is None \
                        and not isinstance(items[j].type, ca.ArrayDecl) and x not in _ids(st.rvalue):
                    later = items[j + 1:nd]
                    inplace = (not (_ids(st.rvalue) & {d.name for d in later})
                               and all(d.init is None or isinstance(d.init, ca.Constant) for d in later))
                    for where in (["in place"] if inplace else []) + (["last"] if j < nd - 1 else []):
                        f2 = copy.deepcopy(fn)
                        it = R.blocks(f2)[bi].block_items
                        a = it.pop(nd)
                        d = it.pop(j) if where == "last" else it[j]
                        d.init = a.rvalue
                        if where == "last":
                            it.insert(nd - 1, d)
                        yield f"initialize {x} in its declaration ({where})", f2
        # split
        for j in range(nd):
            d = items[j]
            if (not _local_decl(d) or d.init is None or isinstance(d.init, ca.InitList)
                    or isinstance(d.type, ca.ArrayDecl) or "const" in (d.quals or [])):
                continue
            later = items[j + 1:nd]
            if any(x.init is not None and not isinstance(x.init, ca.Constant) for x in later):
                continue
            f2 = copy.deepcopy(fn)
            it = R.blocks(f2)[bi].block_items
            d2 = it[j]
            e, d2.init = d2.init, None
            it.insert(nd, ca.Assignment("=", ca.ID(d2.name), e))
            yield f"split the initializer of {d.name}", f2


# --- 9. index <-> pointer --------------------------------------------------------------

def mv_index_ptr(fn):
    """p[i] <-> *(p + i), p[0] <-> *p."""
    pred = lambda n, p, s: isinstance(n, ca.ArrayRef) and isinstance(n.name, (ca.ID, ca.StructRef))
    for f2, n, p, s in _each(fn, pred):
        old = text(n)[:40]
        if _cval(n.subscript) == 0:
            set_child(p, s, ca.UnaryOp("*", n.name))
        else:
            set_child(p, s, ca.UnaryOp("*", ca.BinaryOp("+", n.name, n.subscript)))
        yield f"pointer form of {old}", f2

    def deref(n, p, s):
        if not (isinstance(n, ca.UnaryOp) and n.op == "*") or isinstance(p, ca.FuncCall) and s == "name":
            return False
        e = n.expr
        return isinstance(e, (ca.ID, ca.StructRef)) or isinstance(e, ca.BinaryOp) and e.op == "+"
    for f2, n, p, s in _each(fn, deref):
        old = text(n)[:40]
        e = n.expr
        if isinstance(e, ca.BinaryOp):
            set_child(p, s, ca.ArrayRef(e.left, e.right))
        else:
            set_child(p, s, ca.ArrayRef(e, _const(0)))
        yield f"index form of {old}", f2


# --- 10. merge nested ifs --------------------------------------------------------------

def mv_merge_if(fn):
    """if (a) { if (b) S }  ->  if (a && b) S (no elses)."""
    def pred(n, p, s):
        if not isinstance(n, ca.If) or n.iffalse is not None:
            return False
        inner = _single(n.iftrue)
        return isinstance(inner, ca.If) and inner.iffalse is None
    for f2, n, p, s in _each(fn, pred):
        inner = _single(n.iftrue)
        set_child(p, s, ca.If(ca.BinaryOp("&&", n.cond, inner.cond), inner.iftrue, None))
        yield f"merge if ({text(n.cond)[:20]}) if ({text(inner.cond)[:20]})", f2


# --- 11. integer casts -----------------------------------------------------------------

CAST_TYPES = ["u32", "s32", "u16", "s16", "u8", "s8"]


def _cast_site(n, p, s):
    if isinstance(n, ca.Constant) or n is None:
        return False
    if isinstance(p, ca.Assignment) and s == "rvalue" and p.op == "=":
        return True
    if isinstance(p, ca.ExprList):
        return True  # call arguments (ExprList only appears there and in commas)
    return isinstance(p, ca.BinaryOp) and p.op in COMPARE


def _pointerish(n):
    return (isinstance(n, ca.UnaryOp) and n.op == "&" or isinstance(n, ca.Constant) and n.type == "string"
            or isinstance(n, ca.Cast) and not isinstance(n.to_type.type, ca.TypeDecl))


def mv_cast(fn, limit=40):
    """[sem] Add or remove an integer cast on an assigned value, a call argument, a compared operand."""
    out = 0
    remove = lambda n, p, s: _cast_site(n, p, s) and isinstance(n, ca.Cast) and _int_name(n.to_type) in CAST_TYPES
    for f2, n, p, s in _each(fn, remove):
        if out >= limit:
            return
        set_child(p, s, n.expr)
        out += 1
        yield f"[sem] drop cast {text(n)[:40]}", f2
    add = lambda n, p, s: _cast_site(n, p, s) and not _pointerish(n)
    ks = _sites(fn, add)
    for t in CAST_TYPES:
        for k in ks:
            if out >= limit:
                return
            f2, n, p, s = _copy_at(fn, k)
            if isinstance(n, ca.Cast) and _int_name(n.to_type) == t:
                continue
            set_child(p, s, ca.Cast(_typename(t), n))
            out += 1
            yield f"[sem] cast ({t}) {text(n)[:40]}", f2


# --- 12. common subexpression in a temporary -------------------------------------------

def _pure_access(n):
    """A memory access chain (StructRef / ArrayRef / *) with no side effects."""
    if not isinstance(n, (ca.StructRef, ca.ArrayRef)) and not (isinstance(n, ca.UnaryOp) and n.op == "*"):
        return False
    for m, _, _ in walk(n):
        if isinstance(m, (ca.FuncCall, ca.Assignment, ca.TernaryOp, ca.Typeof)):
            return False
        if isinstance(m, ca.UnaryOp) and m.op not in ("*", "&", "-", "~", "sizeof"):
            return False
    return True


def _reads(stmt):
    """(node, parent, slot) of pure accesses in STMT that are reads: not stored
    to, not under & or ++/--, not in sizeof, a type, or a called function."""
    out = []

    def lv(n, p, s):  # an lvalue: its own value is not read, its address parts are
        if isinstance(n, ca.ArrayRef):
            rd(n.name, n, "name")
            rd(n.subscript, n, "subscript")
        elif isinstance(n, ca.StructRef):
            (rd if n.type == "->" else lv)(n.name, n, "name")
        elif isinstance(n, ca.UnaryOp) and n.op == "*":
            rd(n.expr, n, "expr")
        elif isinstance(n, ca.Cast):
            lv(n.expr, n, "expr")

    def rd(n, p, s):
        if n is None or isinstance(n, (ca.Typename, ca.Typeof)):
            return
        if isinstance(n, ca.Assignment):
            lv(n.lvalue, n, "lvalue")
            rd(n.rvalue, n, "rvalue")
            return
        if isinstance(n, ca.UnaryOp) and n.op in ("&", "p++", "p--", "++", "--"):
            lv(n.expr, n, "expr")
            return
        if isinstance(n, ca.UnaryOp) and n.op == "sizeof":
            return
        if isinstance(n, ca.Decl):
            rd(n.init, n, "init")
            return
        if isinstance(n, ca.FuncCall):
            rd(n.args, n, "args")
            return
        if _pure_access(n):
            out.append((n, p, s))
        for name, c in n.children():
            if not (isinstance(n, ca.StructRef) and name == "field"):
                rd(c, n, name)

    rd(stmt, None, None)
    return out


def _sub_texts(e):
    return {text(m) for m, p, s in walk(e) if isinstance(m, (ca.ID, ca.StructRef, ca.ArrayRef, ca.UnaryOp))
            and not (isinstance(p, ca.StructRef) and s == "field")}


def _kills(stmt, subs):
    """Whether STMT may change an access whose parts have the texts SUBS."""
    for m, _, _ in walk(stmt):
        if isinstance(m, ca.FuncCall):
            return True
        tgt = None
        if isinstance(m, ca.Assignment):
            tgt = m.lvalue
        elif isinstance(m, ca.UnaryOp) and m.op in ("p++", "p--", "++", "--"):
            tgt = m.expr
        if tgt is not None:
            if text(tgt) in subs:
                return True
            if isinstance(tgt, ca.UnaryOp) and tgt.op == "*" and any(isinstance(x, ca.Cast) for x, _, _ in walk(tgt)):
                return True  # a type-punned store may alias anything
    return False


def _part(st, part):
    return st.cond if part == "cond" else st


def _cse_range(items, j, subs):
    """(index, part) units from items[j] on in which an access whose parts have
    the texts SUBS keeps its value; part "cond" is only an if's condition."""
    units = []
    for jj in range(j, len(items)):
        st = items[jj]
        if not _kills(st, subs):
            units.append((jj, None))
            continue
        if isinstance(st, ca.If) and not _kills(st.cond, subs):
            units.append((jj, "cond"))
        elif isinstance(st, ca.Assignment) and not _kills(st.rvalue, subs) and not _kills(st.lvalue, subs):
            units.append((jj, None))  # the store itself comes after the reads
        elif isinstance(st, ca.Assignment) and isinstance(st.rvalue, ca.FuncCall) and not _kills(st.rvalue.args, subs):
            units.append((jj, None))
        elif isinstance(st, ca.FuncCall) and not _kills(st.args, subs):
            units.append((jj, None))  # the arguments are read before the call
        break
    return units


def mv_cse_temp(fn):
    """[sem] A repeated memory access within one block read once into a typeof temporary."""
    top_names = _top_names(fn)
    for bi, b in enumerate(R.blocks(fn)):
        items = b.block_items
        nd = _n_decls(items)
        seen = set()
        for j in range(nd, len(items)):
            for n, _, _ in _reads(items[j]):
                key = text(n)
                if key in seen or "volatile" in key:
                    continue
                seen.add(key)
                subs = _sub_texts(n)
                # the statements (or an if's condition) from j while the value holds
                units = _cse_range(items, j, subs)
                count = sum(1 for jj, part in units for r in _reads(_part(items[jj], part)) if text(r[0]) == key)
                if count < 2:
                    continue
                f2 = copy.deepcopy(fn)
                b2 = R.blocks(f2)[bi]
                it = b2.block_items
                t = new_name(f2)
                found = [r for jj, part in units for r in _reads(_part(it[jj], part)) if text(r[0]) == key]
                # an access used only as an array base may be an array member:
                # declare the temporary with the decayed pointer type
                if all(isinstance(r[1], ca.ArrayRef) and r[2] == "name" for r in found):
                    decl = R.typeof_decl(t, ca.UnaryOp("&", ca.ArrayRef(n, _const(0))))
                else:
                    decl = R.typeof_decl(t, n)
                for r in found:
                    set_child(r[1], r[2], ca.ID(t))
                it.insert(j, ca.Assignment("=", ca.ID(t), copy.deepcopy(n)))
                R.add_local(b2, decl, n.coord)
                yield f"[sem] {key[:40]} in {t} (x{count})", f2


# --- 13. adjacent declarations ---------------------------------------------------------

def mv_swap_decls(fn):
    """Swap two adjacent declarations in a block."""
    for bi, b in enumerate(R.blocks(fn)):
        items = b.block_items
        nd = _n_decls(items)
        for j in range(nd - 1):
            x, y = items[j], items[j + 1]
            if x.name in _ids(y.init) | _ids(y.type):
                continue
            if all(d.init is not None and not isinstance(d.init, ca.Constant) for d in (x, y)) \
                    and (_impure(x.init) or _impure(y.init)):
                continue
            f2 = copy.deepcopy(fn)
            it = R.blocks(f2)[bi].block_items
            it[j], it[j + 1] = it[j + 1], it[j]
            yield f"swap declarations {x.name}/{y.name}", f2


# --- 14. early return <-> else ---------------------------------------------------------

def _ends_in_jump(st):
    it = _items(st)
    return bool(it) and isinstance(it[-1], JUMPS)


def mv_early_return(fn):
    """if (c) { ...; return; } REST <-> if (c) { ...; return; } else { REST }."""
    def fwd(n, p, s):
        return (isinstance(n, ca.If) and n.iffalse is None and isinstance(p, ca.Compound)
                and p.block_items.index(n) < len(p.block_items) - 1 and _ends_in_jump(n.iftrue))
    for f2, n, p, s in _each(fn, fwd):
        i = p.block_items.index(n)
        rest = p.block_items[i + 1:]
        del p.block_items[i + 1:]
        n.iffalse = rest[0] if len(rest) == 1 and isinstance(rest[0], ca.Compound) else _block(rest)
        yield f"else for the rest after if ({text(n.cond)[:30]})", f2

    def rev(n, p, s):
        return isinstance(n, ca.If) and n.iffalse is not None and _is_stmt(p, s) and _ends_in_jump(n.iftrue)
    for f2, n, p, s in _each(fn, rev):
        e = n.iffalse
        n.iffalse = None
        rest = [e] if isinstance(e, ca.Compound) and _n_decls(e.block_items) else _items(e)
        _replace_stmt(p, s, [n] + rest)
        yield f"lift the else after if ({text(n.cond)[:30]})", f2


# --- registry --------------------------------------------------------------------------

def _last_assign(st):
    """(container list, index) of ST's last statement if it is `ID = E;`, else None."""
    items = st.block_items if isinstance(st, ca.Compound) else None
    last = items[-1] if items else st
    if isinstance(last, ca.Assignment) and last.op == "=":
        return last
    return None


def mv_sink_store(fn):
    """if (c) v = A; else v = B; L = v;  ->  if (c) L = A; else L = B;  (the store in each branch, no
    temporary; v must not be read later). And the reverse: one temporary for a store both branches make."""
    blocks = [n for n, _, _ in walk(fn) if isinstance(n, ca.Compound)]
    for bk, b in enumerate(blocks):
        if not b.block_items:
            continue
        items = b.block_items
        for i in range(len(items) - 1):
            st, nxt = items[i], items[i + 1]
            path = []  # the if may end a nested block: `{ u32 mask = ...; if (...) ... }`
            while isinstance(st, ca.Compound) and st.block_items:
                path.append(len(st.block_items) - 1)
                st = st.block_items[-1]
            if not (isinstance(st, ca.If) and st.iffalse is not None):
                continue

            def at(it2):
                x = it2[i]
                for k in path:
                    x = x.block_items[k]
                return x
            la, lb = _last_assign(st.iftrue), _last_assign(st.iffalse)
            if la and lb and isinstance(la.lvalue, ca.ID) and isinstance(lb.lvalue, ca.ID) \
                    and la.lvalue.name == lb.lvalue.name and isinstance(nxt, ca.Assignment) \
                    and nxt.op == "=" and isinstance(nxt.rvalue, ca.ID) and nxt.rvalue.name == la.lvalue.name:
                v = la.lvalue.name
                later = [u for st2 in items[i + 2:] for u in _uses(st2, v)]
                if later or _uses(nxt.lvalue, v):
                    continue
                f2 = copy.deepcopy(fn)
                b2 = [n for n, _, _ in walk(f2) if isinstance(n, ca.Compound)][bk]
                it2 = b2.block_items
                for br in (at(it2).iftrue, at(it2).iffalse):
                    a = _last_assign(br)
                    a.lvalue = copy.deepcopy(it2[i + 1].lvalue)
                del it2[i + 1]
                yield f"store {text(nxt.lvalue)} in each branch (no {v})", f2
            elif la and lb and text(la.lvalue) == text(lb.lvalue) and not isinstance(la.lvalue, ca.ID):
                if _ids(la.lvalue) & set(_declared_names(st)):
                    continue  # branch-local objects cannot be named after the branch
                f2 = copy.deepcopy(fn)
                b2 = [n for n, _, _ in walk(f2) if isinstance(n, ca.Compound)][bk]
                it2 = b2.block_items
                t = new_name(f2)
                target = copy.deepcopy(la.lvalue)
                for br in (at(it2).iftrue, at(it2).iffalse):
                    _last_assign_any(br).lvalue = ca.ID(t)
                it2.insert(i + 1, ca.Assignment("=", target, ca.ID(t)))
                R.add_local(b2, R.typeof_decl(t, target), target.coord)
                yield f"one temporary for the store to {text(la.lvalue)}", f2


def _last_assign_any(st):
    items = st.block_items if isinstance(st, ca.Compound) else None
    return items[-1] if items else st


MOVES = [mv_loop_form, mv_cmp_flip, mv_zero_test, mv_mul_shift, mv_compound, mv_ternary, mv_decl_scope,
         mv_init_merge, mv_index_ptr, mv_merge_if, mv_cast, mv_cse_temp, mv_swap_decls, mv_early_return,
         mv_sink_store]


def family(gen):
    name = gen.__name__
    return name[3:] if name[:3] in ("rw_", "mv_") else name


FAMILIES = {family(g): g for g in MOVES + list(R.REWRITES)}


def all_moves(fn, families=None, include_rewrites=True, lines=None):
    """(family, description, new FuncDef) for every move site; FAMILIES: a set of family names to keep
    (family = the generator's name without the rw_/mv_ prefix). Includes rewrites.REWRITES when
    include_rewrites. Swallow exceptions per generator (a broken generator must not stop the others)."""
    for gen in MOVES + (list(R.REWRITES) if include_rewrites else []):
        fam = family(gen)
        if families is not None and fam not in families:
            continue
        try:
            variants = gen(fn, lines=lines) if gen is R.rw_related_const else gen(fn)
            for desc, f2 in variants:
                yield fam, desc, fix_dangling_else(f2)
        except Exception:
            continue


# --- self-test -------------------------------------------------------------------------

def _main():
    import argparse
    import difflib
    import subprocess
    import tempfile
    import time
    import traceback
    ap = argparse.ArgumentParser(description="run every move on one function and show a sample of each")
    ap.add_argument("file")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--matching", action="store_true", help="read the default build's C, not the NONMATCHING draft")
    ap.add_argument("--compile", action="store_true", help="compile every variant with agbcc")
    ap.add_argument("--strict", action="store_true", help="raise generator exceptions")
    args = ap.parse_args()
    import build
    import fixsearch
    import split
    if not args.matching:
        build.DEFINES.append("-DNONMATCHING")
    func = args.func or split.c_functions(os.path.join(build.ROOT, args.file))[0]
    _, _, fn, prefix = fixsearch.prepare(args.file, func)
    base = text(fn).splitlines(keepends=True)
    t0 = time.time()
    per = {}
    for gen in MOVES + list(R.REWRITES):
        fam = family(gen)
        try:
            per[fam] = [(d, fix_dangling_else(f2)) for d, f2 in gen(fn)]
        except Exception:
            if args.strict:
                raise
            print(f"!! {fam} raised:\n{traceback.format_exc()}")
            per[fam] = []
    elapsed = time.time() - t0
    total = sum(len(v) for v in per.values())
    bad = {}
    if args.compile:
        cc = os.path.join(build.ROOT, "build", "scope", "agbcc", "gcc", "agbcc")
        if not os.path.exists(cc):
            cc = build.AGBCC

        def ok(f2):
            with tempfile.NamedTemporaryFile("w", suffix=".c", delete=False) as fh:
                fh.write(prefix + "\n" + text(f2))
            r = subprocess.run([cc] + build.CFLAGS + ["-o", "/dev/null", fh.name], capture_output=True, text=True)
            os.unlink(fh.name)
            return r.returncode == 0 and "error" not in r.stderr, r.stderr
        from concurrent.futures import ThreadPoolExecutor
        with ThreadPoolExecutor(8) as ex:
            for fam, vs in per.items():
                res = list(ex.map(lambda v: ok(v[1]), vs))
                bad[fam] = [(vs[i][0], e) for i, (good, e) in enumerate(res) if not good]
    for fam, vs in per.items():
        line = f"== {fam}: {len(vs)} variants"
        if args.compile:
            line += f", {len(bad[fam])} do not compile"
        print(line)
        if vs:
            d, f2 = vs[0]
            print(f"   {d}")
            diff = list(difflib.unified_diff(base, text(f2).splitlines(keepends=True), n=1))[2:]
            for ln in diff[:12]:
                print("   " + ln.rstrip("\n"))
        for d, e in bad.get(fam, [])[:3]:
            print(f"   !! {d}: {e.strip().splitlines()[-1] if e.strip() else 'failed'}")
    print(f"{total} variants in {elapsed:.2f}s ({1000 * elapsed / max(total, 1):.1f} ms each)")


if __name__ == "__main__":
    _main()

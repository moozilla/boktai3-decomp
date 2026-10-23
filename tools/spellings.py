"""C spellings that flip one named agbcc decision: the inverse step of tools/step.py.

step.py names the decision where a draft's code first differs from the ROM
(an evaluation order, a cse block boundary, a hoisted invariant, a narrowing,
a signed compare...). Each rewrite here is a C spelling known to flip one such
decision; they came from real matches (notes/compiler-debugger-2026-10-23.md).
Like tools/rewrites.py, each is a generator: sp_X(fn) yields (description, new
FuncDef) for every site where it applies, without touching fn.

DECISIONS maps a decision (an agbccsrc topic or a step.py note kind) to the
spellings to try first. tools/fix.py applies them, together with the generic
rewrites, to the function and to the inline helpers it calls.
"""
import copy

from perm_pycparser import c_ast as ca  # tools/permute.py puts the permuter on sys.path
from rewrites import (_enclosing_stmt, add_local, blocks, int_decl, is_stmt_assign, new_name, nth, reads_of,
                      set_child, sites, text, walk)

CMP = ("<", "<=", ">", ">=")


def _assign_in(stmt):
    """The single assignment STMT is, or holds as its only statement."""
    if isinstance(stmt, ca.Compound) and stmt.block_items and len(stmt.block_items) == 1:
        stmt = stmt.block_items[0]
    return stmt if isinstance(stmt, ca.Assignment) and stmt.op == "=" else None


def tname(t):
    """The type name T (as in a cast)."""
    return ca.Typename(None, [], None, [], ca.TypeDecl(None, [], None, ca.IdentifierType([t])))


STMT_SLOTS = {(ca.If, "iftrue"), (ca.If, "iffalse"), (ca.For, "stmt"), (ca.While, "stmt"),
              (ca.DoWhile, "stmt"), (ca.Label, "stmt"), (ca.Case, "stmts"), (ca.Default, "stmts")}


def _stmt_of(fn, node):
    """(parent, slot, statement) for the innermost statement holding NODE: a block item, or the
    unbraced body of an if/loop, so a statement put before it stays on the same path."""
    best = None
    for n, p, s in walk(fn):
        if p is None or n is fn.body:
            continue
        base = s.split("[")[0]
        if isinstance(p, ca.Compound) or (type(p), base) in STMT_SLOTS:
            if isinstance(n, ca.Compound):
                continue
            if any(x is node for x, _, _ in walk(n)):
                best = (p, s, n)
    return best


def _put_before(fn, node, decl):
    """Wrap the statement holding NODE as { DECL; statement }."""
    found = _stmt_of(fn, node)
    if not found:
        return False
    p, s, st = found
    set_child(p, s, ca.Compound([decl, st]))
    return True


def _renamed(t, name):
    t = copy.deepcopy(t)
    for n, _, _ in walk(t):
        if isinstance(n, ca.TypeDecl):
            n.declname = name
    return t


def _helper(name, params, expr):
    """static inline u32 NAME(PARAMS) { return EXPR; }, PARAMS [(name, declarator type)]."""
    plist = ca.ParamList([ca.Decl(name=n, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[],
                                  type=_renamed(t, n), init=None, bitsize=None, asmlabel=None)
                          for n, t in params])
    ftype = ca.FuncDecl(plist, ca.TypeDecl(name, [], None, ca.IdentifierType(["u32"])))
    decl = ca.Decl(name=name, quals=[], align=[], storage=["static"], funcspec=["inline"], gcc_attributes=[],
                   type=ftype, init=None, bitsize=None, asmlabel=None)
    return ca.FuncDef(decl, None, ca.Compound([ca.Return(expr)]))


def sp_helper_arg(fn):
    """E op K  ->  H(K) with static inline u32 H(u32 k) { return E op k; }  (an inline helper's
    arguments are evaluated before its body: the ROM loads the constant before E). Locals E reads
    become further arguments."""
    local = {n.name: n.type for n, _, _ in walk(fn) if isinstance(n, ca.Decl) and n is not fn.decl}
    pred = lambda n, p, s: (isinstance(n, ca.BinaryOp) and n.op in ("&", "|", "^", "+", "-", "<<", ">>")
                            and isinstance(n.right, ca.Constant) and not isinstance(n.left, ca.Constant)
                            and not (isinstance(p, ca.Assignment) and s == "lvalue"))
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        b, par, slot = nth(f2, pred, k)
        if any(isinstance(n, (ca.FuncCall, ca.Assignment)) for n, _, _ in walk(b.left)):
            continue
        used = []
        for n, p, s in walk(b.left):
            if isinstance(n, ca.ID) and n.name in local and not (isinstance(p, ca.StructRef) and s == "field") \
                    and n.name not in used:
                used.append(n.name)
        name = new_name(f2, "Arg")
        kname = "k_"
        h = _helper(name, [(kname, ca.TypeDecl(kname, [], None, ca.IdentifierType(["u32"])))] +
                    [(u, local[u]) for u in used],
                    ca.BinaryOp(b.op, copy.deepcopy(b.left), ca.ID(kname)))
        call = ca.FuncCall(ca.ID(name), ca.ExprList([b.right] + [ca.ID(u) for u in used]))
        set_child(par, slot, call)
        yield f"{text(b)[:36]} as an inline helper taking {text(b.right)}", (f2, [h])


def sp_specialize(fn, helpers):
    """H(a, 0, 0) with H an inline helper  ->  H_(a) with the constants written in H's body (the
    original's helper took fewer arguments: its constants are loaded where the body uses them, not
    before it). Each result is also tried with the body's shared-constant spellings."""
    pred = lambda n, p, s: (isinstance(n, ca.FuncCall) and isinstance(n.name, ca.ID) and n.name.name in helpers
                            and n.args and any(isinstance(a, ca.Constant) for a in n.args.exprs))
    import itertools
    for k in range(sites(fn, pred)):
        c0, _, _ = nth(fn, pred, k)
        consts = [i for i, a in enumerate(c0.args.exprs) if isinstance(a, ca.Constant)]
        subsets = [set(x) for r in range(len(consts), 0, -1) for x in itertools.combinations(consts, r)][:8]
        for sub in subsets:
            f2 = copy.deepcopy(fn)
            c, _, _ = nth(f2, pred, k)
            h = copy.deepcopy(helpers[c.name.name])
            params = h.decl.type.args.params
            if len(params) != len(c.args.exprs):
                break
            name = new_name(f2, c.name.name + "_")
            h.decl.name = name
            for n, _, _ in walk(h.decl.type):
                if isinstance(n, ca.TypeDecl) and n.declname == c.name.name:
                    n.declname = name
            keep_p, keep_a = [], []
            for i, (prm, a) in enumerate(zip(params, c.args.exprs)):
                if i in sub and not any(isinstance(n, ca.Assignment) and text(n.lvalue) == prm.name
                                        for n, _, _ in walk(h.body)):
                    for r in reads_of(h.body, prm.name):
                        set_child(r[1], r[2], copy.deepcopy(a))
                else:
                    keep_p.append(prm)
                    keep_a.append(a)
            h.decl.type.args.params = keep_p or [ca.Typename(None, [], None, [], ca.TypeDecl(
                None, [], None, ca.IdentifierType(["void"])))]
            c.name.name = name
            c.args.exprs = keep_a
            desc = f"{text(c)[:30]}: constants written in the helper"
            yield desc, (f2, [h])
            for sp in (sp_addr_then_const, sp_scoped_operand, sp_inline_decl, sp_addr_local):
                for d2, h2 in sp(h):
                    yield desc + "; " + d2, (f2, [h2])


SIZES = {"u8": 1, "s8": 1, "u16": 2, "s16": 2, "u32": 4, "s32": 4}


def sp_split_offset(fn):
    """*(T *)(X + K)  ->  ((T *)(X + K - m*sizeof(T)))[m], or the same through a local pointer
    (the ROM adds part of the offset first: `adds rB, #A; ldr rD, [rB, #m*size]`)."""
    def pred(n, p, s):
        return (isinstance(n, ca.UnaryOp) and n.op == "*" and isinstance(n.expr, ca.Cast)
                and isinstance(n.expr.expr, ca.BinaryOp) and n.expr.expr.op == "+"
                and isinstance(n.expr.expr.right, ca.Constant))
    for k in range(sites(fn, pred)):
        u, _, _ = nth(fn, pred, k)
        tt = text(u.expr.to_type).replace("*", "").strip()
        size = SIZES.get(tt)
        if not size:
            continue
        kval = int(u.expr.expr.right.value.rstrip("uUlL"), 0)
        for m in range(1, 9):
            k1 = kval - m * size
            if k1 < 0:
                break
            for local in (False, True):
                f2 = copy.deepcopy(fn)
                u2, p2, s2 = nth(f2, pred, k)
                base = copy.deepcopy(u2.expr)
                base.expr.right = ca.Constant("int", hex(k1).upper().replace("0X", "0x"))
                if local:
                    v = new_name(f2, "d")
                    d = ca.Decl(name=v, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[],
                                type=_renamed(base.to_type.type, v), init=base, bitsize=None, asmlabel=None)
                    set_child(p2, s2, ca.ArrayRef(ca.ID(v), ca.Constant("int", str(m))))
                    if not _put_before(f2, p2 if not isinstance(p2, ca.ArrayRef) else p2, d):
                        continue
                else:
                    set_child(p2, s2, ca.ArrayRef(base, ca.Constant("int", str(m))))
                yield f"offset {hex(kval)} as {hex(k1)} + [{m}]{' through a local' if local else ''}", f2


def sp_addr_local(fn):
    """L op= E  ->  { typeof(L) *q = &L; *q op= E; }  (the ROM computes the address before the value)."""
    pred = lambda n, p, s: (isinstance(n, ca.Assignment) and isinstance(n.lvalue, (ca.StructRef, ca.ArrayRef))
                            and not (isinstance(n.lvalue, ca.StructRef) and n.lvalue.type == "." and
                                     isinstance(n.lvalue.name, ca.ID)))
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        a, _, _ = nth(f2, pred, k)
        q = new_name(f2, "q")
        lv = a.lvalue
        qd = ca.Decl(name=q, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[],
                     type=ca.PtrDecl([], ca.TypeDecl(q, [], None, ca.Typeof(copy.deepcopy(lv)))),
                     init=ca.UnaryOp("&", copy.deepcopy(lv)), bitsize=None, asmlabel=None)
        a.lvalue = ca.UnaryOp("*", ca.ID(q))
        if _put_before(f2, a, qd):
            yield f"address of {text(lv)[:30]} in a local first", f2


def sp_goto_loop(fn):
    """for (i = a; c; i++) body  ->  i = a; if (!c) goto E; L: body; i++; if (c) goto L; E:;
    loop.c only optimizes loops it sees as loops: when the ROM recomputes an invariant address
    inside the loop and does no strength reduction, the original loop was written with goto."""
    pred = lambda n, p, s: isinstance(n, (ca.For, ca.While)) and not any(
        isinstance(x, (ca.Continue, ca.Break)) for x, _, _ in walk(n.stmt))
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        lp, par, slot = nth(f2, pred, k)
        top, end = new_name(f2, "loop"), new_name(f2, "done")
        cond = lp.cond or ca.Constant("int", "1")
        items = []
        if isinstance(lp, ca.For) and lp.init is not None:
            items += lp.init.decls if isinstance(lp.init, ca.DeclList) else [lp.init]
        items.append(ca.If(ca.UnaryOp("!", copy.deepcopy(cond)), ca.Goto(end), None))
        body = lp.stmt.block_items or [] if isinstance(lp.stmt, ca.Compound) else [lp.stmt]
        items.append(ca.Label(top, ca.Compound(list(body))))
        if isinstance(lp, ca.For) and lp.next is not None:
            items.append(lp.next)
        items.append(ca.If(copy.deepcopy(cond), ca.Goto(top), None))
        items.append(ca.Label(end, ca.EmptyStatement()))
        set_child(par, slot, ca.Compound(items))
        yield f"loop {k + 1} written with goto (no loop optimization)", f2


def sp_addr_before_next(fn):
    """L = E; S;  ->  { T *q = &L; S; *q = E; }  for S a simple assignment or declaration (the ROM
    computes L's address, then S's value, then stores: the address was taken in a local first)."""
    for bi, b in enumerate(blocks(fn)):
        items = b.block_items or []
        for i in range(len(items) - 1):
            st, nx = items[i], items[i + 1]
            if not (isinstance(st, ca.Assignment) and st.op == "=" and
                    isinstance(st.lvalue, (ca.ArrayRef, ca.StructRef, ca.UnaryOp))):
                continue
            if not (isinstance(nx, ca.Assignment) and isinstance(nx.lvalue, ca.ID) or isinstance(nx, ca.Decl)):
                continue
            f2 = copy.deepcopy(fn)
            it = blocks(f2)[bi].block_items
            st2, nx2 = it[i], it[i + 1]
            q = new_name(f2, "q")
            lv = st2.lvalue
            qd = ca.Decl(name=q, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[],
                         type=ca.PtrDecl([], ca.TypeDecl(q, [], None, ca.Typeof(copy.deepcopy(lv)))),
                         init=ca.UnaryOp("&", copy.deepcopy(lv)), bitsize=None, asmlabel=None)
            st2.lvalue = ca.UnaryOp("*", ca.ID(q))
            if isinstance(nx2, ca.Decl):  # keep the declaration in scope for what follows
                it[i:i + 2] = [nx2, ca.Compound([qd, st2])]
                nx2.init, init = None, nx2.init
                it[i + 1].block_items.insert(1, ca.Assignment("=", ca.ID(nx2.name), init))
            else:
                it[i:i + 2] = [ca.Compound([qd, nx2, st2])]
            yield f"address of {text(lv)[:24]} before `{text(nx)[:24]}`", f2


def sp_local_to_arg(fn, helpers):
    """H(p, ...) whose body starts with T *a = <expr of its parameters>;  ->  H_(p, <expr>, ...) taking a
    as an argument (an inline helper's arguments are evaluated before its body: the ROM computes
    the value before the helper's first statement and its other arguments)."""
    pred = lambda n, p, s: (isinstance(n, ca.FuncCall) and isinstance(n.name, ca.ID) and n.name.name in helpers
                            and n.args is not None)
    for k in range(sites(fn, pred)):
        c0, _, _ = nth(fn, pred, k)
        h0 = helpers[c0.name.name]
        params = [x.name for x in h0.decl.type.args.params if isinstance(x, ca.Decl)]
        items = h0.body.block_items or []
        for di, d in enumerate(items):
            if not isinstance(d, ca.Decl):
                break
            if d.init is None:
                continue
            ids = [x for x, par, sl in walk(d.init) if isinstance(x, ca.ID)
                   and not (isinstance(par, ca.StructRef) and sl == "field")]
            if d.init is None or not ids or not all(x.name in params for x in ids):
                continue
            for pos in range(len(params) + 1):
                f2 = copy.deepcopy(fn)
                c, _, _ = nth(f2, pred, k)
                h = copy.deepcopy(h0)
                name = new_name(f2, c.name.name + "_")
                h.decl.name = name
                for n, _, _ in walk(h.decl.type):
                    if isinstance(n, ca.TypeDecl) and n.declname == c.name.name:
                        n.declname = name
                dd = h.body.block_items.pop(di)
                arg = copy.deepcopy(dd.init)
                for r in [x for x in walk(arg) if isinstance(x[0], ca.ID) and x[0].name in params
                          and not (isinstance(x[1], ca.StructRef) and x[2] == "field")]:
                    i = params.index(r[0].name)
                    if r[1] is None:
                        arg = copy.deepcopy(c.args.exprs[i])
                    else:
                        set_child(r[1], r[2], copy.deepcopy(c.args.exprs[i]))
                newp = ca.Decl(name=dd.name, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[],
                               type=dd.type, init=None, bitsize=None, asmlabel=None)
                h.decl.type.args.params.insert(pos, newp)
                c.args.exprs.insert(pos, arg)
                c.name.name = name
                yield f"{dd.name} of {h0.decl.name} as argument {pos + 1}", (f2, [h])


def sp_cmp_sign(fn):
    """a < b  ->  (s32)a < b / (u32)a < b  (the branch's signedness is the comparison's type)."""
    pred = lambda n, p, s: isinstance(n, ca.BinaryOp) and n.op in CMP
    for k in range(sites(fn, pred)):
        for t in ("s32", "u32"):
            f2 = copy.deepcopy(fn)
            b, _, _ = nth(f2, pred, k)
            if isinstance(b.left, ca.Cast) and text(b.left.to_type) == t:
                continue
            b.left = ca.Cast(tname(t), b.left)
            yield f"compare {text(b)[:40]} as {t}", f2


def sp_call_narrow(fn):
    """f(x) used as a value  ->  (u8)f(x) / (u16)f(x)  (the callee returns a narrow type)."""
    pred = lambda n, p, s: isinstance(n, ca.FuncCall) and not isinstance(p, (ca.Compound, ca.Cast))
    for k in range(sites(fn, pred)):
        for t in ("u8", "u16", "s8", "s16"):
            f2 = copy.deepcopy(fn)
            c, p, s = nth(f2, pred, k)
            set_child(p, s, ca.Cast(tname(t), c))
            yield f"({t}){text(c.name)}(...)", f2


def sp_ternary(fn):
    """v = A; if (c) v = B;   or   if (c) v = B; else v = A;   ->   v = c ? B : A."""
    for bi, b in enumerate(blocks(fn)):
        items = b.block_items
        for i, st in enumerate(items):
            if not isinstance(st, ca.If):
                continue
            t = _assign_in(st.iftrue)
            if t is None:
                continue
            e = _assign_in(st.iffalse) if st.iffalse is not None else None
            if e is not None and text(e.lvalue) == text(t.lvalue):
                f2 = copy.deepcopy(fn)
                b2 = blocks(f2)[bi]
                s2 = b2.block_items[i]
                t2, e2 = _assign_in(s2.iftrue), _assign_in(s2.iffalse)
                b2.block_items[i] = ca.Assignment("=", t2.lvalue, ca.TernaryOp(s2.cond, t2.rvalue, e2.rvalue))
                yield f"{text(t.lvalue)} = c ? B : A (if/else)", f2
            elif st.iffalse is None and i > 0 and isinstance(items[i - 1], ca.Assignment) and \
                    items[i - 1].op == "=" and text(items[i - 1].lvalue) == text(t.lvalue):
                f2 = copy.deepcopy(fn)
                b2 = blocks(f2)[bi]
                a2, s2 = b2.block_items[i - 1], b2.block_items[i]
                t2 = _assign_in(s2.iftrue)
                b2.block_items[i - 1:i + 1] = [ca.Assignment("=", t2.lvalue, ca.TernaryOp(s2.cond, t2.rvalue,
                                                                                          a2.rvalue))]
                yield f"{text(t.lvalue)} = c ? B : A (default then if)", f2


def sp_into_loop(fn):
    """S; for (...) { body }  ->  for (...) { ...; S; ... }  (loop.c hoists invariants in body order,
    so a value the ROM loads among the hoisted ones was written inside the body)."""
    loops = (ca.For, ca.While, ca.DoWhile)
    for bi, b in enumerate(blocks(fn)):
        items = b.block_items
        for li, lp in enumerate(items):
            if not isinstance(lp, loops) or not isinstance(lp.stmt, ca.Compound):
                continue
            for back in (1, 2, 3):
                si = li - back
                if si < 0 or not isinstance(items[si], ca.Assignment):
                    break
                n = len(lp.stmt.block_items or [])
                for pos in range(n + 1):
                    f2 = copy.deepcopy(fn)
                    b2 = blocks(f2)[bi]
                    st = b2.block_items[si]
                    lp2 = b2.block_items[li]
                    lp2.stmt.block_items.insert(pos, st)
                    del b2.block_items[si]
                    yield f"move `{text(st)[:30]}` into the loop body at {pos}", f2


def sp_flag_take(fn, helper="TakeFlag_"):
    """if (f) f = 0;  ->  if (TakeFlag(&f)) {}  (the helper's join ends cse's extended block, so values
    are not carried across the flag test)."""
    def pred(n, p, s):
        if not isinstance(n, ca.If) or n.iffalse is not None:
            return False
        a = _assign_in(n.iftrue)
        return a is not None and text(a.lvalue) == text(n.cond) and isinstance(a.rvalue, ca.Constant) \
            and a.rvalue.value in ("0", "0x0")
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        n, _, _ = nth(f2, pred, k)
        n.cond = ca.FuncCall(ca.ID(helper), ca.ExprList([ca.UnaryOp("&", n.cond)]))
        n.iftrue = ca.Compound([])
        yield f"take the flag through {helper} with an empty if", f2


def _same(a, b):
    return a is not None and b is not None and text(a) == text(b)


def sp_addr_then_const(fn):
    """L1 = x; L2 = K; L3 = K ...  ->  { T *q = &L1; u32 z = K; *q = x; L2 = z; L3 = z; ... }
    (the first address is evaluated before the shared constant)."""
    for bi, b in enumerate(blocks(fn)):
        items = b.block_items
        stores = [i for i, st in enumerate(items) if isinstance(st, ca.Assignment) and st.op == "="
                  and isinstance(st.lvalue, (ca.ArrayRef, ca.StructRef, ca.UnaryOp))]
        if len(stores) < 2:
            continue
        consts = {}
        for i in stores:
            r = items[i].rvalue
            if isinstance(r, ca.Constant):
                consts.setdefault(r.value, []).append(i)
        for kval, uses in consts.items():
            later = [i for i in stores if i < uses[0]]
            for first in sorted(set(later[-1:] + stores[:1])):
                for zt in ("u32", "u8"):
                    f2 = copy.deepcopy(fn)
                    b2 = blocks(f2)[bi]
                    it = b2.block_items
                    q, z = new_name(f2, "q"), new_name(f2, "z")
                    lv = it[first].lvalue
                    qd = ca.Decl(name=q, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[],
                                 type=ca.PtrDecl([], ca.TypeDecl(q, [], None, ca.Typeof(copy.deepcopy(lv)))),
                                 init=ca.UnaryOp("&", copy.deepcopy(lv)), bitsize=None, asmlabel=None)
                    zd = int_decl(z, zt, ca.Constant("int", kval))
                    body = []
                    for j in range(first, len(it)):
                        st = it[j]
                        if j == first:
                            st.lvalue = ca.UnaryOp("*", ca.ID(q))
                        if isinstance(st, ca.Assignment) and isinstance(st.rvalue, ca.Constant) and \
                                st.rvalue.value == kval:
                            st.rvalue = ca.ID(z)
                        body.append(st)
                    b2.block_items = it[:first] + [ca.Compound([qd, zd] + body)]
                    yield f"address of {text(lv)[:24]} before a shared {zt} {kval}", f2


def sp_inline_decl(fn):
    """T v = E; ... v ...  ->  ... (E) ...  for E a constant, a variable or a cast of one: an alias costs
    a register copy, a constant local moves where the constant is loaded."""
    def pred(n, p, s):
        if not (isinstance(n, ca.Decl) and isinstance(p, ca.Compound) and n.init is not None):
            return False
        e = n.init
        while isinstance(e, ca.Cast):
            e = e.expr
        return isinstance(e, (ca.ID, ca.Constant))
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        d, par, _ = nth(f2, pred, k)
        if any(isinstance(n, ca.Assignment) and text(n.lvalue) == d.name or
               isinstance(n, ca.UnaryOp) and n.op in ("++", "--", "p++", "p--", "&") and text(n.expr) == d.name
               for n, _, _ in walk(par)):
            continue
        init = d.init
        par.block_items.remove(d)
        for r in reads_of(par, d.name):
            set_child(r[1], r[2], copy.deepcopy(init))
        yield f"inline {d.name} = {text(init)[:30]}", f2


def sp_scoped_operand(fn):
    """L op= K / L = E op K  ->  { T k = K; L op= k; }  (the constant is loaded before the address and
    the value: a local initialized at that point)."""
    def pred(n, p, s):
        return isinstance(n, ca.Constant) and n.type == "int" and not isinstance(p, ca.ArrayRef)
    stmts = []
    for b in blocks(fn):
        for st in b.block_items or []:
            if isinstance(st, ca.Assignment):
                stmts.append(st)
    for si in range(len(stmts)):
        for ci, (c, _, _) in enumerate([x for x in walk(stmts[si].rvalue) if pred(*x)]):
            for t in ("u8", "u16", "u32"):
                f2 = copy.deepcopy(fn)
                st2 = [st for b in blocks(f2) for st in b.block_items or [] if isinstance(st, ca.Assignment)][si]
                c2, p2, s2 = [x for x in walk(st2.rvalue) if pred(*x)][ci]
                found = _enclosing_stmt(f2, st2)
                if not found:
                    continue
                blk, _ = found
                v = new_name(f2, "k")
                if p2 is None:
                    st2.rvalue = ca.ID(v)
                else:
                    set_child(p2, s2, ca.ID(v))
                blk.block_items[blk.block_items.index(st2)] = ca.Compound([int_decl(v, t, c2), st2])
                yield f"{t} local for {c.value} in `{text(stmts[si])[:30]}`", f2


def sp_nonvoid(fn):
    """void f()  ->  u32 f() with no return statement (the ROM's epilogue keeps r0: pop {r1})."""
    ft = fn.decl.type
    if isinstance(ft, ca.FuncDecl) and isinstance(ft.type, ca.TypeDecl) and \
            getattr(ft.type.type, "names", None) == ["void"]:
        f2 = copy.deepcopy(fn)
        f2.decl.type.type = ca.TypeDecl(f2.decl.name, [], None, ca.IdentifierType(["u32"]))
        yield "non-void with no return statement", f2


def sp_index_local(fn):
    """a[i] with i an expression  ->  { T j = i; ... a[j] ... }  (the index evaluated first)."""
    pred = lambda n, p, s: isinstance(n, ca.ArrayRef) and not isinstance(n.subscript, (ca.ID, ca.Constant))
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        a, _, _ = nth(f2, pred, k)
        v = new_name(f2, "j")
        d = int_decl(v, "u32", a.subscript)
        a.subscript = ca.ID(v)
        if not _put_before(f2, a, d):
            continue
        yield f"index of {text(a.name)[:24]} in a local first", f2


SPELLINGS = [sp_cmp_sign, sp_call_narrow, sp_ternary, sp_into_loop, sp_flag_take, sp_addr_then_const,
             sp_inline_decl, sp_scoped_operand, sp_nonvoid, sp_index_local, sp_helper_arg,
             sp_split_offset, sp_addr_local, sp_goto_loop,
             sp_addr_before_next]

# decision -> spellings and generic rewrites to try first (names)
DECISIONS = {
    "frame": ["sp_nonvoid", "rw_void"],
    "call result": ["sp_call_narrow"],
    "comparison type": ["sp_cmp_sign", "rw_width"],
    "order": ["sp_helper_arg", "sp_specialize", "sp_local_to_arg", "sp_addr_before_next", "sp_addr_local", "sp_scoped_operand", "sp_addr_then_const", "sp_into_loop", "sp_index_local", "sp_inline_decl",
              "rw_temp_rvalue", "rw_hoist_args", "rw_swap_stmts", "rw_decl_order", "rw_scoped_const"],
    "loop-invariant": ["sp_goto_loop", "sp_into_loop", "sp_inline_decl"],
    "strength-reduce": ["sp_goto_loop", "sp_into_loop", "perm_loop_form"],
    "cse-skip": ["sp_flag_take", "sp_inline_decl", "rw_copy_var"],
    "cse-reuse": ["sp_split_offset", "sp_flag_take", "sp_inline_decl", "rw_copy_var", "rw_reuse_stored"],
    "cse-swap": ["rw_commute", "sp_scoped_operand"],
    "regmove-copy": ["rw_commute", "sp_inline_decl"],
    "local-alloc": ["sp_inline_decl", "sp_ternary", "rw_width", "rw_decl_order", "rw_copy_var", "rw_inline_var"],
    "global-alloc": ["sp_inline_decl", "sp_ternary", "rw_width", "rw_decl_order", "rw_copy_var"],
    "move2add": ["sp_specialize", "sp_addr_then_const", "sp_scoped_operand", "rw_share_const"],
    "reload": ["sp_specialize", "sp_addr_then_const", "sp_inline_decl", "rw_share_const"],
}


def all_spellings(fn):
    for sp in SPELLINGS:
        try:
            for desc, f2 in sp(fn):
                yield sp.__name__, desc, f2
        except Exception:
            continue

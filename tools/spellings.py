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
from rewrites import (_enclosing_stmt, add_local, blocks, int_decl, is_stmt_assign, new_name, nth, reads_of, typeof_decl,
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
    if isinstance(st, ca.Decl):
        # wrapping a declaration in a block would end its scope: put DECL before it instead
        if not isinstance(p, ca.Compound):
            return False
        p.block_items.insert(p.block_items.index(st), decl)
        return True
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
                # and straight into its one use: `v = A; if (c) v = B; x = v;` -> `x = c ? B : A;`
                # (ActorPhase_ApproachNeutral: the ternary alone is neutral, so the beam drops it)
                nxt = b2.block_items[i] if i < len(b2.block_items) else None
                v = t2.lvalue
                if isinstance(v, ca.ID) and isinstance(nxt, ca.Assignment) and nxt.op == "=" and \
                        text(nxt.rvalue) == v.name and sum(isinstance(x, ca.ID) and x.name == v.name and not (
                        isinstance(par, ca.StructRef) and slot == "field") for x, par, slot in walk(f2)) == 2:
                    f3 = copy.deepcopy(f2)
                    b3 = blocks(f3)[bi]
                    tern, use = b3.block_items[i - 1], b3.block_items[i]
                    use.rvalue = tern.rvalue
                    del b3.block_items[i - 1]
                    for b4 in blocks(f3):
                        b4.block_items = [d for d in b4.block_items
                                          if not (isinstance(d, ca.Decl) and d.name == v.name)]
                    yield f"{text(nxt.lvalue)[:24]} = c ? B : A (default then if, {v.name} inlined)", f3


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
                st0 = items[si]
                # sound only for an invariant store: its operands and target unchanged by the loop,
                # nothing between it and its new place reading the target, no side effects
                if st0.op != "=" or any(isinstance(x, (ca.FuncCall, ca.Assignment)) or
                                        isinstance(x, ca.UnaryOp) and x.op in ("++", "--", "p++", "p--")
                                        for x, _, _ in walk(st0.rvalue)):
                    continue
                target = text(st0.lvalue)
                names = {x.name for x, _, _ in walk(st0.rvalue) if isinstance(x, ca.ID)}
                if isinstance(st0.lvalue, ca.ID):
                    names.add(st0.lvalue.name)
                elif any(isinstance(x, ca.Assignment) and not isinstance(x.lvalue, ca.ID) for x, _, _ in walk(lp)):
                    continue  # a store through memory in the loop could change the target
                if any(_assigns(lp, nm) for nm in names):
                    continue
                mem = not isinstance(st0.lvalue, ca.ID)  # a callee can read a store through memory

                def reads(node):
                    return node is not None and any(text(x) == target or mem and isinstance(x, ca.FuncCall)
                                                    for x, _, _ in walk(node))
                if any(reads(x) for x in items[si + 1:li]):
                    continue
                if isinstance(lp, (ca.For, ca.While)):
                    if reads(lp.cond) or reads(getattr(lp, "next", None)):
                        continue
                    rest = _after(fn, lp)  # a loop that may not run must not hold what follows it
                    if rest is None or any(reads(x) for x in rest):
                        continue
                body0 = lp.stmt.block_items or []
                n = next((k for k, x in enumerate(body0) if reads(x)), len(body0))
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
                if not _fits(c, t):
                    continue
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


def sp_loop_weight(fn):
    """S;  ->  do { S; } while (0)  for an assignment or call statement S. flow counts a pseudo's
    references weighted by loop depth (REG_N_REFS += loop_depth), and do/while(0) still emits
    loop notes, so this doubles the weight of every register S sets or uses in the allocation
    priority (refs * log2 refs / life) without changing the code. It raises a variable that the
    ROM allocates ahead of a competitor (rasolve's "raise priority"); sub_08136F58 matched so."""
    def pred(n, p, s):
        return isinstance(n, (ca.Assignment, ca.FuncCall, ca.UnaryOp)) and isinstance(p, ca.Compound)
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        a, par, s = nth(f2, pred, k)
        set_child(par, s, ca.DoWhile(ca.Constant("int", "0"), ca.Compound([a])))
        yield f"do {{ {text(a)[:30]}; }} while (0)", f2


# --- spellings harvested from history (tools/harvest.py cases, October 24) -----------------

def _ids(node, name):
    """ID nodes named NAME in NODE (struct field names excluded)."""
    return [n for n, p, s in walk(node) if isinstance(n, ca.ID) and n.name == name
            and not (isinstance(p, ca.StructRef) and s == "field")]


def _is_const(n, value=None):
    if not isinstance(n, ca.Constant) or n.type != "int":
        return False
    try:
        v = int(n.value.rstrip("uUlL"), 0)
    except ValueError:
        return False
    return value is None or v == value


def _fits(c, t):
    """Whether constant C keeps its value in integer type T."""
    try:
        v = int(c.value.rstrip("uUlL"), 0)
    except ValueError:
        return False
    lo, hi = {"u8": (0, 255), "s8": (-128, 127), "u16": (0, 65535), "s16": (-32768, 32767)}.get(t, (-2 ** 31, 2 ** 32))
    return lo <= v <= hi


def _cint(n):
    return int(n.value.rstrip("uUlL"), 0)


def _assigns(node, name):
    """Statements in NODE that change variable NAME (assignment, ++/--, address taken)."""
    out = []
    for n, p, s in walk(node):
        if isinstance(n, ca.Assignment) and isinstance(n.lvalue, ca.ID) and n.lvalue.name == name:
            out.append(n)
        elif isinstance(n, ca.UnaryOp) and n.op in ("++", "--", "p++", "p--", "&") and \
                isinstance(n.expr, ca.ID) and n.expr.name == name:
            out.append(n)
    return out


def _countdown(items, i):
    """(var, K, init index or None) when items[i] is `do { body } while (--v >= 0)` (or with a trailing
    `v--;` and `while (v >= 0)`), v set to the constant K just before and not read in the body."""
    lp = items[i]
    if not isinstance(lp, ca.DoWhile) or i == 0:
        return None
    c = lp.cond
    body = lp.stmt.block_items if isinstance(lp.stmt, ca.Compound) else [lp.stmt]
    if not (isinstance(c, ca.BinaryOp) and c.op == ">=" and _is_const(c.right, 0)):
        return None
    if isinstance(c.left, ca.UnaryOp) and c.left.op == "--" and isinstance(c.left.expr, ca.ID):
        v, rest = c.left.expr.name, body
    elif isinstance(c.left, ca.ID) and body and isinstance(body[-1], ca.UnaryOp) and \
            body[-1].op in ("--", "p--") and isinstance(body[-1].expr, ca.ID) and body[-1].expr.name == c.left.name:
        v, rest = c.left.name, body[:-1]
    else:
        return None
    prev = items[i - 1]
    if isinstance(prev, ca.Assignment) and prev.op == "=" and isinstance(prev.lvalue, ca.ID) and \
            prev.lvalue.name == v and _is_const(prev.rvalue):
        k = _cint(prev.rvalue)
    elif isinstance(prev, ca.Decl) and prev.name == v and prev.init is not None and _is_const(prev.init):
        k = _cint(prev.init)
    else:
        return None
    if any(_ids(st, v) for st in rest) or k < 0:
        return None
    return v, k, rest


def sp_count_up(fn):
    """v = K; do { body } while (--v >= 0);  ->  for (v = 0; v < K + 1; v++) body   (body not reading v;
    harvested from Registry3F74_InitAnimations and ResultPanel_ClearFiveTiles)."""
    for bi, b in enumerate(blocks(fn)):
        for i in range(len(b.block_items)):
            m = _countdown(b.block_items, i)
            if not m:
                continue
            f2 = copy.deepcopy(fn)
            it = blocks(f2)[bi].block_items
            v, k, _ = m
            _, _, rest = _countdown(it, i)
            prev = it[i - 1]
            lp = ca.For(ca.Assignment("=", ca.ID(v), ca.Constant("int", "0")),
                        ca.BinaryOp("<", ca.ID(v), ca.Constant("int", str(k + 1))),
                        ca.UnaryOp("p++", ca.ID(v)), ca.Compound(list(rest)))
            if isinstance(prev, ca.Decl):
                prev.init = None
                it[i] = lp
            else:
                it[i - 1:i + 1] = [lp]
            yield f"count-down loop on {v} as for ({v} = 0; {v} < {k + 1}; {v}++)", f2


def sp_count_down(fn):
    """for (v = 0; v < N; v++) body  ->  v = N - 1; do { body } while (--v >= 0);  (body not reading v,
    v not read after the loop; the inverse of sp_count_up)."""
    def pred(n, p, s):
        if not (isinstance(n, ca.For) and isinstance(p, ca.Compound) and isinstance(n.init, ca.Assignment)
                and isinstance(n.init.lvalue, ca.ID) and _is_const(n.init.rvalue, 0)):
            return False
        v = n.init.lvalue.name
        c = n.cond
        if not (isinstance(c, ca.BinaryOp) and c.op in ("<", "<=") and isinstance(c.left, ca.ID)
                and c.left.name == v and _is_const(c.right)):
            return False
        if not (isinstance(n.next, ca.UnaryOp) and n.next.op in ("p++", "++") and isinstance(n.next.expr, ca.ID)
                and n.next.expr.name == v):
            return False
        return not _ids(n.stmt, v) and len(_ids(fn, v)) == 4 and \
            not any(isinstance(x, (ca.Continue, ca.Break)) for x, _, _ in walk(n.stmt))
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        lp, par, slot = nth(f2, pred, k)
        v = lp.init.lvalue.name
        n = _cint(lp.cond.right) + (1 if lp.cond.op == "<=" else 0)
        if n < 1:
            continue
        body = lp.stmt.block_items if isinstance(lp.stmt, ca.Compound) else [lp.stmt]
        loop = ca.DoWhile(ca.BinaryOp(">=", ca.UnaryOp("--", ca.ID(v)), ca.Constant("int", "0")),
                          ca.Compound(list(body or [])))
        i = par.block_items.index(lp)
        par.block_items[i:i + 1] = [ca.Assignment("=", ca.ID(v), ca.Constant("int", str(n - 1))), loop]
        yield f"for loop on {v} as a count-down do/while from {n - 1}", f2


def _counter(lp):
    """(v, body list, next stmt or None) for a loop whose counter v steps by one: a for with v++ as
    next, or a do/while/while whose body ends with v++."""
    body = lp.stmt.block_items if isinstance(lp.stmt, ca.Compound) else [lp.stmt]
    body = list(body or [])
    inc = lambda x: isinstance(x, ca.UnaryOp) and x.op in ("p++", "++") and isinstance(x.expr, ca.ID)
    if isinstance(lp, ca.For) and inc(lp.next):
        return lp.next.expr.name, body, None
    if isinstance(lp, (ca.DoWhile, ca.While, ca.For)) and body and inc(body[-1]):
        return body[-1].expr.name, body[:-1], body[-1]
    return None


def sp_ptr_walk(fn):
    """for (...; i++) { ... &A[i] ... A[i] ... }  ->  T *q = &A[0]; for (...) { ... q ... *q ...; q++; }
    (the original walked a pointer: ModifierStats_ApplyKind*, sub_08217C7C). A must not change in the loop."""
    loops = [n for n, _, _ in walk(fn) if isinstance(n, (ca.For, ca.While, ca.DoWhile))]
    for li in range(len(loops)):
        lp = loops[li]
        m = _counter(lp)
        if not m:
            continue
        v, body, _ = m
        if any(isinstance(x, ca.Continue) for x, _, _ in walk(lp.stmt)):
            continue
        if any(_assigns(st, v) for st in body):
            continue
        arrays = {}
        for n, p, s in walk(lp.stmt):
            if isinstance(n, ca.ArrayRef) and isinstance(n.subscript, ca.ID) and n.subscript.name == v and \
                    not any(isinstance(x, (ca.FuncCall, ca.Assignment)) for x, _, _ in walk(n.name)):
                arrays.setdefault(text(n.name), n.name)
        for key, arr in arrays.items():
            names = {x.name for x, _, _ in walk(arr) if isinstance(x, ca.ID)}
            if any(_assigns(lp.stmt, nm) for nm in names):
                continue
            f2 = copy.deepcopy(fn)
            lp2 = [n for n, _, _ in walk(f2) if isinstance(n, (ca.For, ca.While, ca.DoWhile))][li]
            found = None
            for n, p, s in walk(f2):
                if any(x is lp2 for x in getattr(n, "block_items", None) or []):
                    found = n
            if found is None:
                continue
            q = new_name(f2, "q")
            elem = ca.ArrayRef(copy.deepcopy(arr), ca.Constant("int", "0"))
            qd = ca.Decl(name=q, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[],
                         type=ca.PtrDecl([], ca.TypeDecl(q, [], None, ca.Typeof(copy.deepcopy(elem)))),
                         init=ca.UnaryOp("&", elem), bitsize=None, asmlabel=None)
            for n, p, s in list(walk(lp2.stmt)):
                if isinstance(n, ca.UnaryOp) and n.op == "&" and isinstance(n.expr, ca.ArrayRef) and \
                        text(n.expr.name) == key and isinstance(n.expr.subscript, ca.ID) and n.expr.subscript.name == v:
                    set_child(p, s, ca.ID(q))
            for n, p, s in list(walk(lp2.stmt)):
                if isinstance(n, ca.ArrayRef) and text(n.name) == key and isinstance(n.subscript, ca.ID) and \
                        n.subscript.name == v:
                    set_child(p, s, ca.UnaryOp("*", ca.ID(q)))
            if not isinstance(lp2.stmt, ca.Compound):
                lp2.stmt = ca.Compound([lp2.stmt])
            items = lp2.stmt.block_items
            items.insert(len(items) - (1 if m[2] is not None else 0), ca.UnaryOp("p++", ca.ID(q)))
            i = found.block_items.index(lp2)
            found.block_items.insert(i, qd)
            yield f"walk a pointer over {key[:24]} instead of indexing by {v}", f2


def sp_ptr_index(fn):
    """q = E; for (i = 0; ...; i++) { ... q ...; q += K; }  ->  for (...) { ... (E + i * K) ... }
    (the original indexed: sub_081C1434). q must not be read after the loop."""
    loops = [n for n, _, _ in walk(fn) if isinstance(n, ca.For)]
    for li, lp in enumerate(loops):
        if not isinstance(lp.init, ca.ExprList) or len(lp.init.exprs) != 2:
            continue
        a, b = lp.init.exprs
        if not (isinstance(a, ca.Assignment) and _is_const(a.rvalue, 0) and isinstance(a.lvalue, ca.ID)):
            a, b = b, a
        if not (isinstance(a, ca.Assignment) and _is_const(a.rvalue, 0) and isinstance(a.lvalue, ca.ID)
                and isinstance(b, ca.Assignment) and b.op == "=" and isinstance(b.lvalue, ca.ID)):
            continue
        i, q = a.lvalue.name, b.lvalue.name
        if not (isinstance(lp.next, ca.UnaryOp) and lp.next.op in ("p++", "++") and text(lp.next.expr) == i):
            continue
        body = lp.stmt.block_items if isinstance(lp.stmt, ca.Compound) else None
        if not body or not (isinstance(body[-1], ca.Assignment) and body[-1].op == "+=" and
                            text(body[-1].lvalue) == q and _is_const(body[-1].rvalue)):
            continue
        if any(_assigns(st, q) or _assigns(st, i) for st in body[:-1]):
            continue
        rest = _after(fn, lp)
        if rest is None:
            continue
        first = next((x for st in rest for x, p, s in walk(st) if (isinstance(x, ca.ID) and x.name == q)), None)
        if first is not None and not any(isinstance(st, ca.Assignment) and st.op == "=" and st.lvalue is first
                                         for st in rest):
            continue  # q is read after the loop (without being set again first)
        f2 = copy.deepcopy(fn)
        lp2 = [n for n, _, _ in walk(f2) if isinstance(n, ca.For)][li]
        a2, b2 = lp2.init.exprs if text(lp2.init.exprs[0].lvalue) == i else lp2.init.exprs[::-1]
        k = lp2.stmt.block_items[-1].rvalue
        lp2.stmt.block_items = lp2.stmt.block_items[:-1]
        for n, p, s in list(walk(lp2.stmt)):
            if isinstance(n, ca.ID) and n.name == q and not (isinstance(p, ca.StructRef) and s == "field"):
                set_child(p, s, ca.BinaryOp("+", copy.deepcopy(b2.rvalue),
                                            ca.BinaryOp("*", ca.ID(i), copy.deepcopy(k))))
        lp2.init = a2
        yield f"index {q} as {text(b2.rvalue)[:20]} + {i} * {text(k)} instead of stepping it", f2


def sp_rmw_temp(fn):
    """L op= E  ->  { s32 t = L op E; L = t; }  (sub_0821FB8C: the value computed in its own variable
    before the store)."""
    ops = ("|=", "&=", "+=", "-=", "^=", "<<=", ">>=")
    pred = lambda n, p, s: isinstance(n, ca.Assignment) and n.op in ops and isinstance(p, ca.Compound)
    for k in range(sites(fn, pred)):
        for tn in ("s32", "u32", None):
            f2 = copy.deepcopy(fn)
            a, par, _ = nth(f2, pred, k)
            t = new_name(f2, "v")
            val = ca.BinaryOp(a.op[:-1], copy.deepcopy(a.lvalue), a.rvalue)
            d = int_decl(t, tn, val) if tn else typeof_decl(t, a.lvalue, val)
            par.block_items[par.block_items.index(a)] = ca.Compound([d, ca.Assignment("=", a.lvalue, ca.ID(t))])
            yield f"{text(a.lvalue)[:24]} {a.op} through a {tn or 'typeof'} temporary", f2


def sp_chain_assign(fn):
    """a = K; b = K;  ->  a = b = K;  (sub_08222A04: b is stored first, from one register)."""
    for bi, b in enumerate(blocks(fn)):
        it = b.block_items
        for i in range(len(it) - 1):
            x, y = it[i], it[i + 1]
            if not all(isinstance(z, ca.Assignment) and z.op == "=" and not isinstance(z.rvalue, ca.Assignment)
                       for z in (x, y)):
                continue
            if text(x.rvalue) != text(y.rvalue) or not isinstance(x.rvalue, (ca.Constant, ca.ID)):
                continue
            for first, second in ((x, y), (y, x)):
                f2 = copy.deepcopy(fn)
                it2 = blocks(f2)[bi].block_items
                f, s_ = (it2[i], it2[i + 1]) if first is x else (it2[i + 1], it2[i])
                it2[i:i + 2] = [ca.Assignment("=", f.lvalue, ca.Assignment("=", s_.lvalue, s_.rvalue))]
                yield f"{text(f.lvalue)[:16]} = {text(s_.lvalue)[:16]} = {text(s_.rvalue)}", f2


def sp_fold_copy(fn):
    """v = E1; v op= E2;  ->  v = E1 op E2;  (SceneActorControl_UpdateHeightReturn: `delta = ground;
    delta -= y;`. cse.c make_regs_eqv makes the copy's destination the canonical register of E1's
    value when it outlives the block, so E1's computation moves to the copy; with no copy E1 keeps
    its own pseudo and place. The `cse-canon` decision, rtlprobe.py canon)."""
    for bi, b in enumerate(blocks(fn)):
        it = b.block_items
        for i in range(len(it) - 1):
            x, y = it[i], it[i + 1]
            if not (isinstance(x, ca.Assignment) and x.op == "=" and isinstance(x.lvalue, ca.ID)
                    and isinstance(y, ca.Assignment) and y.op in ("+=", "-=", "|=", "&=", "^=", "<<=", ">>=")
                    and isinstance(y.lvalue, ca.ID) and y.lvalue.name == x.lvalue.name
                    and not isinstance(x.rvalue, ca.Assignment)
                    and not any(isinstance(n, ca.ID) and n.name == x.lvalue.name for n, _, _ in walk(y.rvalue))):
                continue
            f2 = copy.deepcopy(fn)
            it2 = blocks(f2)[bi].block_items
            x2, y2 = it2[i], it2[i + 1]
            it2[i:i + 2] = [ca.Assignment("=", x2.lvalue, ca.BinaryOp(y2.op[:-1], x2.rvalue, y2.rvalue))]
            yield f"{x2.lvalue.name} = {text(x2.rvalue)[:16]} {y2.op[:-1]} {text(y2.rvalue)[:16]} in one assignment", f2


def sp_sign_shift(fn):
    """E < 0  ->  (u32)E >> 31,  E >= 0  ->  !((u32)E >> 31)  (sub_08222A04: the sign bit tested by
    a shift, not a compare)."""
    pred = lambda n, p, s: isinstance(n, ca.BinaryOp) and n.op in ("<", ">=") and _is_const(n.right, 0)
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        c, par, slot = nth(f2, pred, k)
        sh = ca.BinaryOp(">>", ca.Cast(tname("u32"), c.left), ca.Constant("int", "31"))
        set_child(par, slot, sh if c.op == "<" else ca.UnaryOp("!", sh))
        yield f"sign of {text(c.left)[:24]} by a shift", f2


def sp_field_extend(fn):
    """(E << k) >> k  <->  E & (2^(32-k) - 1)  (Ghost_TurnTowardHeading: `((cur - want) & 7) <= 4`, the
    octant difference masked, not sign-extended). A type hypothesis: a field read signed or unsigned."""
    def shifted(n):
        return isinstance(n, ca.BinaryOp) and n.op == ">>" and isinstance(n.left, ca.BinaryOp) and \
            n.left.op == "<<" and isinstance(n.right, ca.Constant) and _same(n.right, n.left.right)

    def masked(n):
        if not (isinstance(n, ca.BinaryOp) and n.op == "&" and isinstance(n.right, ca.Constant)):
            return False
        try:
            m = int(n.right.value.rstrip("uUlL"), 0)
        except ValueError:
            return False
        return 1 < m < 0x80000000 and m & (m + 1) == 0
    pred = lambda n, p, s: shifted(n) or masked(n)
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        n, par, slot = nth(f2, pred, k)
        if shifted(n):
            bits = 32 - int(n.right.value, 0)
            new = ca.BinaryOp("&", n.left.left, ca.Constant("int", str((1 << bits) - 1)))
            desc = f"{text(n)[:24]} masked to {bits} bits"
        else:
            bits = int(n.right.value.rstrip("uUlL"), 0).bit_length()
            sh = ca.Constant("int", str(32 - bits))
            new = ca.BinaryOp(">>", ca.BinaryOp("<<", n.left, sh), copy.deepcopy(sh))
            desc = f"{text(n)[:24]} sign-extended from {bits} bits"
        set_child(par, slot, new)
        yield desc, f2


def sp_narrow_mask(fn):
    """E & K  ->  (u8)(E & K)  (Ghost_TurnTowardHeading and Lun_TurnTowardSavedDirection:
    `s32 current = (u8)(((dir + 0x70) >> 5) & 7)`). The narrowed AND is done in QImode, so its
    constant K is a QImode register, and cse shares a constant only within one mode: a later int
    `& K` loads K again instead of reusing it, as the ROM does. Sound: E & K fits the narrow type."""
    def mask(n):
        if not (isinstance(n, ca.BinaryOp) and n.op == "&" and isinstance(n.right, ca.Constant)):
            return None
        try:
            return int(n.right.value.rstrip("uUlL"), 0)
        except ValueError:
            return None
    pred = lambda n, p, s: mask(n) is not None and 0 < mask(n) <= 0xFFFF and not isinstance(p, ca.Cast)
    for k in range(sites(fn, pred)):
        for t, top in (("u8", 0xFF), ("u16", 0xFFFF)):
            f2 = copy.deepcopy(fn)
            n, par, slot = nth(f2, pred, k)
            if mask(n) > top:
                continue
            set_child(par, slot, ca.Cast(tname(t), n))
            yield f"({t})({text(n)[:28]})", f2


def sp_copy_uses(fn):
    """T *v = E; ... E ... v ...  ->  move single uses between the copy v and its source E
    (which of the two pseudos gets the higher allocation priority, and so the callee-saved register,
    follows their use counts; Slot7_UpdateMotionPhase's copy turned out to be sp_param_alias). Sound
    while neither v nor what E reads is reassigned. The C-level replacement for a `do { } while (0)`
    loop-weight wrapper around one use."""
    decls = [d for d in fn.body.block_items if isinstance(d, ca.Decl) and d.init is not None
             and isinstance(d.type, ca.PtrDecl)]
    assigned = {text(n.lvalue) for n, _, _ in walk(fn.body) if isinstance(n, ca.Assignment)} | \
               {text(n.expr) for n, _, _ in walk(fn.body) if isinstance(n, ca.UnaryOp) and n.op in ("++", "--", "p++", "p--", "&")}
    for d in decls:
        src = text(d.init)
        reads = {x.name for x, _, _ in walk(d.init) if isinstance(x, ca.ID)}
        if d.name in assigned or reads & assigned:
            continue
        def is_src(n, par, slot, src=src, d=d):
            return text(n) == src and n is not d.init and not isinstance(par, ca.Decl)
        def is_var(n, par, slot, d=d):
            return isinstance(n, ca.ID) and n.name == d.name and not (isinstance(par, ca.StructRef) and slot == "field")
        for k in range(sites(fn, is_src)):
            f2 = copy.deepcopy(fn)
            n, par, slot = nth(f2, is_src, k)
            set_child(par, slot, ca.ID(d.name))
            yield f"use {d.name} for {src[:24]} (use {k + 1})", f2
        for k in range(sites(fn, is_var)):
            f2 = copy.deepcopy(fn)
            n, par, slot = nth(f2, is_var, k)
            set_child(par, slot, copy.deepcopy(d.init))
            yield f"use {src[:24]} for {d.name} (use {k + 1})", f2


def sp_param_alias(fn):
    """T *p (a parameter)  ->  T *self = p; at entry, and self for p in one arm of an if
    (Slot7_UpdateMotionPhase: `BeginPhase(self, 4); self->previousPhase = 4;` in the else arm). cse
    puts p back for the copy only inside the extended block where it saw `self = p` (cse.c
    make_regs_eqv; the block ends at the arm's label), so the arm's uses keep a pseudo of their own
    and it gets another register. The two arms then differ, and jump.c's cross-jumping
    (find_cross_jump) no longer merges their tails. The ROM's sign: an entry copy of the parameter
    (`adds r7, r4, #0`) used only in one arm, and two arm tails that differ only in that base
    register. Sound while p is never reassigned."""
    if not (isinstance(fn.decl.type, ca.FuncDecl) and fn.decl.type.args):
        return
    changed = {text(n.lvalue) for n, _, _ in walk(fn.body) if isinstance(n, ca.Assignment)} | \
              {text(n.expr) for n, _, _ in walk(fn.body) if isinstance(n, ca.UnaryOp)
               and n.op in ("++", "--", "p++", "p--", "&")}
    params = [p for p in fn.decl.type.args.params
              if isinstance(p, ca.Decl) and p.name and isinstance(p.type, ca.PtrDecl) and p.name not in changed]
    ifs = [n for n, _, _ in walk(fn.body) if isinstance(n, ca.If)]
    for prm in params:
        for k, node in enumerate(ifs):
            for arm in ("iftrue", "iffalse"):
                if getattr(node, arm) is None or not reads_of(getattr(node, arm), prm.name):
                    continue
                f2 = copy.deepcopy(fn)
                node2 = [n for n, _, _ in walk(f2.body) if isinstance(n, ca.If)][k]
                names = {n.name for n, _, _ in walk(f2) if isinstance(n, (ca.ID, ca.Decl)) and getattr(n, "name", None)}
                v = "self" if "self" not in names else new_name(f2, "self")
                for r, par, slot in reads_of(getattr(node2, arm), prm.name):
                    set_child(par, slot, ca.ID(v))
                d = copy.deepcopy(prm)
                d.name, d.type = v, _renamed(prm.type, v)
                d.init = ca.ID(prm.name)
                add_local(f2, d)
                which = "then" if arm == "iftrue" else "else"
                yield f"{v} = {prm.name} at entry, used in the {which} arm of `if ({text(node.cond)[:24]})`", f2


ASSOC = ("|", "&", "^", "+", "*")


def sp_compound_assign(fn):
    """L = (L op A) op B  ->  L op= A op B, L = L op A  ->  L op= A, and L op= A  ->  L = L op A
    (sub_08114E68: `sprite->flags |= 0x11 | spriteFlags;`). c-typeck.c build_modify_expr builds a
    compound assignment's L once, through stabilize_reference: its address (here a pointer spilled
    to the stack) is computed one time and serves both the load and the store, and L's load comes
    before the right side is expanded. Written out, L is a second expression with its own address
    computation and its own place in the evaluation order. Sound for the associative operators."""
    pred = lambda n, p, s: isinstance(n, ca.Assignment) and isinstance(n.rvalue, ca.BinaryOp)
    for k in range(sites(fn, pred)):
        n, _, _ = nth(fn, pred, k)
        if n.op == "=":
            lv, rv = text(n.lvalue), n.rvalue
            if rv.op in ASSOC + ("-", "<<", ">>") and text(rv.left) == lv:
                f2 = copy.deepcopy(fn)
                a, _, _ = nth(f2, pred, k)
                a.op, a.rvalue = rv.op + "=", a.rvalue.right
                yield f"{lv} {rv.op}= ... (one address for load and store)", f2
            if rv.op in ASSOC and isinstance(rv.left, ca.BinaryOp) and rv.left.op == rv.op and text(rv.left.left) == lv:
                f2 = copy.deepcopy(fn)
                a, _, _ = nth(f2, pred, k)
                inner = a.rvalue.left
                a.op, a.rvalue = rv.op + "=", ca.BinaryOp(rv.op, inner.right, a.rvalue.right)
                yield f"{lv} {rv.op}= A {rv.op} B (one address for load and store)", f2
    pred2 = lambda n, p, s: isinstance(n, ca.Assignment) and n.op[:-1] in ASSOC + ("-", "<<", ">>")
    for k in range(sites(fn, pred2)):
        f2 = copy.deepcopy(fn)
        a, _, _ = nth(f2, pred2, k)
        op = a.op[:-1]
        a.op, a.rvalue = "=", ca.BinaryOp(op, copy.deepcopy(a.lvalue), a.rvalue)
        yield f"{text(a.lvalue)[:24]} = {text(a.lvalue)[:24]} {op} ... written out", f2


WORD_TYPES = {"u32", "s32", "int", "unsigned int", "vu32", "uintptr_t"}


def sp_alias_read(fn):
    """(T)x->f  ->  *(T *)&x->f  (sub_080B03BC: `actor->word104 = 0; actor->word104 =
    *(u32 *)&actor->entryTarget228;`). agbcc -O2 gives each type an alias set and flow.c deletes a
    store as dead when a later store writes the same address and no read between them may alias it;
    a read of another type (the void * field) never aliases a u32 store, so the ROM's surviving
    first store means the read was a u32. A type hypothesis: the field was declared T."""
    def pred(n, p, s):
        return (isinstance(n, ca.Cast) and isinstance(n.to_type.type, ca.TypeDecl)
                and " ".join(getattr(n.to_type.type.type, "names", [])) in WORD_TYPES
                and isinstance(n.expr, (ca.StructRef, ca.ArrayRef)))
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        n, par, slot = nth(f2, pred, k)
        t = " ".join(n.to_type.type.type.names)
        ptr = ca.Typename(None, [], None, [], ca.PtrDecl([], ca.TypeDecl(None, [], None, ca.IdentifierType(t.split()))))
        set_child(par, slot, ca.UnaryOp("*", ca.Cast(ptr, ca.UnaryOp("&", n.expr))))
        yield f"read {text(n.expr)[:28]} as {t} (alias set)", f2


def _after(fn, node):
    """Statements that can run after NODE finishes: the items after it and after each enclosing
    statement in their blocks (an if's other branch is not among them). None inside a loop."""
    parent = {id(n): p for n, p, _ in walk(fn)}
    out, cur = [], node
    while cur is not fn.body and id(cur) in parent:
        p = parent[id(cur)]
        if isinstance(p, (ca.For, ca.While, ca.DoWhile)):
            return None
        if isinstance(p, ca.Compound):
            out += p.block_items[p.block_items.index(cur) + 1:]
        cur = p
    return out


def sp_fresh_var(fn):
    """v op= K; ... v ...  ->  T w = v op K; ... w ...  for the rest of the block, when nothing after the
    block reads v (Storage_GetSlotPosition: a new variable, not the modified parameter)."""
    for bi, b in enumerate(blocks(fn)):
        rest = _after(fn, b)
        if rest is None or b is fn.body:
            continue
        it = b.block_items
        for i, st in enumerate(it):
            if not (isinstance(st, ca.Assignment) and st.op in ("+=", "-=", "&=", "|=", "<<=", ">>=", "^=")
                    and isinstance(st.lvalue, ca.ID)):
                continue
            v = st.lvalue.name
            later = [x for x in it[i + 1:]]
            if any(_assigns(x, v) for x in later):
                continue
            if not any(_ids(x, v) for x in later) or any(_ids(x, v) for x in rest):
                continue
            for tn in ("s32", "u32"):
                f2 = copy.deepcopy(fn)
                it2 = blocks(f2)[bi].block_items
                w = new_name(f2, "w")
                st2 = it2[i]
                for x in it2[i + 1:]:
                    for n, p, s in list(walk(x)):
                        if isinstance(n, ca.ID) and n.name == v and not (isinstance(p, ca.StructRef) and s == "field"):
                            set_child(p, s, ca.ID(w))
                it2[i] = int_decl(w, tn, ca.BinaryOp(st2.op[:-1], ca.ID(v), st2.rvalue))
                yield f"{tn} {w} = {v} {st2.op[:-1]} ... instead of changing {v}", f2


def sp_neg_mul(fn):
    """x = -x  ->  x *= -1  (ActorGroup_UpdateMemberSway)."""
    pred = lambda n, p, s: isinstance(n, ca.Assignment) and n.op == "=" and isinstance(n.rvalue, ca.UnaryOp) \
        and n.rvalue.op == "-" and text(n.rvalue.expr) == text(n.lvalue)
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        a, _, _ = nth(f2, pred, k)
        a.op, a.rvalue = "*=", ca.UnaryOp("-", ca.Constant("int", "1"))
        yield f"{text(a.lvalue)[:24]} *= -1", f2


def sp_member_ptr(fn):
    """&X used twice or more (as &X or X.f)  ->  T *q = &X at the top, q / q->f  (RegionActor_
    UpdateHealthAnimation: the member's address kept in a local). X's base must not change."""
    found = {}
    for n, p, s in walk(fn.body):
        if isinstance(n, ca.UnaryOp) and n.op == "&" and isinstance(n.expr, ca.StructRef):
            found.setdefault(text(n.expr), n.expr)
        elif isinstance(n, ca.StructRef) and n.type == "." and isinstance(n.name, ca.StructRef):
            found.setdefault(text(n.name), n.name)
    for key, x in found.items():
        uses = [1 for n, p, s in walk(fn.body)
                if (isinstance(n, ca.UnaryOp) and n.op == "&" and text(n.expr) == key)
                or (isinstance(n, ca.StructRef) and n.type == "." and text(n.name) == key)]
        if len(uses) < 2:
            continue
        names = {i.name for i, _, _ in walk(x) if isinstance(i, ca.ID)}
        if any(isinstance(i, (ca.FuncCall, ca.ArrayRef)) for i, _, _ in walk(x)) or any(_assigns(fn.body, nm) for nm in names):
            continue
        # only an address computed without loading anything (p->a.b, s.a): computed once at the top,
        # it must not depend on memory that the function or its callees may change
        y = x
        while isinstance(y, ca.StructRef) and (y.type == "." or isinstance(y.name, ca.ID)):
            y = y.name
        if not isinstance(y, ca.ID):
            continue
        f2 = copy.deepcopy(fn)
        q = new_name(f2, "q")
        for n, p, s in list(walk(f2.body)):
            if isinstance(n, ca.UnaryOp) and n.op == "&" and text(n.expr) == key:
                set_child(p, s, ca.ID(q))
            elif isinstance(n, ca.StructRef) and n.type == "." and text(n.name) == key:
                n.name, n.type = ca.ID(q), "->"
        add_local(f2, ca.Decl(name=q, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[],
                              type=ca.PtrDecl([], ca.TypeDecl(q, [], None, ca.Typeof(copy.deepcopy(x)))),
                              init=ca.UnaryOp("&", copy.deepcopy(x)), bitsize=None, asmlabel=None))
        yield f"&{key[:24]} in a local", f2


def sp_offset_local(fn):
    """P + i * K (or i << K) as an address  ->  { s32 off = i * K; ... P + off ... }  (CollectionBook_
    UpdateMenu, ActorGroup_CreateFromScript: the scaled index computed first)."""
    pred = lambda n, p, s: (isinstance(n, ca.BinaryOp) and n.op == "+" and isinstance(n.right, ca.BinaryOp)
                            and n.right.op in ("*", "<<") and _is_const(n.right.right))
    for k in range(sites(fn, pred)):
        for tn in ("s32", "u32"):
            f2 = copy.deepcopy(fn)
            e, _, _ = nth(f2, pred, k)
            off = new_name(f2, "off")
            d = int_decl(off, tn, e.right)
            e.right = ca.ID(off)
            if not _put_before(f2, e, d):
                continue
            yield f"{tn} offset {text(d.init)[:20]} computed first", f2


def sp_const_local(fn):
    """... E op K ... in a condition, call or return  ->  { T k = K; ... E op k ... }  (sub_08220204:
    `{ u32 m = 0x1000; if (atk->attr & m) ... }`; sp_scoped_operand does the same for assignments)."""
    def pred(n, p, s):
        return isinstance(n, ca.Constant) and n.type == "int" and isinstance(p, ca.BinaryOp) and \
            not _is_const(n, 0)
    owners = []
    for b in blocks(fn):
        for st in b.block_items or []:
            if isinstance(st, ca.If):
                owners.append(st.cond)
            elif isinstance(st, (ca.FuncCall, ca.Return)):
                owners.append(st)
    for oi in range(len(owners)):
        cs = [x for x in walk(owners[oi]) if pred(*x)]
        for ci in range(len(cs)):
            for t in ("u32", "s32", "u16", "u8"):
                # an unsigned int operand makes the comparison, division or shift unsigned (u8/u16 promote to int)
                if not _fits(cs[ci][0], t) or t == "u32" and cs[ci][1].op in ("<", ">", "<=", ">=", "/", "%", ">>"):
                    continue
                f2 = copy.deepcopy(fn)
                own2 = []
                for b in blocks(f2):
                    for st in b.block_items or []:
                        if isinstance(st, ca.If):
                            own2.append(st.cond)
                        elif isinstance(st, (ca.FuncCall, ca.Return)):
                            own2.append(st)
                c2, p2, s2 = [x for x in walk(own2[oi]) if pred(*x)][ci]
                v = new_name(f2, "k")
                set_child(p2, s2, ca.ID(v))
                if not _put_before(f2, own2[oi], int_decl(v, t, c2)):
                    continue
                yield f"{t} local for {c2.value} in `{text(owners[oi])[:30]}`", f2


def sp_load_local(fn):
    """... X->f op E ...  ->  { T t = X->f; ... t op E ... }  (ForgingBook_RefreshEntrySprites:
    `entries = p->entry; e = entries + idx`): a field read into its own variable just before."""
    def pred(n, p, s):
        return isinstance(n, (ca.StructRef, ca.ArrayRef)) and isinstance(p, ca.BinaryOp) and \
            not any(isinstance(x, (ca.FuncCall, ca.Assignment)) for x, _, _ in walk(n))
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        n, p, s_ = nth(f2, pred, k)
        t = new_name(f2, "v")
        d = typeof_decl(t, n, copy.deepcopy(n))
        set_child(p, s_, ca.ID(t))
        if not _put_before(f2, p, d):
            continue
        yield f"{text(n)[:28]} read into a local first", f2


GLOBAL_AT = __import__("re").compile(r"^gUnk_([0-9A-F]{8})$")


def sp_symbol_literal(fn):
    """gUnk_XXXXXXXX  ->  (*(typeof(gUnk_XXXXXXXX) *)0xXXXXXXXX)  (Path_SnapToRandomPointInBox,
    ActorEffect_UpdateApproachLaunch, Ratatosk_UpdateSunOrbPlacement: the original named the address,
    not the symbol, and agbcc treats a constant address differently from a symbol in cse)."""
    names = []
    for n, p, s in walk(fn.body):
        if isinstance(n, ca.ID) and GLOBAL_AT.match(n.name) and n.name not in names:
            names.append(n.name)
    for name in names:
        f2 = copy.deepcopy(fn)
        addr = ca.Constant("int", "0x" + GLOBAL_AT.match(name).group(1))
        for n, p, s in list(walk(f2.body)):
            if isinstance(n, ca.ID) and n.name == name and not isinstance(p, ca.Typeof):
                ptr = ca.Typename(None, [], None, [], ca.PtrDecl([], ca.TypeDecl(None, [], None, ca.Typeof(ca.ID(name)))))
                set_child(p, s, ca.UnaryOp("*", ca.Cast(ptr, copy.deepcopy(addr))))
        yield f"{name} by its address", f2


def sp_outline_store(fn):
    """L op= E / L = E  ->  H(&L, E) with static inline void H(T *a, u32 v) { *a op= v; }  (the
    helpers of Player_UpdateStatusVisuals, SceneActorControl_UpdateEntryWait, SceneHud_InitFromScript:
    an inline helper's arguments are evaluated before its body, so the address comes first)."""
    ops = ("=", "|=", "&=", "+=", "-=", "^=")
    pred = lambda n, p, s: (isinstance(n, ca.Assignment) and n.op in ops and isinstance(p, ca.Compound)
                            and isinstance(n.lvalue, (ca.StructRef, ca.ArrayRef, ca.UnaryOp))
                            and not isinstance(n.rvalue, ca.Assignment))
    for k in range(sites(fn, pred)):
        for vt in ("u32", "s32"):
            f2 = copy.deepcopy(fn)
            a, par, s_ = nth(f2, pred, k)
            name = new_name(f2, "Store")
            pa = ca.PtrDecl([], ca.TypeDecl("a_", [], None, ca.Typeof(copy.deepcopy(a.lvalue))))
            pv = ca.TypeDecl("v_", [], None, ca.IdentifierType([vt]))
            plist = ca.ParamList([ca.Decl(name=n_, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[],
                                          type=t_, init=None, bitsize=None, asmlabel=None)
                                  for n_, t_ in (("a_", pa), ("v_", pv))])
            ftype = ca.FuncDecl(plist, ca.TypeDecl(name, [], None, ca.IdentifierType(["void"])))
            decl = ca.Decl(name=name, quals=[], align=[], storage=["static"], funcspec=["inline"],
                           gcc_attributes=[], type=ftype, init=None, bitsize=None, asmlabel=None)
            h = ca.FuncDef(decl, None, ca.Compound([ca.Assignment(a.op, ca.UnaryOp("*", ca.ID("a_")), ca.ID("v_"))]))
            set_child(par, s_, ca.FuncCall(ca.ID(name), ca.ExprList([ca.UnaryOp("&", a.lvalue), a.rvalue])))
            yield f"{text(a)[:30]} through an inline store helper ({vt})", (f2, [h])


def sp_outline_setter(fn):
    """b->f op= E  ->  Set(b, E) with static inline void Set(T *o, typeof(b->f) v) { o->f op= v; }
    (PanelSequence_InitializeFromScript: the ROM loads the stored value before it adds the field's
    offset. expand_assignment computes the destination address first; an inline helper's arguments
    are evaluated into its parameters before the body, so the value comes first. The mirror of
    sp_outline_store, whose helper takes the address)."""
    ops = ("=", "|=", "&=", "+=", "-=", "^=")
    types = {n.name: n.type for n, _, _ in walk(fn) if isinstance(n, ca.Decl) and n is not fn.decl}
    if isinstance(fn.decl.type.args, ca.ParamList):
        types.update({p.name: p.type for p in fn.decl.type.args.params if isinstance(p, ca.Decl) and p.name})
    pred = lambda n, p, s: (isinstance(n, ca.Assignment) and n.op in ops and isinstance(p, ca.Compound)
                            and isinstance(n.lvalue, ca.StructRef) and n.lvalue.type == "->"
                            and isinstance(n.lvalue.name, ca.ID) and n.lvalue.name.name in types
                            and isinstance(types[n.lvalue.name.name], ca.PtrDecl)
                            and not isinstance(n.rvalue, ca.Assignment))
    for k in range(sites(fn, pred)):
        f2 = copy.deepcopy(fn)
        a, par, s_ = nth(f2, pred, k)
        b = a.lvalue.name.name
        name = new_name(f2, "Set")
        po = _renamed(types[b], "o_")
        for n_, _, _ in walk(po):
            if isinstance(n_, ca.TypeDecl):
                n_.quals = [q for q in n_.quals if q != "const"]
        null = ca.Cast(ca.Typename(None, [], None, [], _renamed(types[b], None)), ca.Constant("int", "0"))
        pv = ca.TypeDecl("v_", [], None, ca.Typeof(ca.StructRef(null, "->", copy.deepcopy(a.lvalue.field))))
        plist = ca.ParamList([ca.Decl(name=n_, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[],
                                      type=t_, init=None, bitsize=None, asmlabel=None)
                              for n_, t_ in (("o_", po), ("v_", pv))])
        ftype = ca.FuncDecl(plist, ca.TypeDecl(name, [], None, ca.IdentifierType(["void"])))
        decl = ca.Decl(name=name, quals=[], align=[], storage=["static"], funcspec=["inline"],
                       gcc_attributes=[], type=ftype, init=None, bitsize=None, asmlabel=None)
        body = ca.Assignment(a.op, ca.StructRef(ca.ID("o_"), "->", copy.deepcopy(a.lvalue.field)), ca.ID("v_"))
        h = ca.FuncDef(decl, None, ca.Compound([body]))
        set_child(par, s_, ca.FuncCall(ca.ID(name), ca.ExprList([ca.ID(b), a.rvalue])))
        yield f"{text(a)[:30]} through an inline setter (value first)", (f2, [h])


def _trip(items, i):
    """(counter, trip count expr, body list, statements to drop) when items[i] is a counting loop whose
    counter is not read in its body: for (c = N; c != 0 / c > 0; c--), for (c = K; c >= 0; c--),
    c = N; do { B; c--; } while (c != 0), and c = K; do B while (--c >= 0)."""
    lp = items[i]
    dec = lambda x, v: isinstance(x, ca.UnaryOp) and x.op in ("--", "p--") and isinstance(x.expr, ca.ID) \
        and x.expr.name == v
    body = lambda st: list((st.block_items or []) if isinstance(st, ca.Compound) else [st])
    if isinstance(lp, ca.For) and isinstance(lp.init, ca.Assignment) and isinstance(lp.init.lvalue, ca.ID):
        v = lp.init.lvalue.name
        c = lp.cond
        if not dec(lp.next, v) or not isinstance(c, ca.BinaryOp) or not (isinstance(c.left, ca.ID) and c.left.name == v):
            return None
        if c.op in ("!=", ">") and _is_const(c.right, 0):
            n = lp.init.rvalue
        elif c.op == ">=" and _is_const(c.right, 0) and _is_const(lp.init.rvalue):
            n = ca.Constant("int", str(_cint(lp.init.rvalue) + 1))
        else:
            return None
        b = body(lp.stmt)
        return (v, n, b, []) if not any(_ids(x, v) for x in b) else None
    if isinstance(lp, ca.DoWhile) and i > 0:
        m = _countdown(items, i)
        if m:
            v, k, rest = m
            return v, ca.Constant("int", str(k + 1)), rest, [items[i - 1]]
        c, b = lp.cond, body(lp.stmt)
        if isinstance(c, ca.BinaryOp) and c.op == "!=" and _is_const(c.right, 0) and isinstance(c.left, ca.ID) \
                and b and dec(b[-1], c.left.name):
            v = c.left.name
            for j in range(i - 1, -1, -1):
                st = items[j]
                if isinstance(st, ca.Assignment) and st.op == "=" and isinstance(st.lvalue, ca.ID) and st.lvalue.name == v:
                    if any(_ids(x, v) for x in b[:-1]):
                        return None
                    return v, st.rvalue, b[:-1], [st]
                if any(_ids(st, v) for _ in [0]) or isinstance(st, (ca.For, ca.While, ca.DoWhile)):
                    return None
    return None


def sp_loop_normal(fn):
    """Any counting loop whose counter the body does not read  ->  for (c = 0; c < N; c++) body; and,
    when the loop sits in `if (N > 0)`, also without the guard (StatusPanel_ClearLowerRows,
    SceneHud_FillDigitCells, TilemapPanel_LoadScriptTileList: drafts often spell the ROM's count-down
    loop, the original counted up)."""
    for bi, b in enumerate(blocks(fn)):
        for i in range(len(b.block_items)):
            m = _trip(b.block_items, i)
            if not m:
                continue
            for unguard in (False, True):
                f2 = copy.deepcopy(fn)
                b2 = blocks(f2)[bi]
                it = b2.block_items
                v, n, body, drop = _trip(it, i)
                lp = ca.For(ca.Assignment("=", ca.ID(v), ca.Constant("int", "0")),
                            ca.BinaryOp("<", ca.ID(v), copy.deepcopy(n)), ca.UnaryOp("p++", ca.ID(v)),
                            ca.Compound(list(body)))
                it[i] = lp
                for st in drop:
                    if isinstance(st, ca.Decl):
                        st.init = None
                    else:
                        it.remove(st)
                if unguard:
                    guard = None
                    for n2, p2, s2 in walk(f2):
                        if isinstance(n2, ca.If) and n2.iffalse is None and (n2.iftrue is b2) and \
                                isinstance(n2.cond, ca.BinaryOp) and n2.cond.op == ">" and _is_const(n2.cond.right, 0) \
                                and text(n2.cond.left) == text(n):
                            guard = (n2, p2, s2)
                    if not guard:
                        continue
                    g, gp, gs = guard
                    set_child(gp, gs, b2)
                yield f"loop on {v} counted up to {text(n)[:20]}" + (" without its guard" if unguard else ""), f2


def sp_ptr_deinduct(fn):
    """for (i = 0; ...; i++) { ... *q ...; q++; }  with q = B before the loop  ->  ... B[i] ...  (the
    original indexed and loop.c made the pointer: TilemapPanel_LoadScriptTileList,
    CallbackPool_InvokeActive). q steps once per pass, unconditionally, and is not read after the loop."""
    loops = [n for n, _, _ in walk(fn) if isinstance(n, ca.For)]
    step = lambda x, q: (isinstance(x, ca.UnaryOp) and x.op in ("p++", "++") and isinstance(x.expr, ca.ID)
                         and x.expr.name == q)
    for li, lp in enumerate(loops):
        if not (isinstance(lp.init, ca.Assignment) and isinstance(lp.init.lvalue, ca.ID) and _is_const(lp.init.rvalue, 0)
                and isinstance(lp.next, ca.UnaryOp) and lp.next.op in ("p++", "++")
                and text(lp.next.expr) == lp.init.lvalue.name):
            continue
        i = lp.init.lvalue.name
        body = lp.stmt.block_items if isinstance(lp.stmt, ca.Compound) else [lp.stmt]
        body = body or []
        # pointers stepped by a top-level q++ (or *q++ inside a top-level statement)
        cands = []
        for st in body:
            if step(st, None if not isinstance(st, ca.UnaryOp) else getattr(st.expr, "name", None)):
                cands.append(st.expr.name)
            for x, p, s_ in walk(st):
                if isinstance(x, ca.UnaryOp) and x.op == "*" and isinstance(x.expr, ca.UnaryOp) and \
                        x.expr.op == "p++" and isinstance(x.expr.expr, ca.ID) and isinstance(st, ca.Assignment):
                    cands.append(x.expr.expr.name)
        for q in dict.fromkeys(cands):
            f2 = copy.deepcopy(fn)
            lp2 = [n for n, _, _ in walk(f2) if isinstance(n, ca.For)][li]
            parent = next((n for n, _, _ in walk(f2) if isinstance(n, ca.Compound) and lp2 in (n.block_items or [])), None)
            if parent is None:
                continue
            # q = B in the same block before the loop, nothing in between touching q
            k = parent.block_items.index(lp2)
            init = None
            for j in range(k - 1, -1, -1):
                st = parent.block_items[j]
                if isinstance(st, ca.Assignment) and st.op == "=" and isinstance(st.lvalue, ca.ID) and st.lvalue.name == q:
                    init = st
                    break
                if _ids(st, q):
                    break
            if init is None:
                continue
            b2 = lp2.stmt.block_items if isinstance(lp2.stmt, ca.Compound) else [lp2.stmt]
            if any(_ids(x, n_) for x in b2 for n_ in {y.name for y, _, _ in walk(init.rvalue) if isinstance(y, ca.ID)}
                   if any(_assigns(x, n_) for x in b2)):
                continue
            uses = [x for st in b2 for x in _ids(st, q)]
            steps = [st for st in b2 if step(st, q)]
            posts = [(x, p, s_) for st in b2 for x, p, s_ in walk(st)
                     if isinstance(x, ca.UnaryOp) and x.op == "p++" and isinstance(x.expr, ca.ID) and x.expr.name == q]
            if len(steps) + len(posts) != 1 or any(_assigns(st, q) and not step(st, q) and not posts for st in b2):
                continue
            rest = _after(f2, lp2)
            if rest is None:
                continue
            first = next((x for st in rest for x in _ids(st, q)), None)
            if first is not None and not any(isinstance(st, ca.Assignment) and st.op == "=" and st.lvalue is first
                                             for st in rest):
                continue
            base = init.rvalue
            idx = lambda: ca.ArrayRef(copy.deepcopy(base), ca.ID(i))
            if steps:
                b2.remove(steps[0])
            for st in list(b2):
                for x, p, s_ in list(walk(st)):
                    if isinstance(x, ca.UnaryOp) and x.op == "*" and isinstance(x.expr, ca.UnaryOp) and \
                            x.expr.op == "p++" and text(x.expr.expr) == q:
                        set_child(p, s_, idx())
                for x, p, s_ in list(walk(st)):
                    if isinstance(x, ca.UnaryOp) and x.op == "*" and isinstance(x.expr, ca.ID) and x.expr.name == q:
                        set_child(p, s_, idx())
                    elif isinstance(x, ca.StructRef) and x.type == "->" and isinstance(x.name, ca.ID) and x.name.name == q:
                        x.name, x.type = idx(), "."
                for x, p, s_ in list(walk(st)):
                    if isinstance(x, ca.ID) and x.name == q and p is not None and \
                            not (isinstance(p, ca.StructRef) and s_ == "field"):
                        set_child(p, s_, ca.UnaryOp("&", idx()))
            if not _ids(f2, q) or all(x is init.lvalue for x in _ids(f2, q)):
                parent.block_items.remove(init)
            yield f"{q} as {text(base)[:24]}[{i}] instead of a stepped pointer", f2


def sp_arg_to_local(fn, helpers):
    """H(&b->f, ...)  ->  H_(b, ...) whose body starts with T a = &b_->f  (SceneActor_IsFacingNearbyTarget:
    the helper took the object and found the field itself, so the address is computed inside the
    inline body, after the other arguments). The inverse of sp_local_to_arg."""
    types = {n.name: n.type for n, _, _ in walk(fn) if isinstance(n, ca.Decl) and n is not fn.decl}
    pred = lambda n, p, s: (isinstance(n, ca.FuncCall) and isinstance(n.name, ca.ID) and n.name.name in helpers
                            and n.args is not None)
    for k in range(sites(fn, pred)):
        c0, _, _ = nth(fn, pred, k)
        h0 = helpers[c0.name.name]
        if not isinstance(h0.decl.type.args, ca.ParamList):
            continue
        params = [x for x in h0.decl.type.args.params if isinstance(x, ca.Decl)]
        for i, arg in enumerate(c0.args.exprs):
            if i >= len(params) or isinstance(arg, (ca.ID, ca.Constant)):
                continue
            bases = {x.name for x, par, sl in walk(arg) if isinstance(x, ca.ID)
                     and not (isinstance(par, ca.StructRef) and sl == "field")}
            if len(bases) != 1 or any(isinstance(x, (ca.FuncCall, ca.Assignment)) for x, _, _ in walk(arg)):
                continue
            b = bases.pop()
            if b not in types:
                continue
            f2 = copy.deepcopy(fn)
            c, _, _ = nth(f2, pred, k)
            h = copy.deepcopy(h0)
            name = new_name(f2, c.name.name + "_")
            h.decl.name = name
            for n, _, _ in walk(h.decl.type):
                if isinstance(n, ca.TypeDecl) and n.declname == c.name.name:
                    n.declname = name
            old = h.decl.type.args.params[i]
            bp = b + "_"
            h.decl.type.args.params[i] = ca.Decl(name=bp, quals=[], align=[], storage=[], funcspec=[],
                                                 gcc_attributes=[], type=_renamed(types[b], bp), init=None,
                                                 bitsize=None, asmlabel=None)
            init = copy.deepcopy(arg)
            for x, par, sl in list(walk(init)):
                if isinstance(x, ca.ID) and x.name == b and par is not None:
                    set_child(par, sl, ca.ID(bp))
            if isinstance(init, ca.ID):
                init = ca.ID(bp)
            local = ca.Decl(name=old.name, quals=[], align=[], storage=[], funcspec=[], gcc_attributes=[],
                            type=old.type, init=init, bitsize=None, asmlabel=None)
            h.body.block_items = [local] + list(h.body.block_items or [])
            c.args.exprs[i] = ca.ID(b)
            c.name.name = name
            yield f"{h0.decl.name} takes {b} and computes {text(arg)[:24]} itself", (f2, [h])


SUBWORD = ("u8", "s8", "u16", "s16", "char", "short")


def sp_const_literal(fn):
    """v = 7; ... x & v ...  ->  ... x & 7 ... for a local only ever set to one constant (the inverse of
    sp_const_local; ActorParticles_SpawnWideBurst). A local is promoted to SImode (PROMOTE_MODE), so
    cse folds it to the constant at each use; a literal in a 16-bit expression gets an HImode pseudo
    of its own (see sp_helper_ret_word)."""
    decls = [n for n, _, _ in walk(fn) if isinstance(n, ca.Decl) and n is not fn.decl]
    names = [d.name for d in decls]
    for d in decls:
        v = d.name
        if names.count(v) != 1 or not isinstance(d.type, ca.TypeDecl) or d in (fn.decl.type.args.params
                                                                                 if fn.decl.type.args else []):
            continue
        vals, ok = [], True
        if d.init is not None:
            vals.append(d.init)
        for n, par, sl in walk(fn.body):
            if isinstance(n, ca.Assignment) and isinstance(n.lvalue, ca.ID) and n.lvalue.name == v:
                if n.op != "=" or not isinstance(par, ca.Compound):
                    ok = False
                vals.append(n.rvalue)
            elif (isinstance(n, ca.UnaryOp) and n.op in ("&", "++", "--", "p++", "p--")
                  and isinstance(n.expr, ca.ID) and n.expr.name == v):
                ok = False
        if not ok or not vals or not all(isinstance(x, ca.Constant) and x.value == vals[0].value for x in vals):
            continue
        f2 = copy.deepcopy(fn)
        for n, par, sl in list(walk(f2.body)):
            if isinstance(n, ca.Compound) and n.block_items:
                n.block_items = [x for x in n.block_items
                                 if not (isinstance(x, ca.Decl) and x.name == v)
                                 and not (isinstance(x, ca.Assignment) and isinstance(x.lvalue, ca.ID)
                                          and x.lvalue.name == v)]
        for n, par, sl in list(walk(f2.body)):
            if isinstance(n, ca.ID) and n.name == v and par is not None and not (
                    isinstance(par, ca.StructRef) and sl == "field"):
                set_child(par, sl, copy.deepcopy(vals[0]))
        yield f"literal {vals[0].value} for {v}", f2


def sp_helper_ret_word(fn, helpers):
    """static inline u16 H(void)  ->  static inline s32 H(void) (ActorParticles_SpawnWideBurst, like the
    matched sub_08239D38: the random-table helper returned a word). Rule (optabs.c widen_operand,
    reload.c find_equiv_reg, jump.c no-op moves): `s->y = H() & 7` with a 16-bit field is done in
    HImode and widened to SImode for andsi3. With a u16 H the constant's HImode pseudo is the operand
    that andsi3 forces into an SImode copy, and cse folds that copy to the constant. With a word H the
    constant's HImode pseudo stays an operand through a paradoxical subreg, which cse cannot fold.
    Allocated to a high register it is set in HImode and reloaded in SImode, and reload, reload_cse and
    jump's no-op move deletion all refuse to reuse the low register that set it (another mode): the
    ROM's `movs rN, #C; mov H, rN; mov rN, H`. A word-returning helper is tried as u16/s16 too."""
    called = {n.name.name for n, _, _ in walk(fn) if isinstance(n, ca.FuncCall) and isinstance(n.name, ca.ID)}
    for name, h in helpers.items():
        if name not in called or not isinstance(h.decl.type, ca.FuncDecl):
            continue
        ret = h.decl.type.type
        if not (isinstance(ret, ca.TypeDecl) and isinstance(ret.type, ca.IdentifierType)):
            continue
        cur = " ".join(ret.type.names)
        # and the inverse, for a candidate that keeps the copy where the ROM reuses the register
        widths = (("s32", "u32") if ret.type.names[-1] in SUBWORD else
                  ("u16", "s16") if ret.type.names[-1] in ("s32", "u32", "int") else ())
        for t in widths:
            h2 = copy.deepcopy(h)
            h2.decl.type.type.type = ca.IdentifierType([t])
            yield f"{name} returns {t} (was {cur})", (copy.deepcopy(fn), [h2])



def sp_result_into_arg(fn, helpers):
    """return H(E, ...);  ->  { T v = E; v = H(v, ...); return v; }, and y = H(E, ...);  ->  y = E;
    y = H(y, ...);  for an inline helper H whose parameter and result have type T (y of type T too),
    in the function or in the helpers it calls (sub_08137E28: `x = position->x >> 8; x =
    Chase_CellIndex(x, z); return x;` in Chase_CellAt). Rule (integrate.c expand_inline_function;
    agbccsrc.py inline-result): an inlined call's return value is stored straight into the
    assignment's target register (map->inline_target). An argument that overlaps the target is first
    copied to a fresh pseudo, but cse sees that the variable still holds that value and rewrites the
    copy's uses back to it. So x, the index and the result end up one pseudo: it lives as long as the
    result, crosses the calls the result crosses and takes a call-saved register (the ROM's x and cell
    both r4), and the add keeps the operand order of the inline body's saved RTL (`adds r4, r0, r4`;
    `x = rows[z] + x` written out gives `adds r4, r4, r0`, since optabs.c expand_binop puts the
    target's operand first). With E passed as an expression it gets a pseudo of its own, allocated
    apart from the result (AGBCC_PREF=cell:4 gives the same match). The types must agree so that E
    reaches H converted exactly as before."""
    def tname_of(t):
        return text(t).replace("\n", " ").strip() if t is not None else None

    def params_of(h):
        args = h.decl.type.args
        return [x for x in (args.params if args else []) if isinstance(x, ca.Decl)]

    def var_type(f, name):
        for n, _, _ in walk(f):
            if isinstance(n, ca.Decl) and n.name == name and isinstance(n.type, ca.TypeDecl):
                return n.type
        return None

    def name_for(e, f):
        fields = [n.field.name for n, _, _ in walk(e) if isinstance(n, ca.StructRef)]
        used = {n.name for n, p, slot in walk(f) if isinstance(n, (ca.ID, ca.Decl)) and getattr(n, "name", None)
                and not (isinstance(p, ca.StructRef) and slot == "field")}
        if fields and fields[-1] not in used and fields[-1].isidentifier():
            return fields[-1]
        return new_name(f, "v")

    for tname, target in [(None, fn)] + list(helpers.items()):
        for bi, b in enumerate(blocks(target)):
            for si, st in enumerate(b.block_items):
                call = None
                if isinstance(st, ca.Return) and isinstance(st.expr, ca.FuncCall):
                    call = st.expr
                elif (isinstance(st, ca.Assignment) and st.op == "=" and isinstance(st.lvalue, ca.ID)
                      and isinstance(st.rvalue, ca.FuncCall)):
                    call = st.rvalue
                    if any(isinstance(n, ca.ID) and n.name == st.lvalue.name for n, _, _ in walk(call)):
                        continue
                if call is None or not isinstance(call.name, ca.ID) or call.name.name not in helpers \
                        or not call.args:
                    continue
                h = helpers[call.name.name]
                ps = params_of(h)
                if len(ps) != len(call.args.exprs):
                    continue
                ret = tname_of(getattr(h.decl.type.type, "type", None))
                for ai, arg in enumerate(call.args.exprs):
                    pt = tname_of(getattr(ps[ai].type, "type", None))
                    if isinstance(arg, (ca.ID, ca.Constant)) or pt is None or pt != ret:
                        continue
                    if isinstance(st, ca.Assignment):
                        vt = var_type(target, st.lvalue.name)
                        if vt is None or tname_of(vt.type) != pt:
                            continue
                    t2 = copy.deepcopy(target)
                    b2 = blocks(t2)[bi]
                    st2 = b2.block_items[si]
                    if isinstance(st2, ca.Return):
                        v = name_for(arg, t2)
                        c2 = st2.expr
                        e = c2.args.exprs[ai]
                        c2.args.exprs[ai] = ca.ID(v)
                        d = copy.deepcopy(ps[ai])
                        d.name, d.init = v, e
                        d.type.declname = v
                        b2.block_items[si] = ca.Compound([d, ca.Assignment("=", ca.ID(v), c2), ca.Return(ca.ID(v))])
                    else:
                        v = st2.lvalue.name
                        c2 = st2.rvalue
                        e = c2.args.exprs[ai]
                        c2.args.exprs[ai] = ca.ID(v)
                        b2.block_items[si:si + 1] = [ca.Assignment("=", ca.ID(v), e), st2]
                    where = f" in {tname}" if tname else ""
                    desc = f"{v} = {text(arg)[:24]}; {v} = {call.name.name}({v}, ...){where}"
                    yield desc, ((t2, []) if tname is None else (copy.deepcopy(fn), [t2]))

SPELLINGS = [sp_cmp_sign, sp_call_narrow, sp_ternary, sp_into_loop, sp_flag_take, sp_addr_then_const,
             sp_inline_decl, sp_scoped_operand, sp_nonvoid, sp_index_local, sp_helper_arg,
             sp_split_offset, sp_addr_local, sp_goto_loop,
             sp_addr_before_next, sp_loop_weight,
             sp_count_up, sp_count_down, sp_ptr_walk, sp_ptr_index, sp_rmw_temp, sp_chain_assign,
             sp_sign_shift, sp_fresh_var, sp_neg_mul, sp_member_ptr, sp_offset_local, sp_const_local,
             sp_load_local, sp_symbol_literal, sp_outline_store, sp_outline_setter, sp_loop_normal, sp_ptr_deinduct,
             sp_field_extend, sp_narrow_mask, sp_alias_read, sp_copy_uses, sp_fold_copy, sp_const_literal,
             sp_param_alias, sp_compound_assign]
# type hypotheses: they assert a type the C does not show (a callee that returns u8, a comparison
# done unsigned), so they may change what the draft does. That is their job: a byte match proves the
# ROM does the same. spelltest.py --soundness reports them apart from the rewrites, which must not.
TYPE_SPELLINGS = [sp_cmp_sign, sp_call_narrow, sp_field_extend, sp_alias_read]
# spellings that also edit or add the inline helpers the function calls: sp(fn, helpers)
HELPER_SPELLINGS = [sp_specialize, sp_local_to_arg, sp_arg_to_local, sp_helper_ret_word, sp_result_into_arg]

# decision -> spellings and generic rewrites to try first (names)
DECISIONS = {
    "frame": ["sp_nonvoid", "rw_void"],
    "dead store": ["sp_alias_read"],
    "cse-canon": ["sp_fold_copy", "sp_copy_uses", "sp_param_alias", "rw_copy_var", "sp_inline_decl"],
    "call result": ["sp_call_narrow"],
    "comparison type": ["sp_cmp_sign", "rw_width", "sp_sign_shift", "sp_field_extend"],
    "order": ["sp_compound_assign", "sp_helper_arg", "sp_specialize", "sp_local_to_arg", "sp_arg_to_local", "sp_addr_before_next", "sp_addr_local", "sp_scoped_operand", "sp_addr_then_const", "sp_into_loop", "sp_index_local", "sp_inline_decl",
              "sp_offset_local", "sp_const_local", "sp_load_local", "sp_outline_store", "sp_outline_setter", "sp_rmw_temp", "sp_chain_assign", "sp_fresh_var", "sp_member_ptr",
              "rw_commute", "rw_temp_rvalue", "rw_hoist_args", "rw_swap_stmts", "rw_decl_order", "rw_scoped_const"],
    "loop-invariant": ["sp_goto_loop", "sp_into_loop", "sp_inline_decl", "sp_ptr_walk", "sp_ptr_index"],
    "strength-reduce": ["sp_goto_loop", "sp_into_loop", "perm_loop_form", "sp_ptr_walk", "sp_ptr_index",
                        "sp_count_up", "sp_count_down", "sp_loop_normal", "sp_ptr_deinduct"],
    "cse-skip": ["sp_arg_to_local", "sp_flag_take", "sp_inline_decl", "rw_copy_var"],
    "cse-reuse": ["sp_narrow_mask", "sp_arg_to_local", "sp_member_ptr", "sp_symbol_literal", "sp_load_local", "sp_split_offset", "sp_flag_take", "sp_inline_decl", "rw_copy_var", "rw_reuse_stored"],
    "cse-follow": ["sp_narrow_mask", "sp_flag_take", "sp_inline_decl", "rw_copy_var"],
    "cse-swap": ["rw_commute", "sp_scoped_operand"],
    "cross-jump": ["sp_param_alias", "rw_copy_var", "sp_copy_uses"],
    "regmove-copy": ["rw_commute", "sp_inline_decl"],
    "local-alloc": ["sp_corpus_helper", "sp_helper_ret_word", "sp_result_into_arg", "sp_copy_uses", "sp_param_alias", "sp_loop_weight", "sp_narrow_mask", "sp_inline_decl", "sp_ternary", "rw_width", "rw_decl_order", "rw_copy_var", "rw_inline_var"],
    "global-alloc": ["sp_corpus_helper", "sp_helper_ret_word", "sp_result_into_arg", "sp_copy_uses", "sp_param_alias", "sp_loop_weight", "sp_narrow_mask", "sp_inline_decl", "sp_ternary", "rw_width", "rw_decl_order", "rw_copy_var"],
    "move2add": ["sp_specialize", "sp_addr_then_const", "sp_scoped_operand", "rw_share_const"],
    "reload-mode": ["sp_corpus_helper", "sp_helper_ret_word", "sp_const_literal", "rw_inline_var", "sp_inline_decl", "rw_width", "sp_narrow_mask"],
    "reload": ["sp_compound_assign", "sp_corpus_helper", "sp_helper_ret_word", "sp_symbol_literal", "sp_const_local", "sp_specialize", "sp_addr_then_const", "sp_inline_decl", "rw_share_const",
               "sp_outline_setter", "sp_outline_store", "sp_split_offset", "sp_offset_local"],
}


def all_spellings(fn):
    for sp in SPELLINGS:
        try:
            for desc, f2 in sp(fn):
                yield sp.__name__, desc, f2
        except Exception:
            continue

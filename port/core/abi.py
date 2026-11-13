"""Whole-program ABI normalization for the full-game wasm link.

The GBA calling convention forgives what wasm does not: a call through a
prototype with the wrong argument count, or a code pointer read from ROM data.
This pass works on the per-unit LLVM IR (after data binding and MMIO routing):

* Direct calls are rewritten to the callee's defining signature. Missing
  arguments become zero, extra arguments are dropped and scalar types are
  converted, as an ARM call would have left them in registers.
* Every function with a GBA address keeps that address as its value. A C
  expression such as ``task->update = Foo`` stores 0x08xxxxxx|1, exactly the
  word the ROM build stores, so ROM tables and C agree on code pointers.
* Indirect calls in game code go through ``port_icall``, which looks the
  address up in a generated table of adapters with one canonical signature
  (eight words in, a 64-bit result out, like r0..r3 plus stack and r0:r1).

No instruction body from the ROM is used; the IR is clang's for the game's C.
"""
import re

CANON_SLOTS = 12
SYNTHETIC_BASE = 0x0E000000  # unmapped on the GBA; used for static helpers
_ATTR = re.compile(
    r'^(?:noundef|nonnull|zeroext|signext|inreg|noalias|nocapture|readonly|writeonly|readnone|'
    r'returned|immarg|noext|nofree|nest|swiftself|writable|dead_on_unwind|'
    r'align \d+|[a-z_]+\((?:[^()]|\([^()]*\))*\))\s+')
_SCALARS = {'i1', 'i8', 'i16', 'i32', 'i64', 'ptr', 'float', 'double', 'void'}


def split_top(text, sep=','):
    parts, depth, cur = [], 0, []
    for ch in text:
        if ch in '([{<':
            depth += 1
        elif ch in ')]}>':
            depth -= 1
        if ch == sep and depth == 0:
            parts.append(''.join(cur).strip())
            cur = []
        else:
            cur.append(ch)
    tail = ''.join(cur).strip()
    if tail:
        parts.append(tail)
    return parts


def match_paren(text, start):
    """Index just past the parenthesis group opening at text[start]."""
    depth = 0
    i = start
    in_str = False
    while i < len(text):
        ch = text[i]
        if in_str:
            if ch == '"':
                in_str = False
        elif ch == '"':
            in_str = True
        elif ch in '([{':
            depth += 1
        elif ch in ')]}':
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    raise ValueError('unbalanced: ' + text)


def take_type(text):
    """Split a leading IR type from the rest of the text."""
    text = text.lstrip()
    if text[:1] in '{[<':
        end = match_paren(text.replace('<', '(').replace('>', ')'), 0) if text[0] == '<' else match_paren(text, 0)
        return text[:end], text[end:].lstrip()
    m = re.match(r'(\S+)\s*(.*)$', text, re.S)
    return m[1], m[2]


def strip_attrs(text):
    while True:
        m = _ATTR.match(text)
        if not m:
            return text
        text = text[m.end():]


def last_type(prefix):
    """The return type at the end of a define/declare/call prefix."""
    prefix = prefix.rstrip()
    if prefix.endswith('}') or prefix.endswith(']') or prefix.endswith('>'):
        close = prefix[-1]
        open_ = {'}': '{', ']': '[', '>': '<'}[close]
        depth = 0
        for i in range(len(prefix) - 1, -1, -1):
            if prefix[i] == close:
                depth += 1
            elif prefix[i] == open_:
                depth -= 1
                if depth == 0:
                    return prefix[i:]
    return prefix.split()[-1]


class Sig:
    __slots__ = ('ret', 'params', 'varargs', 'cc')

    def __init__(self, ret, params, varargs=False, cc=''):
        self.ret, self.params, self.varargs, self.cc = ret, params, varargs, cc

    def key(self):
        return (self.ret, tuple(p[0] for p in self.params), self.varargs)

    def wasm_key(self):
        return (wclass(self.ret), tuple(wclass(p[0]) for p in self.params), self.varargs)


def wclass(t):
    if t in ('i1', 'i8', 'i16', 'i32', 'ptr'):
        return 'i32'
    if t == 'i64':
        return 'i64'
    if t == 'float':
        return 'f32'
    if t == 'double':
        return 'f64'
    if t == 'void':
        return 'void'
    return 'agg:' + t


DEF_RE = re.compile(r'^(define|declare)\s(.*?)@([-\w.$]+)\((.*)$')


def parse_params(text, named):
    """text starts just after '('; returns (params, varargs, rest_after_paren)."""
    end = match_paren('(' + text, 0) - 1
    inner, rest = text[:end - 1], text[end:]
    params = []
    varargs = False
    for p in split_top(inner):
        if not p:
            continue
        if p == '...':
            varargs = True
            continue
        ty, after = take_type(p)
        attrs = after
        if named:
            m = re.search(r'\s*(%[-\w.$]+)$', after)
            if m:
                attrs = after[:m.start()]
        params.append((ty, attrs.strip()))
    return params, varargs, rest


def parse_def(line):
    m = DEF_RE.match(line)
    if not m:
        return None
    kind, prefix, name, tail = m.groups()
    params, varargs, rest = parse_params(tail, kind == 'define')
    words = prefix.split()
    cc = ''
    for w in words:
        if w in ('fastcc', 'coldcc') or w.startswith('cc'):
            cc = w
    ret = last_type(prefix)
    internal = kind == 'define' and bool(re.search(r'\b(internal|private)\b', prefix))
    return kind, name, Sig(ret, params, varargs, cc), internal, rest


CALL_RE = re.compile(r'^(\s*)(?:(%[-\w.$]+) = )?(?:(tail|musttail|notail) )?call (.*)$')
CALLEE_RE = re.compile(r'([@%][-\w.$]+|inttoptr \(i32 -?\d+ to ptr\))\(')


def parse_call(line):
    m = CALL_RE.match(line)
    if not m:
        return None
    indent, dest, _tail, rest = m.groups()
    if rest.startswith('void asm') or ' asm ' in rest[:40]:
        return None
    cm = CALLEE_RE.search(rest)
    if not cm:
        return None
    head = rest[:cm.start()]
    callee = cm[1]
    start = cm.end() - 1
    end = match_paren(rest, start)
    args_text = rest[start + 1:end - 1]
    trailer = rest[end:]
    fnty = None
    hm = re.search(r'\(([^()]*)\)\s*$', head)
    if hm and ('...' in hm[1] or hm[1] == ''):
        fnty = hm[0]
        head = head[:hm.start()]
    ret = last_type(head) if head.strip() else 'void'
    cc = ''
    for w in head.split():
        if w in ('fastcc', 'coldcc'):
            cc = w
    args = []
    for a in split_top(args_text):
        ty, after = take_type(a)
        value = strip_attrs(after)
        attrs = after[:len(after) - len(value)].strip()
        args.append((ty, value.strip(), attrs))
    return {'indent': indent, 'dest': dest, 'callee': callee, 'ret': ret, 'args': args,
            'trailer': trailer, 'fnty': fnty, 'cc': cc, 'head': head}


class Namer:
    def __init__(self):
        self.n = 0

    def __call__(self, tag='t'):
        self.n += 1
        return f'%__abi.{tag}{self.n}'


def convert(value, src, dst, out, fresh, signed=False):
    """Emit instructions converting value of type src to dst; return new value."""
    if src == dst:
        return value
    if dst == 'void':
        return None
    if src == 'void' or value is None:
        return zero(dst)
    bits = {'i1': 1, 'i8': 8, 'i16': 16, 'i32': 32, 'i64': 64}
    if src == 'ptr':
        v = fresh()
        out.append(f'{v} = ptrtoint ptr {value} to i32')
        return convert(v, 'i32', dst, out, fresh)
    if dst == 'ptr':
        v = convert(value, src, 'i32', out, fresh, signed)
        r = fresh()
        out.append(f'{r} = inttoptr i32 {v} to ptr')
        return r
    if src == 'float':
        v = fresh()
        out.append(f'{v} = bitcast float {value} to i32')
        return convert(v, 'i32', dst, out, fresh)
    if src == 'double':
        v = fresh()
        out.append(f'{v} = bitcast double {value} to i64')
        return convert(v, 'i64', dst, out, fresh)
    if dst == 'float':
        v = convert(value, src, 'i32', out, fresh)
        r = fresh()
        out.append(f'{r} = bitcast i32 {v} to float')
        return r
    if dst == 'double':
        v = convert(value, src, 'i64', out, fresh)
        r = fresh()
        out.append(f'{r} = bitcast i64 {v} to double')
        return r
    if src in bits and dst in bits:
        r = fresh()
        if bits[src] > bits[dst]:
            out.append(f'{r} = trunc {src} {value} to {dst}')
        else:
            op = 'sext' if signed and src != 'i1' else 'zext'
            out.append(f'{r} = {op} {src} {value} to {dst}')
        return r
    raise ValueError(f'cannot convert {src} -> {dst}')


def zero(t):
    if t == 'ptr':
        return 'null'
    if t in ('float', 'double'):
        return '0.0'
    if t.startswith('{') or t.startswith('['):
        return 'zeroinitializer'
    return '0'


def to_slots(value, ty, out, fresh, signed):
    """Canonical i32 words for one argument."""
    if ty in ('i64', 'double'):
        v = convert(value, ty, 'i64', out, fresh)
        lo, hi, sh = fresh(), fresh(), fresh()
        out.append(f'{lo} = trunc i64 {v} to i32')
        out.append(f'{sh} = lshr i64 {v}, 32')
        out.append(f'{hi} = trunc i64 {sh} to i32')
        return [lo, hi]
    if ty not in _SCALARS:
        raise ValueError('aggregate argument ' + ty)
    return [convert(value, ty, 'i32', out, fresh, signed)]


def from_slots(slots, ty, out, fresh):
    if ty in ('i64', 'double'):
        lo, hi = slots[0], slots[1]
        a, b, s, o = fresh(), fresh(), fresh(), fresh()
        out.append(f'{a} = zext i32 {lo} to i64')
        out.append(f'{b} = zext i32 {hi} to i64')
        out.append(f'{s} = shl i64 {b}, 32')
        out.append(f'{o} = or i64 {a}, {s}')
        return convert(o, 'i64', ty, out, fresh), 2
    return convert(slots[0], 'i32', ty, out, fresh), 1


def is_signed(attrs):
    return 'signext' in attrs


def emit_call(indent, dest, cc, ret, callee, args, trailer='', fresh=None):
    a = ', '.join(f'{t} {v}' for t, v in args)
    cc = (cc + ' ') if cc else ''
    if not dest and ret != 'void':
        # An unnamed non-void result would take the next %N and shift numbering.
        dest = fresh()
    if dest and ret != 'void':
        return f'{indent}{dest} = call {cc}{ret} {callee}({a}){trailer}'
    return f'{indent}call {cc}{ret} {callee}({a}){trailer}'


PADDED = []  # (callee, given, wanted) for calls passing fewer arguments
RESULT = []  # the discarded result adapt_direct just named, if any


def adapt_direct(call, sig, fresh, fill=None, publish=False):
    """Rewrite a direct call to the defining signature. Returns lines or None.

    fill(i, type, out) supplies a missing argument (the register value an ARM
    call would have passed); publish stores r0..r3 to port_regs first."""
    have = [(t, v, at) for t, v, at in call['args']]
    same = (call['ret'] == sig.ret and len(have) == len(sig.params) and
            all(h[0] == p[0] for h, p in zip(have, sig.params)))
    if sig.varargs or call['fnty'] or (same and not publish):
        return None
    out = []
    args = []
    if len(have) < len(sig.params):
        PADDED.append((call['callee'][1:], len(have), len(sig.params)))
    for i, (pty, pattrs) in enumerate(sig.params):
        if i < len(have):
            t, v, at = have[i]
            if t == pty:
                args.append((pty, v))
                continue
            if t not in _SCALARS or pty not in _SCALARS:
                args.append((pty, v if t == pty else zero(pty)))
                continue
            args.append((pty, convert(v, t, pty, out, fresh, is_signed(at))))
        else:
            v = fill(i, pty, out) if fill and pty in _SCALARS else None
            args.append((pty, v if v is not None else zero(pty)))
    if publish:
        # The registers hold what the call site passed, including arguments
        # beyond the definition's parameters (a (void) wrapper forwards them).
        passed = args + [(t, v) for t, v, at in have[len(args):]]
        slots = []
        for t, v in passed:
            if t in _SCALARS and t != 'void':
                slots += to_slots(v, t, out, fresh, False)
        for i, v in enumerate(slots[:4]):
            out.append(f'store i32 {v}, ptr getelementptr (i8, ptr @port_regs, i32 {4 * i}), align 4')
    ind = call['indent']
    lines = [ind + x for x in out]
    want = call['ret']
    dest = call['dest']
    if dest and want != 'void':
        if sig.ret == want:
            lines.append(emit_call(ind, dest, sig.cc, sig.ret, call['callee'], args))
        elif sig.ret == 'void':
            lines.append(emit_call(ind, None, sig.cc, 'void', call['callee'], args, fresh=fresh))
            lines.append(f'{ind}{dest} = {identity(want, zero(want))}')
        else:
            tmp = fresh()
            lines.append(emit_call(ind, tmp, sig.cc, sig.ret, call['callee'], args))
            conv = []
            if want in _SCALARS and sig.ret in _SCALARS:
                v = convert(tmp, sig.ret, want, conv, fresh)
            else:
                v = zero(want)
            lines += [ind + x for x in conv]
            lines.append(f'{ind}{dest} = {identity(want, v)}')
    elif sig.ret in _SCALARS and sig.ret != 'void':
        # The call site discards a result the callee returns: name it, since
        # the next call may read it as r0 (RESULT tells the caller).
        tmp = fresh()
        lines.append(emit_call(ind, tmp, sig.cc, sig.ret, call['callee'], args))
        RESULT[:] = [(tmp, sig.ret)]
    else:
        lines.append(emit_call(ind, None, sig.cc, sig.ret, call['callee'], args, fresh=fresh))
    return lines


def identity(ty, value):
    """An instruction that just names value (LLVM has no plain copy)."""
    if ty == 'ptr':
        return f'getelementptr i8, ptr {value}, i32 0'
    if ty in ('float', 'double'):
        return f'fadd {ty} {value}, 0.0' if value != '0.0' else f'fadd {ty} 0.0, 0.0'
    if ty.startswith('i'):
        return f'add {ty} {value}, 0'
    return f'select i1 true, {ty} {value}, {ty} {value}'


def adapt_indirect(call, fresh):
    out = []
    slots = []
    for t, v, at in call['args']:
        if t not in _SCALARS:
            raise ValueError('aggregate indirect argument ' + t)
        slots += to_slots(v, t, out, fresh, is_signed(at))
    if len(slots) > CANON_SLOTS:
        raise ValueError('too many indirect call words')
    slots += ['0'] * (CANON_SLOTS - len(slots))
    ind = call['indent']
    lines = [ind + x for x in out]
    r = fresh()
    args = ', '.join(f'i32 {s}' for s in slots)
    lines.append(f'{ind}{r} = call i64 @port_icall(ptr {call["callee"]}, {args})')
    want = call['ret']
    if call['dest'] and want != 'void':
        conv = []
        if want in ('i64', 'double'):
            v = convert(r, 'i64', want, conv, fresh)
        else:
            w = fresh()
            conv.append(f'{w} = trunc i64 {r} to i32')
            v = convert(w, 'i32', want, conv, fresh)
        lines += [ind + x for x in conv]
        lines.append(f'{ind}{call["dest"]} = {identity(want, v)}')
    return lines


def adapter(name, sig, symbol):
    """Canonical-signature thunk for a defined function."""
    fresh = Namer()
    body = [f'store i32 %a{i}, ptr getelementptr (i8, ptr @port_regs, i32 {4 * i}), align 4'
            for i in range(4)]
    params = [f'%a{i}' for i in range(CANON_SLOTS)]
    idx = 0
    args = []
    for pty, pattrs in sig.params:
        if pty not in _SCALARS:
            if 'byval' in pattrs or 'sret' in pattrs or pty == 'ptr':
                pty = 'ptr'
            else:
                return None
        need = 2 if pty in ('i64', 'double') else 1
        if idx + need > CANON_SLOTS:
            return None
        v, used = from_slots(params[idx:idx + need], pty, body, fresh)
        idx += used
        attr = ''
        m = re.search(r'\b(byval|sret)\((%[\w.]+|[^)]*)\)', pattrs)
        if m:
            attr = m[0] + ' '
        args.append((pty + (' ' + attr.strip() if attr else ''), v))
    cc = (sig.cc + ' ') if sig.cc else ''
    a = ', '.join(f'{t} {v}' for t, v in args)
    if sig.ret == 'void':
        body.append(f'call {cc}void @{name}({a})')
        body.append('ret i64 0')
    elif sig.ret in _SCALARS:
        r = fresh()
        body.append(f'{r} = call {cc}{sig.ret} @{name}({a})')
        v = convert(r, sig.ret, 'i64', body, fresh)
        body.append(f'ret i64 {v}')
    else:
        r = fresh()
        body.append(f'{r} = call {cc}{sig.ret} @{name}({a})')
        body.append('ret i64 0')
    head = ', '.join(f'i32 {p}' for p in params)
    return f'define hidden i64 @{symbol}({head}) {{\n  ' + '\n  '.join(body) + '\n}\n'


def scan_module(text):
    """Collect external definitions/declarations and value uses of functions."""
    defs, decls, internals = {}, {}, {}
    for line in text.splitlines():
        if line.startswith('define ') or line.startswith('declare '):
            parsed = parse_def(line)
            if not parsed:
                continue
            kind, name, sig, internal, _ = parsed
            if kind == 'define':
                (internals if internal else defs)[name] = sig
            else:
                decls[name] = sig
    return defs, decls, internals


def value_refs(text, names):
    """Function names used other than as a direct callee."""
    used = set()
    for m in re.finditer(r'@([-\w.$]+)(?!\()', text):
        if m[1] in names:
            used.add(m[1])
    return used


PORT_REGS_DECL = '@port_regs = external global [4 x i32]'
LABEL_RE = re.compile(r'^[-\w.$]+:')


def inline_functions(text):
    """Internal functions marked inline: still calls in unoptimized IR, but
    inlined in the ROM, so they leave r0..r3 alone as far as callers see."""
    groups = {m[1] for m in re.finditer(r'^attributes (#\d+) = \{[^}]*\b(?:inlinehint|alwaysinline)\b', text, re.M)}
    return {m[1] for m in re.finditer(r'^define internal [^@]*@([-\w.$]+)\(.*\)[^{]*?(#\d+)', text, re.M)
            if m[2] in groups}


def _clobbers(callee):
    return not (callee.startswith('@llvm.') or callee.startswith('@port_bus_'))


def rewrite_module(text, defs_global, addresses, module_id, game, synthetic, readers=()):
    """Return rewritten IR and adapter info for this module.

    defs_global: name -> Sig of external definitions in the whole program.
    addresses:   name -> code address word for functions with a GBA address.
    synthetic:   dict filled with (module_id, name) -> address for static code.
    readers:     functions that read their incoming registers (see below).

    A call passing fewer arguments than the callee takes relied on ARM register
    state. r0 right after another call is that call's result; before any call,
    r0..r3 are still the caller's own incoming arguments. A function whose
    incoming registers are read this way snapshots port_regs at entry; adapters
    and direct callers of such functions publish r0..r3 there.
    """
    fresh = Namer()
    PADDED.clear()
    found_readers = set()
    unknown = []
    defs, decls, internals = scan_module(text)
    inline = inline_functions(text)
    funcs = set(defs) | set(decls) | set(internals)
    values = {}
    for name in (value_refs(text, funcs) if game else ()):
        if name.startswith('llvm.'):
            continue
        if name in internals:
            key = (module_id, name)
            if key not in synthetic:
                synthetic[key] = SYNTHETIC_BASE + 4 * (len(synthetic) + 1)
            values[name] = synthetic[key]
        elif name in addresses:
            values[name] = addresses[name]
    out = []
    needed_decls = {}
    state = {'params': [], 'name': None, 'calls': 0, 'last': None, 'snap': set(), 'define_at': 0}

    def fill(i, pty, emit):
        last = state['last']
        if i == 0 and last and last[1] not in ('void',) and last[1] in _SCALARS:
            return convert(last[0], last[1], pty, emit, fresh)
        if state['calls'] == 0:
            if i < len(state['params']):
                t, v = state['params'][i]
                if t in _SCALARS:
                    return convert(v, t, pty, emit, fresh)
            if i < 4:
                state['snap'].add(i)
                if state['name']:
                    found_readers.add(state['name'])
                return convert(f'%__abi.reg{i}', 'i32', pty, emit, fresh)
        unknown.append((state['name'], i))
        return None

    for line in text.splitlines():
        if line.startswith('declare '):
            parsed = parse_def(line)
            if parsed and parsed[1] in defs_global and not parsed[1].startswith('llvm.'):
                line = render_declare(parsed[1], defs_global[parsed[1]])
            out.append(line)
            continue
        if line.startswith('define '):
            parsed = parse_def(line)
            names = re.findall(r'(%[-\w.$]+)(?=\s*[,)])', line[line.index('(', line.index('@')):])
            state.update(name=parsed[1] if parsed else None, calls=0, last=None, snap=set(),
                         define_at=len(out), block=f'%{len(names)}', ends={},
                         params=[(p[0], n) for p, n in zip(parsed[2].params, names)] if parsed else [])
            out.append(line)
            continue
        if line == '}' and state['snap']:
            loads = [f'  %__abi.reg{i} = load i32, ptr getelementptr (i8, ptr @port_regs, i32 {4 * i}), align 4'
                     for i in sorted(state['snap'])]
            out[state['define_at'] + 1:state['define_at'] + 1] = loads
            state['snap'] = set()
        if LABEL_RE.match(line):
            # r0 survives branches: inherit the last call result when every
            # (already seen) predecessor ended with the same one.
            state['ends'][state['block']] = state['last']
            state['block'] = '%' + line.split(':')[0]
            pm = re.search(r'; preds = (.*)$', line)
            preds = [p.strip() for p in pm[1].split(',')] if pm else []
            seen = [state['ends'].get(p, 'unseen') for p in preds]
            state['last'] = seen[0] if seen and all(x == seen[0] for x in seen) and seen[0] != 'unseen' else None
        if not line.startswith(' ') or ' call ' not in line:
            out.append(sub_values(line, values) if values else line)
            continue
        call = parse_call(line)
        if not call:
            out.append(sub_values(line, values) if values else line)
            continue
        callee = call['callee']
        lines = None
        RESULT.clear()
        if callee.startswith('@'):
            name = callee[1:]
            if name in defs_global and name not in internals:
                lines = adapt_direct(call, defs_global[name], fresh, fill, publish=name in readers)
        elif game:
            lines = adapt_indirect(call, fresh)
            needed_decls['port_icall'] = ('declare i64 @port_icall(ptr, ' +
                                          ', '.join(['i32'] * CANON_SLOTS) + ')')
        if lines is None:
            out.append(sub_values(line, values) if values else line)
        else:
            out += [sub_values(x, values) if values else x for x in lines]
        if _clobbers(callee) and callee[1:] not in inline:
            state['calls'] += 1
            state['last'] = (call['dest'], call['ret']) if call['dest'] else (RESULT[0] if RESULT else None)
    result = '\n'.join(out) + '\n'
    if game:
        # Indirect calls can reach any function; drop optimizer guarantees.
        result = re.sub(r'\b(?:nocallback)\b', '', result)
    if '@port_regs' in result and not re.search(r'^@port_regs\b', result, re.M):
        result += PORT_REGS_DECL + '\n'
    for name, decl in needed_decls.items():
        if not re.search(r'^declare [^\n]*@' + re.escape(name) + r'\(', result, re.M):
            result += decl + '\n'
    return result, values, internals, list(PADDED), found_readers, unknown


def render_declare(name, sig):
    params = ', '.join(p[0] for p in sig.params)
    if sig.varargs:
        params = (params + ', ...') if params else '...'
    cc = (sig.cc + ' ') if sig.cc else ''
    return f'declare {cc}{sig.ret} @{name}({params})'


def sub_values(line, values):
    def repl(m):
        name = m[1]
        if name in values:
            return f'inttoptr (i32 {values[name]} to ptr)'
        return m[0]
    return re.sub(r'@([-\w.$]+)(?!\(|[-\w.$])', repl, line)

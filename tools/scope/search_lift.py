"""Bounded symbolic Thumb value/address observations for matching sessions.

This is a forward abstract interpretation of decoded instructions, not an inverse
compiler or a semantic equivalence proof. Natural loops are analyzed with header
symbols: only exact backedge equations x_next=x+constant become inductions.
Register names disappear from resolved expressions; ambiguity remains explicit.
"""
from __future__ import annotations

from collections import deque
from dataclasses import dataclass, field
import hashlib
from functools import lru_cache
import json
import re

from search_structure import (_row, _blocks, _effects, _number, _reg, _registers,
                              LOADS, STORES, CALLS, CONDITIONS)

from pathlib import Path

IMPLEMENTATION_ID = hashlib.sha256(Path(__file__).read_bytes()).hexdigest()

MASK = 0xFFFFFFFF
REGS = tuple('r%d' % n for n in range(13)) + ('sp', 'lr', 'flags')
MAX_TERMS = 8
MAX_DEPTH = 9
MAX_NODES = 96
MAX_ALTERNATIVES = 4
UNKNOWN = ('unknown', 'unresolved')
STACK = ('stack',)


def _unknown(reason):
    return ('unknown', reason)


def _const(value):
    return ('const', value & MASK)


def _signed(value):
    value &= MASK
    return value - 0x100000000 if value & 0x80000000 else value




def _parts(value):
    if value[0] == 'const':
        return {}, _signed(value[1])
    if value[0] == 'linear':
        return dict(value[1]), value[2]
    return {value: 1}, 0


def _linear(terms, constant):
    terms = {v: _signed(c) for v, c in terms.items() if c & MASK}
    constant = _signed(constant)
    if not terms:
        return _const(constant)
    if len(terms) == 1 and not constant:
        value, scale = next(iter(terms.items()))
        if scale == 1:
            return value
    if len(terms) > MAX_TERMS:
        return _unknown('expression term budget')
    result = ('linear', tuple(sorted(terms.items(), key=lambda p: repr(p[0]))), constant)
    return result if _depth(result) <= MAX_DEPTH and _nodes(result) <= MAX_NODES else _unknown('expression depth budget')


def _add(a, b, factor=1):
    aa, ac = _parts(a)
    bb, bc = _parts(b)
    out = dict(aa)
    for key, val in bb.items():
        out[key] = out.get(key, 0) + factor * val
    return _linear(out, ac + factor * bc)


def _scale(value, scale):
    terms, constant = _parts(value)
    return _linear({key: coefficient * scale for key, coefficient in terms.items()}, constant * scale)


def _delta(value, original):
    terms, constant = _parts(_add(value, original, -1))
    return constant if not terms else None


def _phi(values):
    values = set(values)
    if len(values) == 1:
        return next(iter(values))
    flat = set()
    for value in values:
        flat.update(value[1] if value[0] == 'phi' else (value,))
    if len(flat) > MAX_ALTERNATIVES:
        return _unknown('join alternative budget')
    result = ('phi', tuple(sorted(flat, key=repr)))
    return result if _depth(result) <= MAX_DEPTH and _nodes(result) <= MAX_NODES else _unknown('join depth budget')


def _op(op, a, b=None):
    if op == 'add':
        return _add(a, b)
    if op == 'sub':
        return _add(a, b, -1)
    if op == 'mul' and b[0] == 'const':
        return _scale(a, b[1])
    if op == 'mul' and a[0] == 'const':
        return _scale(b, a[1])
    if op == 'lsl' and b[0] == 'const' and b[1] < 32:
        return _scale(a, 1 << b[1])
    if a[0] == 'const' and (b is None or b[0] == 'const'):
        x, y = a[1], b[1] if b else 0
        if op == 'and': return _const(x & y)
        if op == 'or': return _const(x | y)
        if op == 'xor': return _const(x ^ y)
        if op == 'not': return _const(~x)
        if op == 'mul': return _const(x * y)
        if op == 'lsr': return _const(x >> y if y < 32 else 0)
        if op == 'asr': return _const(_signed(x) >> min(y, 31))
    if op == 'and' and b[0] == 'const':
        if b[1] == MASK: return a
        if b[1] == 0: return _const(0)
    result = (op, a) if b is None else (op, a, b)
    return result if _depth(result) <= MAX_DEPTH and _nodes(result) <= MAX_NODES else _unknown('expression depth budget')




def _is_stack(value):
    terms, _ = _parts(value)
    return terms.get(STACK) == 1 and len(terms) == 1


def _stack_in(value):
    # A load's address is not the loaded value. Forwarded stack pointers remain
    # explicit arithmetic expressions, while loads carry provenance separately.
    if not isinstance(value, tuple) or value[0] in ('load', 'call-result'):
        return False
    if _is_stack(value):
        return True
    return any(_stack_in(v) for v in _children(value) if isinstance(v, tuple))


def _reglist(text):
    out = []
    for item in text.strip('{} !').split(','):
        item = item.strip()
        if '-' in item:
            left, right = item.split('-', 1)
            if re.fullmatch(r'r\d+', left) and re.fullmatch(r'r\d+', right):
                out.extend('r%d' % n for n in range(int(left[1:]), int(right[1:]) + 1))
        elif _registers(item):
            out.append(_reg(item))
    number = lambda r: 13 if r == 'sp' else 14 if r == 'lr' else 15 if r == 'pc' else int(r[1:])
    return sorted(set(out), key=number)


@dataclass
class State:
    regs: dict
    memory: dict
    escaped: bool = False
    pending_stack_stores: dict = field(default_factory=dict)

    def copy(self):
        return State(dict(self.regs), dict(self.memory), self.escaped, dict(self.pending_stack_stores))


def _initial():
    return State({r: ('arg', int(r[1:])) if r in ('r0', 'r1', 'r2', 'r3') else STACK if r == 'sp'
                  else ('input', r) for r in REGS}, {})


def _merge(states):
    if not states:
        return _initial()
    regs = {r: _phi(s.regs.get(r, UNKNOWN) for s in states) for r in REGS}
    common = set(states[0].memory)
    for state in states[1:]:
        common &= state.memory.keys()
    memory = {key: _phi(state.memory[key] for state in states) for key in common}
    pending_keys = set(states[0].pending_stack_stores)
    for state in states[1:]:
        pending_keys &= state.pending_stack_stores.keys()
    pending = {key: tuple(sorted({pc for state in states for pc in state.pending_stack_stores[key]})) for key in pending_keys}
    return State(regs, memory, any(s.escaped for s in states), pending)


def _dominator_loops(blocks):
    ids = {b['id'] for b in blocks}
    preds = {bid: [] for bid in ids}
    for b in blocks:
        for edge in b['successors']:
            preds[edge['block']].append(b['id'])
    reachable, pending = set(), [blocks[0]['id']]
    by_id = {b['id']: b for b in blocks}
    while pending:
        current = pending.pop()
        if current in reachable:
            continue
        reachable.add(current)
        pending.extend(e['block'] for e in by_id[current]['successors'])
    dom = {bid: ({bid} if bid == blocks[0]['id'] else set(reachable)) for bid in reachable}
    changed = True
    while changed:
        changed = False
        for b in blocks[1:]:
            bid = b['id']
            if bid not in reachable:
                continue
            incoming = [dom[p] for p in preds[bid] if p in reachable]
            new = {bid} | (set.intersection(*incoming) if incoming else set())
            if new != dom[bid]:
                dom[bid] = new
                changed = True
    loops = {}
    for b in blocks:
        for edge in b['successors']:
            header = edge['block']
            if b['id'] in reachable and header in dom.get(b['id'], set()):
                loops.setdefault(header, []).append(b['id'])
    details = {}
    for n, (header, back) in enumerate(sorted(loops.items(), key=lambda p: by_id[p[0]]['start'])):
        members, pending = {header}, list(back)
        while pending:
            node = pending.pop()
            if node in members:
                continue
            members.add(node)
            pending.extend(preds[node])
        details[header] = dict(id='loop%d' % n, backedges=back,
                               external=[p for p in preds[header] if p not in back], members=members)
    return preds, details, reachable


def _operand(text, state, offset, entry_address=None):
    text = text.strip()
    if text in state.regs:
        return state.regs[text]
    canonical = _reg(text)
    if canonical == 'pc':
        return (('code', ((entry_address + offset + 4) & ~3) - entry_address) if entry_address is not None
                else _unknown('PC alignment unknown without entry address'))
    if canonical in state.regs:
        return state.regs[canonical]
    if text and (text[0] in '#=-' or text[0].isdigit()):
        number = _number(text.lstrip('='))
        if number is not None:
            return _const(number)
    return ('symbol', text.lstrip('=')) if text.startswith('=') else _unknown('unparsed operand ' + text)


@lru_cache(maxsize=4096)
def _address_parts(text):
    match = re.fullmatch(r'\[([^]]+)\]!?', text.strip())
    return tuple(p.strip() for p in match.group(1).split(',')) if match else None


def _address(text, state, offset, entry_address=None):
    parts = _address_parts(text)
    if not parts:
        return _unknown('unparsed address ' + text)
    result = _operand(parts[0], state, offset, entry_address)
    for part in parts[1:]:
        result = _add(result, _operand(part, state, offset, entry_address))
    return result


def _store(state, address, width, value, *, explicit=False, site=None):
    # Current-frame stack cells stay private until an address escapes. Other
    # memory is not forwarded: unrelated pointer bases may alias.
    if not _is_stack(address):
        if _stack_in(value):
            state.escaped = True
        state.memory = {key: val for key, val in state.memory.items() if _is_stack(key[0]) and not state.escaped}
        return
    _, offset = _parts(address)
    for key in list(state.memory):
        other, size = key
        if _is_stack(other):
            _, pos = _parts(other)
            if offset < pos + size and pos < offset + width:
                del state.memory[key]
                state.pending_stack_stores.pop(key, None)
    state.memory[(address, width)] = value
    if explicit:
        state.pending_stack_stores[(address, width)] = (site,)


def _load(state, address, width, signed, offset):
    value = state.memory.get((address, width)) if _is_stack(address) else None
    if value is not None:
        if width == 4:
            return value
        if value[0] == 'const':
            raw = value[1] & ((1 << (8 * width)) - 1)
            if signed and raw & (1 << (8 * width - 1)):
                raw -= 1 << (8 * width)
            return _const(raw)
        return ('sext' if signed else 'zext', 8 * width, value)
    return ('load', width, bool(signed), address, offset)


def _transfer(row, incoming, observations=None, literals=None, entry_address=None):
    state = incoming
    op, args, pc = row['op'], row['operands'], row['offset']
    record = lambda kind, **fields: observations.append(dict(kind=kind, offset=pc, instruction=row['text'], **fields)) if observations is not None else None
    val = lambda text: _operand(text, state, pc, entry_address)
    if op in CALLS | {'swi', 'svc', 'cmp', 'cmn', 'tst', 'teq'} or op.endswith('s') and op not in CONDITIONS:
        state.regs['flags'] = _unknown('flags changed by ' + op)
    if op in LOADS and len(args) >= 2:
        dst = _reg(args[0])
        if args[1].startswith('='):
            state.regs[dst] = val(args[1])
            return state
        address = _address(args[1], state, pc, entry_address)
        width, signed = LOADS[op]
        # Raw PC-relative loads may still point at a decoded literal pool.
        terms, displacement = _parts(address)
        code_terms = [(key, scale) for key, scale in terms.items() if key[0] == 'code']
        literal = None
        if len(code_terms) == 1 and len(terms) == 1 and code_terms[0][1] == 1:
            literal = (literals or {}).get(code_terms[0][0][1] + displacement)
        value = _const(literal) if literal is not None else _load(state, address, width, signed, pc)
        record('load', address=address, width=width, signed=signed, value=value)
        state.regs[dst] = value
    elif op in STORES and len(args) >= 2:
        address, width, value = _address(args[1], state, pc, entry_address), STORES[op], val(args[0])
        record('store', address=address, width=width, signed=None, value=value)
        _store(state, address, width, value, explicit=True, site=pc)
    elif op in ('push', 'pop'):
        registers = _reglist(args[0]) if args else []
        sp = state.regs['sp']
        base = _add(sp, _const(-4 * len(registers))) if op == 'push' else sp
        for index, register in enumerate(registers):
            address = _add(base, _const(4 * index))
            if op == 'push':
                _store(state, address, 4, state.regs.get(register, UNKNOWN))
            else:
                state.regs[register] = _load(state, address, 4, False, pc)
        state.regs['sp'] = base if op == 'push' else _add(sp, _const(4 * len(registers)))
    elif op in ('ldm', 'ldmia', 'stm', 'stmia') and len(args) >= 2:
        base_reg = _reg(args[0].rstrip('!'))
        base = state.regs.get(base_reg, UNKNOWN)
        registers = _reglist(args[1])
        for index, register in enumerate(registers):
            address = _add(base, _const(index * 4))
            if op.startswith('ldm'):
                value = _load(state, address, 4, False, pc)
                record('load', address=address, width=4, signed=False, value=value, multiple_index=index)
                state.regs[register] = value
            else:
                value = state.regs.get(register, UNKNOWN)
                record('store', address=address, width=4, signed=None, value=value, multiple_index=index)
                _store(state, address, 4, value, explicit=True, site=pc)
        if '!' in args[0]:
            state.regs[base_reg] = _add(base, _const(len(registers) * 4))
    elif op in CALLS or op in ('swi', 'svc'):
        target = val(args[0]) if args else _unknown('call target')
        arguments = tuple(state.regs['r%d' % n] for n in range(4))
        stack_args = []
        for n in range(8):
            address = _add(state.regs['sp'], _const(4 * n))
            if (address, 4) not in state.memory or (address, 4) not in state.pending_stack_stores:
                break
            stack_args.append((n, state.memory[(address, 4)], state.pending_stack_stores[(address, 4)]))
        record('call', target=target, arguments=arguments, stack_arguments=tuple(stack_args))
        if state.escaped or any(_stack_in(a) for a in arguments) or any(_stack_in(v) for _, v, _ in stack_args):
            state.memory.clear()
            state.escaped = True
        state.pending_stack_stores.clear()
        state.regs.update({r: _unknown('call clobber') for r in ('r0', 'r1', 'r2', 'r3', 'r12', 'lr')})
        state.regs['r0'] = ('call-result', target, arguments, pc)
    elif op in ('mov', 'movs') and len(args) == 2:
        state.regs[_reg(args[0])] = val(args[1])
    elif op in ('mvn', 'mvns', 'neg', 'negs') and len(args) == 2:
        state.regs[_reg(args[0])] = _op('not', val(args[1])) if op.startswith('mvn') else _scale(val(args[1]), -1)
    elif op in ('adr',) and len(args) == 2:
        state.regs[_reg(args[0])] = (_add(val('pc'), _const(_number(args[1]))) if _number(args[1]) is not None
                                     else _unknown('unparsed ADR operand'))
    elif op.rstrip('s') in ('add', 'sub', 'mul', 'and', 'orr', 'eor', 'bic', 'lsl', 'lsr', 'asr', 'rsb') and len(args) in (2, 3):
        code = op.rstrip('s')
        dst = _reg(args[0])
        left, right = (val(args[0]), val(args[1])) if len(args) == 2 else (val(args[1]), val(args[2]))
        if code in ('lsl', 'lsr', 'asr') and _number(args[-1]) is None:
            right = _op('and', right, _const(255))
        if code == 'rsb':
            left, right, code = right, left, 'sub'
        if code == 'bic':
            right, code = _op('not', right), 'and'
        state.regs[dst] = _op({'orr': 'or', 'eor': 'xor'}.get(code, code), left, right)
    elif op == 'cmp' and len(args) == 2:
        state.regs['flags'] = ('compare', val(args[0]), val(args[1]))
    elif op in CONDITIONS:
        comparison = state.regs.get('flags', UNKNOWN)
        relations = {'beq': '==', 'bne': '!=', 'bgt': '>', 'bge': '>=', 'blt': '<', 'ble': '<=',
                     'bhs': '>=', 'bcs': '>=', 'blo': '<', 'bcc': '<', 'bhi': '>', 'bls': '<='}
        domain = ('equality' if op in ('beq', 'bne') else 'signed' if op in ('bgt', 'bge', 'blt', 'ble')
                  else 'unsigned' if op in ('bhs', 'bcs', 'blo', 'bcc', 'bhi', 'bls') else 'processor-flags')
        known = comparison[0] == 'compare'
        record('branch', condition=op, domain=domain, relation=relations.get(op), target=row.get('target'),
               arguments=comparison[1:] if known else (), status='known-compare' if known else 'unknown-flags')
    elif op in {'b', 'bx', 'cmn', 'tst', 'teq', 'nop'} or row['data']:
        pass
    else:
        _, writes, known = _effects(row)
        for register in writes:
            if register != 'flags':
                state.regs[register] = _unknown('unsupported instruction ' + op)
        if not known:
            state.regs = {r: _unknown('unsupported instruction ' + op) for r in REGS}
            state.memory.clear()
            state.pending_stack_stores.clear()
            state.escaped = True
            record('unknown', reason='unsupported instruction')
    return state


def _children(value):
    return value[1:] if value and isinstance(value[0], str) else value


@lru_cache(maxsize=16384)
def _depth(value):
    if not isinstance(value, tuple):
        return 0
    tagged = bool(value and isinstance(value[0], str))
    return int(tagged) + max((_depth(x) for x in _children(value) if isinstance(x, tuple)), default=0)


@lru_cache(maxsize=16384)
def _nodes(value):
    if not isinstance(value, tuple):
        return 0
    tagged = bool(value and isinstance(value[0], str))
    return int(tagged) + sum(_nodes(x) for x in _children(value) if isinstance(x, tuple))


def _contains(value, needle):
    if value == needle:
        return True
    return isinstance(value, tuple) and any(_contains(x, needle) for x in _children(value) if isinstance(x, tuple))


@lru_cache(maxsize=16384)
def _has_unknown(value):
    if not isinstance(value, tuple):
        return False
    if value and isinstance(value[0], str) and value[0] in ('unknown', 'loop', 'input', 'phi'):
        return True
    return any(_has_unknown(x) for x in _children(value) if isinstance(x, tuple))


def _expand(value, solutions, active=(), cache=None, partial=False):
    key = (value, active, partial)
    if cache is not None and key in cache:
        return cache[key]
    result = _expand_uncached(value, solutions, active, cache, partial)
    if cache is not None:
        cache[key] = result
    return result


def _expand_uncached(value, solutions, active=(), cache=None, partial=False):
    if not isinstance(value, tuple):
        return value
    if value and value[0] == 'loop':
        if value not in solutions and partial:
            return value
        if value in active:
            return _unknown('recursive loop equation')
        return _expand(solutions.get(value, _unknown('unsolved loop equation')), solutions, active + (value,), cache, partial)
    if value and value[0] == 'linear':
        result = _const(value[2])
        for atom, scale in value[1]:
            result = _add(result, _scale(_expand(atom, solutions, active, cache, partial), scale))
        return result
    if value and value[0] == 'phi':
        return _phi(_expand(v, solutions, active, cache, partial) for v in value[1])
    result = tuple(_expand(v, solutions, active, cache, partial) if isinstance(v, tuple) else v for v in value)
    return result if _depth(result) <= MAX_DEPTH and _nodes(result) <= MAX_NODES else _unknown('expanded expression budget')


@lru_cache(maxsize=16384)
def _structural(value):
    """A comparison descriptor, not a temporal equality assertion for loads."""
    if not isinstance(value, tuple):
        return value
    if value and value[0] == 'load':
        return ('load', value[1], value[2], _structural(value[3]))
    if value and value[0] == 'call-result':
        return ('call-result', _structural(value[1]), _structural(value[2]))
    return tuple(_structural(v) if isinstance(v, tuple) else v for v in value)


@lru_cache(maxsize=16384)
def _id(value):
    return hashlib.sha256(repr(value).encode()).hexdigest()[:16]


@lru_cache(maxsize=16384)
def _pretty(value):
    if not isinstance(value, tuple):
        return str(value)
    kind = value[0]
    if kind == 'const': return str(value[1]) if value[1] < 10 else '0x%X' % value[1]
    if kind == 'arg': return 'arg%d' % value[1]
    if kind == 'stack': return 'entry_sp'
    if kind == 'iter': return 'iteration[%s]' % value[1]
    if kind == 'symbol': return value[1]
    if kind == 'code': return 'code+0x%X' % value[1]
    if kind == 'input': return 'entry_%s' % value[1]
    if kind == 'unknown': return '?{%s}' % value[1]
    if kind == 'loop': return '?{%s:%s}' % (value[1], value[2])
    if kind == 'phi': return '{' + ' | '.join(_pretty(v) for v in value[1]) + '}'
    if kind == 'linear':
        pieces = []
        for atom, coefficient in value[1]:
            text = _pretty(atom)
            if atom[0] == 'phi': text = '(' + text + ')'
            pieces.append((coefficient < 0, text if abs(coefficient) == 1 else '%s*%s' % (_pretty(_const(abs(coefficient))), text)))
        if value[2]:
            pieces.append((value[2] < 0, _pretty(_const(abs(value[2])))))
        return ''.join(('-' if negative and index == 0 else ' - ' if negative else '' if index == 0 else ' + ') + text
                       for index, (negative, text) in enumerate(pieces)) or '0'
    if kind == 'load':
        return 'load_%s%d(%s)@+0x%X' % ('s' if value[2] else 'u', 8 * value[1], _pretty(value[3]), value[4])
    if kind == 'call-result': return 'return(%s)@+0x%X' % (_pretty(value[1]), value[3])
    if kind in ('zext', 'sext'): return '%s%d(%s)' % (kind, value[1], _pretty(value[2]))
    symbols = {'and': '&', 'or': '|', 'xor': '^', 'mul': '*', 'lsl': '<<', 'lsr': '>>u', 'asr': '>>s'}
    if kind == 'not': return '~(' + _pretty(value[1]) + ')'
    if kind in symbols: return '(' + _pretty(value[1]) + ' ' + symbols[kind] + ' ' + _pretty(value[2]) + ')'
    return str(value)


def _value(value):
    return dict(id=_id(value), text=_pretty(value), expression=value,
                certainty='ambiguous' if _has_unknown(value) else 'symbolic',
                structural_key=None if _has_unknown(value) else _id(_structural(value)))


def _address_view(value):
    result = _value(value)
    terms, displacement = _parts(value)
    result['displacement'] = displacement
    result['terms'] = [{'value': _value(atom), 'scale': scale} for atom, scale in terms.items()]
    result['stack_relative'] = _is_stack(value)
    return result


def lift(rows, data_ranges=(), *, entry_address=None):
    """Lift decoded Thumb instructions into bounded symbolic observations.

    Accepts scope decoded tuples, dictionaries, or instruction strings. Offsets
    are function-relative. `data_ranges` are half-open function-relative pairs.
    `entry_address` resolves raw PC alignment; ordinary decoded `ldr =literal`
    needs no extra context. Results are JSON-serializable; unresolved joins and unsupported instructions
    cannot receive comparable structural keys.
    """
    normalized = [_row(row, n) for n, row in enumerate(rows)]
    if entry_address is None:
        explicit = {r['address'] - r['offset'] for r in normalized if 'address' in r and r['address'] != r['offset']}
        if len(explicit) == 1:
            entry_address = explicit.pop()
        elif normalized and normalized[0]['offset'] >= 0x08000000:
            entry_address = 0  # absolute-address rows already determine alignment
    for row in normalized:
        if any(lo <= row['offset'] < hi for lo, hi in data_ranges):
            row['data'] = True
    literals = {}
    for row in normalized:
        if row['text'].lstrip().startswith('.word ') and row['operands']:
            number = _number(row['operands'][0])
            if number is not None:
                literals[row['offset']] = number
    blocks = _blocks(normalized)
    if not blocks:
        return dict(summary=dict(instructions=0, memory=0, calls=0), memory=[], calls=[], branches=[], values=[], loops=[], warnings=[])
    by_id = {b['id']: b for b in blocks}
    predecessors, loops, reachable = _dominator_loops(blocks)
    entry = blocks[0]['id']
    inputs, outputs = {}, {}
    queue, queued = deque([entry]), {entry}
    visits, limit = 0, max(64, 10 * len(blocks))
    initial = _initial()

    def incoming(bid):
        states = [outputs[p] for p in predecessors[bid] if p in outputs]
        if bid == entry:
            states.insert(0, initial)
        if not states:
            return None
        merged = _merge(states)
        if bid in loops:
            loop = loops[bid]
            seed_states = [outputs[p] for p in loop['external'] if p in outputs]
            if bid == entry:
                seed_states.insert(0, initial)
            seed = _merge(seed_states) if seed_states else None
            merged.regs = {r: merged.regs[r] if r == 'sp' else ('loop', loop['id'], r) for r in REGS}
            merged.memory = {key: ('loop', loop['id'], 'memory:' + _id(key)) for key in (seed.memory if seed else {})}
        return merged

    while queue and visits < limit:
        bid = queue.popleft()
        queued.discard(bid)
        state = incoming(bid)
        if state is None or inputs.get(bid) == state and bid in outputs:
            continue
        inputs[bid] = state
        state = state.copy()
        visits += 1
        for row in by_id[bid]['instructions']:
            state = _transfer(row, state, literals=literals, entry_address=entry_address)
        if outputs.get(bid) != state:
            outputs[bid] = state
            for edge in by_id[bid]['successors']:
                dst = edge['block']
                if dst not in queued:
                    queue.append(dst)
                    queued.add(dst)

    solutions, loop_report = {}, []
    # Project inner-loop invariants before solving an outer backedge. A value
    # reset to a base stays a join; only explicit header+constant is induction.
    for bid, loop in sorted(loops.items(), key=lambda item: (len(item[1]['members']), by_id[item[0]]['start'])):
        seed_states = [outputs[p] for p in loop['external'] if p in outputs]
        if bid == entry:
            seed_states.insert(0, initial)
        seed = _merge(seed_states) if seed_states else None
        back = [outputs[p] for p in loop['backedges'] if p in outputs]
        equations = []
        if seed is None or len(back) != len(loop['backedges']):
            loop_report.append(dict(id=loop['id'], header=by_id[bid]['start'], status='unreachable-or-incomplete', equations=[]))
            continue
        candidates = [(r, seed.regs[r], [s.regs[r] for s in back]) for r in REGS if r != 'sp']
        candidates += [('memory:' + _id(key), value, [s.memory.get(key, UNKNOWN) for s in back]) for key, value in seed.memory.items()]
        # Resolve invariant step registers before any induction, regardless of
        # hard-register number. Otherwise r4 += r7 would depend on solve order.
        for _ in range(len(candidates)):
            changed = False
            for name, start, values in candidates:
                symbol = ('loop', loop['id'], name)
                if symbol in solutions:
                    continue
                projected = [_expand(value, solutions, partial=True) for value in values]
                initial_value = _expand(start, solutions, partial=True)
                if all(value == symbol for value in projected) or all(value == initial_value for value in projected):
                    solutions[symbol] = initial_value
                    changed = True
            if not changed:
                break
        for name, start, values in candidates:
            if ('loop', loop['id'], name) in solutions:
                continue
            start = _expand(start, solutions, partial=True)
            values = [_expand(value, solutions, partial=True) for value in values]
            symbol = ('loop', loop['id'], name)
            steps = [_delta(value, symbol) for value in values]
            if steps and steps[0] is not None and all(d == steps[0] for d in steps):
                step = steps[0]
                solution = start if step == 0 else _add(start, _scale(('iter', loop['id']), step))
                status = 'invariant' if step == 0 else 'affine-induction'
            elif not any(_contains(value, symbol) for value in values):
                solution, step, status = _phi([start] + values), None, 'join'
            else:
                solution, step, status = _unknown('non-affine loop recurrence'), None, 'unresolved'
            solutions[symbol] = solution
            if status != 'invariant':
                equations.append(dict(register=name, status=status, stride=step, initial=_value(start), expression=_value(solution)))
        loop_report.append(dict(id=loop['id'], header=by_id[bid]['start'], status='analyzed', equations=equations))

    loop_report.sort(key=lambda item: item['header'])
    observations = []
    for block in blocks:
        bid = block['id']
        if bid not in inputs:
            continue
        state = inputs[bid].copy()
        for row in block['instructions']:
            state = _transfer(row, state, observations, literals, entry_address)
    memory, calls, branches, unknown, graph = [], [], [], [], {}

    def add_graph(value):
        if not isinstance(value, tuple):
            return
        if value and isinstance(value[0], str):
            key = _id(value)
            if key in graph:
                return
            graph[key] = _value(value)
        for child in _children(value):
            if isinstance(child, tuple):
                add_graph(child)

    expanded, expansion_cache = {}, {}
    def resolve(value):
        if value not in expanded:
            expanded[value] = _expand(value, solutions, cache=expansion_cache)
        return expanded[value]

    for observation in observations:
        kind = observation['kind']
        if kind in ('load', 'store'):
            address = resolve(observation['address'])
            value = resolve(observation['value'])
            record = {**observation, 'address': _address_view(address), 'value': _value(value)}
            record['identity_key'] = None if _has_unknown(address) else _id((kind, observation['width'], observation['signed'], _structural(address)))
            record['identity_basis'] = 'structural symbolic address and access type; does not assert temporal memory equivalence'
            memory.append(record)
            add_graph(address)
            add_graph(value)
        elif kind == 'branch':
            arguments = tuple(resolve(v) for v in observation['arguments'])
            record = {**observation, 'arguments': [_value(v) for v in arguments]}
            record['identity_key'] = (None if not arguments or any(_has_unknown(v) for v in arguments)
                                      or observation['relation'] is None else
                                      _id((observation['domain'], observation['relation'], _structural(arguments))))
            branches.append(record)
            for value in arguments:
                add_graph(value)
        elif kind == 'call':
            target = resolve(observation['target'])
            arguments = tuple(resolve(v) for v in observation['arguments'])
            stack_arguments = [(n, resolve(v), sites) for n, v, sites in observation['stack_arguments']]
            record = {**observation, 'target': _value(target), 'arguments': [_value(v) for v in arguments],
                      'stack_arguments': [dict(word=n, value=_value(v), store_offsets=sites,
                                              status='candidate: explicit contiguous SP store since previous call; arity unknown')
                                          for n, v, sites in stack_arguments],
                      'arguments_are': 'register ABI locations and explicit outgoing stack candidates; callee arity is unknown',
                      'identity_key': None if _has_unknown(target) else _id(_structural(target))}
            calls.append(record)
            add_graph(target)
            for value in arguments:
                add_graph(value)
        else:
            unknown.append(observation)
    warnings = []
    if queue:
        warnings.append('Fixed-point iteration budget reached; observations are provisional and cannot be paired as determined identities.')
        for access in memory:
            access['identity_key'] = None
            access['address']['certainty'] = 'ambiguous'
            access['address']['structural_key'] = None
        for branch in branches:
            branch['identity_key'] = None
        for call in calls:
            call['identity_key'] = None
    if unknown:
        warnings.append('Unsupported instructions invalidate affected register and memory knowledge.')
    return dict(version=1,
                summary=dict(instructions=sum(len(b['instructions']) for b in blocks), memory=len(memory), calls=len(calls),
                             comparable_memory=sum(m['identity_key'] is not None for m in memory),
                             ambiguous_memory=sum(m['identity_key'] is None for m in memory),
                             loops=len(loops), branches=len(branches), known_compare_branches=sum(b['status'] == 'known-compare' for b in branches), fixed_point_visits=visits, fixed_point_complete=not queue,
                             unreachable_blocks=len(set(by_id) - reachable), unsupported=len(unknown)),
                memory=memory, calls=calls, branches=branches, values=list(graph.values()), loops=loop_report,
                unknown_instructions=unknown, warnings=warnings,
                assumptions=['32-bit Thumb arithmetic modulo 2^32; ABI call-clobbered r0-r3/r12/lr',
                             'current stack frame is private until a stack-derived pointer escapes; escaped frame values are invalidated on calls',
                             'load/call expressions describe provenance and structure, not equality of observations at different times'],
                interpretation='Machine symbolic observations constrain the program but do not identify unique source variables, types, or C spellings.')


def compare(target, current):
    """Pair observable addresses/types, exposing duplicate identities and unknowns."""
    from collections import defaultdict
    left, right = defaultdict(list), defaultdict(list)
    for item in target.get('memory', []):
        if item['identity_key']:
            left[item['identity_key']].append(item)
    for item in current.get('memory', []):
        if item['identity_key']:
            right[item['identity_key']].append(item)
    paired, target_only, current_only = [], [], []
    for key in sorted(set(left) | set(right)):
        a, b = left[key], right[key]
        shared = min(len(a), len(b))
        for n in range(shared):
            paired.append(dict(identity_key=key, target_offset=a[n]['offset'], current_offset=b[n]['offset'],
                               address=a[n]['address']['text'], kind=a[n]['kind'], width=a[n]['width'], signed=a[n]['signed'],
                               multiplicity_ambiguous=len(a) > 1 or len(b) > 1,
                               value_structure_same=a[n]['value']['structural_key'] is not None and a[n]['value']['structural_key'] == b[n]['value']['structural_key']))
        target_only.extend(a[shared:])
        current_only.extend(b[shared:])
    unresolved = dict(target=[m for m in target.get('memory', []) if not m['identity_key']],
                      current=[m for m in current.get('memory', []) if not m['identity_key']])
    return dict(paired=paired, target_only=target_only, current_only=current_only, unresolved=unresolved,
                interpretation='Address/type correspondence only; ordering, aliasing, calls, and dynamic path counts remain separate constraints.')

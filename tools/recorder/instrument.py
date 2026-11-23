"""The recorder's compiler pass: instrument one unit's LLVM IR (port/build_game.py --record).

Input is a unit's IR after the port's ABI pass, compiled with -g. `opt -passes=sroa`
first turns locals into SSA values, so the remaining loads and stores are real memory
traffic (globals, heap, GBA RAM, address-taken locals). Then every

  store / load            -> __rec_st/__rec_ld(site, address, value, width code); 64-bit: *64
  llvm.memcpy / memmove   -> __rec_cpy(site, dst, src, length)
  llvm.memset             -> __rec_set(site, dst, byte, length)
  port_bus_load/store_*   -> the same, for the bus access (MMIO, keys, sensor)
  basic block (not entry) -> __rec_bb(site)
  function entry / return -> __rec_fn(site, 0) / __rec_fn(site, 1)

calls the runtime in tools/recorder/rec_rt.c, which logs according to its mode.
A site id is kind(3 bits) | unit(13) | local(16); the kind gives the event's length
(see rec_rt.c). The static facts the slicer needs (every instruction of every
instrumented function with its C line and column, block labels, the site ids, SSA
variable names) are written beside the IR as JSON.
"""
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'port/core'))
from abi import split_top, take_type  # noqa: E402

K_BB, K_LOAD, K_STORE, K_WIDE, K_CPY, K_SET, K_AGG, K_FN = range(8)
WCODE = {1: 0, 2: 1, 4: 2, 8: 3}

DBG = re.compile(r',\s*!(?:dbg|tbaa|range|noundef|nonnull|llvm\.\w+|annotation|srcloc|heapallocsite|DIAssignID|align|prof|invariant\.\w+|alias\.scope|noalias|callees|nosanitize|noalias\.addrspace)\s+!\d+')
DBG_ID = re.compile(r'!dbg !(\d+)')
LABEL = re.compile(r'^("?[-\w.$]+"?):(?:\s|$)')
DEFINE = re.compile(r'^define [^@]*@("?[-\w.$]+"?)\((.*)\)[^(]*?(?:!dbg !(\d+))?\s*\{$')
RET = re.compile(r'^(\s+)ret\b')
PHI = re.compile(r'^\s+%[-\w.$]+ = phi ')
DBGVAL = re.compile(r'^\s+#dbg_(?:value|declare)\((?:\w+ )?(%[-\w.$]+|[^,]+), !(\d+),')
DILOC = re.compile(r'^!(\d+) = (?:distinct )?!DILocation\(line: (\d+)(?:, column: (\d+))?, scope: !(\d+)')
DIVAR = re.compile(r'^!(\d+) = (?:distinct )?!DILocalVariable\(name: "([^"]+)"')
DISP = re.compile(r'^!(\d+) = (?:distinct )?!DISubprogram\(name: "([^"]+)".*?line: (\d+)')

DECLS = """
declare void @__rec_bb(i32)
declare void @__rec_fn(i32, i32)
declare void @__rec_ld(i32, ptr, i32, i32)
declare void @__rec_st(i32, ptr, i32, i32)
declare void @__rec_ld64(i32, ptr, i64)
declare void @__rec_st64(i32, ptr, i64)
declare void @__rec_cpy(i32, ptr, ptr, i32)
declare void @__rec_set(i32, ptr, i32, i32)
declare void @__rec_agg(i32, ptr, i32)
declare void @__rec_aggst(i32, ptr, i32)
"""

WIDTH = {'i1': 1, 'i8': 1, 'i16': 2, 'i32': 4, 'ptr': 4, 'float': 4, 'i64': 8, 'double': 8}


def operands(text):
    """Top-level operands of an instruction tail, metadata attachments dropped."""
    return [p for p in split_top(text) if not p.startswith('!') and not re.match(r'align \d+$', p)]


def parse_load(line):
    m = re.match(r'^(\s+)(%[-\w.$]+) = load (volatile )?(.*)$', line)
    if not m or m[3]:
        return None
    ops = operands(m[4])
    if len(ops) < 2 or not ops[1].startswith('ptr '):
        return None
    return m[1], m[2], ops[0].strip(), ops[1][4:].strip()


def parse_store(line):
    m = re.match(r'^(\s+)store (volatile )?(.*)$', line)
    if not m or m[2]:
        return None
    ops = operands(m[3])
    if len(ops) < 2 or not ops[1].startswith('ptr '):
        return None
    ty, value = take_type(ops[0])
    value = re.sub(r'^(?:noundef|nonnull)\s+', '', value.strip())
    return m[1], ty, value, ops[1][4:].strip()


def call_args(line, callee):
    """(indent, dest, [(type, value)]) of a call to CALLEE, attributes dropped."""
    m = re.match(r'^(\s+)(?:(%[-\w.$]+) = )?(?:tail )?call [^@]*@' + re.escape(callee) + r'\(', line)
    if not m:
        return None
    start = m.end() - 1
    depth = 0
    for i in range(start, len(line)):
        if line[i] in '([{<':
            depth += 1
        elif line[i] in ')]}>':
            depth -= 1
            if depth == 0:
                break
    args = []
    for a in split_top(line[start + 1:i]):
        ty, rest = take_type(a)
        rest = re.sub(r'^(?:(?:noundef|nonnull|noalias|nocapture|readonly|writeonly|immarg|zeroext|signext|'
                      r'align \d+|captures\([^)]*\))\s+)+', '', rest.strip())
        args.append((ty, rest))
    return m[1], m[2], args


def site(kind, unit, local):
    if local >= 0x10000:
        raise ValueError('more than 65536 sites in one unit')
    return (kind << 29) | (unit << 16) | local


def as_word(ty, value, fresh):
    """(instructions, operand, wide) turning VALUE of type TY into an i32 or i64 operand."""
    if ty == 'i32':
        return [], 'i32 ' + value, False
    if ty in ('i1', 'i8', 'i16'):
        n = fresh()
        return [f'{n} = zext {ty} {value} to i32'], 'i32 ' + n, False
    if ty == 'ptr':
        n = fresh()
        return [f'{n} = ptrtoint ptr {value} to i32'], 'i32 ' + n, False
    if ty == 'float':
        n = fresh()
        return [f'{n} = bitcast float {value} to i32'], 'i32 ' + n, False
    if ty == 'i64':
        return [], 'i64 ' + value, True
    if ty == 'double':
        n = fresh()
        return [f'{n} = bitcast double {value} to i64'], 'i64 ' + n, True
    return None, None, None


def instrument(text, unit, unit_name):
    """Return (instrumented IR, static facts)."""
    lines = text.splitlines()
    meta = {}
    locs = {}
    variables = {}
    subprograms = {}
    for line in lines:
        if not line.startswith('!'):
            continue
        m = DILOC.match(line)
        if m:
            locs[int(m[1])] = (int(m[2]), int(m[3] or 0))
            continue
        m = DIVAR.match(line)
        if m:
            variables[int(m[1])] = m[2]
            continue
        m = DISP.match(line)
        if m:
            subprograms[int(m[1])] = (m[2], int(m[3]))
    del meta
    counter = [0]

    def next_local():
        counter[0] += 1
        return counter[0]

    serial = [0]

    def fresh():
        serial[0] += 1
        return f'%__r{serial[0]}'

    out = []
    functions = []
    fn = None
    block = None
    i = 0
    n = len(lines)
    pending_phi_end = False

    def loc_of(line):
        m = DBG_ID.search(line)
        if m and int(m[1]) in locs:
            return locs[int(m[1])]
        return (0, 0)

    def emit_mark(kind_site):
        out.append(f'  call void @__rec_bb(i32 {kind_site})')

    def add_inst(dest, body, line, sid=None):
        ln, col = loc_of(line)
        block['insts'].append([dest, DBG.sub('', body.strip()), ln, col, sid])

    while i < n:
        line = lines[i]
        if fn is None:
            m = DEFINE.match(line)
            if m and m[3]:
                name = m[1].strip('"')
                params = []
                for p in _split_params(m[2]):
                    words = p.split()
                    if words and words[-1].startswith('%'):
                        params.append([words[0], words[-1]])
                entry_label = str(len(params)) if all(re.fullmatch(r'%\d+', p[1]) for p in params) else 'entry'
                sp = subprograms.get(int(m[3]), (name, 0))
                fsite = site(K_FN, unit, next_local())
                fn = {'name': name, 'line': sp[1], 'params': params, 'blocks': [], 'vars': {}, 'site': fsite}
                functions.append(fn)
                out.append(line)
                block = {'label': entry_label, 'site': fsite, 'insts': []}
                fn['blocks'].append(block)
                out.append(f'  call void @__rec_fn(i32 {fsite}, i32 0)')
                i += 1
                continue
            out.append(line)
            i += 1
            continue
        # inside an instrumented function
        if line == '}':
            fn = None
            block = None
            out.append(line)
            i += 1
            continue
        m = LABEL.match(line)
        if m:
            block = {'label': m[1].strip('"'), 'site': site(K_BB, unit, next_local()), 'insts': []}
            fn['blocks'].append(block)
            out.append(line)
            pending_phi_end = True
            i += 1
            continue
        if not line.strip() or line.lstrip().startswith(';'):
            out.append(line)
            i += 1
            continue
        if pending_phi_end and not PHI.match(line):
            emit_mark(block['site'])
            pending_phi_end = False
        m = DBGVAL.match(line)
        if m:
            if m[1].startswith('%') and int(m[2]) in variables:
                fn['vars'].setdefault(m[1], variables[int(m[2])])
            out.append(line)
            i += 1
            continue
        # multi-line switch
        if line.lstrip().startswith('switch '):
            body = [line]
            while not lines[i].strip().startswith(']'):
                i += 1
                body.append(lines[i])
            out.extend(body)
            add_inst(None, ' '.join(s.strip() for s in body), line)
            i += 1
            continue
        dest_m = re.match(r'^\s+(%[-\w.$]+) = ', line)
        dest = dest_m[1] if dest_m else None
        handled = False
        m = parse_load(line)
        if m:
            indent, d, ty, ptr = m
            pre, word, wide = as_word(ty, d, fresh)
            out.append(line)
            if pre is None:
                sid = site(K_AGG, unit, next_local())
                sz = fresh()
                szi = fresh()
                out.append(f'{indent}{sz} = getelementptr {ty}, ptr null, i32 1')
                out.append(f'{indent}{szi} = ptrtoint ptr {sz} to i32')
                out.append(f'{indent}call void @__rec_agg(i32 {sid}, ptr {ptr}, i32 {szi})')
            else:
                sid = site(K_WIDE if wide else K_LOAD, unit, next_local())
                out.extend(indent + p for p in pre)
                if wide:
                    out.append(f'{indent}call void @__rec_ld64(i32 {sid}, ptr {ptr}, {word})')
                else:
                    out.append(f'{indent}call void @__rec_ld(i32 {sid}, ptr {ptr}, {word}, i32 {WCODE[WIDTH[ty]]})')
            add_inst(d, line, line, sid)
            handled = True
        if not handled:
            m = parse_store(line)
            if m:
                indent, ty, value, ptr = m
                pre, word, wide = as_word(ty, value, fresh)
                out.append(line)
                if pre is None:
                    sid = site(K_AGG, unit, next_local())
                    sz = fresh()
                    szi = fresh()
                    out.append(f'{indent}{sz} = getelementptr {ty}, ptr null, i32 1')
                    out.append(f'{indent}{szi} = ptrtoint ptr {sz} to i32')
                    out.append(f'{indent}call void @__rec_aggst(i32 {sid}, ptr {ptr}, i32 {szi})')
                else:
                    sid = site(K_WIDE if wide else K_STORE, unit, next_local())
                    out.extend(indent + p for p in pre)
                    if wide:
                        out.append(f'{indent}call void @__rec_st64(i32 {sid}, ptr {ptr}, {word})')
                    else:
                        out.append(f'{indent}call void @__rec_st(i32 {sid}, ptr {ptr}, {word}, i32 {WCODE[WIDTH[ty]]})')
                add_inst(None, line, line, sid)
                handled = True
        if not handled:
            callee = re.search(r'call [^@]*@(port_bus_(?:load|store)_\w+|llvm\.mem(?:cpy|move|set)[\w.]*)\(', line)
            parsed = call_args(line, callee[1]) if callee else None
            if parsed:
                indent, d, args = parsed
                name = callee[1]
                if name.startswith('port_bus_load_') and d and len(args) == 1:
                    ty = re.search(r'call (\S+) @', line)[1]
                    pre, word, _ = as_word(ty, d, fresh)
                    sid = site(K_LOAD, unit, next_local())
                    out.append(line)
                    out.extend(indent + p for p in pre)
                    out.append(f'{indent}call void @__rec_ld(i32 {sid}, ptr {args[0][1]}, {word}, i32 {WCODE[WIDTH[ty]]})')
                    add_inst(d, line, line, sid)
                    handled = True
                elif name.startswith('port_bus_store_') and len(args) == 2:
                    ty, value = args[1]
                    pre, word, _ = as_word(ty, value, fresh)
                    sid = site(K_STORE, unit, next_local())
                    out.append(line)
                    out.extend(indent + p for p in pre)
                    out.append(f'{indent}call void @__rec_st(i32 {sid}, ptr {args[0][1]}, {word}, i32 {WCODE[WIDTH[ty]]})')
                    add_inst(None, line, line, sid)
                    handled = True
                elif name.startswith(('llvm.memcpy', 'llvm.memmove')) and len(args) >= 3:
                    sid = site(K_CPY, unit, next_local())
                    length = args[2][1]
                    if args[2][0] == 'i64':
                        t = fresh()
                        out.append(f'{indent}{t} = trunc i64 {length} to i32')
                        length = t
                    out.append(f'{indent}call void @__rec_cpy(i32 {sid}, ptr {args[0][1]}, ptr {args[1][1]}, i32 {length})')
                    out.append(line)
                    add_inst(None, line, line, sid)
                    handled = True
                elif name.startswith('llvm.memset') and len(args) >= 3:
                    sid = site(K_SET, unit, next_local())
                    v = fresh()
                    out.append(f'{indent}{v} = zext i8 {args[1][1]} to i32')
                    length = args[2][1]
                    if args[2][0] == 'i64':
                        t = fresh()
                        out.append(f'{indent}{t} = trunc i64 {length} to i32')
                        length = t
                    out.append(f'{indent}call void @__rec_set(i32 {sid}, ptr {args[0][1]}, i32 {v}, i32 {length})')
                    out.append(line)
                    add_inst(None, line, line, sid)
                    handled = True
        if not handled:
            if RET.match(line):
                out.append(f"  call void @__rec_fn(i32 {fn['site']}, i32 1)")
            out.append(line)
            add_inst(dest, line, line)
        i += 1
    out.append(DECLS)
    facts = {'unit': unit, 'name': unit_name, 'functions': functions}
    return strip_debug('\n'.join(out) + '\n'), facts


def strip_debug(text):
    """The facts carry the lines; the linked module needs no DWARF (disk, link time)."""
    text = re.sub(r'^\s+#dbg_\w+\(.*\n', '', text, flags=re.M)
    text = re.sub(r', !dbg !\d+', '', text)
    text = re.sub(r' !dbg !\d+', '', text)
    text = re.sub(r'^!llvm\.dbg\.cu = .*\n', '', text, flags=re.M)
    return text


def _split_params(text):
    parts, depth, cur = [], 0, ''
    for ch in text:
        if ch in '([{<':
            depth += 1
        elif ch in ')]}>':
            depth -= 1
        if ch == ',' and depth == 0:
            parts.append(cur.strip())
            cur = ''
        else:
            cur += ch
    if cur.strip():
        parts.append(cur.strip())
    return parts


def write_facts(path, facts):
    path.write_text(json.dumps(facts, separators=(',', ':')))

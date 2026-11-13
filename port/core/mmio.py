"""Route volatile scalar game accesses through the portable device bus.

LLVM IR is used only to preserve the C frontend's exact access widths and address
expressions. Ordinary memory keeps its volatile access in the C bus functions.
No instruction body or matching compiler output is supplied by this pass.
"""
import re


def _split(text):
    """Operands at top level (constant expressions contain commas)."""
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
    parts.append(cur.strip())
    return parts


def instrument_mmio(text):
    used = set()
    loads = stores = 0
    result = []
    load = re.compile(r'^(\s*)(%[\w.]+) = load volatile (i8|i16|i32|ptr), (.*)$')
    store = re.compile(r'^(\s*)store volatile (i8|i16|i32|ptr) (.*)$')
    for line in text.splitlines():
        match = load.match(line)
        if match:
            indent, dest, kind, rest = match.groups()
            operands = _split(rest)
            if not operands[0].startswith('ptr '):
                result.append(line)
                continue
            pointer = operands[0][4:]
            name = 'port_bus_load_' + kind
            line = f'{indent}{dest} = call {kind} @{name}(ptr {pointer})'
            used.add((name, kind, 'ptr'))
            loads += 1
        else:
            match = store.match(line)
            if match:
                indent, kind, rest = match.groups()
                operands = _split(rest)
                if len(operands) < 2 or not operands[1].startswith('ptr '):
                    result.append(line)
                    continue
                value, pointer = operands[0], operands[1][4:]
                name = 'port_bus_store_' + kind
                line = f'{indent}call void @{name}(ptr {pointer}, {kind} {value})'
                used.add((name, 'void', 'ptr, ' + kind))
                stores += 1
        result.append(line)
    result += [f'declare {ret} @{name}({args})' for name, ret, args in sorted(used)]
    output = '\n'.join(result) + '\n'
    # Calls may trigger DMA, handlers and callbacks beyond the original access.
    output = re.sub(r'\b(?:nofree|norecurse|nosync|willreturn|mustprogress|nocallback)\b', '', output)
    output = re.sub(r'\bmemory\([^)]*\)', '', output)
    return output, {'volatile_loads': loads, 'volatile_stores': stores}

"""Conservative transport through Thumb stack stores and reloads.

The physical register/memory graph is never rewritten.  This module adds a
second view: when every resolved predecessor supplies a last store covering a
reload, describe the stored bits and, only when the complete register value is
unchanged, substitute the reload's machine definition with its source values.

``strict`` invalidates stack knowledge on every external call or possibly
aliasing store. ``abi-private`` is explicitly conditional on stronger ABI and
non-escape assumptions; it is not inferred merely from a call lacking SP args.
Neither policy proves unknown indirect edges, jump-table bounds, or C types.
"""

from __future__ import annotations

from collections import Counter, deque
from dataclasses import dataclass, field, replace
from typing import Any, Mapping, Sequence


MAX_VALUES = 64
MAX_STORES = 64
MAX_SLOTS = 512
MAX_OFFSET = 1 << 20
MASK = 0xFFFFFFFF
BASE_ASSUMPTIONS = (
    "ARM7TDMI Thumb little-endian memory; entry SP is word aligned.",
    "Sequential instruction execution without unmodeled asynchronous stack writes.",
    "Transport is conditional on the resolved CFG: unmodeled entry/indirect edges and jump-table index bounds are not proved.",
)
PRIVATE_ASSUMPTIONS = (
    "abi-private: callees preserve SP and do not inspect or write the caller's private frame except through escaped pointers or stack arguments.",
    "abi-private: no forged, dangling, preexisting, or hidden pointer aliases a newly allocated private-frame slot.",
)


def _number(text: str) -> int | None:
    try:
        text = text.strip().lstrip("#=")
        return int(text, 16 if "0x" in text.lower() else 10)
    except ValueError:
        return None


def _signed(value: int) -> int:
    value &= MASK
    return value if value < 0x80000000 else value - 0x100000000


def _ref(index: int, role: str) -> str:
    return f"n{index}:{role}"


def _split_ref(value: str) -> tuple[int, str] | None:
    if not value.startswith("n") or ":" not in value:
        return None
    head, role = value.split(":", 1)
    return (int(head[1:]), role) if head[1:].isdigit() else None


class _Expressions:
    """Small, bounded expression domain: one entry-SP base or a constant."""

    def __init__(self, instructions, dependencies, address):
        self.instructions = instructions
        self.dependencies = dependencies
        self.address = address
        self.cache: dict[str, tuple[str, int] | None] = {}
        self.active: set[str] = set()

    def sources(self, values: Sequence[str]) -> tuple[str, int] | None:
        result = {self.value(value) for value in values}
        return next(iter(result)) if len(result) == 1 and None not in result else None

    def value(self, reference: str) -> tuple[str, int] | None:
        if reference == "entry:sp":
            return "sp", 0
        if reference.startswith("pc:"):
            return "constant", int(reference.split(":", 1)[1]) & MASK
        if reference in self.cache:
            return self.cache[reference]
        parsed = _split_ref(reference)
        if parsed is None or reference in self.active or len(self.active) >= 64:
            return None
        index, role = parsed
        ins = self.instructions[index]
        deps = self.dependencies[index]
        self.active.add(reference)
        result = None

        def operand(text, input_role):
            number = _number(text)
            return ("constant", number & MASK) if number is not None else self.sources(deps.get(input_role, ()))

        if role == "link" and ins.opcode == "bl" and ins.control == "branch":
            result = "constant", (self.address + ins.offset + ins.size) | 1
        elif role == "base" and ins.memory and ins.memory.writeback:
            base = self.sources(deps.get("base", ()))
            if base:
                delta = 4 * ins.memory.count * (-1 if ins.opcode == "push" else 1)
                result = base[0], _signed(base[1] + delta) if base[0] == "sp" else (base[1] + delta) & MASK
        elif role == "value" and ins.opcode in {"mov", "movs"} and len(ins.arguments) == 2:
            result = operand(ins.arguments[1], "input0")
        elif role == "value" and ins.opcode == "ldr" and len(ins.arguments) == 2 and ins.arguments[1].startswith("="):
            number = _number(ins.arguments[1])
            if number is not None:
                result = "constant", number & MASK
            elif ins.target is not None:
                result = "constant", (self.address + ins.target) & MASK
        elif role == "value":
            args = ins.arguments[1:] if len(ins.arguments) >= 3 else ins.arguments
            if len(args) == 2:
                left, right = operand(args[0], "input0"), operand(args[1], "input1")
                op = ins.opcode.removesuffix("s")
                if left and right and op in {"add", "sub"}:
                    sign = 1 if op == "add" else -1
                    if left[0] == "constant" and right[0] == "constant":
                        result = "constant", (left[1] + sign * right[1]) & MASK
                    elif left[0] == "sp" and right[0] == "constant":
                        result = "sp", _signed(left[1] + sign * right[1])
                    elif op == "add" and left[0] == "constant" and right[0] == "sp":
                        result = "sp", _signed(left[1] + right[1])
                    elif op == "sub" and left[0] == right[0] == "sp":
                        result = "constant", (left[1] - right[1]) & MASK
                elif left and right and left[0] == right[0] == "constant":
                    a, b = left[1], right[1]
                    shift = b if _number(args[1]) is not None else b & 255
                    if op == "lsl":
                        result = "constant", (a << shift) & MASK if shift < 32 else 0
                    elif op == "lsr":
                        result = "constant", a >> shift if shift < 32 else 0
                    elif op == "asr":
                        result = "constant", (_signed(a) >> min(shift, 32)) & MASK
                    elif op == "and":
                        result = "constant", a & b
                    elif op == "orr":
                        result = "constant", a | b
                    elif op == "eor":
                        result = "constant", a ^ b
                    elif op == "bic":
                        result = "constant", a & (~b & MASK)
                    elif op == "mul":
                        result = "constant", a * b & MASK
        self.active.remove(reference)
        if result and result[0] == "sp" and abs(result[1]) > MAX_OFFSET:
            result = None
        self.cache[reference] = result
        return result

    def may_stack_pointer(self, refs: Sequence[str], pointer_values: frozenset[str]) -> bool:
        # Reachability through a DAG or cycle, not recursively enumerated paths.
        # Loaded addresses are deliberately excluded from loaded-value taint.
        todo, seen = list(refs), set()
        while todo:
            reference = todo.pop()
            if reference in seen:
                continue
            seen.add(reference)
            if reference == "entry:sp" or reference in pointer_values:
                return True
            value = self.value(reference)
            if value and value[0] == "sp":
                return True
            if value and value[0] == "constant":
                continue
            parsed = _split_ref(reference)
            if parsed is None:
                continue
            index, role = parsed
            ins = self.instructions[index]
            if role == "value" and ins.memory and ins.memory.kind == "read":
                continue  # Its address is not the loaded value; tracked below.
            if role.startswith("value") and ins.opcode == "pop":
                continue
            if role == "value":
                todo.extend(value for key, values in self.dependencies[index].items()
                            if key.startswith("input") for value in values)
            elif role == "base":
                todo.extend(self.dependencies[index].get("base", ()))
        return False

    def narrow_identity(self, refs: Sequence[str], width: int, signed: bool) -> bool:
        for reference in refs:
            value = self.value(reference)
            if value and value[0] == "constant":
                low = value[1] & ((1 << width) - 1)
                extended = low - (1 << width) if signed and low & (1 << (width - 1)) else low
                if extended & MASK == value[1]:
                    continue
            parsed = _split_ref(reference)
            if parsed:
                index, role = parsed
                mem = self.instructions[index].memory
                if role == "value" and mem and mem.kind == "read":
                    if (not signed and not mem.signed and mem.width <= width
                        or signed and (mem.signed and mem.width <= width or not mem.signed and mem.width < width)):
                        continue
            return False
        return bool(refs)


@dataclass(frozen=True)
class _Cell:
    values: frozenset[str]
    stores: frozenset[int]
    assumptions: frozenset[str] = frozenset()
    pointer: bool = False


@dataclass
class _State:
    sp: int | None = 0
    cells: dict[tuple[int, int], _Cell] = field(default_factory=dict)
    escaped: bool = False
    pointer_values: frozenset[str] = frozenset()
    pointer_stored: bool = False

    def copy(self):
        return _State(self.sp, self.cells.copy(), self.escaped, self.pointer_values, self.pointer_stored)


def analyze_stack(instructions, blocks, dependencies, *, address: int = 0,
                  policy: str = "strict", call_arities: Mapping[str, int] | None = None
                  ) -> tuple[tuple[dict[str, tuple[str, ...]], ...], dict[str, Any]]:
    """Analyze stack identity transport without modifying the physical graph.

    A slot exists at a join only if every reachable predecessor supplies it.
    Differing possible source values are retained as a bounded reaching set;
    missing, overlapping, ambiguous, unaligned, or clobbered values stay unknown.
    The optional private policy preserves only allocated negative-SP slots at
    calls whose supplied arity is <4, proving no stack argument words under the
    same explicit one-word-prototype assumption used by contracts.py.
    """
    if policy not in {"strict", "abi-private"}:
        raise ValueError("stack_policy must be 'strict' or 'abi-private'")
    call_arities = call_arities or {}
    expr = _Expressions(instructions, dependencies, address)
    warnings: set[str] = set()
    report: dict[str, Any] = {
        "version": 1, "policy": policy, "cfg_scope": "resolved-edges-only",
        "assumptions": list(BASE_ASSUMPTIONS + (PRIVATE_ASSUMPTIONS if policy == "abi-private" else ())),
        "value_aliases": {}, "facts": [], "warnings": [], "stats": {},
        "semantic_proof": False,
    }
    if not instructions or not blocks:
        return tuple(dependencies), report

    def merge(states):
        first = states[0]
        sp = first.sp if all(state.sp == first.sp for state in states) else None
        shared = set(first.cells)
        for state in states[1:]:
            shared.intersection_update(state.cells)
        cells = {}
        for key in shared if sp is not None else ():
            cells_at = [state.cells[key] for state in states]
            values = frozenset().union(*(cell.values for cell in cells_at))
            stores = frozenset().union(*(cell.stores for cell in cells_at))
            if len(values) > MAX_VALUES or len(stores) > MAX_STORES:
                warnings.add("A join exceeded the bounded value/provenance set; affected slots became unknown.")
                continue
            cells[key] = _Cell(values, stores,
                frozenset().union(*(cell.assumptions for cell in cells_at)), any(cell.pointer for cell in cells_at))
        return _State(sp, cells, any(state.escaped for state in states),
                      frozenset().union(*(state.pointer_values for state in states)),
                      any(state.pointer_stored for state in states))

    def stack_address(ins, state):
        mem = ins.memory
        if mem is None:
            return None
        base = ("sp", state.sp) if mem.base == "sp" and state.sp is not None else expr.sources(dependencies[ins.index].get("base", ()))
        if base is None or base[0] != "sp":
            return None
        displacement = mem.displacement
        if mem.index:
            index = expr.sources(dependencies[ins.index].get("index", ()))
            if index is None or index[0] != "constant":
                return None
            displacement += _signed(index[1])
        value = _signed(base[1] + displacement)
        return value if abs(value) <= MAX_OFFSET else None

    def sp_after(ins, state):
        definitions = [item for item in ins.definitions if item.register == "sp"]
        if not definitions:
            return state.sp
        if ins.opcode in {"push", "pop"} and state.sp is not None:
            return state.sp + 4 * ins.memory.count * (-1 if ins.opcode == "push" else 1)
        if ins.opcode in {"add", "adds", "sub", "subs"} and state.sp is not None:
            args = ins.arguments[1:] if len(ins.arguments) == 3 else ins.arguments
            if len(args) == 2 and args[0].lower() in {"sp", "r13"}:
                number = _number(args[1])
                if number is None:
                    value = expr.sources(dependencies[ins.index].get("input1", ()))
                    number = _signed(value[1]) if value and value[0] == "constant" else None
                if number is not None:
                    return state.sp + number * (-1 if ins.opcode.startswith("sub") else 1)
        value = expr.value(_ref(ins.index, definitions[-1].role))
        return value[1] if value and value[0] == "sp" else None

    def transfer(block, incoming, *, emit=False):
        state = incoming.copy()

        def event(ins, kind, status, **extra):
            if emit:
                report["facts"].append({"instruction": ins.index, "offset": ins.offset,
                                        "opcode": ins.opcode, "kind": kind, "status": status, **extra})

        def invalidate(ins, reason):
            count = len(state.cells)
            state.cells.clear()
            event(ins, "clobber", "invalidated", reason=reason, slots=count)

        for index in block.instructions:
            ins = instructions[index]
            deps = dependencies[index]
            new_sp = sp_after(ins, state)
            if new_sp is not None and (abs(new_sp) > MAX_OFFSET or new_sp % 4):
                new_sp = None
            if not ins.supported:
                invalidate(ins, "unsupported instruction may change stack/registers")
                state.sp, state.escaped, state.pointer_stored = None, True, True
                continue

            if ins.control in {"call", "tailcall"}:
                arg_refs = [value for role, values in deps.items() if role.startswith("arg") for value in values]
                passed_pointer = expr.may_stack_pointer(arg_refs, state.pointer_values)
                arity = call_arities.get(ins.callee)
                no_stack_args = arity is not None and 0 <= arity < 4
                # Unknown incoming stack arguments can contain an earlier local
                # pointer store even when r0-r3 currently contain no SP alias.
                possible_stack_escape = not no_stack_args and state.pointer_stored
                state.escaped |= passed_pointer or possible_stack_escape
                if policy == "abi-private" and not state.escaped and no_stack_args and state.sp is not None:
                    old_count = len(state.cells)
                    state.cells = {key: replace(cell, assumptions=cell.assumptions | {"abi-private"})
                                   for key, cell in state.cells.items()
                                   if state.sp <= key[0] and key[0] + key[1] // 8 <= 0}
                    event(ins, "call", "conditional-preserve", preserved_slots=len(state.cells),
                          invalidated_slots=old_count - len(state.cells), assumptions=["abi-private"],
                          reason="explicit private-frame contract and known register-only prototype")
                else:
                    invalidate(ins, "external call may alias stack" if policy == "strict" else
                               "private-frame contract unavailable: escaped pointer, unknown stack arguments, or unknown SP")
                if state.escaped:
                    state.pointer_values |= frozenset(_ref(index, item.role) for item in ins.definitions if item.register.startswith("r"))
                continue

            mem = ins.memory
            if mem:
                address_at = stack_address(ins, state)
                is_push = ins.opcode == "push"
                active_sp = new_sp if is_push else state.sp
                roles = [(item.role, item.register) for item in (ins.uses if mem.kind == "write" else ins.definitions)
                         if item.role.startswith("value")]
                if not roles:
                    roles = [("value", None)]
                ambiguous_multiple = mem.writeback and ins.opcode.startswith(("ldm", "stm")) and any(
                    register == mem.base for _, register in roles)
                aligned = address_at is not None and mem.width in {8, 16, 32} and address_at % (mem.width // 8) == 0
                if ambiguous_multiple:
                    aligned = False
                if mem.kind == "write":
                    source_refs = [value for role, _ in roles for value in deps.get(role, ())]
                    pointer = expr.may_stack_pointer(source_refs, state.pointer_values)
                    state.pointer_stored |= pointer
                    if pointer and (address_at is None or active_sp is None or address_at < active_sp
                                    or address_at + mem.count * mem.width // 8 > 0):
                        state.escaped = True
                        event(ins, "escape", "observed", reason="stack-derived address stored outside known private frame")
                    if not aligned:
                        if (address_at is None and policy == "abi-private" and not state.escaped
                            and not expr.may_stack_pointer(deps.get("base", ()), state.pointer_values)):
                            # This is a declared no-hidden-alias assumption, not
                            # a conclusion inferred from the address syntax.
                            state.cells = {key: replace(cell, assumptions=cell.assumptions | {"abi-private"})
                                           for key, cell in state.cells.items()
                                           if state.sp is not None and state.sp <= key[0] and key[0] + key[1] // 8 <= 0}
                            event(ins, "store", "conditional-preserve", reason="explicit no-hidden-private-frame-alias contract",
                                  assumptions=["abi-private"])
                        else:
                            invalidate(ins, "unknown or unaligned store address may alias stack")
                    else:
                        for k, (role, _) in enumerate(roles):
                            slot = address_at + k * mem.width // 8
                            end = slot + mem.width // 8
                            overwritten = [key for key in state.cells if key[0] < end and slot < key[0] + key[1] // 8]
                            for key in overwritten:
                                del state.cells[key]
                            values = frozenset(deps.get(role, ()))
                            in_allocation = active_sp is not None and slot >= active_sp
                            if values and len(values) <= MAX_VALUES and in_allocation:
                                state.cells[slot, mem.width] = _Cell(values, frozenset((ins.offset,)), pointer=expr.may_stack_pointer(values, state.pointer_values))
                                event(ins, "store", "stored", stack_offset=slot, width=mem.width,
                                      values=sorted(values), overwritten_slots=len(overwritten), source_role=role)
                            else:
                                event(ins, "store", "unknown", stack_offset=slot, width=mem.width,
                                      reason="unknown SP, outside allocated stack, or unbounded source values")
                        if len(state.cells) > MAX_SLOTS:
                            invalidate(ins, "stack-slot budget exceeded")
                            warnings.add("Stack-slot budget exceeded; transport was invalidated.")
                else:
                    for k, (role, _) in enumerate(roles):
                        slot = address_at + k * mem.width // 8 if address_at is not None else None
                        definition = _ref(index, role)
                        cell_key = next((key for key in state.cells if slot is not None and key[0] <= slot
                                         and slot + mem.width // 8 <= key[0] + key[1] // 8), None) if aligned else None
                        cell = state.cells.get(cell_key) if cell_key else None
                        if cell is None:
                            if state.pointer_stored or state.escaped:
                                state.pointer_values |= frozenset((definition,))
                            event(ins, "load", "unknown", stack_offset=slot, width=mem.width,
                                  definition=definition, reason="no aligned covering store on every resolved predecessor")
                            continue
                        if cell.pointer:
                            state.pointer_values |= frozenset((definition,))
                        shift = (slot - cell_key[0]) * 8
                        extension = "sign" if mem.signed else "zero" if mem.width < 32 else "none"
                        identity = shift == 0 and (mem.width == 32 or expr.narrow_identity(cell.values, mem.width, mem.signed))
                        fact = {"stack_offset": slot, "width": mem.width, "extension": extension,
                                "source_width": cell_key[1], "bit_offset": shift, "values": sorted(cell.values),
                                "stores": sorted(cell.stores), "definition": definition,
                                "assumptions": sorted(cell.assumptions), "certainty": "all-resolved-reaching-paths"}
                        event(ins, "load", "forwarded" if identity else "typed-bits", **fact)
                        if emit and identity:
                            report["value_aliases"][definition] = {**fact, "identity": True}

            if new_sp != state.sp:
                old_sp = state.sp
                if new_sp is None:
                    invalidate(ins, "SP relation to entry became unknown or unaligned")
                elif old_sp is not None and new_sp > old_sp:
                    state.cells = {key: value for key, value in state.cells.items() if key[0] >= new_sp}
                state.sp = new_sp
                event(ins, "sp", "known" if new_sp is not None else "unknown", before=old_sp, after=new_sp)
        return state

    incoming: list[_State | None] = [None for _ in blocks]
    outgoing: list[_State | None] = [None for _ in blocks]
    queue, queued = deque([0]), {0}
    iterations = 0
    iteration_budget = max(1024, len(blocks) * 64)
    while queue and iterations < iteration_budget:
        bid = queue.popleft()
        queued.discard(bid)
        block = blocks[bid]
        if not block.reachable:
            continue
        predecessors = [outgoing[parent] for parent in block.predecessors
                        if blocks[parent].reachable and outgoing[parent] is not None]
        if bid == 0:
            predecessors.append(_State())
        if not predecessors:
            continue
        state = merge(predecessors)
        incoming[bid] = state
        after = transfer(block, state)
        iterations += 1
        if after != outgoing[bid]:
            outgoing[bid] = after
            for _, successor in block.successors:
                if successor not in queued:
                    queue.append(successor)
                    queued.add(successor)
    if queue:
        report["warnings"] = ["Stack fixed-point budget exhausted; all transport remains unknown."]
        report["stats"] = {"iterations": iterations, "budget_exhausted": True, "forwarded_loads": 0}
        return tuple(dependencies), report
    for block in blocks:
        if incoming[block.id] is not None:
            transfer(block, incoming[block.id], emit=True)

    # Resolve chains of spilled reloads without expanding arithmetic expression
    # graphs. Cyclic reload aliases are left physical, not guessed away.
    raw_aliases = report["value_aliases"]
    normalized = {}
    cache = {}

    def expand(reference, active=frozenset()):
        if reference not in raw_aliases:
            return frozenset((reference,)), frozenset(), frozenset()
        if reference in cache:
            return cache[reference]
        if reference in active or len(active) >= 64:
            return None
        record = raw_aliases[reference]
        values, stores, assumptions = set(), set(record["stores"]), set(record["assumptions"])
        for source in record["values"]:
            expanded = expand(source, active | {reference})
            if expanded is None:
                return None
            values.update(expanded[0]); stores.update(expanded[1]); assumptions.update(expanded[2])
            if len(values) > MAX_VALUES or len(stores) > MAX_STORES:
                return None
        result = frozenset(values), frozenset(stores), frozenset(assumptions)
        cache[reference] = result
        return result

    unresolved = []
    for reference, record in raw_aliases.items():
        expanded = expand(reference)
        if expanded is None:
            unresolved.append(reference)
            continue
        normalized[reference] = {**record, "values": sorted(expanded[0]), "stores": sorted(expanded[1]),
                                 "assumptions": sorted(expanded[2])}
    if unresolved:
        warnings.add("Cyclic or oversized reload-alias chains remain physical definitions.")
        for fact in report["facts"]:
            if fact.get("definition") in unresolved and fact["status"] == "forwarded":
                fact["status"] = "unknown-alias-chain"
    report["value_aliases"] = normalized
    report["unresolved_aliases"] = sorted(unresolved)
    transported = tuple({role: tuple(sorted({source for value in values
                                           for source in normalized.get(value, {}).get("values", [value])}))
                         for role, values in item.items()} for item in dependencies)
    report["block_sp"] = [{"block": block.id,
                            "entry": incoming[block.id].sp if incoming[block.id] else None,
                            "exit": outgoing[block.id].sp if outgoing[block.id] else None,
                            "escaped": outgoing[block.id].escaped if outgoing[block.id] else None}
                           for block in blocks]
    counts = Counter(fact["status"] for fact in report["facts"])
    report["stats"] = {"iterations": iterations, "budget_exhausted": False,
                       "forwarded_loads": len(normalized), "typed_loads": counts["typed-bits"],
                       "unknown_loads": sum(fact["kind"] == "load" and fact["status"].startswith("unknown") for fact in report["facts"]),
                       "stack_loads": sum(fact["kind"] == "load" and fact.get("stack_offset") is not None for fact in report["facts"]),
                       "unknown_stack_loads": sum(fact["kind"] == "load" and fact.get("stack_offset") is not None
                           and fact["status"].startswith("unknown") for fact in report["facts"]),
                       "stores": counts["stored"], "invalidations": counts["invalidated"],
                       "conditional_preservations": counts["conditional-preserve"],
                       "changed_dependency_roles": sum(dependencies[i].get(role) != values
                           for i, item in enumerate(transported) for role, values in item.items())}
    report["warnings"] = sorted(warnings)
    return transported, report

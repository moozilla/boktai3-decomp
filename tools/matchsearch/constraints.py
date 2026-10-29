"""A conservative machine-value graph for matching search.

``analyze`` accepts scope.decode's rows or scope's human-readable ``rom`` output.
``compare`` returns independent distances and per-block boundary contracts.  A
zero distance is graph compatibility under the listed assumptions, NEVER a byte
match or a proof of C equivalence.  In particular, the graph describes machine
definitions, not reconstructed GCC pseudos or original source variables.

The useful invariant is stronger than blanking register names: operand roles,
reaching definitions (including loop-carried alternatives), individual flags,
memory ordering, ABI registers and branch destinations survive canonicalization.
Copies are transparent for data provenance but remain in the instruction score.
No dependency on capstone, the compiler, the ROM, or an optional solver is needed.
"""

from __future__ import annotations

from collections import Counter, defaultdict, deque
from dataclasses import asdict, dataclass, field
from difflib import SequenceMatcher
from hashlib import sha256
import json
import re
from typing import Any, Iterable, Mapping, Sequence


REGISTER_RE = re.compile(r"\b(?:r(?:1[0-5]|[0-9])|sp|lr|pc|sb|sl|fp|ip)\b", re.I)
NUMBER_RE = re.compile(r"(?<![\w])[-+]?(?:0x[0-9a-f]+|\d+)(?![\w])", re.I)
REGISTERS = tuple(f"r{i}" for i in range(13)) + ("sp", "lr", "pc")
FLAGS = ("N", "Z", "C", "V")
STATE_KEYS = REGISTERS + tuple(f"@{f}" for f in FLAGS) + ("@memory",)
ALIASES = {"sb": "r9", "sl": "r10", "fp": "r11", "ip": "r12",
           "r13": "sp", "r14": "lr", "r15": "pc"}
CONDITIONS = {"eq": "Z", "ne": "Z", "cs": "C", "hs": "C", "cc": "C", "lo": "C",
              "mi": "N", "pl": "N", "vs": "V", "vc": "V", "hi": "CZ", "ls": "CZ",
              "ge": "NV", "lt": "NV", "gt": "ZNV", "le": "ZNV"}


def _digest(value: Any) -> str:
    return sha256(json.dumps(value, sort_keys=True, separators=(",", ":")).encode()).hexdigest()[:24]


def _reg(text: str) -> str:
    text = text.lower().strip().rstrip("!")
    return ALIASES.get(text, text)


def _registers(text: str) -> tuple[str, ...]:
    return tuple(_reg(m.group()) for m in REGISTER_RE.finditer(text))


def _register_list(text: str) -> tuple[str, ...]:
    """Architectural register-list order, independent of assembler spelling."""
    registers = set()
    for part in text.strip().strip("{}").split(","):
        ends = part.strip().split("-")
        if len(ends) == 2 and all(_reg(item) in REGISTERS for item in ends):
            first, last = (REGISTERS.index(_reg(item)) for item in ends)
            registers.update(REGISTERS[first:last + 1])
        else:
            registers.update(_registers(part))
    return tuple(sorted(registers, key=REGISTERS.index))


def _number(text: str) -> int | None:
    text = text.strip().lstrip("#=")
    try:
        return int(text, 16 if "0x" in text.lower() else 10)
    except ValueError:
        return None


def _arguments(text: str) -> tuple[str, ...]:
    """Commas inside register lists and memory operands do not divide operands."""
    result, start, depth = [], 0, 0
    for i, char in enumerate(text):
        depth += char in "[{("
        depth -= char in "]})"
        if char == "," and depth == 0:
            result.append(text[start:i].strip())
            start = i + 1
    if text[start:].strip():
        result.append(text[start:].strip())
    return tuple(result)


@dataclass(frozen=True)
class Operand:
    role: str
    register: str


@dataclass(frozen=True)
class MemoryAccess:
    kind: str
    width: int
    signed: bool
    base: str
    index: str | None = None
    displacement: int = 0
    count: int = 1
    writeback: bool = False


@dataclass(frozen=True)
class Instruction:
    index: int
    offset: int
    size: int
    text: str
    opcode: str
    arguments: tuple[str, ...]
    uses: tuple[Operand, ...]
    definitions: tuple[Operand, ...]
    shape: tuple[Any, ...]
    relaxed_shape: tuple[Any, ...]
    target: int | None = None
    control: str = "fallthrough"
    callee: str | None = None
    memory: MemoryAccess | None = None
    copy_source: str | None = None
    supported: bool = True


@dataclass
class Block:
    id: int
    start: int
    end: int
    instructions: tuple[int, ...]
    successors: list[tuple[str, int]] = field(default_factory=list)
    predecessors: list[int] = field(default_factory=list)
    reachable: bool = False
    live_in: dict[str, tuple[str, ...]] = field(default_factory=dict)
    live_out: dict[str, tuple[str, ...]] = field(default_factory=dict)


@dataclass
class Analysis:
    name: str
    address: int
    instructions: tuple[Instruction, ...]
    blocks: list[Block]
    # One dictionary per instruction: semantic input role -> reaching definitions.
    dependencies: tuple[dict[str, tuple[str, ...]], ...]
    data: tuple[tuple[int, int, str, int | None], ...]
    warnings: tuple[str, ...]
    fingerprints: dict[str, str]
    back_edges: tuple[tuple[int, int], ...]
    assumptions: tuple[str, ...]
    jump_tables: dict[int, tuple[int, ...]] = field(default_factory=dict)
    resolved_addresses: list[dict[str, Any]] = field(default_factory=list)
    transport_dependencies: tuple[dict[str, tuple[str, ...]], ...] = ()
    stack_transport: dict[str, Any] = field(default_factory=dict)

    def to_dict(self, *, detail: bool = True) -> dict[str, Any]:
        value = {
            "name": self.name, "address": self.address,
            "instruction_count": len(self.instructions), "block_count": len(self.blocks),
            "calls": [{"offset": i.offset, "callee": i.callee} for i in self.instructions if i.callee],
            "memory": [{"offset": i.offset, **asdict(i.memory)} for i in self.instructions if i.memory],
            "back_edges": self.back_edges, "fingerprints": self.fingerprints,
            "warnings": self.warnings, "assumptions": self.assumptions,
            "jump_tables": self.jump_tables,
            "resolved_addresses": self.resolved_addresses,
            "stack_transport": self.stack_transport if detail else {
                key: self.stack_transport.get(key) for key in ("policy", "cfg_scope", "assumptions", "warnings", "stats")},
        }
        if detail:
            value.update(instructions=[asdict(i) for i in self.instructions],
                         blocks=[asdict(b) for b in self.blocks],
                         dependencies=self.dependencies, transport_dependencies=self.transport_dependencies, data=self.data)
        return value


def _normalize_operand(text: str, *, relaxed: bool = False) -> str:
    text = REGISTER_RE.sub(lambda m: "SP" if _reg(m.group()) == "sp" else
                           "PC" if _reg(m.group()) == "pc" else "R", text)
    text = re.sub(r"func\+0x[0-9a-f]+", "LOCAL", text, flags=re.I)
    text = NUMBER_RE.sub(lambda m: "N" if relaxed else str(_number(m.group())), text)
    return re.sub(r"\s+", "", text)


def _describe(index: int, offset: int, size: int, text: str, target: int | None,
              address: int, call_arities: Mapping[str, int],
              code_offsets: set[int]) -> Instruction:
    split = text.split(None, 1)
    op = split[0].lower().removesuffix(".w").removesuffix(".n")
    args = _arguments(split[1] if len(split) > 1 else "")
    uses: list[Operand] = []
    defs: list[Operand] = []
    memory, callee, copy_source = None, None, None
    control, supported = "fallthrough", True
    if target is None:
        local_literal = re.search(r"=func\+0x([0-9a-f]+)", text, re.I)
        if local_literal:
            target = int(local_literal[1], 16)

    def use(role: str, value: str) -> None:
        regs = _registers(value)
        uses.extend(Operand(role if len(regs) == 1 else f"{role}.{k}", r) for k, r in enumerate(regs))

    def define(role: str, value: str) -> None:
        defs.append(Operand(role, _reg(value)))

    def flags(names: str) -> None:
        defs.extend(Operand(f"flag.{f}", f"@{f}") for f in names)

    def abi_inputs() -> None:
        count = call_arities.get(callee or "", 4)
        for k in range(min(count, 4)):
            use(f"arg{k}", f"r{k}")
        use("stack", "sp")
        uses.append(Operand("memory", "@memory"))

    def memory_effect() -> None:
        # A total memory-effect chain is conservative: no inferred alias freedom.
        uses.append(Operand("memory", "@memory"))
        defs.append(Operand("memory", "@memory"))

    cond = op[1:] if op.startswith("b") else ""
    if op in ("bl", "blx"):
        local = target
        if op == "bl" and local is None and args:
            match = re.fullmatch(r"func\+0x([0-9a-f]+)", args[0], re.I)
            number = _number(args[0])
            local = int(match[1], 16) if match else number - address if number is not None else None
        if op == "bl" and local in code_offsets:
            # Thumb-1's short B cannot reach a distant owner tail. agbcc uses
            # BL as a long jump; its architectural effect is LR = next PC | 1,
            # not an external-call summary. Do not invent a fallthrough edge.
            target, control = local, "branch"
            define("link", "lr")
        else:
            callee = args[0] if args else "unknown"
            control = "call"
            abi_inputs()
            if args and _registers(args[0]):
                use("callee", args[0])
            # The supplied Thumb ABI marks r11 fixed and call-used as well.
            for r in ("r0", "r1", "r2", "r3", "r11", "r12", "lr"):
                define(f"call.{r}", r)
            flags("NZCV")
            defs.append(Operand("memory", "@memory"))
    elif op == "b" or cond in CONDITIONS:
        control = "branch" if op == "b" else "conditional"
        if target is None and args:
            m = re.search(r"func\+0x([0-9a-f]+)", args[0], re.I)
            number = _number(args[0])
            target = int(m.group(1), 16) if m else number - address if number is not None else None
        if cond in CONDITIONS:
            uses.extend(Operand(f"condition.{f}", f"@{f}") for f in CONDITIONS[cond])
        elif target is None:
            callee = args[0] if args else "unknown"
            control = "tailcall"
            abi_inputs()
    elif op == "bx":
        control = "indirect"
        if args:
            use("destination", args[0])
        # Could be a return or a tail call. Do not invent a known return signature.
        abi_inputs()
    elif op in ("push", "pop"):
        regs = _register_list(args[0]) if args else ()
        use("base", "sp")
        define("base", "sp")
        for k, r in enumerate(regs):
            if op == "push":
                use(f"value{k}", r)
            else:
                define(f"value{k}", r)
        memory = MemoryAccess("write" if op == "push" else "read", 32, False,
                              "sp", displacement=-4 * len(regs) if op == "push" else 0,
                              count=len(regs), writeback=True)
        memory_effect()
        if op == "pop" and "pc" in regs:
            control = "return"
            use("result", "r0")
    elif op.startswith(("ldm", "stm")):
        base = _reg(args[0]) if args else "unknown"
        regs = _register_list(args[1]) if len(args) > 1 else ()
        writeback = bool(args and args[0].endswith("!"))
        use("base", base)
        if writeback:
            define("base", base)
        for k, r in enumerate(regs):
            if op.startswith("ldm"):
                define(f"value{k}", r)
            else:
                use(f"value{k}", r)
        memory = MemoryAccess("read" if op.startswith("ldm") else "write", 32, False,
                              base, count=len(regs), writeback=writeback)
        memory_effect()
        if "pc" in regs and op.startswith("ldm"):
            control = "indirect"
    elif op.startswith(("ldr", "str")) and len(args) >= 2:
        load = op.startswith("ldr")
        if load:
            define("value", args[0])
        else:
            use("value", args[0])
        if args[1].startswith("="):
            # scope resolves PC literal loads to their actual value/symbol.
            pass
        else:
            mem_args = _arguments(args[1].strip("[]!"))
            base = _reg(mem_args[0]) if mem_args else "unknown"
            idx = _reg(mem_args[1]) if len(mem_args) > 1 and _registers(mem_args[1]) else None
            disp = _number(mem_args[1]) or 0 if len(mem_args) > 1 and idx is None else 0
            use("base", base)
            if idx:
                use("index", idx)
            signed = op in ("ldrsb", "ldrsh")
            width = 8 if op.endswith("b") else 16 if op.endswith("h") else 32
            memory = MemoryAccess("read" if load else "write", width, signed, base, idx, disp)
            memory_effect()
            if args[1].endswith("!"):
                define("base", base)
        if load and _reg(args[0]) == "pc":
            control = "indirect"
    elif op in ("cmp", "cmn", "tst", "teq"):
        for k, arg in enumerate(args):
            use(f"input{k}", arg)
        flags("NZCV" if op in ("cmp", "cmn") else "NZ")
    elif op in ("mov", "movs", "mvn", "mvns") and len(args) == 2:
        define("value", args[0])
        use("input0", args[1])
        if op in ("mov", "movs") and len(_registers(args[1])) == 1:
            copy_source = _reg(args[1])
        if op.endswith("s"):
            flags("NZ")
        if _reg(args[0]) == "pc":
            control = "indirect"
    elif op in ("add", "adds", "sub", "subs", "adc", "adcs", "sbc", "sbcs", "rsb", "rsbs",
                "and", "ands", "orr", "orrs", "eor", "eors", "bic", "bics", "mul", "muls",
                "lsl", "lsls", "lsr", "lsrs", "asr", "asrs", "ror", "rors", "neg", "negs",
                "adr") and args:
        define("value", args[0])
        inputs = args[1:] if len(args) >= 3 or op in ("neg", "negs", "adr") else args
        for k, arg in enumerate(inputs):
            use(f"input{k}", arg)
        if op == "adr":
            use("pc", "pc")
        if op in ("adc", "adcs", "sbc", "sbcs"):
            uses.append(Operand("carry", "@C"))
        if op.endswith("s"):
            if op in ("adds", "subs", "adcs", "sbcs", "rsbs", "negs"):
                flags("NZCV")
            elif op in ("lsls", "lsrs", "asrs", "rors"):
                shift = _number(inputs[-1]) if len(inputs) > 1 else None
                # Thumb immediate LSR/ASR #0 encodes a shift by 32, but scope's
                # disassembler writes #32. A zero/variable shift may preserve C.
                flags("NZ" if shift == 0 else "NZC")
                if shift is None:
                    uses.append(Operand("carry_if_zero_shift", "@C"))
            else:
                flags("NZ")
        if op in ("add", "adds") and len(inputs) == 2 and _number(inputs[1]) == 0:
            copy_source = _reg(inputs[0]) if _registers(inputs[0]) else None
        if op in ("lsl", "lsls") and len(inputs) == 2 and _number(inputs[1]) == 0:
            copy_source = _reg(inputs[0]) if _registers(inputs[0]) else None
        if _reg(args[0]) == "pc":
            control = "indirect"
    elif op in ("nop",):
        pass
    elif op in ("swi", "svc", "bkpt"):
        callee = f"{op}:{','.join(args)}"
        control, supported = "call", False
        abi_inputs()
        defs.extend(Operand(f"unknown.{r}", r) for r in STATE_KEYS)
    else:
        # Unsupported instructions cannot silently certify a compatible region.
        supported = False
        uses.extend(Operand(f"unknown.{r}", r) for r in STATE_KEYS)
        defs.extend(Operand(f"unknown.{r}", r) for r in STATE_KEYS)

    shape_args = tuple(_normalize_operand(a) for a in args)
    loose_args = tuple(_normalize_operand(a, relaxed=True) for a in args)
    if control in ("branch", "conditional"):
        shape_args = loose_args = ("LOCAL" if target is not None else "EXTERNAL",)
    if control in ("call", "tailcall"):
        loose_args = ("CALLEE",)
    return Instruction(index, offset, size, text, op, args, tuple(uses), tuple(defs),
                       (op, shape_args), (op, loose_args), target, control, callee, memory,
                       copy_source, supported)


def _rows(source: str | Iterable[Sequence[Any]], address: int) -> tuple[list[tuple], str, int]:
    if not isinstance(source, str):
        return [tuple(row) for row in source], "", address
    result, name = [], ""
    for line in source.splitlines():
        header = re.match(r"(\S+)\s+@\s+([0-9a-f]{8})\b", line, re.I)
        if header:
            name, address = header[1], int(header[2], 16)
            continue
        # Scope text uses hexadecimal relative offsets; objdump absolute offsets
        # are accepted too when an address was supplied.
        match = re.match(r"^\s*([0-9a-f]+):?\s+((?:\.?[a-z])[^\n]*?)\s*$", line, re.I)
        if not match:
            continue
        offset, text = int(match[1], 16), match[2].strip()
        if address and offset >= address:
            offset -= address
        text = re.split(r"\s+(?:;|@|//)", text, maxsplit=1)[0].rstrip()
        result.append((offset, 0, text, text, None))
    for k, row in enumerate(result):
        distance = result[k + 1][0] - row[0] if k + 1 < len(result) else 0
        default = 4 if row[2].startswith(("bl ", ".word")) else 2
        result[k] = (row[0], distance if distance in (1, 2, 4) else default, *row[2:])
    return result, name, address


def _jump_tables(instructions: tuple[Instruction, ...], data: Sequence[tuple],
                 address: int) -> dict[int, tuple[int, ...]]:
    """Recognize agbcc's indexed absolute-word Thumb switch dispatch.

    This recovers the possible destination relation, not a proof of the index
    range. Read-only table bytes and the complete scale/add/load/jump chain are
    required. The bounds comparison remains visible in the surrounding CFG.
    """
    code_at = {i.offset for i in instructions}
    words = {}
    for offset, size, text, target in data:
        if size != 4 or not text.startswith(".word "):
            continue
        number = _number(text.split(None, 1)[1])
        if target is None and number is not None:
            target = (number & ~1) - address
        if target in code_at:
            words[offset] = target
    found = {}
    for n, branch in enumerate(instructions):
        if branch.opcode not in ("mov", "bx") or branch.control != "indirect" or n < 4:
            continue
        register = _reg(branch.arguments[-1])
        load, add, literal, scale = instructions[n - 1], instructions[n - 2], instructions[n - 3], instructions[n - 4]
        if load.opcode != "ldr" or not load.memory or _reg(load.arguments[0]) != register:
            continue
        if load.memory.displacement or load.memory.index:
            continue
        if add.opcode not in ("add", "adds") or len(add.arguments) != 3:
            continue
        if _reg(add.arguments[0]) != load.memory.base:
            continue
        if literal.opcode != "ldr" or len(literal.arguments) != 2:
            continue
        table_match = re.fullmatch(r"=func\+0x([0-9a-f]+)", literal.arguments[1], re.I)
        if not table_match or _reg(literal.arguments[0]) not in map(_reg, add.arguments[1:]):
            continue
        if scale.opcode not in ("lsl", "lsls") or _number(scale.arguments[-1]) != 2:
            continue
        if _reg(scale.arguments[0]) not in map(_reg, add.arguments[1:]):
            continue
        start = int(table_match[1], 16)
        targets = []
        while start + 4 * len(targets) in words and len(targets) < 256:
            targets.append(words[start + 4 * len(targets)])
        if targets:
            found[n] = tuple(targets)
    return found


def _cfg(instructions: tuple[Instruction, ...], jump_tables: Mapping[int, tuple[int, ...]]) -> tuple[list[Block], dict[int, int], list[str]]:
    if not instructions:
        return [], {}, []
    at = {i.offset: i.index for i in instructions}
    starts = {0}
    warnings = []
    for targets in jump_tables.values():
        starts.update(at[t] for t in targets)
    for i, ins in enumerate(instructions):
        if ins.target in at and ins.control in ("branch", "conditional"):
            starts.add(at[ins.target])
        if i + 1 < len(instructions):
            if ins.control != "fallthrough" or ins.offset + ins.size != instructions[i + 1].offset:
                starts.add(i + 1)
        if ins.control in ("branch", "conditional") and ins.target not in at:
            warnings.append(f"Branch at +0x{ins.offset:X} leaves the decoded CFG or targets data")
        if ins.index in jump_tables:
            warnings.append(f"Recovered {len(jump_tables[ins.index])} possible jump-table destinations at +0x{ins.offset:X}; index bounds are not proved")
        elif ins.control == "indirect":
            warnings.append(f"Indirect control flow at +0x{ins.offset:X}: successors unresolved")
    indices = sorted(starts) + [len(instructions)]
    blocks = [Block(k, instructions[a].offset, instructions[b - 1].offset + instructions[b - 1].size,
                    tuple(range(a, b))) for k, (a, b) in enumerate(zip(indices, indices[1:]))]
    which = {i: b.id for b in blocks for i in b.instructions}
    for block in blocks:
        last = instructions[block.instructions[-1]]
        nxt = block.id + 1
        if last.control in ("branch", "conditional") and last.target in at:
            block.successors.append(("taken", which[at[last.target]]))
        if last.index in jump_tables:
            block.successors.extend((f"case{k}", which[at[t]]) for k, t in enumerate(jump_tables[last.index]))
        if last.control in ("fallthrough", "conditional", "call") and nxt < len(blocks):
            if last.offset + last.size == blocks[nxt].start:
                block.successors.append(("fallthrough", nxt))
        for _, successor in block.successors:
            blocks[successor].predecessors.append(block.id)
    todo = [0]
    while todo:
        block = blocks[todo.pop()]
        if block.reachable:
            continue
        block.reachable = True
        todo.extend(b for _, b in block.successors)
    if any(not b.reachable for b in blocks):
        warnings.append("Decoded blocks are unreachable in the resolved CFG; indirect targets may be missing")
    return blocks, which, warnings


def _ref(index: int, role: str) -> str:
    return f"n{index}:{role}"


def _ref_index(value: str) -> int | None:
    return int(value[1:value.index(":")]) if value.startswith("n") else None


def _dataflow(instructions: tuple[Instruction, ...], blocks: list[Block], which: dict[int, int], address: int) -> tuple:
    if not blocks:
        return (), ()
    empty = {r: frozenset() for r in STATE_KEYS}
    entry = {r: frozenset((f"entry:{r}",)) for r in STATE_KEYS}
    incoming = [empty.copy() for _ in blocks]
    outgoing = [empty.copy() for _ in blocks]

    def read_register(ins: Instruction, register: str, state: dict) -> frozenset:
        if register == "pc":
            value = address + ins.offset + 4
            if ins.opcode == "adr" or (ins.memory and ins.memory.base == "pc"):
                value &= ~3
            return frozenset((f"pc:{value}",))
        return state[register]

    def transfer(block: Block, state: dict, record: list | None = None) -> dict:
        state = state.copy()
        for k in block.instructions:
            ins = instructions[k]
            if record is not None:
                record[k] = {o.role: tuple(sorted(read_register(ins, o.register, state))) for o in ins.uses}
            before = state
            state = state.copy()
            for definition in ins.definitions:
                if ins.copy_source and definition.role == "value":
                    state[definition.register] = read_register(ins, ins.copy_source, before)
                else:
                    state[definition.register] = frozenset((_ref(k, definition.role),))
        return state

    queue, queued = deque(range(len(blocks))), set(range(len(blocks)))
    while queue:
        bid = queue.popleft()
        queued.discard(bid)
        block = blocks[bid]
        # Text after a nonfallthrough jump can still have a textual edge into
        # reachable code. Its synthetic entry state is not an execution path
        # from this function's entry and must not contaminate that merge.
        parents = [outgoing[p] for p in block.predecessors
                   if not block.reachable or blocks[p].reachable]
        if bid == 0 or not block.predecessors:
            parents.append(entry)
        state = {r: frozenset().union(*(p[r] for p in parents)) for r in STATE_KEYS}
        incoming[bid] = state
        after = transfer(block, state)
        if after != outgoing[bid]:
            outgoing[bid] = after
            for _, successor in block.successors:
                if successor not in queued:
                    queue.append(successor)
                    queued.add(successor)

    dependencies: list[dict] = [{} for _ in instructions]
    consumers: dict[str, set[int]] = defaultdict(set)
    for block in blocks:
        transfer(block, incoming[block.id], dependencies)
    for ins, deps in zip(instructions, dependencies):
        for operand in ins.uses:
            for reference in deps[operand.role]:
                consumers[reference].add(ins.index)
                src = _ref_index(reference)
                if src is None or which[src] != which[ins.index]:
                    key = f"{operand.register}:{operand.role}"
                    old = blocks[which[ins.index]].live_in.get(key, ())
                    blocks[which[ins.index]].live_in[key] = tuple(sorted(set(old) | {reference}))
    for block in blocks:
        for r, references in outgoing[block.id].items():
            used_outside = tuple(sorted(ref for ref in references
                                if any(which[c] != block.id for c in consumers[ref])))
            if used_outside:
                block.live_out[r] = used_outside
    back_edges = tuple((b.id, successor) for b in blocks for _, successor in b.successors
                       if blocks[successor].start <= b.start)
    return tuple(dependencies), back_edges


def _addresses(instructions: tuple[Instruction, ...], dependencies: Sequence[dict], address: int) -> list[dict[str, Any]]:
    """Project 32-bit machine arithmetic into affine address facts.

    Loads and unknown/loop-carried values remain opaque terms. This deliberately
    does not infer that two loads alias or that a source pointer has a given type.
    """
    mask = 0xFFFFFFFF
    cache: dict[str, tuple] = {}
    active: set[str] = set()

    def atom(name: str) -> tuple:
        return ((name, 1),), 0

    def constant(value: int) -> tuple:
        return (), value & mask

    def combine(x: tuple, y: tuple, sign: int = 1) -> tuple:
        terms = dict(x[0])
        for term, coefficient in y[0]:
            terms[term] = (terms.get(term, 0) + sign * coefficient) & mask
        return tuple(sorted((term, coefficient) for term, coefficient in terms.items() if coefficient)), (x[1] + sign * y[1]) & mask

    def scale(x: tuple, factor: int) -> tuple:
        return tuple((term, coefficient * factor & mask) for term, coefficient in x[0]
                     if coefficient * factor & mask), x[1] * factor & mask

    def sources(refs: Sequence[str]) -> tuple:
        values = {resolve(ref) for ref in refs}
        if len(values) == 1:
            return next(iter(values))
        return atom("phi:" + "|".join(sorted(refs)) if refs else "unknown-input")

    def resolve(ref: str) -> tuple:
        if ref.startswith("pc:"):
            return constant(int(ref.split(":", 1)[1]))
        if ref in cache:
            return cache[ref]
        idx = _ref_index(ref)
        if idx is None or ref in active or len(active) >= 64:
            return atom(ref)
        active.add(ref)
        ins = instructions[idx]
        role = ref.split(":", 1)[1]
        value = atom(ref)

        def operand(argument: str, input_role: str) -> tuple:
            number = _number(argument)
            return constant(number) if number is not None else sources(dependencies[idx].get(input_role, ()))

        if role == "link" and ins.opcode == "bl" and ins.control == "branch":
            value = constant((address + ins.offset + ins.size) | 1)
        elif role == "value" and ins.opcode in ("mov", "movs") and len(ins.arguments) == 2:
            value = operand(ins.arguments[1], "input0")
        elif role == "value" and ins.opcode == "ldr" and len(ins.arguments) == 2 and ins.arguments[1].startswith("="):
            number = _number(ins.arguments[1])
            if number is not None:
                value = constant(number)
            elif ins.target is not None:
                value = constant(address + ins.target)
        elif role == "base" and ins.opcode in ("push", "pop", "ldm", "ldmia", "stm", "stmia") and ins.memory:
            value = combine(sources(dependencies[idx].get("base", ())), constant(
                -4 * ins.memory.count if ins.opcode == "push" else 4 * ins.memory.count))
        elif role == "value" and ins.opcode in ("add", "adds", "sub", "subs", "rsb", "rsbs", "lsl", "lsls",
                                                   "lsr", "lsrs", "asr", "asrs", "and", "ands", "orr", "orrs",
                                                   "eor", "eors", "bic", "bics", "mul", "muls"):
            args = ins.arguments[1:] if len(ins.arguments) >= 3 else ins.arguments
            if len(args) == 2:
                x, y = operand(args[0], "input0"), operand(args[1], "input1")
                op = ins.opcode.removesuffix("s")
                if op == "add":
                    value = combine(x, y)
                elif op == "sub":
                    value = combine(x, y, -1)
                elif op == "rsb":
                    value = combine(y, x, -1)
                elif op == "lsl" and not y[0]:
                    shift = y[1] if _number(args[1]) is not None else y[1] & 255
                    value = scale(x, 1 << shift) if shift < 32 else constant(0)
                elif op == "mul" and not y[0]:
                    value = scale(x, y[1])
                elif op == "mul" and not x[0]:
                    value = scale(y, x[1])
                elif not x[0] and not y[0]:
                    a, b = x[1], y[1]
                    shift = b if _number(args[1]) is not None else b & 255
                    if op == "lsr":
                        value = constant(a >> shift if shift < 32 else 0)
                    elif op == "asr":
                        signed = a if a < 0x80000000 else a - 0x100000000
                        value = constant(signed >> min(shift, 32))
                    elif op == "and":
                        value = constant(a & b)
                    elif op == "orr":
                        value = constant(a | b)
                    elif op == "eor":
                        value = constant(a ^ b)
                    elif op == "bic":
                        value = constant(a & ~b)
        active.remove(ref)
        cache[ref] = value
        return value

    result = []
    for ins in instructions:
        mem = ins.memory
        if mem is None:
            continue
        expr = combine(sources(dependencies[ins.index].get("base", ())), constant(mem.displacement))
        if mem.index:
            expr = combine(expr, sources(dependencies[ins.index].get("index", ())))
        signed = lambda n: n if n < 0x80000000 else n - 0x100000000
        result.append({"offset": ins.offset, "kind": mem.kind, "width": mem.width, "signed": mem.signed,
                       "terms": [{"value": name, "coefficient": signed(coefficient)} for name, coefficient in expr[0]],
                       "displacement": signed(expr[1]),
                       "absolute": expr[1] if not expr[0] else None,
                       "has_phi": any(name.startswith("phi:") for name, _ in expr[0])})
    return result


def analyze(source: str | Iterable[Sequence[Any]], *, name: str = "", address: int = 0,
            call_arities: Mapping[str, int] | None = None, stack_policy: str = "strict") -> Analysis:
    """Lift decoded Thumb instructions into a conservative reaching-value graph.

    Scope rows are ``(offset, size, text, key, local_target_or_none)``. Pass decoded
    rows rather than strings to preserve instruction/data boundaries precisely.
    ``call_arities`` can remove unknown unused register arguments from call edges;
    absent signatures conservatively consume all four argument registers.
    ``stack_policy`` defaults to strict invalidation on external calls/unknown
    stores. ``abi-private`` adds explicit private-frame/non-escape assumptions;
    transported dependencies remain separate from the physical graph.
    """
    rows, inferred_name, address = _rows(source, address)
    instructions, data, warnings = [], [], []
    code_offsets = {row[0] for row in rows if not row[2].lstrip().startswith(".")}
    for row in rows:
        offset, size, text = row[:3]
        target = row[4] if len(row) >= 5 else None
        if text.lstrip().startswith("."):
            data.append((offset, size, text, target))
            continue
        ins = _describe(len(instructions), offset, size, text, target, address, call_arities or {}, code_offsets)
        instructions.append(ins)
        if not ins.supported:
            warnings.append(f"Unsupported opcode at +0x{offset:X}: {ins.opcode}")
    insns = tuple(instructions)
    jump_tables = _jump_tables(insns, data, address)
    blocks, which, cfg_warnings = _cfg(insns, jump_tables)
    dependencies, back_edges = _dataflow(insns, blocks, which, address)
    assumptions = (
        "Machine definitions are not original C variables or GCC pseudos.",
        "Memory effects are conservatively ordered; alias independence is not inferred.",
        "Calls without supplied signatures consume r0-r3 and may change memory and caller-saved registers.",
        "Indirect branches have unresolved successors; region compatibility is not a semantic proof.",
    )
    graph = [(i.shape, dependencies[i.index]) for i in insns]
    fingerprints = {
        "shape": _digest([i.shape for i in insns]),
        "dependencies": _digest(graph),
        "control": _digest([(b.instructions, b.successors) for b in blocks]),
        "effects": _digest([(i.callee, (i.memory.kind, i.memory.width, i.memory.signed,
                                        i.memory.displacement, i.memory.count) if i.memory else None)
                            for i in insns if i.callee or i.memory]),
    }
    from .stackflow import analyze_stack
    transport_dependencies, stack_transport = analyze_stack(
        insns, blocks, dependencies, address=address, policy=stack_policy, call_arities=call_arities)
    stack_transport["cfg_warnings"] = list(cfg_warnings)
    return Analysis(name or inferred_name, address, insns, blocks, dependencies, tuple(data),
                    tuple(warnings + cfg_warnings), fingerprints, back_edges, assumptions, jump_tables,
                    _addresses(insns, dependencies, address), transport_dependencies, stack_transport)


def _align(a: Sequence[Instruction], b: Sequence[Instruction]) -> list[tuple[int | None, int | None]]:
    """Structural anchors, then same-operation pairing for changed constants.

    This is correspondence evidence, not a unique inverse. Duplicate anchors are
    counted by compare; dependencies and branch endpoints validate every pair.
    """
    result = []
    matcher = SequenceMatcher(None, [i.shape for i in a], [i.shape for i in b], autojunk=False)
    for tag, x0, x1, y0, y1 in matcher.get_opcodes():
        if tag == "equal":
            result.extend(zip(range(x0, x1), range(y0, y1)))
            continue
        relaxed = SequenceMatcher(None, [i.relaxed_shape for i in a[x0:x1]],
                                  [i.relaxed_shape for i in b[y0:y1]], autojunk=False)
        for loose_tag, u0, u1, v0, v1 in relaxed.get_opcodes():
            if loose_tag == "equal":
                result.extend(zip(range(x0 + u0, x0 + u1), range(y0 + v0, y0 + v1)))
            else:
                result.extend((u, None) for u in range(x0 + u0, x0 + u1))
                result.extend((None, v) for v in range(y0 + v0, y0 + v1))
    return result


def compare(target: Analysis, candidate: Analysis, *, detail_limit: int = 30) -> dict[str, Any]:
    """Compare shape, scalar operands, dependencies, CFG and effects separately.

    The returned vector is multi-objective. A search should preserve different
    frontiers/fingerprints rather than collapsing it into a universal gradient.
    """
    a, b = target.instructions, candidate.instructions
    pairs = _align(a, b)
    mapping = {x: y for x, y in pairs if x is not None and y is not None}
    inverse = {y: x for x, y in mapping.items()}
    a_at, b_at = {i.offset: i.index for i in a}, {i.offset: i.index for i in b}
    a_block = {i: block.id for block in target.blocks for i in block.instructions}
    b_block = {i: block.id for block in candidate.blocks for i in block.instructions}
    distances = {k: 0 for k in ("instructions", "operands", "dependencies", "control", "calls", "memory", "data")}
    issues, dependency_bad, control_bad, scalar_bad = [], set(), set(), set()
    register_votes: dict[str, dict[str, list[int]]] = defaultdict(lambda: defaultdict(list))

    def translate(reference: str) -> str:
        idx = _ref_index(reference)
        if idx is None:
            return reference
        suffix = reference[reference.index(":") + 1:]
        return _ref(mapping[idx], suffix) if idx in mapping else f"unmapped:{reference}"

    for x, y in pairs:
        if x is None or y is None:
            distances["instructions"] += 1
            ins = a[x] if y is None else b[y]
            distances["dependencies"] += len(ins.uses)
            distances["control"] += ins.control != "fallthrough"
            if len(issues) < detail_limit:
                issues.append({"kind": "missing" if y is None else "extra",
                               "target_offset": a[x].offset if x is not None else None,
                               "candidate_offset": b[y].offset if y is not None else None,
                               "instruction": ins.text})
            continue
        left, right = a[x], b[y]
        if left.shape != right.shape:
            distances["operands"] += 1
            scalar_bad.add(x)
            if len(issues) < detail_limit:
                issues.append({"kind": "operand", "target_offset": left.offset,
                               "candidate_offset": right.offset, "target": left.text, "candidate": right.text})
        da, db = target.dependencies[x], candidate.dependencies[y]
        for role in sorted(set(da) | set(db)):
            expected = set(map(translate, da.get(role, ())))
            actual = set(db.get(role, ()))
            if expected != actual:
                distances["dependencies"] += 1
                dependency_bad.add(x)
                if len(issues) < detail_limit:
                    issues.append({"kind": "dependency", "target_offset": left.offset,
                                   "candidate_offset": right.offset, "role": role,
                                   "target_sources": da.get(role, ()), "candidate_sources": db.get(role, ())})
        left_regs = {o.role: o.register for o in left.uses}
        for operand in right.uses:
            if operand.register.startswith("@") or operand.role not in left_regs:
                continue
            references = db.get(operand.role, ())
            # Retain contradictions; do not majority-vote an alleged original
            # register assignment. A machine value can travel through copies.
            if len(references) == 1:
                register_votes[references[0]][left_regs[operand.role]].append(left.offset)
        bad_control = left.control != right.control
        if left.control in ("branch", "conditional"):
            ta, tb = a_at.get(left.target), b_at.get(right.target)
            bad_control |= ta is None or tb is None or mapping.get(ta) != tb
        elif left.target is not None or right.target is not None:
            ta, tb = a_at.get(left.target), b_at.get(right.target)
            if ta is not None or tb is not None:
                bad_control |= ta is None or tb is None or mapping.get(ta) != tb
            elif left.target != right.target:
                # Local data addresses are not freely interchangeable. Resolving
                # object identities across relocated pools requires more evidence.
                distances["operands"] += 1
                scalar_bad.add(x)
        if x in target.jump_tables or y in candidate.jump_tables:
            target_cases = tuple(mapping.get(a_at[t]) for t in target.jump_tables.get(x, ()))
            candidate_cases = tuple(b_at[t] for t in candidate.jump_tables.get(y, ()))
            bad_control |= target_cases != candidate_cases
        if bad_control:
            distances["control"] += 1
            control_bad.add(x)
            if len(issues) < detail_limit:
                issues.append({"kind": "control", "target_offset": left.offset,
                               "candidate_offset": right.offset,
                               "target_destination": left.target, "candidate_destination": right.target})

    def edit_distance(seq_a: Sequence, seq_b: Sequence) -> int:
        return sum(max(x1 - x0, y1 - y0) for tag, x0, x1, y0, y1
                   in SequenceMatcher(None, seq_a, seq_b, autojunk=False).get_opcodes() if tag != "equal")

    distances["calls"] = edit_distance([i.callee for i in a if i.callee], [i.callee for i in b if i.callee])

    def memory_key(ins: Instruction) -> tuple:
        mem = ins.memory
        assert mem is not None
        return (mem.kind, mem.width, mem.signed, "stack" if mem.base == "sp" else "address",
                mem.displacement, bool(mem.index), mem.count, mem.writeback)

    distances["memory"] = edit_distance([memory_key(i) for i in a if i.memory],
                                        [memory_key(i) for i in b if i.memory])
    distances["data"] = edit_distance([(size, text) for _, size, text, _ in target.data],
                                      [(size, text) for _, size, text, _ in candidate.data])
    regions = []
    for block in target.blocks:
        mapped = [mapping.get(i) for i in block.instructions]
        candidate_blocks = {b_block[i] for i in mapped if i is not None}
        complete = all(i is not None for i in mapped) and len(candidate_blocks) == 1
        cb = candidate.blocks[next(iter(candidate_blocks))] if len(candidate_blocks) == 1 else None
        complete &= cb is not None and tuple(mapped) == cb.instructions
        shape_ok = complete and not any(i in scalar_bad for i in block.instructions)
        deps_ok = complete and not any(i in dependency_bad for i in block.instructions)
        control_ok = complete and not any(i in control_bad for i in block.instructions)
        known = all(a[i].supported for i in block.instructions) and block.reachable
        known &= bool(cb and cb.reachable and all(b[i].supported for i in cb.instructions))
        identical = complete and all(a[x].text == b[y].text for x, y in zip(block.instructions, mapped))
        compatible = bool(shape_ok and deps_ok and control_ok and known)
        verdict = "same-instructions" if identical and control_ok else "dependency-compatible" if compatible else "unresolved"
        canonical_dependencies = []
        boundary_unmapped = []
        candidate_dependency_hash = None
        canonical_successors = []
        if complete and cb is not None:
            def canonical_reference(reference: str) -> str:
                source_index = _ref_index(reference)
                if source_index is None:
                    return reference
                if source_index not in inverse:
                    boundary_unmapped.append(reference)
                    return f"unmapped:{reference}"
                return _ref(inverse[source_index], reference.split(":", 1)[1])

            canonical_dependencies = [{role: sorted(canonical_reference(ref) for ref in refs)
                                       for role, refs in candidate.dependencies[i].items()}
                                      for i in cb.instructions]
            for edge_label, successor in cb.successors:
                first_instruction = candidate.blocks[successor].instructions[0]
                target_instruction = inverse.get(first_instruction)
                if target_instruction is None:
                    boundary_unmapped.append(f"block:{successor}")
                canonical_successors.append((edge_label, a_block.get(target_instruction)))
            if not boundary_unmapped and known:
                # All producer IDs and edge endpoints use target coordinates.
                # Inserting unrelated earlier candidate code cannot invalidate
                # this regional fingerprint merely by renumbering instructions.
                candidate_dependency_hash = _digest({
                    "instructions": [b[i].shape for i in cb.instructions],
                    "dependencies": canonical_dependencies,
                    "successors": canonical_successors,
                })
        regions.append({"target_block": block.id, "target_range": [block.start, block.end],
                        "candidate_block": cb.id if cb else None,
                        "candidate_range": [cb.start, cb.end] if cb else None,
                        "verdict": verdict, "register_neutral": compatible,
                        "shape_equal": bool(shape_ok), "dependencies_equal": bool(deps_ok),
                        "control_equal": bool(control_ok),
                        "live_in": block.live_in, "live_out": block.live_out,
                        "candidate_dependency_hash": candidate_dependency_hash,
                        "candidate_dependencies_in_target_coordinates": canonical_dependencies,
                        "candidate_successors_in_target_coordinates": canonical_successors,
                        "unmapped_boundary_values": sorted(set(boundary_unmapped))})
    contradictions = [{"candidate_value": value,
                       "target_registers": {reg: offsets[:8] for reg, offsets in sorted(votes.items())}}
                      for value, votes in register_votes.items() if len(votes) > 1]
    counts_a, counts_b = Counter(i.shape for i in a), Counter(i.shape for i in b)
    duplicate_pairs = sum(counts_a[a[x].shape] > 1 and counts_b[b[y].shape] > 1 for x, y in mapping.items())
    warnings = list(dict.fromkeys(target.warnings + candidate.warnings))
    if duplicate_pairs:
        warnings.append(f"{duplicate_pairs} aligned instructions have duplicate structural anchors; correspondence is heuristic")
    if contradictions:
        warnings.append("A candidate machine value appears in multiple target registers; copy/allocation correspondence remains a relation")
    return {
        "vector": distances,
        "fingerprints": candidate.fingerprints,
        "regions": regions,
        "compatible_regions": sum(r["register_neutral"] for r in regions),
        "instruction_identical_regions": sum(r["verdict"] == "same-instructions" for r in regions),
        "alignment": [[a[x].offset if x is not None else None, b[y].offset if y is not None else None]
                      for x, y in pairs],
        "issues": issues,
        "register_correspondence_conflicts": contradictions[:detail_limit],
        "register_correspondence_conflict_count": len(contradictions),
        "warnings": warnings,
        "assumptions": target.assumptions,
        "semantic_proof": False,
    }

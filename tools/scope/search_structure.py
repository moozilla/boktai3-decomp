"""Constrained observations for matching search, without another compiler invocation.

The ROM determines machine behavior, not unique C variables. In particular, a hard
register can hold several pseudos and reload temporaries, while coalescing can hide
copies. This module exposes reaching definitions and ambiguity rather than claiming
that register lifetimes recover original variables. Its block-local SSA signatures
preserve dependencies and survive unrelated register choices in earlier regions.
"""
from __future__ import annotations

import collections
import difflib
import hashlib
import functools
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2]
REG = re.compile(r"\b(?:r(?:1[0-5]|[0-9])|sp|lr|pc|ip|sl|fp|sb)\b", re.I)
ALIASES = {"r13": "sp", "r14": "lr", "r15": "pc", "ip": "r12", "sl": "r10", "fp": "r11", "sb": "r9"}
CONDITIONS = {"beq", "bne", "bcs", "bcc", "bhs", "blo", "bmi", "bpl", "bvs", "bvc", "bhi", "bls", "bge", "blt", "bgt", "ble"}
CALLS = {"bl", "blx"}
STORES = {"str": 4, "strh": 2, "strb": 1}
LOADS = {"ldr": (4, False), "ldrh": (2, False), "ldrb": (1, False), "ldrsh": (2, True), "ldrsb": (1, True)}
PASS_SOURCES = {
    "allocation": [("global.c", "allocno_compare"), ("global.c", "find_reg"), ("local-alloc.c", "local_alloc")],
    "reload": [("reload1.c", "reload"), ("reload.c", "find_reloads")],
    "canonicalization": [("cse.c", "canon_reg"), ("cse.c", "cse_basic_block")],
    "control_flow": [("jump.c", "jump_optimize"), ("jump.c", "thread_jumps")],
    "loop": [("loop.c", "loop_optimize"), ("loop.c", "strength_reduce")],
    "expression": [("fold-const.c", "fold"), ("combine.c", "try_combine")],
    "copies": [("regmove.c", "regmove_optimize"), ("regmove.c", "fixup_match_1")],
    "pre": [("gcse.c", "pre_gcse"), ("gcse.c", "compute_insert_delete")],
}


def _hash(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(",", ":")).encode()).hexdigest()[:20]


def _reg(value):
    value = value.lower()
    return ALIASES.get(value, value)


def _registers(text):
    return [_reg(m.group()) for m in REG.finditer(text)]


def _split_operands(text):
    out, start, depth = [], 0, 0
    for i, ch in enumerate(text):
        depth += ch in "[{("
        depth -= ch in "]})"
        if ch == "," and depth == 0:
            out.append(text[start:i].strip())
            start = i + 1
    if text[start:].strip():
        out.append(text[start:].strip())
    return out


def _number(text):
    try:
        return int(text.replace("#", "").strip(), 0)
    except ValueError:
        return None


def _row(value, index):
    if isinstance(value, str):
        d = {"offset": 2 * index, "size": 2, "text": value, "target": None}
    elif isinstance(value, dict):
        d = dict(value)
        d["offset"] = d.get("offset", d.get("address", 2 * index))
        d["text"] = d.get("text", d.get("asm", ""))
        d["target"] = d.get("target", d.get("target_offset"))
        d.setdefault("size", 2)
    else:
        d = {"offset": value[0], "size": value[1], "text": value[2], "target": value[4] if len(value) > 4 else None}
    text = d["text"].strip()
    op, _, args = text.partition(" ")
    d.update(op=op.lower().split(".")[0], operands=_split_operands(args), data=op.startswith("."))
    if d["target"] is None:
        m = re.search(r"func\+0x([0-9a-f]+)", text, re.I)
        if m:
            d["target"] = int(m.group(1), 16)
    return d


def _effects(row):
    """Register effects, conservative for calls and unusual instructions.

    Uses precede definitions: two-address operations and address writeback keep
    their dependencies. Flags are values too. Unknown ops are marked, never proof.
    """
    op, args = row["op"], row["operands"]
    regs = [_registers(a) for a in args]
    flat = [r for operand in regs for r in operand]
    reads, writes, known = [], [], True
    if row["data"]:
        return [], [], True
    if op in CALLS or op in {"swi", "svc"}:
        reads = flat + ["r0", "r1", "r2", "r3", "sp"]
        writes = ["r0", "r1", "r2", "r3", "r12", "lr", "flags"]
    elif op in CONDITIONS:
        reads = ["flags"]
    elif op in {"b", "bx"}:
        reads = flat
    elif op in {"cmp", "cmn", "tst", "teq"}:
        reads, writes = flat, ["flags"]
    elif op == "push":
        reads, writes = flat + ["sp"], ["sp"]
    elif op == "pop":
        reads, writes = ["sp"], flat + ["sp"]
    elif op.startswith(("ldm", "stm")):
        if op.startswith("ldm"):
            reads, writes = regs[0], flat[1:] + regs[0]
        else:
            reads, writes = flat, regs[0]
    elif op in STORES:
        reads = flat
        if any("!" in a for a in args[1:]):
            writes = regs[1][:1]
    elif op in LOADS or op in {"adr"}:
        writes = regs[0] if regs else []
        reads = [r for operand in regs[1:] for r in operand]
        if any("!" in a for a in args[1:]):
            writes += regs[1][:1]
    elif op in {"mov", "movs", "mvn", "mvns", "neg", "negs"}:
        writes = regs[0] if regs else []
        reads = [r for operand in regs[1:] for r in operand]
        if op.endswith("s"):
            writes += ["flags"]
    elif op.rstrip("s") in {"add", "sub", "mul", "and", "orr", "eor", "bic", "lsl", "lsr", "asr", "ror", "adc", "sbc", "rsb"}:
        writes = regs[0] if regs else []
        reads = [r for operand in regs[1:] for r in operand]
        if len(args) == 2:
            reads = regs[0] + reads
        if op.rstrip("s") in {"adc", "sbc", "ror"}:
            reads += ["flags"]
        if op.endswith("s"):
            writes += ["flags"]
    elif op in {"nop"}:
        pass
    else:
        reads, writes, known = flat, flat[:1], False
    return list(dict.fromkeys(reads)), list(dict.fromkeys(writes)), known


def _terminates(row):
    return (row["op"] in {"b", "bx"} or
            row["op"] not in CALLS and "pc" in _effects(row)[1])


def _memory(row):
    op, args = row["op"], row["operands"]
    if op not in LOADS and op not in STORES:
        if op.startswith(("ldm", "stm")) or op in {"push", "pop"}:
            return {"offset": row["offset"], "kind": "multiple", "instruction": row["text"], "width": 4}
        return None
    width, signed = LOADS.get(op, (STORES.get(op), False))
    addr = args[1] if len(args) > 1 else ""
    regs = _registers(addr)
    m = re.search(r"#(-?(?:0x[0-9a-f]+|\d+))", addr, re.I)
    return {"offset": row["offset"], "kind": "load" if op in LOADS else "store", "width": width,
            "signed": signed if op in LOADS else None, "base": regs[0] if regs else None,
            "index": regs[1:] or None, "displacement": int(m.group(1), 0) if m else 0 if "[" in addr else None,
            "literal": addr[1:] if addr.startswith("=") else None,
            "address_evidence": "machine address only; source object/type is not inferred"}


def _blocks(rows):
    code = [r for r in rows if not r["data"]]
    if not code:
        return []
    offsets = {r["offset"]: i for i, r in enumerate(code)}
    leaders = {0}
    for i, row in enumerate(code):
        if row["op"] in CONDITIONS | {"b", "bx"} or _terminates(row):
            if row["target"] in offsets:
                leaders.add(offsets[row["target"]])
            if i + 1 < len(code):
                leaders.add(i + 1)
    starts = sorted(leaders)
    blocks = [{"id": f"B{i}", "start": code[a]["offset"], "instructions": code[a:b]}
              for i, (a, b) in enumerate(zip(starts, starts[1:] + [len(code)]))]
    owners = {r["offset"]: b["id"] for b in blocks for r in b["instructions"]}
    for i, b in enumerate(blocks):
        last = b["instructions"][-1]
        successors = []
        if last["op"] in CONDITIONS | {"b"} and last["target"] in owners:
            successors.append({"kind": "taken", "block": owners[last["target"]]})
        if i + 1 < len(blocks) and not _terminates(last):
            successors.append({"kind": "fallthrough", "block": blocks[i + 1]["id"]})
        b["successors"] = successors
    return blocks


def _canonical(block):
    """SSA-like value IDs, distinct for every write, with alpha-renamed live-ins.

    This preserves repeated operands and use-after-redefinition relationships;
    regex blanking registers does neither. Boundaries are deliberately open:
    live-in equality across distinct machine registers cannot be proved here.
    """
    state, incoming, value_n, signature, binding = {}, {}, 0, [], []
    for row in block["instructions"]:
        reads, writes, known = _effects(row)
        for reg in reads:
            if reg not in state:
                state[reg] = f"in{len(incoming)}"
                incoming[reg] = state[reg]
        old = dict(state)
        uses = [(reg, old[reg]) for reg in reads]
        defs = []
        for reg in writes:
            state[reg] = f"v{value_n}"
            value_n += 1
            defs.append((reg, state[reg]))
        # Position in the operand matters, independently of unique read lists.
        normalized_args = []
        for i, operand in enumerate(row["operands"]):
            def subst(m):
                reg = _reg(m.group())
                if i == 0 and reg in writes and row["op"] not in STORES and not row["op"].startswith("stm"):
                    return state[reg]
                return old.get(reg, state.get(reg, "unknown"))
            normalized_args.append(REG.sub(subst, operand))
        if row["op"] in CONDITIONS | {"b"}:
            normalized_args = ["backedge" if row["target"] is not None and row["target"] <= row["offset"] else "forward"]
        signature.append([row["op"], normalized_args, [v for _, v in uses], [v for _, v in defs], known])
        binding.append({"offset": row["offset"], "reads": dict(uses), "writes": dict(defs)})
    return {"signature": signature, "fingerprint": _hash(signature), "live_ins": incoming,
            "live_out_values": state, "bindings": binding,
            "supported": all(s[-1] for s in signature)}


def _reaching(blocks):
    """Set-valued reaching definitions, including CFG joins and backedges."""
    pred = collections.defaultdict(list)
    for b in blocks:
        for edge in b["successors"]:
            pred[edge["block"]].append(b["id"])
    entry = {f"r{i}": {f"entry:r{i}"} for i in range(13)}
    entry.update({r: {f"entry:{r}"} for r in ("sp", "lr", "pc", "flags")})
    outgoing = {b["id"]: {} for b in blocks}
    incoming = {}
    changed = True
    iterations = 0
    while changed:
        changed, iterations = False, iterations + 1
        for bi, b in enumerate(blocks):
            state = collections.defaultdict(set)
            if bi == 0:
                for r, ds in entry.items():
                    state[r].update(ds)
            for p in pred[b["id"]]:
                for r, ds in outgoing[p].items():
                    state[r].update(ds)
            incoming[b["id"]] = {r: set(ds) for r, ds in state.items()}
            for row in b["instructions"]:
                for r in _effects(row)[1]:
                    state[r] = {f"{row['offset']:x}:{r}"}
            result = dict(state)
            if result != outgoing[b["id"]]:
                outgoing[b["id"]] = result
                changed = True
    defs, uses, ambiguities = {}, [], []
    for b in blocks:
        state = {r: set(ds) for r, ds in incoming[b["id"]].items()}
        for row in b["instructions"]:
            reads, writes, known = _effects(row)
            for r in reads:
                reaching = sorted(state.get(r, {f"unreached:{r}"}))
                use = {"offset": row["offset"], "register": r, "definitions": reaching}
                uses.append(use)
                if len(reaching) > 1:
                    ambiguities.append(use)
                for d in reaching:
                    defs.setdefault(d, {"id": d, "register": r, "uses": []})["uses"].append(row["offset"])
            for r in writes:
                key = f"{row['offset']:x}:{r}"
                defs.setdefault(key, {"id": key, "register": r, "offset": row["offset"], "uses": []})
                state[r] = {key}
    return {"definitions": list(defs.values()), "uses": uses, "ambiguous_uses": ambiguities,
            "fixed_point_iterations": iterations,
            "interpretation": "machine reaching definitions; reload, coalescing and register reuse prevent unique source-pseudo recovery"}


def fingerprint(rows, data_ranges=()):
    """Machine observations and dependency-sensitive signatures, JSON serializable."""
    rows = [_row(r, i) for i, r in enumerate(rows)]
    for row in rows:
        if any(lo <= row["offset"] < hi for lo, hi in data_ranges):
            row["data"] = True
    blocks = _blocks(rows)
    for b in blocks:
        b.update(_canonical(b))
    calls = [{"offset": r["offset"], "target": " ".join(r["operands"]), "kind": r["op"]}
             for r in rows if not r["data"] and r["op"] in CALLS]
    memory = [m for r in rows if (m := _memory(r))]
    constants = [{"offset": r["offset"], "value": int(m.group(1), 0)}
                 for r in rows if not r["data"] and r["op"] not in CONDITIONS | {"b", "bl", "blx"}
                 for m in re.finditer(r"[#=](-?(?:0x[0-9a-f]+|\d+))", r["text"], re.I)]
    control = [{"block": b["id"], "terminator": b["instructions"][-1]["op"], "successors": b["successors"]}
               for b in blocks]
    return {"instruction_count": sum(not r["data"] for r in rows), "blocks": blocks,
            "calls": calls, "memory": memory, "constants": constants, "control": control,
            "webs": _reaching(blocks), "fingerprint": _hash([b["signature"] for b in blocks]),
            "unknown_instructions": [{"offset": r["offset"], "text": r["text"]} for r in rows if not _effects(r)[2]]}


@functools.lru_cache(maxsize=64)
def _source_function_line(path, symbol, mtime_ns):
    text = Path(path).read_text(errors="replace")
    match = re.search(r"^" + re.escape(symbol) + r"\s*\(", text, re.M)
    return text.count("\n", 0, match.start()) + 1 if match else None


def compiler_references(category, compiler_root=None):
    """Resolve actual source function lines in the agbcc copy being executed."""
    root = Path(compiler_root) if compiler_root else ROOT / "build/scope/agbcc/gcc"
    if (root / "gcc").is_dir():
        root /= "gcc"
    out = []
    for filename, symbol in PASS_SOURCES.get(category, []):
        path = root / filename
        if not path.is_file():
            continue
        line = _source_function_line(str(path), symbol, path.stat().st_mtime_ns)
        if line is None:
            continue
        # Persist repo-relative links, never resolve a shared symlink to a home path.
        try:
            display = str(path.relative_to(ROOT))
        except ValueError:
            display = "agbcc/gcc/" + filename
        out.append({"file": display, "line": line, "symbol": symbol, "reference": f"{display}:{line}"})
    return out


def _source_facts(source, function):
    # This is source evidence and possible move sites, not a C semantic parser.
    clean = re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                   lambda m: "\n" * m.group().count("\n") + " " * (len(m.group()) - m.group().count("\n")), source, flags=re.S)
    m = re.search(r"\b" + re.escape(function) + r"\s*\([^;{}]*\)\s*\{", clean)
    if not m:
        return {"found": False, "sites": []}
    start, depth, end = m.start(), 1, m.end()
    while end < len(clean) and depth:
        depth += (clean[end] == "{") - (clean[end] == "}")
        end += 1
    body = clean[start:end]
    patterns = {"loop": r"\b(?:for|while|do)\b", "branch": r"\b(?:if|switch)\b",
                "comparison": r"(?:==|!=|<=|>=|<|>)", "return": r"\breturn\b", "goto": r"\bgoto\b",
                "field_access": r"(?:->|\.)\s*[A-Za-z_]\w*", "call": r"\b[A-Za-z_]\w*\s*\("}
    sites = []
    for kind, pattern in patterns.items():
        for site in re.finditer(pattern, body):
            token = site.group().strip()
            if kind == "call" and token[:-1].strip() in {"if", "for", "while", "switch", "sizeof", function}:
                continue
            sites.append({"kind": kind, "line": source.count("\n", 0, start + site.start()) + 1, "text": token})
    return {"found": True, "start_line": source.count("\n", 0, start) + 1,
            "end_line": source.count("\n", 0, end) + 1, "sites": sorted(sites, key=lambda s: (s["line"], s["kind"]))}


def _region_pairs(target, current):
    """Align blocks by dependency signatures, then pair unmatched gaps by skeleton.

    No instruction diff alignment is needed. Exact block signatures remain anchors
    when preceding blocks grow or registers change. Repeated blocks are ambiguous;
    the ordinal monotone alignment is retained and explicitly marked.
    """
    aa, bb = target["blocks"], current["blocks"]
    a, b = [x["fingerprint"] for x in aa], [x["fingerprint"] for x in bb]
    pairs = []
    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, a, b, autojunk=False).get_opcodes():
        if tag == "equal":
            pairs.extend((i1 + k, j1 + k) for k in range(i2 - i1))
        else:
            pairs.extend((i1 + k if i1 + k < i2 else None, j1 + k if j1 + k < j2 else None)
                         for k in range(max(i2 - i1, j2 - j1)))
    out = []
    for ai, bi in pairs:
        ta, cb = aa[ai] if ai is not None else None, bb[bi] if bi is not None else None
        exact_shape = bool(ta and cb and ta["signature"] == cb["signature"])
        if exact_shape:
            ta_text = [x["text"] for x in ta["instructions"]]
            cb_text = [x["text"] for x in cb["instructions"]]
            normalize_targets = lambda xs: [re.sub(r"func\+0x[0-9a-f]+", "func+TARGET", x, flags=re.I) for x in xs]
            binding = "same" if normalize_targets(ta_text) == normalize_targets(cb_text) else "different"
        else:
            binding = "unresolved"
        out.append({"target": ta["id"] if ta else None, "current": cb["id"] if cb else None,
                    "target_start": ta["start"] if ta else None, "current_start": cb["start"] if cb else None,
                    "local_shape": "same" if exact_shape else "different" if ta and cb else "missing",
                    "global_binding": binding,
                    "dependency_supported": bool(ta and cb and ta["supported"] and cb["supported"]),
                    "alignment_ambiguous": bool(ta and a.count(ta["fingerprint"]) > 1 or cb and b.count(cb["fingerprint"]) > 1),
                    "target_fingerprint": ta["fingerprint"] if ta else None,
                    "current_fingerprint": cb["fingerprint"] if cb else None,
                    "differences": _region_differences(ta, cb, exact_shape)})
    return out


def _region_differences(target, current, same_shape):
    """A small concrete witness, rather than forcing policy to compare hashes."""
    aa = target["instructions"] if target else []
    bb = current["instructions"] if current else []
    signatures_a = target["signature"] if target else []
    signatures_b = current["signature"] if current else []
    out = []
    if same_shape:
        pairs = [(i, i) for i in range(len(aa))
                 if re.sub(r"func\+0x[0-9a-f]+", "func+TARGET", aa[i]["text"], flags=re.I) !=
                    re.sub(r"func\+0x[0-9a-f]+", "func+TARGET", bb[i]["text"], flags=re.I)]
    else:
        akeys = [json.dumps(x, separators=(",", ":")) for x in signatures_a]
        bkeys = [json.dumps(x, separators=(",", ":")) for x in signatures_b]
        pairs = []
        for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, akeys, bkeys, autojunk=False).get_opcodes():
            if tag != "equal":
                pairs.extend((i1 + k if i1 + k < i2 else None, j1 + k if j1 + k < j2 else None)
                             for k in range(max(i2 - i1, j2 - j1)))
            if len(pairs) >= 4:
                break
    for i, j in pairs[:4]:
        a, b = aa[i] if i is not None else None, bb[j] if j is not None else None
        kind = "binding" if same_shape else "dependency" if a and b and a["op"] == b["op"] else "operation"
        out.append({"kind": kind,
                    "target": {"offset": a["offset"], "text": a["text"], "memory": _memory(a)} if a else None,
                    "current": {"offset": b["offset"], "text": b["text"], "memory": _memory(b)} if b else None,
                    "target_values": signatures_a[i][2:4] if i is not None else None,
                    "current_values": signatures_b[j][2:4] if j is not None else None})
    return out


def analyze(source, function, target_rows=(), current_rows=(), *, target_bytes=None, current_bytes=None,
            compiler_root=None, evaluation=None, target_data=(), current_data=()):
    """Compare decoded machine evidence with C move sites. Never compiles.

    target_data/current_data are offset ranges for pools/padding, including zero
    halfwords which scope.decode presents as movs r0,r0.
    target_rows/current_rows accept scope.decode's five-tuples, instruction strings,
    or dicts with offset/size/text/target. Byte identity is the sole exact verdict.
    evaluation may hold scope diff rows under `diff` to attach source lines and UIDs.
    """
    target, current = fingerprint(target_rows, target_data), fingerprint(current_rows, current_data)
    regions = _region_pairs(target, current)
    facts = _source_facts(source, function)
    same_calls = [x["target"] for x in target["calls"]] == [x["target"] for x in current["calls"]]
    # Base register assignment is a global binding choice. Width, signedness,
    # displacement and order are observed machine constraints independent of it.
    memkey = lambda m: (m["kind"], m.get("width"), m.get("signed"), m.get("displacement"), m.get("literal"))
    same_memory = [memkey(m) for m in target["memory"]] == [memkey(m) for m in current["memory"]]
    same_control = target["control"] == current["control"]
    same_constants = collections.Counter(x["value"] for x in target["constants"]) == collections.Counter(x["value"] for x in current["constants"])
    unknown = target["unknown_instructions"] + current["unknown_instructions"]
    actions = []

    def action(category, reason, moves, command):
        actions.append({"category": category, "reason": reason, "moves": list(dict.fromkeys(moves)),
                        "compiler": compiler_references(category, compiler_root), "diagnostic": command,
                        "confidence": "hypothesis; verify with trace or scope knob before attributing the compiler decision"})
    if not same_calls:
        action("control_flow", "Static call order/targets differ; recover the intended call/control skeleton first.",
               ["free-edit", "free-edit", "free-edit", "invert-if"], "calls")
    if not same_memory:
        action("expression", "Memory operation width, signedness, displacement or sequence differs.",
               ["free-edit", "introduce-temporary", "free-edit", "free-edit"], "why all")
    if not same_control:
        action("control_flow", "Basic-block edges or branch kinds differ; later branch offsets alone are not a cause.",
               ["invert-if", "free-edit", "while-to-for", "free-edit"], "probe")
    local_different = sum(r["local_shape"] != "same" for r in regions)
    binding_different = sum(r["local_shape"] == "same" and r["global_binding"] == "different" for r in regions)
    if local_different:
        action("expression", f"{local_different} region(s) have different dependency-sensitive instruction shape.",
               ["introduce-temporary", "free-edit", "commute", "independent-statements"], "why all")
        if any(s["kind"] == "loop" for s in facts["sites"]):
            action("loop", "A source loop exists and local shape differs; check induction/hoist decisions in that region.",
                   ["while-to-for", "introduce-temporary", "free-edit"], "giv")
    if binding_different:
        action("allocation", f"{binding_different} region(s) retain local dependencies with different hard-register bindings.",
               ["declaration-order", "free-edit", "commute", "introduce-temporary", "free-edit"], "life")
        action("reload", "Equal local shape does not determine whether global allocation or reload chose the binding.",
               ["free-edit", "introduce-temporary", "free-edit"], "spill")
    if unknown:
        action("expression", "Some opcodes are conservatively modeled; inspect those instructions before trusting local shape.",
               ["free-edit"], "why all")
    diff = (evaluation or {}).get("diff", [])
    evidence = [{k: r[k] for k in ("line", "uid", "pass", "rom", "draft", "tag") if k in r}
                for r in diff if isinstance(r, dict) and r.get("tag") != "="][:32]
    byte_known = target_bytes is not None and current_bytes is not None
    return {"schema": 1, "function": function, "exact": target_bytes == current_bytes if byte_known else None,
            "target": target, "current": current, "regions": regions, "source": facts,
            "constraints": {"call_order": {"same": same_calls, "status": "observed-static-machine", "target": target["calls"]},
                            "memory_accesses": {"same": same_memory, "status": "observed-machine; object identity unresolved", "target": target["memory"]},
                            "control_flow": {"same": same_control, "status": "observed-static-CFG", "target": target["control"]},
                            "constants": {"same": same_constants, "status": "observed-machine-immediates; includes address displacements", "target": target["constants"]}},
            "summary": {"regions": len(regions), "local_shape_same": sum(r["local_shape"] == "same" for r in regions),
                        "binding_different": binding_different, "unknown_instructions": len(unknown)},
            "actions": actions, "difference_evidence": evidence,
            "degrees_of_freedom": sorted({m for a in actions for m in a["moves"]}),
            "limits": ["Machine evidence constrains behavior, not unique C syntax, types or variables.",
                       "Local alpha equivalence preserves modeled dependencies; it is not semantic equivalence or byte identity.",
                       "Hard-register reuse, spills and coalescing make source-pseudo reconstruction non-unique.",
                       "Call arguments conservatively read r0-r3; call signatures and indirect targets are unresolved.",
                       "Memory observations are syntactic; aliasing, volatile ordering and path conditions require semantic checking."]}

"""Call-anchored regions and explicit value obligations at their boundaries.

These reports test correspondence hypotheses. They are not equivalence proofs:
call anchors and instruction pairing remain heuristic, and memory alias/effect
assumptions are those recorded by the underlying Analysis. Whole-function byte
identity is still the acceptance test.
"""
from __future__ import annotations

from collections import Counter, defaultdict, deque
from typing import Any

from .constraints import Analysis, _align, _ref_index


def _align_region(a, b, *, cell_limit: int = 250_000) -> tuple[list[tuple], str]:
    """Minimum edit-cost correspondence inside one call interval.

    Greedy longest-subsequence anchors can jump to another repeated store
    sequence when a few constants change. Bounded dynamic programming pairs
    identical shapes at cost zero, changed scalar operands at cost one, and
    insertions/deletions at cost one. Different operation shapes cannot pair.
    Ties are deterministic, not evidence of a unique inverse.
    """
    n, m = len(a), len(b)
    if n * m > cell_limit:
        return _align(a, b), "greedy-fallback"
    width = m + 1
    steps = bytearray((n + 1) * width)
    previous = list(range(width))
    for j in range(1, width):
        steps[j] = 2
    for i, left in enumerate(a, 1):
        current = [i] + [0] * m
        steps[i * width] = 1
        for j, right in enumerate(b, 1):
            remove, insert = previous[j] + 1, current[j - 1] + 1
            pair = (previous[j - 1] + (left.shape != right.shape)
                    if left.relaxed_shape == right.relaxed_shape else n + m + 1)
            if pair <= remove and pair <= insert:
                current[j] = pair
            elif remove <= insert:
                current[j], steps[i * width + j] = remove, 1
            else:
                current[j], steps[i * width + j] = insert, 2
        previous = current
    pairs, i, j = [], n, m
    while i or j:
        direction = steps[i * width + j]
        if direction == 0:
            i, j = i - 1, j - 1
            pairs.append((i, j))
        elif direction == 1:
            i -= 1
            pairs.append((i, None))
        else:
            j -= 1
            pairs.append((None, j))
    pairs.reverse()
    return pairs, "bounded-minimum-edit"


def _direct_calls(analysis: Analysis) -> list[tuple[int, str]]:
    return [(i.index, i.callee) for i in analysis.instructions
            if i.control == "call" and i.callee and "func+" not in i.callee
            and not any(operand.role == "callee" for operand in i.uses)]


def align_regions(target: Analysis, candidate: Analysis) -> tuple[list[tuple], list[dict[str, Any]], dict[str, Any]]:
    """Align within ordered external-call anchors; never cross an anchor.

    Local BL long jumps are control edges, not ABI-call landmarks. Repeated
    callees remain explicitly ambiguous even when their sequence agrees.
    """
    from difflib import SequenceMatcher
    left, right = _direct_calls(target), _direct_calls(candidate)
    matcher = SequenceMatcher(None, [name for _, name in left], [name for _, name in right], autojunk=False)
    anchors = [(left[block.a + k][0], right[block.b + k][0])
               for block in matcher.get_matching_blocks() for k in range(block.size)]
    pairs, regions, methods = [], [], Counter()
    a0 = b0 = 0
    for ordinal, (a1, b1) in enumerate(anchors + [(len(target.instructions), len(candidate.instructions))]):
        has_call = ordinal < len(anchors)
        local, method = _align_region(target.instructions[a0:a1], candidate.instructions[b0:b1])
        methods[method] += 1
        pairs.extend((x + a0 if x is not None else None, y + b0 if y is not None else None)
                     for x, y in local)
        if has_call:
            pairs.append((a1, b1))
        end_a, end_b = a1 + int(has_call), b1 + int(has_call)
        if end_a > a0 or end_b > b0:
            regions.append({"id": f"call:{ordinal}" if has_call else "tail",
                            "target_indices": (a0, end_a), "candidate_indices": (b0, end_b),
                            "callee": target.instructions[a1].callee if has_call else None,
                            "instruction_alignment": method,
                            "target_call_offset": target.instructions[a1].offset if has_call else None,
                            "candidate_call_offset": candidate.instructions[b1].offset if has_call else None})
        a0, b0 = end_a, end_b
    counts = Counter(name for _, name in left)
    return pairs, regions, {"kind": "ordered-external-call-anchors", "target_calls": len(left),
        "candidate_calls": len(right), "matched_calls": len(anchors),
        "instruction_methods": dict(methods),
        "ambiguity": "Repeated anchors and tied minimum-cost paths are heuristic; dependency and control obligations remain necessary.",
        "repeated_callees": sum(count > 1 for count in counts.values()),
        "call_sequence_equal": [n for _, n in left] == [n for _, n in right],
        "proof": False}


def _dependencies(analysis: Analysis, transported: bool) -> tuple:
    value = getattr(analysis, "transport_dependencies", ()) if transported else ()
    return value or analysis.dependencies


def _transport_provenance(analysis: Analysis, instruction: int, role: str) -> list[dict[str, Any]]:
    """Direct reload aliases used by one obligation, including its assumptions."""
    aliases = getattr(analysis, "stack_transport", {}).get("value_aliases", {})
    return [{"definition": reference, **aliases[reference]}
            for reference in analysis.dependencies[instruction].get(role, ()) if reference in aliases]


def _canonical(reference: str, inverse: dict[int, int]) -> str:
    index = _ref_index(reference)
    if index is None:
        return reference
    if index not in inverse:
        return "unmapped:" + reference
    return f"n{inverse[index]}:{reference.split(':', 1)[1]}"


def _relation(target: Analysis, candidate: Analysis, pairs: list[tuple], *, transported: bool) -> dict[str, Any]:
    """A conservative structural relation with bad/unknown producers propagated.

    This catches identical region instructions fed by different constants or
    computations outside the region. Cyclic pairs are coinductive graph
    correspondence, not a claim that arbitrary loops are semantically equal.
    """
    mapping = {x: y for x, y in pairs if x is not None and y is not None}
    inverse = {y: x for x, y in mapping.items()}
    a, b = target.instructions, candidate.instructions
    da, db = _dependencies(target, transported), _dependencies(candidate, transported)
    a_at, b_at = {i.offset: i.index for i in a}, {i.offset: i.index for i in b}
    reachable_a = {i for block in target.blocks if block.reachable for i in block.instructions}
    reachable_b = {i for block in candidate.blocks if block.reachable for i in block.instructions}
    # 0=corresponding; 1=unresolved; 2=different under this instruction pairing.
    state = {x: 0 for x in mapping}
    reasons: dict[int, list[str]] = defaultdict(list)
    followers: dict[int, set[int]] = defaultdict(set)
    for x, y in mapping.items():
        left, right = a[x], b[y]
        if not left.supported or not right.supported or x not in reachable_a or y not in reachable_b:
            state[x] = 1
            reasons[x].append("unsupported or unresolved reachability")
        if left.shape != right.shape:
            state[x] = 2
            reasons[x].append("producer operation or scalar operand differs")
        if left.control != right.control:
            state[x] = 2
            reasons[x].append("control kind differs")
        if left.control in ("branch", "conditional"):
            ta, tb = a_at.get(left.target), b_at.get(right.target)
            if ta is None or tb is None or ta not in mapping:
                state[x] = max(state[x], 1)
                reasons[x].append("unmapped control destination")
            elif mapping[ta] != tb:
                state[x] = 2
                reasons[x].append("control destination differs")
        elif left.target is not None or right.target is not None:
            # scope normalizes pointers into this function to LOCAL just as it
            # normalizes branch labels. Their pointed-to instruction must also
            # correspond. A literal/data address has no instruction mapping;
            # equal spelling or an equal relative offset is insufficient to
            # establish the pointed-to data's correspondence.
            ta, tb = a_at.get(left.target), b_at.get(right.target)
            if ta is None or tb is None or ta not in mapping:
                state[x] = max(state[x], 1)
                reasons[x].append("unmapped local address destination")
            elif mapping[ta] != tb:
                state[x] = 2
                reasons[x].append("local address destination differs")
        if x in target.jump_tables or y in candidate.jump_tables:
            expected = tuple(mapping.get(a_at[t]) for t in target.jump_tables.get(x, ()))
            actual = tuple(b_at[t] for t in candidate.jump_tables.get(y, ()))
            if None in expected:
                state[x] = max(state[x], 1)
            elif expected != actual:
                state[x] = 2
                reasons[x].append("switch destinations differ")
        for role in da[x].keys() | db[y].keys():
            expected = set(da[x].get(role, ()))
            actual = {_canonical(ref, inverse) for ref in db[y].get(role, ())}
            if any(ref.startswith("unmapped:") for ref in actual):
                state[x] = max(state[x], 1)
                reasons[x].append(f"unmapped producer for {role}")
            elif expected != actual:
                state[x] = 2
                reasons[x].append(f"producer set differs for {role}")
            for reference in expected:
                producer = _ref_index(reference)
                if producer is not None:
                    if producer in mapping:
                        followers[producer].add(x)
                    else:
                        state[x] = max(state[x], 1)
    queue = deque(x for x, value in state.items() if value)
    while queue:
        producer = queue.popleft()
        for consumer in followers[producer]:
            if state[consumer] < state[producer]:
                state[consumer] = state[producer]
                if not reasons[consumer]:
                    reasons[consumer].append(f"upstream producer at +0x{a[producer].offset:X} does not correspond")
                queue.append(consumer)
    return {"mapping": mapping, "inverse": inverse, "state": state, "reasons": reasons,
            "target_dependencies": da, "candidate_dependencies": db}


def _value_status(expected: tuple, actual: tuple, relation: dict[str, Any]) -> tuple[str, list[str]]:
    converted = {_canonical(ref, relation["inverse"]) for ref in actual}
    wanted = set(expected)
    if any(ref.startswith("unmapped:") for ref in converted):
        return "unresolved", ["candidate producer has no instruction correspondence"]
    if wanted != converted:
        return "different", ["reaching value sets differ under the selected correspondence"]
    states = [relation["state"].get(_ref_index(ref), 1) for ref in wanted if _ref_index(ref) is not None]
    if 2 in states:
        return "different", ["corresponding producer has different upstream constraints"]
    if 1 in states:
        return "unresolved", ["upstream producer correspondence is unresolved"]
    return "corresponding", []


def _range(analysis: Analysis, bounds: tuple[int, int]) -> list[int] | None:
    start, end = bounds
    if start == end:
        return None
    return [analysis.instructions[start].offset,
            analysis.instructions[end - 1].offset + analysis.instructions[end - 1].size]


def _boundary_rows(target: Analysis, candidate: Analysis, region: dict, relation: dict,
                   *, detail_limit: int) -> tuple[dict, list[dict], dict, list[dict]]:
    a0, a1 = region["target_indices"]
    b0, b1 = region["candidate_indices"]
    a, b = target.instructions, candidate.instructions
    da, db = relation["target_dependencies"], relation["candidate_dependencies"]
    inputs, outputs = [], []
    input_counts, output_counts = Counter(), Counter()
    for x in range(a0, a1):
        y = relation["mapping"].get(x)
        if y is None or not b0 <= y < b1:
            continue
        for role in da[x].keys() | db[y].keys():
            left, right = da[x].get(role, ()), db[y].get(role, ())
            external = any(_ref_index(ref) is None or not a0 <= _ref_index(ref) < a1 for ref in left)
            external |= any(_ref_index(ref) is None or not b0 <= _ref_index(ref) < b1 for ref in right)
            if not external:
                continue
            status, reason = _value_status(left, right, relation)
            input_counts[status] += 1
            inputs.append({"target_offset": a[x].offset, "candidate_offset": b[y].offset,
                "role": role, "status": status, "reason": reason,
                "target_values": list(left), "candidate_values": list(right),
                "target_transport": _transport_provenance(target, x, role),
                "candidate_transport": _transport_provenance(candidate, y, role),
                "target_register": next((o.register for o in a[x].uses if o.role == role), None),
                "candidate_register": next((o.register for o in b[y].uses if o.role == role), None)})
    # Values defined in this region and consumed outside it form output obligations.
    for x, deps in enumerate(da):
        if a0 <= x < a1:
            continue
        y = relation["mapping"].get(x)
        for role, refs in deps.items():
            if not any(_ref_index(ref) is not None and a0 <= _ref_index(ref) < a1 for ref in refs):
                continue
            if y is None:
                status, reason, candidate_values = "unresolved", ["outside consumer is not aligned"], ()
            else:
                candidate_values = db[y].get(role, ())
                status, reason = _value_status(refs, candidate_values, relation)
            output_counts[status] += 1
            outputs.append({"target_consumer_offset": a[x].offset,
                "candidate_consumer_offset": b[y].offset if y is not None else None,
                "role": role, "status": status, "reason": reason,
                "target_values": list(refs), "candidate_values": list(candidate_values),
                "target_transport": _transport_provenance(target, x, role),
                "candidate_transport": _transport_provenance(candidate, y, role) if y is not None else []})
    order = {"different": 0, "unresolved": 1, "corresponding": 2}
    inputs.sort(key=lambda row: (order[row["status"]], row["target_offset"], row["role"]))
    outputs.sort(key=lambda row: (order[row["status"]], row["target_consumer_offset"], row["role"]))
    return dict(input_counts), inputs[:detail_limit], dict(output_counts), outputs[:detail_limit]


def compare_boundaries(target: Analysis, candidate: Analysis, *, region: str | None = None,
                       limit: int = 12, detail_limit: int = 8, include_equal: bool = False) -> dict[str, Any]:
    """Bounded reports for model-selected whole-function source experiments."""
    if limit < 1 or detail_limit < 0:
        raise ValueError("limit must be positive and detail_limit nonnegative")
    pairs, regions, alignment = align_regions(target, candidate)
    raw = _relation(target, candidate, pairs, transported=False)
    transported = _relation(target, candidate, pairs, transported=True)
    reports = []
    for item in regions:
        a0, a1 = item["target_indices"]
        b0, b1 = item["candidate_indices"]
        local = [(x, y) for x, y in pairs if (x is not None and a0 <= x < a1)
                 or (y is not None and b0 <= y < b1)]
        missing = sum(x is None or y is None for x, y in local)
        changed = sum(target.instructions[x].shape != candidate.instructions[y].shape
                      for x, y in local if x is not None and y is not None)
        physical = Counter()
        for x, y in local:
            if x is None or y is None:
                continue
            for role in target.dependencies[x].keys() | candidate.dependencies[y].keys():
                status, _ = _value_status(target.dependencies[x].get(role, ()),
                    candidate.dependencies[y].get(role, ()), raw)
                physical[status] += 1
        inputs, in_details, outputs, out_details = _boundary_rows(
            target, candidate, item, transported, detail_limit=detail_limit)
        # Boundary values alone do not establish correspondence inside a
        # region. A changed operand order or branch destination may affect
        # only internal consumers (including the final return), so it has no
        # crossing obligation. Keep that contradiction visible. Use the
        # transported relation here: physical reload definitions intentionally
        # remain separate from justified stack-value forwarding.
        states = Counter({0: "corresponding", 1: "unresolved", 2: "different"}[
            transported["state"].get(x, 1)] for x, y in local
            if x is not None and y is not None)
        complete = missing == 0 and changed == 0
        known = complete and not (states.get("different") or states.get("unresolved")
                                  or inputs.get("different") or inputs.get("unresolved")
                                  or outputs.get("different") or outputs.get("unresolved"))
        reports.append({**item, "target_range": _range(target, (a0, a1)),
            "candidate_range": _range(candidate, (b0, b1)),
            "target_instructions": a1 - a0, "candidate_instructions": b1 - b0,
            "unmatched_instructions": missing, "changed_operands": changed,
            "physical_constraints": dict(physical), "transported_inputs": inputs,
            "instruction_correspondence": dict(states),
            "transported_outputs": outputs, "input_obligations": in_details,
            "output_obligations": out_details,
            "status": "corresponding-under-alignment" if known else "open",
            "proof": False})
    totals = Counter(row["status"] for row in reports)
    total_changes = {"unmatched_instructions": sum(r["unmatched_instructions"] for r in reports),
                     "changed_operands": sum(r["changed_operands"] for r in reports)}
    if region is not None:
        selected = [row for row in reports if row["id"] == region]
        if not selected:
            raise KeyError(f"unknown region {region!r}")
    else:
        selected = [r for r in reports if include_equal or r["status"] == "open"]
        selected.sort(key=lambda r: (-(r["unmatched_instructions"] + r["changed_operands"]),
            -r["transported_inputs"].get("different", 0), r["target_indices"][0]))
        selected = selected[:limit]
    target_stack = getattr(target, "stack_transport", {})
    candidate_stack = getattr(candidate, "stack_transport", {})
    policies = {"target": target_stack.get("policy", "unavailable"),
                "candidate": candidate_stack.get("policy", "unavailable")}
    return {"alignment": alignment, "region_count": len(reports), "region_summary": dict(totals),
            "structural_changes": total_changes, "regions": selected,
            "stack_policy": policies["target"] if policies["target"] == policies["candidate"] else "mixed",
            "stack_policies": policies,
            "target_stack_stats": target_stack.get("stats", {}),
            "candidate_stack_stats": candidate_stack.get("stats", {}),
            "target_stack_warnings": target_stack.get("warnings", []) + target_stack.get("cfg_warnings", []),
            "candidate_stack_warnings": candidate_stack.get("warnings", []) + candidate_stack.get("cfg_warnings", []),
            "assumptions": list(dict.fromkeys((*target.assumptions, *candidate.assumptions,
                *target_stack.get("assumptions", []), *candidate_stack.get("assumptions", [])))),
            "interpretation": "Boundary obligations under heuristic call/instruction correspondence; physical memory order remains visible; no semantic pruning or equivalence proof.",
            "proof": False}

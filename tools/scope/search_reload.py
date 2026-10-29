"""Reload demand from existing diagnostic dumps, without another compile.

The native greg dump records per-insn scratch register reservations. R/X records
show successful choices and overlap exclusions, while S records prove which
pseudos finally acquired stack slots. These are different observations: neither
reservation count nor number of choices is an exact simultaneous reload count.
Missing iteration/victim records are exposed rather than inferred as fact.
"""
from __future__ import annotations

import collections
import hashlib
from pathlib import Path
import re

IMPLEMENTATION_ID = hashlib.sha256(Path(__file__).read_bytes()).hexdigest()
ROOT = Path(__file__).resolve().parents[2]
REG = r"\(reg(?:/\w+)*:\w+\s+(\d+)(?:\s+[^()]*)?\)"


def _read(path):
    return path.read_text(errors="replace") if path.is_file() else ""


def _reference(root, filename, symbol, needle=None):
    path = root / filename
    text = _read(path)
    pattern = (r"^" + re.escape(symbol) + r"\s*\(") if needle is None else re.escape(needle)
    hit = re.search(pattern, text, re.M)
    line = text.count("\n", 0, hit.start()) + 1 if hit else None
    try:
        display = str(path.relative_to(ROOT))
    except ValueError:
        display = "agbcc/gcc/" + filename
    return {"file": display, "line": line, "symbol": symbol,
            "reference": f"{display}:{line}" if line is not None else display}


def _reservations(text):
    """Native find_reload_regs scans in dump order; repeated scans stay separate."""
    out, current = [], None
    for line in text.splitlines():
        match = re.fullmatch(r"Spilling for insn (\d+)\.", line.strip())
        if match:
            current = {"scan": len(out), "uid": int(match[1]), "registers": []}
            out.append(current)
            continue
        match = re.fullmatch(r"Spilling reg (\d+)\.", line.strip())
        if match and current is not None:
            current["registers"].append(int(match[1]))
    return out


def _liveness(text):
    """Reconstruct lreg lifetimes; never confuse this with reload chain bitsets.

    Valid live-in headers and REG_DEAD/REG_UNUSED notes are actual dump facts;
    propagating them through instructions is a correspondence used to suggest
    eviction candidates. Reload iterations may already have changed that state.
    """
    events = re.finditer(r"^;; Start of basic block[^\n]*|^;; End of basic block[^\n]*|"
                         r"^\((?:insn|jump_insn|call_insn)(?:/\w+)* \d+ .*?(?=^\(|^;;|\Z)",
                         text, re.M | re.S)
    live, available, out = set(), False, {}
    for event in events:
        value = event.group()
        if value.startswith(";; Start"):
            marker = value.partition("registers live:")[2]
            marker = re.sub(r"\[[^\]]*\]", "", marker)
            live = {int(n) for n in re.findall(r"\b\d+\b", marker)}
            available = "registers live:" in value
            continue
        if value.startswith(";; End"):
            available = False
            continue
        if not available:
            continue
        uid = int(re.match(r"\([^ ]+ (\d+)", value)[1])
        before = set(live)
        definitions = {int(n) for n in re.findall(r"\(set\s+(?:\(subreg:\w+\s+)?" + REG, value)}
        deaths = {int(n) for n in re.findall(r"\(expr_list:REG_(?:DEAD|UNUSED)\s+" + REG, value)}
        live.update(definitions)
        live.difference_update(deaths)
        out[uid] = {"before": before, "after": set(live)}
    return out


def _rtl_shape(rtl):
    # Counts describe RTL syntax only, not the target's constraint/reload analysis.
    return {"memory_operands": len(re.findall(r"\(mem(?:/\w+)*:", rtl)),
            "distinct_pseudos": sorted({int(n) for n in re.findall(REG, rtl) if int(n) >= 16}),
            "modes": sorted(set(re.findall(r"\((?:reg|mem|subreg)(?:/\w+)*:(\w+)", rtl))),
            "constants": [int(n) for n in re.findall(r"\(const_int (-?\d+)", rtl)],
            "evidence": "pre-reload RTL syntax; not a count of required reload registers"}


def _patterns(text, compiler):
    """Instruction recognition names plus the real target constraints, not guesses."""
    machine = compiler / "thumb.md"
    description = _read(machine)
    found, cache = {}, {}
    for event in re.finditer(r"^\((?:insn|jump_insn|call_insn)(?:/\w+)* (\d+) .*?(?=^\(|\Z)",
                             text, re.M | re.S):
        recognized = re.search(r"\d+ \{([^}]+)\}", event.group())
        if not recognized:
            continue
        name = recognized[1]
        if name not in cache:
            pattern = re.search(r'\(define_(?:insn|expand) "' + re.escape(name) + r'"', description)
            if not pattern:
                cache[name] = {"name": name, "compiler": None, "constraints": []}
            else:
                finish = re.search(r"^\(define_", description[pattern.end():], re.M)
                section = description[pattern.start():pattern.end() + finish.start()] if finish else description[pattern.start():]
                constraints = [{"kind": "operand", "mode": m[1], "operand": int(m[2]),
                                "predicate": m[3], "constraint": m[4]}
                               for m in re.finditer(r'\(match_operand:(\w+)\s+(\d+)\s+"([^"]*)"\s+"([^"]*)"', section)]
                constraints += [{"kind": "scratch", "mode": m[1], "operand": int(m[2]), "constraint": m[3]}
                                for m in re.finditer(r'\(match_scratch:(\w+)\s+(\d+)\s+"([^"]*)"', section)]
                cache[name] = {"name": name, "compiler": _reference(compiler, "thumb.md", name, pattern[0]),
                               "constraints": sorted(constraints, key=lambda c: c["operand"]),
                               "evidence": "Recognized machine pattern and its alternatives; the chosen alternative is not recorded."}
        found[int(event[1])] = cache[name]
    return found


def _target_register_use(scope, function, dump_function):
    try:
        _, addresses, sizes = scope.inventory()
        name = next((n for n in (function, dump_function) if n in addresses), None)
        if name is None and re.fullmatch(r"sub_[0-9A-Fa-f]{8}", function):
            address = int(function[4:], 16)
            name = next((n for n, a in addresses.items() if a == address), None)
        if name is None:
            return {"available": False, "reason": "ROM name could not be resolved from this trace"}
        address = addresses[name]
        target = scope.rom_bytes()[scope.off(address):scope.off(address) + sizes[name]]
        rows = scope.decode(target, address, scope.rom_data(name, address, target))
        code = [row for row in rows if not row[2].lstrip().startswith(".")]
        used, excluded = set(), []
        for index, row in enumerate(code):
            text = row[2].strip()
            save_move = index < 9 and re.fullmatch(r"mov r[4-7], (?:r8|sb|sl|fp)", text)
            restore_move = index >= len(code) - 12 and re.fullmatch(r"mov (?:r8|sb|sl|fp), r[3-7]", text)
            if text.startswith(("push ", "pop ")) or save_move or restore_move:
                excluded.append(row[0])
                continue
            used.update(r for r in scope.reg_nums(text) if r < 8)
        return {"available": True, "body_low_registers_used": sorted(used),
                "body_low_registers_not_seen": sorted(set(range(8)) - used),
                "excluded_save_restore_offsets": excluded,
                "evidence": "ROM instruction operands excluding recognized prologue/epilogue saves; no original compiler trace",
                "interpretation": "An unused ROM low register does not prove it was in the original spill set or identify an evicted value."}
    except (KeyError, ValueError, OSError, AttributeError) as error:
        return {"available": False, "reason": str(error)}


def analyze(scope, wd, function):
    """Return strongest supported demand/eviction evidence from existing dumps.

    The caller first collects one diagnostic compile (e.g. search_trace.collect).
    This function does no compilation, subprocess work, or mutation.
    """
    wd = Path(wd)
    compiler = Path(scope.INSTR).parent
    refs = {symbol: _reference(compiler, "reload1.c", symbol) for symbol in (
        "calculate_needs_all_insns", "calculate_needs", "find_reload_regs", "new_spill_reg",
        "maybe_mark_pseudo_spilled", "finish_spills", "allocate_reload_reg", "alter_reg")}
    refs["victim_mark"] = _reference(compiler, "reload1.c", "maybe_mark_pseudo_spilled",
                                     "SET_REGNO_REG_SET (spilled_pseudos, reg);")
    refs["need_before_mutation"] = _reference(compiler, "reload1.c", "find_reload_regs",
                                             "short *group_needs = chain->need.groups;")
    dump_function = scope.dump_func(str(wd), function)
    names = scope.pseudo_names(str(wd), function)
    lines = scope.uid_lines(str(wd))
    allocations = scope.alloc_records(str(wd), function)
    lreg_text = _read(wd / "u.i.lreg")
    lifetime = _liveness(lreg_text)
    pre = {}
    for pass_name in ("lreg", "regmove", "combine", "flow", "loop", "rtl"):
        patterns = _patterns(_read(wd / ("u.i." + pass_name)), compiler)
        for uid, kind, rtl in scope.insn_list(str(wd), pass_name):
            pre.setdefault(uid, {"pass": pass_name, "kind": kind, "rtl": rtl,
                                 "machine_pattern": patterns.get(uid)})
    source_lines = scope.source_line_map(_read(wd / "u.i").splitlines())
    reservations = _reservations(_read(wd / "u.i.greg"))
    seen, growth = set(), []
    per_uid = collections.defaultdict(list)
    for event in reservations:
        uid = event["uid"]
        event["line"] = lines.get(uid)
        event["source"] = source_lines.get(lines.get(uid))
        event["reservation_count"] = len(set(event["registers"]))
        event["pre_reload"] = pre.get(uid)
        event["rtl_shape"] = _rtl_shape(pre[uid]["rtl"]) if uid in pre else None
        additions = sorted(set(event["registers"]) - seen)
        if additions:
            growth.append({"scan": event["scan"], "uid": uid, "line": event["line"],
                           "first_observed_registers": additions})
            seen.update(additions)
        per_uid[uid].append(event)

    choices = collections.defaultdict(list)
    states = scope.spill_states(str(wd), function)
    for choice, uid, reg, line, label in scope.reload_records(str(wd), function):
        state = states.get(choice)
        taken = sorted(r for r, status in state["st"].items() if status == "T") if state else []
        blockers = [{"register": r, "pseudo": int(status[1:]), "name": names.get(int(status[1:]))}
                    for r, status in (state["st"].items() if state else []) if re.fullmatch(r"L\d+", status)]
        choices[uid].append({"choice": choice, "register": reg, "line": line or lines.get(uid),
            "value": label, "overlapping_taken_registers": taken,
            "overlap_lower_bound": len(set(taken) | {reg}) if state else None,
            "live_pseudo_blockers": blockers, "rotation": state,
            "evidence": "R/X successful allocation and overlapping-period exclusions; lower bound, not total demand"})
    inherited = collections.defaultdict(list)
    for uid, pseudo, spill_index, from_uid, kind in scope.inherit_records(str(wd), function):
        inherited[uid].append({"pseudo": pseudo, "name": names.get(pseudo), "spill_index": spill_index,
                               "from_uid": from_uid, "kind": kind})
    insns = []
    for uid in dict.fromkeys([e["uid"] for e in reservations] + list(choices) + list(inherited)):
        records = choices.get(uid, [])
        line = next((r["line"] for r in records if r["line"]), lines.get(uid))
        insns.append({"uid": uid, "line": line, "expansion_line": lines.get(uid),
            "source": source_lines.get(line), "reservation_scans": [e["scan"] for e in per_uid[uid]],
            "reserved_registers_by_scan": [e["registers"] for e in per_uid[uid]],
            "max_reserved_register_count": max((e["reservation_count"] for e in per_uid[uid]), default=0),
            "successful_choices": len(records),
            "overlap_lower_bound": max((r["overlap_lower_bound"] or 0 for r in records), default=0),
            "choices": records, "inheritance": inherited.get(uid, []), "pre_reload": pre.get(uid),
            "rtl_shape": _rtl_shape(pre[uid]["rtl"]) if uid in pre else None})

    evictions, unallocated = [], []
    # Distinct pseudos can reuse a stack offset: preserve every S record rather
    # than reducing them to scope.frame_slots' display mapping by offset.
    slots = []
    for line in _read(wd / "names.txt").splitlines():
        fields = line.split()
        if len(fields) == 7 and fields[:2] == ["S", dump_function]:
            pseudo, offset, size, order, from_reg = map(int, fields[2:])
            slots.append((offset, (pseudo, size, order, from_reg)))
    if not slots:
        slots = list(scope.frame_slots(str(wd), function).items())
    for offset, (pseudo, size, order, from_reg) in sorted(slots, key=lambda item: item[1][2]):
        allocation = allocations.get(pseudo)
        item = {"pseudo": pseudo, "name": names.get(pseudo), "slot_offset": offset,
                "slot_bytes": size, "slot_order": order, "spilled_from_register": from_reg,
                "slot_kind": "reload_assigned" if order > 0 else "incoming_or_preexisting_stack",
                "from_register_valid": order > 0,
                "pre_reload_allocator": allocation[0] if allocation else None,
                "pre_reload_hard_register": allocation[6] if allocation else None,
                "evidence": "S final stack-slot/from-register record"}
        # S order -1 denotes an already-stack parameter. Its from-register cell
        # was never set by alter_reg and can contain the static array's zero or
        # an earlier function's value; it is not evidence of a register eviction.
        if order > 0 and from_reg >= 0:
            occupied = set(range(from_reg, from_reg + max(1, (size + 3) // 4)))
            item["spilled_from_registers"] = sorted(occupied)
            candidates = []
            for event in reservations:
                overlap = sorted(occupied & set(event["registers"]))
                if not overlap:
                    continue
                state = lifetime.get(event["uid"])
                live = state is not None and pseudo in state["before"] | state["after"]
                if live:
                    candidates.append({"scan": event["scan"], "uid": event["uid"], "line": event["line"],
                        "source": event["source"], "selected_registers": overlap,
                        "evidence": "inferred correspondence: reservation uses S's register and lreg lifetime contains this pseudo; reload iteration chain is not recorded"})
            item["candidate_selection_events"] = candidates
            item["earliest_candidate_selection"] = candidates[0] if candidates else None
            item["cause_proved"] = False
            evictions.append(item)
        else:
            unallocated.append(item)
    final_regs = sorted({r for state in states.values() for r in state["regs"]})
    target = _target_register_use(scope, function, dump_function)
    ranked = sorted(insns, key=lambda item: (-item["overlap_lower_bound"], -item["max_reserved_register_count"], item["uid"]))
    evidence_missing = []
    if not reservations:
        evidence_missing.append("Native greg per-instruction reservation records are missing.")
    if not states:
        evidence_missing.append("R/X allocation/overlap records are missing; simultaneous demand cannot be bounded.")
    if not lifetime:
        evidence_missing.append("lreg block live-ins/REG_DEAD notes are missing; eviction candidates cannot be localized.")
    return {"schema": 1, "function": function, "dump_function": dump_function,
            "summary": {"reservation_scans": len(reservations), "instructions_with_reload_evidence": len(insns),
                "successful_reload_choices": sum(len(v) for v in choices.values()),
                "max_observed_overlap_lower_bound": max((i["overlap_lower_bound"] for i in insns), default=0),
                "max_per_insn_reserved_registers": max((i["max_reserved_register_count"] for i in insns), default=0),
                "final_spill_registers": final_regs, "confirmed_stack_evictions": len(evictions),
                "stack_slots_without_a_hard_register": len(unallocated)},
            "pressure_points": ranked[:12], "instructions": insns, "reservation_scans": reservations,
            "observed_selection_union_growth": growth, "evictions": evictions,
            "unallocated_stack_slots": unallocated, "target_register_observation": target,
            "compiler": refs,
            "missing_instrumentation": {"exact_need_vectors": {
                "available": False, "record_at": refs["need_before_mutation"],
                "fields": ["iteration", "uid", "chain->need.regs", "chain->need.groups", "chain->group_size"],
                "reason": "Reservation logs are after class/group demand reduction and do not preserve original demand vectors."},
                "exact_victim_event": {"available": False, "record_at": refs["victim_mark"],
                    "fields": ["iteration", "uid", "pseudo", "old_hard_register", "selected_spill_register", "live_before/live_after"],
                    "reason": "S records prove final eviction but not the instruction/iteration that first marked its pseudo spilled."}},
            "evidence_missing": evidence_missing,
            "limits": ["No extra compile was run; these records describe the caller's existing diagnostic compile.",
                "Diagnostic UID/RTL records normally come from -g and must be compared with the build before treating a counterfactual as the build's behavior.",
                "Successful choice count is not simultaneous scratch demand; inherited/shared/overlapping reload periods differ.",
                "Native reservation scans may repeat across reload iterations. First-seen selection union is not a recorded global spill-set growth schedule.",
                "Stack evictions are actual S records; their candidate causal instructions are inferred from an earlier pass's liveness.",
                "ROM register nonuse is an instruction observation, not an original compiler spill record."]}


def compact(report, limit=6):
    """A worker-sized view; retain the complete report in the evaluation cache."""
    points, groups = [], {}
    for item in report.get("pressure_points", []):
        shape = item.get("rtl_shape") or {}
        key = (item.get("line"), item.get("overlap_lower_bound"), item.get("max_reserved_register_count"),
               shape.get("memory_operands"), tuple(shape.get("modes", [])))
        if key in groups:
            groups[key]["uids"].append(item["uid"])
            continue
        blockers = {}
        for choice in item.get("choices", []):
            for blocker in choice.get("live_pseudo_blockers", []):
                blockers[(blocker["register"], blocker["pseudo"])] = blocker
        point = {"uids": [item["uid"]], "line": item.get("line"), "source": item.get("source"),
                 "expansion_line": item.get("expansion_line"),
                 "observed_overlap_at_least": item.get("overlap_lower_bound"),
                 "max_reserved_registers": item.get("max_reserved_register_count"),
                 "reload_values": list(dict.fromkeys(c["value"] for c in item.get("choices", []))),
                 "live_blockers": list(blockers.values()), "pre_reload_rtl": (item.get("pre_reload") or {}).get("rtl"),
                 "machine_pattern": (item.get("pre_reload") or {}).get("machine_pattern")}
        groups[key] = point
        points.append(point)
    evictions = [{"pseudo": e["pseudo"], "name": e["name"], "from_register": e["spilled_from_register"],
                  "from_registers": e.get("spilled_from_registers", [e["spilled_from_register"]]),
                  "slot_offset": e["slot_offset"], "slot_bytes": e["slot_bytes"],
                  "earliest_candidate": e["earliest_candidate_selection"], "cause_proved": e["cause_proved"]}
                 for e in report.get("evictions", [])]
    return {"summary": report.get("summary"), "pressure_points": points[:limit], "evictions": evictions,
            "unallocated_stack_slots": [{"pseudo": e["pseudo"], "name": e["name"], "slot_offset": e["slot_offset"]}
                                        for e in report.get("unallocated_stack_slots", [])],
            "target_register_observation": report.get("target_register_observation"),
            "missing_instrumentation": {name: {"available": value["available"], "record_at": value["record_at"]}
                                        for name, value in report.get("missing_instrumentation", {}).items()},
            "evidence_missing": report.get("evidence_missing", []),
            "interpretation": "Reservations/overlap exclusions and final S evictions are compiler records; eviction candidates are inferred. Overlap counts are lower bounds, not exact total demand."}

"""Local allocator quantity arithmetic from actual L records, without a compile.

The quantities sorted by local-alloc.c are not flow's individual pseudo live
lengths. Their birth/death coordinates count half instructions after regmove.
Thresholds here describe compiler quantities, never a promised number of C edits.
"""
from __future__ import annotations

import hashlib
from pathlib import Path
import re

IMPLEMENTATION_ID = hashlib.sha256(Path(__file__).read_bytes()).hexdigest()


def priority(refs, lifetime, words=1):
    """QTY_CMP_PRI_BASE, including GCC's truncation of double arithmetic."""
    if refs < 1 or lifetime < 1 or words < 1:
        raise ValueError("positive refs, lifetime and register words required")
    return int(((refs.bit_length() - 1) * refs * words / lifetime) * 10000)


def quantities(text, function, names=None):
    out = []
    names = names or {}
    for line in text.splitlines():
        fields = line.split(maxsplit=11)
        if len(fields) < 11 or fields[:2] != ["L", function]:
            continue
        try:
            block, pseudo, number, score, birth, death, refs, suggestions, hard = map(int, fields[2:11])
        except ValueError:
            continue
        lifetime = death - birth
        sizes = [size for size in range(1, 5)
                 if refs > 0 and lifetime > 0 and priority(refs, lifetime, size) == score]
        out.append({"block": block, "pseudo": pseudo, "quantity": number,
                    "name": names.get(pseudo) or (fields[11] if len(fields) > 11 and fields[11] != "-" else None),
                    "priority": score, "birth": birth, "death": death,
                    "lifetime_half_insns": lifetime, "refs": refs,
                    "suggestions": suggestions, "hard_register": hard,
                    "register_words": sizes[0] if len(sizes) == 1 else None,
                    "register_words_candidates": sizes,
                    "size_evidence": "inferred from actual unmodified priority formula; quantity members and size are not instrumented"})
    return out


def _minimum_refs(refs, lifetime, words, required):
    if priority(refs, lifetime, words) >= required:
        return 0
    lo, hi = refs, max(refs + 1, 2)
    while priority(hi, lifetime, words) < required:
        hi *= 2
    while hi - lo > 1:
        mid = (lo + hi) // 2
        if priority(mid, lifetime, words) >= required:
            hi = mid
        else:
            lo = mid
    return hi - refs


def _shorten(refs, lifetime, words, required):
    if priority(refs, lifetime, words) >= required:
        return 0
    if priority(refs, 1, words) < required:
        return None
    lo, hi = 1, lifetime
    while hi - lo > 1:
        mid = (lo + hi) // 2
        if priority(refs, mid, words) >= required:
            lo = mid
        else:
            hi = mid
    return lifetime - lo


def _lengthen(refs, lifetime, words, maximum):
    if priority(refs, lifetime, words) <= maximum:
        return 0
    if maximum < 0:
        return None
    lo, hi = lifetime, lifetime * 2
    while priority(refs, hi, words) > maximum:
        hi *= 2
    while hi - lo > 1:
        mid = (lo + hi) // 2
        if priority(refs, mid, words) <= maximum:
            hi = mid
        else:
            lo = mid
    return hi - lifetime


def threshold(selected, other, quantity_count):
    """Minimal changes to measured quantity metrics, holding everything else fixed.

    Large blocks use qty_compare_1's deterministic quantity-number tie break.
    The specialized <=3-quantity sort has no such comparator; require a strict
    numeric priority advantage there and label the more limited claim.
    """
    tie_wins = quantity_count > 3 and selected["quantity"] < other["quantity"]
    required = other["priority"] + (0 if tie_wins else 1)
    maximum = selected["priority"] - (0 if tie_wins else 1)
    a_words, b_words = selected["register_words"], other["register_words"]
    numeric = {"required_priority": required,
               "priority_delta": max(0, required - selected["priority"]),
               "equal_priority_wins": tie_wins,
               "tie_rule": "quantity number ascending (qty_compare_1)" if quantity_count > 3 else "strict numeric threshold; specialized small-block sort is not simulated",
               "selected_shorten_half_insns": None,
               "selected_add_refs": None,
               "competitor_lengthen_half_insns": None,
               "competitor_remove_refs": None}
    if a_words is not None:
        numeric["selected_shorten_half_insns"] = _shorten(selected["refs"], selected["lifetime_half_insns"], a_words, required)
        numeric["selected_add_refs"] = _minimum_refs(selected["refs"], selected["lifetime_half_insns"], a_words, required)
    if b_words is not None:
        numeric["competitor_lengthen_half_insns"] = _lengthen(other["refs"], other["lifetime_half_insns"], b_words, maximum)
        for refs in range(other["refs"], 0, -1):
            if priority(refs, other["lifetime_half_insns"], b_words) <= maximum:
                numeric["competitor_remove_refs"] = other["refs"] - refs
                break
    return numeric


def _matches(item, name):
    return name is None or str(item["pseudo"]) == str(name) or item["name"] == name


def analyze(scope, wd, function, name, competitor=None):
    """Read an existing diagnostic workspace. NAME/COMPETITOR may be pseudo IDs.

    Multiple matching names are retained rather than silently picking one after
    inlining. Candidates must overlap in the same block. Actual register-class,
    hard-register and preference restrictions remain separate from this ranking.
    """
    function = scope.dump_func(wd, function)
    path = Path(wd) / "names.txt"
    all_quantities = quantities(path.read_text(errors="replace") if path.is_file() else "", function,
                               scope.pseudo_names(wd, function))
    selected = [q for q in all_quantities if _matches(q, name)]
    result = []
    for item in selected:
        block = [q for q in all_quantities if q["block"] == item["block"]]
        count = max((q["quantity"] for q in block), default=-1) + 1
        rivals = []
        for other in block:
            if other["quantity"] == item["quantity"] or not _matches(other, competitor):
                continue
            lo, hi = max(item["birth"], other["birth"]), min(item["death"], other["death"])
            if lo >= hi:
                continue
            rivals.append({"quantity": other, "overlap_half_insns": [lo, hi],
                           "threshold": threshold(item, other, count),
                           "priority_phase_only": True,
                           "suggestion_phase_may_precede_priority": bool(item["suggestions"] or other["suggestions"])})
        rivals.sort(key=lambda r: (r["threshold"]["priority_delta"], r["quantity"]["quantity"]))
        result.append({"selected": item, "quantity_count": count, "competitors": rivals})
    root = Path(scope.INSTR).parent
    source = root / "local-alloc.c"
    text = source.read_text(errors="replace") if source.is_file() else ""
    hit = re.search(r"^#define QTY_CMP_PRI_BASE", text, re.M)
    line = text.count("\n", 0, hit.start()) + 1 if hit else None
    return {"schema": 1, "function": function, "query": str(name), "competitor_query": competitor,
            "ambiguous": len(selected) > 1, "matches": result,
            "compiler": {"file": "build/scope/agbcc/gcc/local-alloc.c", "line": line,
                         "reference": "build/scope/agbcc/gcc/local-alloc.c" + (":" + str(line) if line else ""),
                         "formula": "trunc(floor_log2(quantity_refs) * quantity_refs * register_words / (death - birth) * 10000)"},
            "limits": ["L records describe quantity heads; tied member pseudos and quantity size are not recorded. Size is inferred only when the unmodified formula uniquely explains observed priority.",
                       "Coordinates are half instructions after regmove, not flow pseudo lifetimes and not C statements. Thresholds hold other quantities fixed; an edit can change refs, grouping, lifetimes and constraints together.",
                       "Priority rank does not prove hard-register placement. Suggested-register allocation runs first; register classes, fixed conflicts and preferences still restrict placement.",
                       "Read unmodified diagnostic records. An active AGBCC_PRI knob can invalidate inferred size and thresholds.",
                       "No match may mean the name labels a tied member rather than a recorded quantity head; select a head pseudo ID from trace."]}

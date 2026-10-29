"""Selected-node compiler decision snapshots for stateful matching search.

collect() intentionally pays for one instrumented compile, not on every expansion.
All allocation/reload facts come from the compiler's own records. Cross-source
identity is an explicitly fallible correspondence by C name and occurrence; it is
never a claim that two arbitrary pseudo numbers denote the same original value.
"""
from __future__ import annotations

import collections
import hashlib
import json
from pathlib import Path
import os
import re

IMPLEMENTATION_ID = hashlib.sha256(Path(__file__).read_bytes()).hexdigest()
import time

try:
    from . import search_structure as structure
except ImportError:
    import search_structure as structure

PASS_FUNCTIONS = {
    "rtl": ("expr.c", "expand_expr"), "jump": ("jump.c", "jump_optimize"),
    "cse": ("cse.c", "cse_basic_block"), "addressof": ("function.c", "purge_addressof"),
    "gcse": ("gcse.c", "gcse_main"), "loop": ("loop.c", "loop_optimize"),
    "cse2": ("cse.c", "cse_basic_block"), "flow": ("flow.c", "life_analysis"),
    "combine": ("combine.c", "combine_instructions"), "regmove": ("regmove.c", "regmove_optimize"),
    "lreg": ("local-alloc.c", "local_alloc"), "greg": ("global.c", "global_alloc"),
    "jump2": ("jump.c", "jump_optimize"), "mach": ("thumb.c", "thumb_reorg"),
}


def _hash(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(",", ":")).encode()).hexdigest()


def _reference(root, filename, symbol):
    path = root / filename
    if not path.is_file():
        return None
    line = structure._source_function_line(str(path), symbol, path.stat().st_mtime_ns)
    if line is None:
        return None
    try:
        display = str(path.relative_to(structure.ROOT))
    except ValueError:
        display = "agbcc/gcc/" + filename
    return {"file": display, "line": line, "symbol": symbol, "reference": f"{display}:{line}"}


def _flow_metrics(wd, pass_name):
    path = Path(wd) / ("u.i." + pass_name)
    text = path.read_text(errors="replace") if path.is_file() else ""
    out = {}
    for m in re.finditer(r"^Register (\d+) used (\d+) times across (\d+) insns([^\n]*)", text, re.M):
        preferred = re.search(r"\bpref ([A-Z_]+)", m.group(4))
        out[int(m.group(1))] = {"refs": int(m.group(2)), "live_length": int(m.group(3)),
                                "preferred_class": preferred.group(1) if preferred else None}
    return out


def _identities(scope, wd, function, records):
    names, lines = scope.pseudo_names(wd, function), scope.uid_lines(wd)
    first = {}
    # Source order at the earliest pass is more stable than priority/rank order.
    for pass_name in scope.PASSES:
        for uid, _, rtl in scope.insn_list(wd, pass_name):
            for pseudo in scope.regs_of(rtl):
                if pseudo >= 16:
                    first.setdefault(pseudo, {"uid": uid, "line": lines.get(uid), "order": len(first)})
    named_counts = collections.Counter(names.get(p) for p in records if names.get(p))
    occurrences, identities = collections.Counter(), {}
    for pseudo in sorted(set(records) | set(first), key=lambda p: (first.get(p, {}).get("order", 1 << 30), p)):
        name = names.get(pseudo)
        line = first.get(pseudo, {}).get("line")
        stem = name or f"@line:{line or 0}"
        occurrences[stem] += 1
        identities[pseudo] = {"key": f"{stem}#{occurrences[stem]}", "name": name,
                              "occurrence": occurrences[stem], "first_line": line,
                              "first_uid": first.get(pseudo, {}).get("uid"),
                              "ambiguous": not bool(name) or named_counts[name] > 1,
                              "correspondence": "source-name+occurrence" if name else "source-line+occurrence; unnamed temporary"}
    return identities


def _normalize_pass(scope, wd, pass_name, identities):
    """Hash real RTL with renumbering noise removed, retaining branch destinations.

    Named pseudos use C identities; unnamed pseudos are alpha-renamed on first
    occurrence per pass. Hard registers remain fixed to expose actual allocation.
    Label definitions stay in sequence, so distinct control flow cannot collapse.
    """
    path = Path(wd) / ("u.i." + pass_name)
    if not path.is_file():
        return None
    text = path.read_text(errors="replace")
    events = list(re.finditer(r"^\((insn|jump_insn|call_insn|code_label)(?:/\w)* (\d+) .*?(?=^\(|\Z)", text, re.M | re.S))
    labels = {int(m.group(2)): f"L{i}" for i, m in enumerate(e for e in events if e.group(1) == "code_label")}
    temporary_names = {}

    def reg_name(m):
        number = int(m.group(3))
        if number < 16:
            name = "hard:" + str(number)
        elif identities.get(number, {}).get("name"):
            name = "source:" + identities[number]["key"]
        else:
            name = temporary_names.setdefault(number, "tmp:" + str(len(temporary_names)))
        return "(reg" + m.group(1) + ":" + m.group(2) + " " + name + ")"

    normalized = []
    for event in events:
        kind, uid = event.group(1), int(event.group(2))
        if kind == "code_label":
            normalized.append(["label", labels[uid]])
            continue
        rtl = scope.one_line(event.group())
        rtl = re.sub(r"\(reg((?:/[a-z]+)*):(\w+) (\d+)(?: [^()]*)?\)", reg_name, rtl)
        rtl = re.sub(r"\(label_ref((?:/\w+)*) (?:(\d+))\)",
                     lambda m: "(label_ref" + m.group(1) + " " + labels.get(int(m.group(2)), "external-label") + ")", rtl)
        # DWARF/source locations and instruction-recognition metadata are not RTL decisions.
        rtl = re.sub(r'"[^"\n]*\.c"\s+\d+', '"source"', rtl)
        rtl = re.sub(r"\s+", " ", rtl).strip()
        normalized.append([kind, rtl])
    return {"hash": _hash(normalized), "instruction_count": sum(x[0] != "label" for x in normalized),
            "label_count": len(labels), "identity_ambiguity": any(x["ambiguous"] for x in identities.values()),
            "normalization": "C-name/occurrence pseudos; first-use anonymous pseudos; fixed hard regs; lexical label definitions"}


def _collect(scope, src, function, env=None):
    """Compile SRC once with full dumps and collect actual compiler decisions.

    scope is an imported tools/scope/scope.py module. Runtime paths are omitted
    from the snapshot so it can be attached to a persistent search node safely.
    """
    started = time.perf_counter()
    code, address, info = scope.compile_draft(str(src), function, env or {})
    wd = info["wd"]
    records = scope.alloc_records(wd, function)
    identities = _identities(scope, wd, function, records)
    flow, lreg = _flow_metrics(wd, "flow"), _flow_metrics(wd, "lreg")
    prefs, conflicts = scope.preferences(wd), scope.conflicts(wd)
    find_reg = scope.find_reg_view(wd, function)
    allocations = []
    for pseudo, record in records.items():
        kind = record[0]
        item = {"pseudo": pseudo, **identities[pseudo], "allocator": "global" if kind == "G" else "local",
                "priority": record[2], "hard_register": record[6], "flow": flow.get(pseudo), "lreg": lreg.get(pseudo),
                "preferred_class": lreg.get(pseudo, {}).get("preferred_class"),
                "find_reg": [{k: sorted(v) if isinstance(v, set) else v for k, v in decision.items()}
                             for decision in find_reg.get(pseudo, [])],
                "preferences": prefs.get(pseudo, []),
                "conflict_keys": sorted(identities.get(p, {"key": "pseudo:" + str(p)})["key"] for p in conflicts.get(pseudo, (set(), set()))[0]),
                "hard_conflicts": sorted(conflicts.get(pseudo, (set(), set()))[1]),
                "named_conflict_keys": sorted(identities[p]["key"] for p in conflicts.get(pseudo, (set(), set()))[0]
                                               if p in identities and identities[p]["name"] and not identities[p]["ambiguous"]),
                "evidence": "AGBCC_DUMPPRI allocation record; actual compiler decision"}
        if kind == "G":
            item.update(rank=record[1], refs=record[3], live_length=record[4], calls_crossed=record[5])
        else:
            item.update(block=record[1], birth=record[3], death=record[4], refs=record[5])
        allocations.append(item)
    states = scope.spill_states(wd, function)
    reloads, occurrences = [], collections.Counter()
    for choice, uid, reg, line, label in scope.reload_records(wd, function):
        named = label != "-" and not re.fullmatch(r"reg\d+", label)
        stem = label if named else f"@line:{line}:{label}"
        occurrences[stem] += 1
        state = states.get(choice)
        rotation = None
        if state:
            rotation = {"start": state["start"], "pass": state["pass"], "rotate": state.get("rotate"), "registers": state["regs"],
                        "standing": {str(r): ("live:" + identities.get(int(s[1:]), {"key": "pseudo:" + s[1:]})["key"] if s.startswith("L") else s)
                                     for r, s in state["st"].items()}}
        reloads.append({"key": f"{stem}#{occurrences[stem]}", "choice": choice, "uid": uid, "line": line,
                        "label": label, "hard_register": reg, "rotation": rotation, "ambiguous": not named,
                        "evidence": "AGBCC_DUMPPRI R/X records; actual reload choice"})
    root = Path(scope.INSTR).parent
    passes = {}
    for pass_name in scope.PASSES:
        value = _normalize_pass(scope, wd, pass_name, identities)
        if value:
            value["compiler"] = _reference(root, *PASS_FUNCTIONS[pass_name]) if pass_name in PASS_FUNCTIONS else None
            passes[pass_name] = value
    _, _, sizes = scope.inventory()
    target = scope.rom_bytes()[scope.off(address):scope.off(address) + sizes[scope.rom_name(str(src), function)]]
    return {"schema": 1, "function": function, "source_hash": hashlib.sha256(Path(src).read_bytes()).hexdigest(),
            "compile_ms": round((time.perf_counter() - started) * 1000, 3), "code_hash": hashlib.sha256(code).hexdigest(),
            "exact": code == target, "trace_matches_build": not bool(info.get("g_differs") or info.get("g_only")),
            "trace_compile": "-g -dp -da instrumented agbcc", "pass_order": list(scope.PASSES), "passes": passes,
            "allocations": sorted(allocations, key=lambda a: a["key"]), "reloads": reloads,
            "inheritance": [{"uid": u, "pseudo": p, "spill_index": i, "from_uid": f, "kind": k}
                            for u, p, i, f, k in scope.inherit_records(wd, function)],
            "compiler": {"allocation": structure.compiler_references("allocation", root),
                         "reload": structure.compiler_references("reload", root),
                         "register_class": [_reference(root, "regclass.c", "regclass"), _reference(root, "regclass.c", "record_reg_classes")]},
            "limits": ["All records describe the diagnostic -g compile; trace_matches_build says whether its output matched the no-g build.",
                       "Cross-node name/occurrence correspondence can be ambiguous after inlining, shadowing or edits.",
                       "Earliest changed pass output locates divergence; it does not prove why a pass made that choice.",
                       "Anonymous temporary alpha-renaming removes numbering noise and can hide pure temporary renamings."]}



def collect(scope, src, function, env=None):
    """Capture default-build decisions without inherited experimental knobs.

    Explicit compiler knobs are permitted for counterfactual API clients, but such
    snapshots never claim default-build exactness or trace_matches_build.
    """
    env = dict(env or {})
    bad = [key for key in env if not key.startswith(("AGBCC_", "CSE_"))]
    if bad:
        raise ValueError("unsupported trace experiment keys: " + ", ".join(bad))
    saved = {key: os.environ.pop(key) for key in list(os.environ)
             if key.startswith(("AGBCC_", "CSE_", "SCOPE_TRANSFORM"))}
    try:
        result = _collect(scope, src, function, env)
        result["knobs"] = env
        result["experiment"] = bool(env)
        if env:
            result["experiment_exact"] = result.get("exact", False)
            result["exact"] = False
            result["diagnostic_matches_no_g"] = result.get("trace_matches_build", False)
            result["trace_matches_build"] = False
        return result
    finally:
        os.environ.update(saved)


def _changes(parent, child, fields):
    before, after = {r["key"]: r for r in parent}, {r["key"]: r for r in child}
    out = []
    for key in sorted(before.keys() | after.keys()):
        a, b = before.get(key), after.get(key)
        changed = {f: {"parent": a.get(f) if a else None, "child": b.get(f) if b else None}
                   for f in fields if (a.get(f) if a else None) != (b.get(f) if b else None)}
        if a is None or b is None or changed:
            out.append({"key": key, "status": "added" if a is None else "removed" if b is None else "changed",
                        "ambiguous": bool((a or {}).get("ambiguous") or (b or {}).get("ambiguous")), "changes": changed})
    return out


def delta(parent, child):
    """Compare two actual compiler snapshots, independent of pseudo renumbering."""
    order = child.get("pass_order") or parent.get("pass_order") or list(PASS_FUNCTIONS)
    passes = [{"pass": p, "parent": parent.get("passes", {}).get(p, {}).get("hash"),
               "child": child.get("passes", {}).get(p, {}).get("hash"),
               "compiler": child.get("passes", {}).get(p, {}).get("compiler") or parent.get("passes", {}).get(p, {}).get("compiler")}
              for p in order if parent.get("passes", {}).get(p, {}).get("hash") != child.get("passes", {}).get(p, {}).get("hash")]
    return {"schema": 1, "earliest_changed_pass": passes[0] if passes else None, "changed_passes": passes,
            "allocations": _changes(parent.get("allocations", []), child.get("allocations", []),
                                    ("allocator", "priority", "rank", "refs", "live_length", "flow", "lreg", "preferred_class", "find_reg", "hard_register", "preferences", "named_conflict_keys", "hard_conflicts")),
            "reloads": _changes(parent.get("reloads", []), child.get("reloads", []), ("hard_register", "rotation")),
            "trace_matches_build": bool(parent.get("trace_matches_build") and child.get("trace_matches_build")),
            "interpretation": "Compiler observations are actual; cross-node identity is name/occurrence correspondence, with ambiguity exposed. Earliest pass divergence is not a causal proof."}

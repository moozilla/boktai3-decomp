"""Matched prior art for a ROM block; token similarity is never semantic proof."""
import difflib
from pathlib import Path
import re


def find(tokens, index, status, files, target, limit=4):
    if len(tokens) < 3:
        return []
    windows = {tuple(tokens[i:i+3]) for i in range(len(tokens)-2)}
    candidates = []
    for name, other in index.items():
        if name == target or status.get(name) != "matched" or not files.get(name):
            continue
        overlap = sum(tuple(other[i:i+3]) in windows for i in range(len(other)-2))
        if overlap:
            candidates.append((overlap, name))
    matches = []
    for _, name in sorted(candidates, reverse=True)[:80]:
        other = index[name]
        match = difflib.SequenceMatcher(None, tokens, other, autojunk=False).find_longest_match()
        if match.size < min(4, len(tokens)):
            continue
        matches.append(dict(function=name, source=files[name], consecutive_instructions=match.size,
                            target_instruction=match.a, matched_instruction=match.b,
                            tokens=other[match.b:match.b+match.size]))
    matches.sort(key=lambda m: (-m["consecutive_instructions"], m["function"]))
    return matches[:limit]


def source_location(root, match):
    """Find a definition, including a file-local friendly-name alias."""
    path = Path(root) / match["source"]
    if not path.is_file():
        return None
    text = path.read_text()
    names = {match["function"]}
    names.update(re.findall(r'^\s*#define\s+(\w+)\s+' + re.escape(match["function"]) + r'\s*$', text, re.M))
    for name in sorted(names):
        found = re.search(r'^[ \t]*(?:[A-Za-z_]\w*[ \t*]+)+' + re.escape(name) + r'\s*\([^;{}]*\)\s*\{', text, re.M)
        if found:
            return dict(line=text.count("\n", 0, found.start()) + 1, name=name)
    return None


def query(scope, compiled, rom_name, region=None, limit=4):
    import structure
    blocks = compiled.get("structure", {}).get("target", {}).get("blocks", [])
    # analyze() stores complete fingerprints under target_graph/current_graph.
    if not blocks:
        from search_structure import fingerprint
        blocks = fingerprint(compiled["target_rows"], compiled.get("target_data", ())) ["blocks"]
    verdicts = compiled.get("structure", {}).get("regions", [])
    chosen = next((b for b in blocks if b["id"] == region), None) if region else next(
        (b for b in blocks if any(r.get("target") == b["id"] and
            (r.get("local_shape") != "same" or r.get("global_binding") != "same") for r in verdicts)
            and len(b["instructions"]) >= 3), None)
    if not chosen:
        raise ValueError("select an existing ROM region with --region Bn (at least three instructions help)")
    names = {address: name for name, address in scope.inventory()[1].items()}
    tokens = [structure.norm_insn(r["text"], names) for r in chosen["instructions"]]
    index = scope.like_index()
    status, files = scope.code_status()
    matches = find(tokens, index, status, files, rom_name, limit)
    for match in matches:
        match["definition"] = source_location(scope.ROOT, match)
    return dict(region=chosen["id"], target_start=chosen["start"], target_instructions=len(tokens),
                matches=matches, interpretation="Prior art ranked by consecutive normalized instruction tokens. "
                "Register normalization is retrieval only, not an equivalence test. Definition lines are function anchors, "
                "not a claimed mapping of this block to C. Read the matched function before transplanting an idiom.")

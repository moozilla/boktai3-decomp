"""Diversity archive, not instruction-diff hill climbing.

No source is deleted when dominated. The frontier is a view selecting useful
contrasting candidates, including explicitly retained hypotheses and a rotating
exploration slot. Scalar rewards do not certify similarity or correctness.
"""
from __future__ import annotations

import math
from typing import Any


OBJECTIVES = ("dependency", "control", "operations", "instruction", "size", "pins", "barriers", "assembly")


def vector(row: dict[str, Any]) -> tuple[float, ...]:
    e = row["evaluation"]
    d = e.get("distances", {})
    c = e.get("concessions", {})
    return (float(d.get("dependency", 1e9)), float(d.get("control", 1e9)),
            float(d.get("operations", 1e9)), float(e.get("instruction_diff", 1e9)),
            float(abs(e.get("candidate_size", 0) - e.get("target_size", 0))),
            float(c.get("register_pins", 0)), float(c.get("asm_barriers", 0)), float(c.get("inline_assembly", 0)))


def dominates(a: tuple[float, ...], b: tuple[float, ...]) -> bool:
    return all(x <= y for x, y in zip(a, b)) and any(x < y for x, y in zip(a, b))


def select(rows: list[dict[str, Any]], *, limit: int = 12, epoch: int = 0) -> list[dict[str, Any]]:
    if limit < 1:
        return []
    rows = [r for r in rows if not r.get("rejected") and r["evaluation"].get("code_hash")]
    if not rows:
        return []
    vectors = {r["id"]: vector(r) for r in rows}
    total = sum(r.get("expansions", 0) for r in rows)

    def rank(r: dict[str, Any]) -> tuple[Any, ...]:
        # This sum ranks alternatives inside a niche, never prunes branches.
        # Source length is a final preference, not an unscaled machine objective.
        v = vectors[r["id"]]
        return (not r["evaluation"].get("exact", False), sum(v), r.get("expansions", 0),
                r["evaluation"].get("source_complexity", 0), r["id"])

    chosen: list[dict[str, Any]] = []
    seen: set[str] = set()

    def take(row: dict[str, Any], reason: str) -> None:
        if row["id"] not in seen and len(chosen) < limit:
            seen.add(row["id"])
            chosen.append({"id": row["id"], "label": row.get("label", ""), "reason": reason,
                           "objectives": dict(zip(OBJECTIVES, vectors[row["id"]])),
                           "source_tokens": row["evaluation"].get("source_complexity"),
                           "exact": bool(row["evaluation"].get("exact")),
                           "expansions": row.get("expansions", 0),
                           "exploration": round(math.sqrt(2 * math.log(total + 2) /
                                                (row.get("expansions", 0) + 1)), 4),
                           "code_hash": row["evaluation"].get("code_hash"),
                           "region_verdicts": row["evaluation"].get("region_summary", {})})

    for row in sorted((r for r in rows if r["evaluation"].get("exact")), key=rank):
        take(row, "ordinary-build exact match")
    for row in sorted((r for r in rows if r.get("retained")), key=rank):
        take(row, "retained hypothesis: " + row.get("retention_reason", ""))

    # Reserve exploration before Pareto candidates fill the view. Even a branch
    # dominated on every current objective can be necessary after another move.
    explored = sorted(rows, key=lambda r: (r.get("expansions", 0), r["created"], r["id"]))
    if limit >= 3:
        take(explored[epoch % len(explored)], "exploration; temporary regressions are allowed")

    for index, name in enumerate(OBJECTIVES):
        take(min(rows, key=lambda r: (vectors[r["id"]][index], rank(r))), f"champion: {name}")

    # A dependency fingerprint and exact-region mask define independent niches.
    niches: dict[tuple[Any, ...], dict[str, Any]] = {}
    for row in rows:
        e = row["evaluation"]
        key = (e.get("structure_hash", e.get("code_hash")), tuple(e.get("exact_regions", [])),
               bool(e.get("concessions", {}).get("register_pins", 0)))
        if key not in niches or rank(row) < rank(niches[key]):
            niches[key] = row
    candidates = sorted(niches.values(), key=rank)
    pareto = [r for r in candidates if not any(dominates(vectors[o["id"]], vectors[r["id"]])
                                               for o in candidates if o["id"] != r["id"])]
    for row in pareto:
        take(row, "nondominated structural niche")
    for row in candidates:
        take(row, "distinct dependency/region niche")
    for row in sorted(rows, key=lambda r: (r.get("expansions", 0), rank(r))):
        take(row, "unexpanded source alternative (equal binaries retain different descendants)")
    return chosen

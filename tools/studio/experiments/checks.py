"""Evaluate falsifiable statements against saved frame and call evidence."""
import operator

OPERATORS = {
    "eq": (operator.eq, "="),
    "ne": (operator.ne, "≠"),
    "lt": (operator.lt, "<"),
    "le": (operator.le, "≤"),
    "gt": (operator.gt, ">"),
    "ge": (operator.ge, "≥"),
}


def validate_checks(raw, trial_ids, fields, probes, frames):
    from .recipe import label
    if not isinstance(raw, list) or len(raw) > 12:
        raise ValueError("Checks must be a list of at most 12 statements")
    checks = []
    for row in raw:
        if not isinstance(row, dict):
            raise ValueError("Each check must be an object")
        statement = label(row.get("statement"), 160, "Check statement")
        trial = row.get("trial")
        if not isinstance(trial, str) or trial not in trial_ids:
            raise ValueError("A check must name one of the recipe's trials")
        start, end = row.get("from", 0), row.get("to", max(frames))
        if type(start) is not int or type(end) is not int or start not in frames or end not in frames or start >= end:
            raise ValueError("Check bounds must be sampled frames in increasing order")
        comparison, target = row.get("compare"), row.get("value")
        if not isinstance(comparison, str) or comparison not in OPERATORS or type(target) is not int \
                or not -(2**32) < target < 2**32:
            raise ValueError("A check requires a comparison (eq, ne, lt, le, gt or ge) and an integer value")
        item = {"statement": statement, "trial": trial, "from": start, "to": end,
                "compare": comparison, "value": target}
        if "field" in row and "function" not in row and row["field"] in fields:
            item.update(field=row["field"], measure="delta")
        elif "function" in row and "field" not in row and row["function"] in probes:
            item.update(function=row["function"], measure="entries")
        else:
            raise ValueError("A check measures a field delta or a probed function's entries")
        checks.append(item)
    return checks


def evaluate(checks, result):
    field_indices = {field["path"]: i for i, field in enumerate(result["fields"])}
    trials = {trial["id"]: trial for trial in result["trials"]}
    answers = []
    for check in checks:
        trial = trials[check["trial"]]
        start, end = check["from"], check["to"]
        measurement = None
        if check["measure"] == "delta":
            snapshots = {snap["relative"]: snap for snap in trial["snapshots"]}
            index = field_indices[check["field"]]
            before, after = [snapshots[frame]["fields"][index] for frame in (start, end)]
            moved = any(snap["fields"][index]["state"] in {"rebound", "unbound"}
                        for snap in trial["snapshots"] if start < snap["relative"] <= end)
            if before["value"] is not None and after["value"] is not None \
                    and before["address"] == after["address"] and not moved:
                measurement = after["value"] - before["value"]
            evidence = {"kind": "field", "index": index, "before": before["value"], "after": after["value"]}
        else:
            index = next(i for i, p in enumerate(trial["probes"]) if p["name"] == check["function"])
            probe = trial["probes"][index]
            measurement = sum(count for frame, count in probe["frames"].items() if start <= int(frame) < end)
            evidence = {"kind": "probe", "index": index}
        compare, sign = OPERATORS[check["compare"]]
        state = "unresolved" if measurement is None else "confirmed" if compare(measurement, check["value"]) else "contradicted"
        answers.append({**check, "measurement": measurement, "sign": sign, "state": state, "evidence": evidence})
    return answers

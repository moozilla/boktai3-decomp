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


def normalize(row, trial_ids, fields, probes, frames):
    if not isinstance(row, dict):
        raise ValueError("Each check must be an object")
    trial = row.get("trial")
    if not isinstance(trial, str) or trial not in trial_ids:
        raise ValueError("A check must name one of the recipe's trials")
    comparison, target = row.get("compare"), row.get("value")
    if not isinstance(comparison, str) or comparison not in OPERATORS or type(target) is not int \
            or not -(2**32) < target < 2**32:
        raise ValueError("A check requires a comparison (eq, ne, lt, le, gt or ge) and an integer value")

    field, function = row.get("field"), row.get("function")
    item = {"trial": trial, "compare": comparison, "value": target}
    if field in fields and "function" not in row:
        if "mask" in row:
            mask = row["mask"]
            if type(mask) is not int or not 0 <= mask < 2**32:
                raise ValueError("A field bit mask must be an unsigned 32-bit integer")
            item["mask"] = mask
        item.update(field=field, measure="delta")
    elif function in probes and "field" not in row and "mask" not in row:
        item.update(function=function, measure="entries")
    else:
        raise ValueError("A check measures a field delta or value, or a probed function's entries")

    if "frame" in row:
        frame = row["frame"]
        if field is None or "from" in row or "to" in row or type(frame) is not int or frame not in frames:
            raise ValueError("An absolute field check needs one captured frame")
        return {**item, "frame": frame, "measure": "value"}

    start, end = row.get("from", 0), row.get("to", max(frames))
    if type(start) is not int or type(end) is not int or start not in frames or end not in frames or start >= end:
        raise ValueError("Check bounds must be sampled frames in increasing order")
    return {**item, "from": start, "to": end}


def validate_checks(raw, trial_ids, fields, probes, frames):
    from .recipe import label

    if not isinstance(raw, list) or len(raw) > 12:
        raise ValueError("Checks must be a list of at most 12 statements")
    answers = []
    for row in raw:
        item = normalize(row, trial_ids, fields, probes, frames)
        item["statement"] = label(row.get("statement"), 160, "Check statement")
        if "when" in row:
            predicates = row["when"]
            if not isinstance(predicates, list) or not 1 <= len(predicates) <= 4:
                raise ValueError("A check supports one to four prerequisites")
            if any(isinstance(p, dict) and "when" in p for p in predicates):
                raise ValueError("Check prerequisites cannot be nested")
            item["when"] = [normalize(p, trial_ids, fields, probes, frames) for p in predicates]
        answers.append(item)
    return answers


def read_value(check, value):
    return (value & check["mask"]) if value is not None and "mask" in check else value


def read_measurement(check, trials, field_indices):
    trial = trials[check["trial"]]
    if check["measure"] == "entries":
        start, end = check["from"], check["to"]
        index = next(i for i, p in enumerate(trial["probes"]) if p["name"] == check["function"])
        probe = trial["probes"][index]
        value = sum(count for frame, count in probe["frames"].items() if start <= int(frame) < end)
        return value, {"kind": "probe", "index": index}

    snapshots = {snap["relative"]: snap["fields"] for snap in trial["snapshots"]}
    index = field_indices[check["field"]]
    evidence = {"kind": "field", "index": index}
    if check["measure"] == "value":
        raw = snapshots[check["frame"]][index]["value"]
        return read_value(check, raw), {**evidence, "value": raw}

    start, end = check["from"], check["to"]
    before, after = [snapshots[frame][index] for frame in (start, end)]
    old, new = read_value(check, before["value"]), read_value(check, after["value"])
    moved = any(rows[index]["state"] in {"rebound", "unbound"}
                for frame, rows in snapshots.items() if start < frame <= end)
    stable = old is not None and new is not None and before["address"] == after["address"] and not moved
    return new - old if stable else None, {**evidence, "before": old, "after": new}


def measured(spec, trials, field_indices):
    value, evidence = read_measurement(spec, trials, field_indices)
    compare, sign = OPERATORS[spec["compare"]]
    meets = None if value is None else compare(value, spec["value"])
    return {**spec, "to": spec.get("frame", spec.get("to")), "measurement": value,
            "sign": sign, "evidence": evidence, "meets": meets}


def evaluate(checks, result):
    field_indices = {field["path"]: i for i, field in enumerate(result["fields"])}
    trials = {trial["id"]: trial for trial in result["trials"]}
    answers = []
    for check in checks:
        actual = measured(check, trials, field_indices)
        prerequisites = [measured(spec, trials, field_indices) for spec in check.get("when", [])]
        if not all(item["meets"] is True for item in prerequisites):
            state = "unresolved"
            actual["reason"] = "prerequisite"
        elif actual["meets"] is None:
            state = "unresolved"
            actual["reason"] = "unbound"
        else:
            state = "confirmed" if actual["meets"] else "contradicted"
        actual.update(state=state, prerequisites=prerequisites)
        answers.append(actual)
    return answers


def expression(pred):
    what = pred.get("field") or pred["function"] + " entries"
    if "mask" in pred:
        what += f" & 0x{pred['mask']:X}"
    if "frame" in pred:
        when = f"frame {pred['frame']}"
    else:
        when = f"frames {pred['from']} to {pred['to']}"
    value = pred["measurement"] if pred["measurement"] is not None else "unknown"
    return f"{pred['trial']} {when} {what} ({value} {pred['sign']} {pred['value']})"

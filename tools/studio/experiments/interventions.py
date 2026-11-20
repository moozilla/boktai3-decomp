"""Validate and schedule deliberate writes to observed scalar C field paths."""
from collections import defaultdict

from . import recipe

MAX_INTERVENTIONS = 16


def validate(operations, observed, duration):
    if not isinstance(operations, list) or len(operations) > MAX_INTERVENTIONS:
        raise ValueError(f"A trial supports at most {MAX_INTERVENTIONS} named interventions")
    plan = []
    targets = set()
    for operation in operations:
        if not isinstance(operation, dict):
            raise ValueError("An intervention needs a frame, an observed field and an integer value")
        frame = recipe.bounded_int(operation.get("frame"), 0, duration - 1, "Intervention frame")
        field = operation.get("field")
        if field not in observed:
            raise ValueError("An intervention must name one of the recipe's observed fields")
        value = recipe.bounded_int(operation.get("value"), -(2**31), 2**32 - 1, "Intervention value")
        key = (frame, field)
        if key in targets:
            raise ValueError("Write a field at most once at any given intervention frame")
        targets.add(key)
        plan.append({"frame": frame, "field": field, "value": value})
    return sorted(plan, key=lambda item: item["frame"])


def encoded(trial, observed):
    """Use the actual target ABI width and signedness even for injected values."""
    compiled = {field.path: (i, field) for i, field in enumerate(observed)}
    by_frame = defaultdict(list)
    for operation in trial.get("interventions", []):
        index, field = compiled[operation["field"]]
        bits = 8 * field.width
        minimum = -(1 << (bits - 1)) if field.signed else 0
        maximum = (1 << (bits - int(field.signed))) - 1
        value = recipe.bounded_int(operation["value"], minimum, maximum, field.path)
        by_frame[operation["frame"]].append(f"setfield {index} {value & ((1 << bits) - 1):X}")
    return dict(by_frame)

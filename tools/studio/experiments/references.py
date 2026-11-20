"""Link measured executions back into the Studio Code explorer."""
from bisect import bisect_right
from collections import Counter, defaultdict
import json



def summarize(result, writes):
    summary = defaultdict(list)
    for trial in result["trials"]:
        snapshots = trial["snapshots"]
        samples = [s["relative"] for s in snapshots]
        root = f"/experiments#run/{result['id']}"
        counts = Counter(
            event["function"]["address"] for event in writes[trial["id"]]
            if event["kind"] == "write" and event["function"]
        )
        store_points = {}
        for snapshot in snapshots:
            for field in snapshot["fields"]:
                event = field["last_write"]
                if event and event["function"]:
                    key = event["function"]["address"]
                    store_points.setdefault(key, (snapshot["relative"], field["field"]))
        for address, count in counts.items():
            anchor = store_points.get(address)
            href = f"{root}/{anchor[0]}/{trial['id']}/field/{anchor[1]}" if anchor else root
            summary[address].append({"kind": "stores", "count": count, "trial": trial["label"], "url": href})
        for index, probe in enumerate(trial["probes"]):
            if not probe["count"] or not probe["function"]:
                continue
            first = min(int(frame) for frame in probe["frames"])
            frame = samples[min(bisect_right(samples, first), len(samples) - 1)]
            href = f"{root}/{frame}/{trial['id']}/probe/{index}"
            summary[probe["function"]["address"]].append(
                {"kind": "entries", "count": probe["count"], "trial": trial["label"], "url": href}
            )
    return dict(summary)


def preview(address, runs, jobs):
    found = []
    for run in runs:
        path = jobs / run["id"] / "functions.json"
        if run["state"] != "complete" or not path.is_file():
            continue
        try:
            with path.open() as source:
                evidence = json.load(source).get(address, [])
        except FileNotFoundError:
            continue
        found.extend({"run": run["id"], "title": run["title"], **row} for row in evidence)
        if len(found) >= 12:
            break
    return found[:12]

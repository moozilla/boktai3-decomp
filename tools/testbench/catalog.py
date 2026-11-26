#!/usr/bin/env python3
"""Generate the testbench registry catalog from the same evidence as Studio."""
import argparse
from collections import defaultdict
from functools import lru_cache
import json
from pathlib import Path
import re
import sys

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "tools/assets"))
sys.path.insert(0, str(ROOT / "tools"))
import actors
import gclfiles
import gcloptions
import gclview


def commands(tree):
    """Yield registry commands even inside nested GCL blocks."""
    if isinstance(tree, (tuple, list)):
        if isinstance(tree, tuple) and len(tree) == 5 and tree[:2] in (("cmd", "chara"), ("cmd", "command")):
            yield tree
        for child in tree:
            yield from commands(child)


def encode_value(node):
    kind, *args = node
    if kind == "int":
        return {"hash": args[0]} if args[1] and 0 <= args[0] <= 0xffff else args[0]
    if kind in ("proc", "text"):
        return {kind: args[0]}
    raise ValueError(kind)


def collect_calls():
    calls = defaultdict(list)
    owners = gclfiles.owners()
    image = actors.image()
    for proc in actors.procs():
        for _, mode, _, args, keywords in commands(gclview.procedure(image, proc)):
            if not args or args[0][0] != "int":
                continue
            recipe = {"proc": proc, "mode": mode, "options": {}, "unresolved": []}
            try:
                recipe["args"] = [encode_value(node) for node in args[1:]]
            except ValueError:
                recipe["unresolved"].append("positional arguments")
            for letter, nodes in keywords:
                try:
                    recipe["options"][letter] = [encode_value(node) for node in nodes]
                except ValueError:
                    recipe["unresolved"].append(letter)
            calls[f"{args[0][1]:04X}"].append((recipe, sorted(owners[proc])))
    return calls


def readable_name(fn):
    base = re.sub(r"_(Create.*|Place.*|Spawn.*|Start.*|Init.*)$", "", fn)
    name = re.sub(r"([a-z0-9])([A-Z])", r"\1 \2", base)
    return name.replace("_", " ")


@lru_cache(None)
def allocation(symbol, depth):
    """Look a few calls deep for an actual allocation. Require separate
    creator evidence for indirect calls and pool allocators."""
    if symbol in {"Task_Create", "SceneActorRegistry_LinkActor"}:
        return symbol
    if depth <= 0:
        return None
    code, names = actors.code()
    if symbol not in names:
        return None
    for callee in actors.calls(names[symbol]):
        if callee == code[names[symbol]]["name"]:
            continue
        found = allocation(callee, depth - 1)
        if found:
            return found
    return None


def build():
    calls = collect_calls()
    schemas = gcloptions.registry_schemas()
    contexts = json.loads((HERE / "contexts.json").read_text())
    entries = []
    for a in actors.actors():
        key = a["key"]
        fn = a["callback_name"]
        observations = calls[key]
        # Identified names take precedence. Keep the implementation name as a lookup alias.
        names = list(dict.fromkeys(
            [r["english"] for r in a["gcl"] if r["english"] and r["confidence"] in ("high", "medium")]
            + [readable_name(fn), fn] + [r["name"] for r in a["gcl"] if r["name"]]
        ))
        if key == "05A5":
            names.insert(0, "Ghoul")
        allocator = allocation(fn, 3) or allocation(actors.code()[0].get(int(a["callback"], 16), {}).get("name"), 3)
        creator = re.search(r"_(Create|Spawn|Place)(?:$|FromScript$|ActorFromScript$)", fn)
        if fn.startswith("sub_"):
            creator = a["loader_name"] and any(r["mode"] == "chara" for r, _ in observations)
        # Allocation alone also describes commands that start an internal helper task.
        role = "spawn" if creator or allocator and a["loader_name"] else "command"
        parameters = schemas[int(key, 16)].copy()
        for recipe, _ in observations:
            for opt, vals in recipe["options"].items():
                parameters.setdefault(opt, {"letter": opt, "kind": "observed", "values": len(vals)})
        variants = {}
        for recipe, scenes in observations:
            recipe = dict(recipe, scene=scenes[0] if len(scenes) == 1 else None)
            signature = json.dumps([recipe["mode"], recipe.get("args"), recipe["options"], recipe["unresolved"]], sort_keys=True)
            if signature not in variants:
                variants[signature] = recipe
        recipes = sorted(variants.values(), key=lambda r: (len(r["unresolved"]), r["scene"] is None, -len(r["options"]), r["proc"]))
        entry = dict(key=key, name=names[0], aliases=names[1:], callback=fn, address=a["callback"],
                            init=a["loader_name"] or fn, role=role, allocation=allocator,
                            parameters=parameters, scenes=sorted({s for _, ss in observations for s in ss}),
                            procedures=sorted({r["proc"] for r, _ in observations}), recipes=recipes[:3])
        entries.append(entry | contexts.get(key, {}))
    entries.extend(json.loads((HERE / "native.json").read_text()))
    return {"version": 1, "entries": entries}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="compare with the checked-in catalog")
    args = parser.parse_args()
    output = HERE / "catalog.json"
    data = build()
    # One registry entry per line to make changes reviewable.
    new = '{"version":1,"entries":[\n' + ',\n'.join(json.dumps(e, ensure_ascii=False, sort_keys=True, separators=(",", ":")) for e in data["entries"]) + '\n]}\n'
    if args.check:
        if output.read_text() != new:
            sys.exit("catalog.json is stale. Regenerate with tools/testbench/catalog.py")
    else:
        output.write_text(new)
    print(f'{sum(e["role"] == "spawn" for e in data["entries"])} candidate creators across {len(data["entries"])} catalog entries')


if __name__ == "__main__":
    main()

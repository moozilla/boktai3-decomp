#!/usr/bin/env python3
"""Stateful matching search, with the model choosing the frontier.

  search.py --session NAME init SOURCE [FUNCTION]
  search.py --session NAME status|moves|frontier|regions
  search.py --session NAME expand MOVE_ID... --reason HYPOTHESIS
  search.py --session NAME import FILE --parent NODE --reason HYPOTHESIS
  search.py --session NAME inspect [NODE] | why [NODE] | checkout NODE [--out FILE]

The default resident keeps compiler fork servers warm between CLI calls. `batch`
reads one JSON command per line; requests use {command, session, ...arguments}.
SCOPE_SEARCH_SERVER=0 executes locally. Runtime artifacts stay under build/.
"""
import argparse
import contextlib
import dataclasses
import io
import json
import os
from pathlib import Path
import subprocess
import sys
import time
import traceback
import uuid

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
BASE = ROOT / "build/search-sol"
EVALUATORS = {}


def serial(value):
    if dataclasses.is_dataclass(value):
        return dataclasses.asdict(value)
    if hasattr(value, "to_dict"):
        return value.to_dict()
    return value


def location(name):
    path = Path(name)
    if path.suffix in (".db", ".sqlite", ".sqlite3") or len(path.parts) > 1:
        return path.resolve()
    if name in ("", ".", ".."):
        raise ValueError("give a session name")
    return BASE / name / "session.sqlite3"


def environment(path):
    from search_eval import Evaluator
    key = str(path)
    if key not in EVALUATORS:
        EVALUATORS[key] = Evaluator(path.parent / "compiler")
    return EVALUATORS[key]


def compact(node, evaluation=None):
    result = evaluation["result"] if evaluation else {}
    if not evaluation:
        for e in reversed(node.get("evaluations", [])):
            if e["evaluator"] == "compile" and not e["options"].get("knobs"):
                result = e["result"]
                break
    return dict(node=node["id"][:12], parents=[p[:12] for p in node.get("parents", [])],
                state=node.get("frontier", {}).get("state"),
                status=result.get("status", "unevaluated"), counts=result.get("counts"),
                compile_ms=result.get("compile_ms"), semantic=result.get("semantic"),
                byte_differences=result.get("byte_differences"),
                protected_regions=result.get("protected_regions"),
                delta=result.get("delta"), diagnostics=result.get("diagnostics") or None)


def protection(result, metadata):
    result = dict(result)
    verdicts = {r["target"]: r for r in result.get("structure", {}).get("regions", []) if r.get("target")}
    result["protected_regions"] = {name: {
        "preserved": verdicts.get(name, {}).get("local_shape") == "same" and
            (mode == "shape" or verdicts.get(name, {}).get("global_binding") == "same"),
        "mode": mode} for name, mode in metadata.get("protected_regions", {}).items()}
    return result


def evaluate(session, ref, request):
    from search_eval import decision_delta
    from search_structure import analyze
    meta = session.metadata()
    node = session.node(ref)
    source = session.source(node["id"])
    evaluator = environment(session.path)
    context = evaluator.context(source, meta["function"], meta["origin"])
    if session.context(ref) != context:
        was_head = session.node("HEAD")["id"] == node["id"]
        node = session.add(source, node["id"], {"kind": "context_refresh"}, context=context,
                           checkout=was_head, note="Compiler environment changed; reevaluate in a new context.")
    options = {"knobs": request.get("knobs", {}), "semantic_trials": request.get("trials", 0)}
    cached = None if request.get("force") else session.evaluation(node["id"], options=options)
    if cached:
        cached["result"] = protection(cached["result"], meta)
        return node, cached, True
    lease_options = dict(options, force_attempt=os.getpid()) if request.get("force") else options
    if not session.claim(node["id"], options=lease_options):
        cached = session.evaluation(node["id"], options=options)
        if cached and not request.get("force"):
            return node, cached, True
        raise ValueError("another worker is evaluating this node; reuse its result when the lease completes")
    try:
        result = evaluator.evaluate(source, meta["function"], meta["origin"], options["knobs"], options["semantic_trials"])
        if result.get("rows"):
            result["structure"] = analyze(source, meta["function"], result["target_rows"], result["rows"],
                compiler_root=ROOT / "build/scope/agbcc/gcc", target_data=result.get("target_data", ()),
                current_data=result.get("current_data", ()))
            result = protection(result, meta)
        if options["knobs"] and not request.get("_no_delta"):
            _, baseline, _ = evaluate(session, node["id"], {"_no_delta": True})
            result["delta"] = decision_delta(baseline["result"], result)
            result["delta"]["comparison"] = "counterfactual versus unmodified build of this same source"
        elif node["parents"] and not request.get("_no_delta"):
            parent_id = node["parents"][0]
            parent = session.evaluation(parent_id, options=options)
            parent_context = session.context(parent_id)
            actual_context = session.context(node["id"])
            versions = ("evaluator", "scope", "fast", "structure", "trace", "semantic", "semantic_runtime", "support", "compiler", "headers", "symbols", "elf")
            environment_changed = any(parent_context.get(k) != actual_context.get(k) for k in versions)
            same_source = session.node(parent_id)["source_hash"] == node["source_hash"]
            if same_source and environment_changed:
                result["delta"] = {"source_unchanged": True, "environment_changed": True}
            else:
                if not parent or environment_changed:
                    baseline, parent, _ = evaluate(session, parent_id, dict(request, _no_delta=True, force=False))
                    parent_id = baseline["id"]
                result["delta"] = decision_delta(parent["result"], result)
                if parent_id != node["parents"][0]:
                    result["delta"]["baseline_refreshed"] = parent_id[:12]
        record = session.evaluate(node["id"], result, options=options)
    finally:
        session.release(node["id"], options=lease_options)
    if result.get("exact"):
        session.set_frontier(node["id"], "solved", reason="Unmodified build bytes equal ROM bytes.")
    # A failed semantic trial is evidence against the draft; keep it inspectable.
    if result.get("semantic", {}).get("verdict") == "DIFFERENT":
        session.note("Differential execution found a counterexample.", node["id"], kind="counterexample",
                     data=result["semantic"])
    return node, record, False


def dispatch(request):
    from search_session import Session
    command = request["command"]
    if command == "ping":
        return {"ready": True, "pid": os.getpid()}
    path = location(request.get("session", "default"))
    path.parent.mkdir(parents=True, exist_ok=True)
    with Session(path) as session:
        if command == "init":
            source_path = Path(request["source"]).resolve()
            source = source_path.read_text()
            evaluator = environment(path)
            function = request.get("function") or evaluator.scope.source_function(str(source_path))
            origin = os.path.relpath(source_path, ROOT)
            context = evaluator.context(source, function, origin)
            node = session.initialize(function, source, context, metadata={"origin": origin})
            node, record, cached = evaluate(session, node["id"], request)
            return {**compact(node, record), "session": os.path.relpath(path, ROOT), "cache_hit": cached}
        if command == "export":
            exported = session.export(request.get("out"))
            return {"out": request["out"], "nodes": len(exported["nodes"])} if request.get("out") else exported
        ref = request.get("node", "HEAD")
        if command == "status":
            return {**compact(session.node(ref)), "metadata": session.metadata(),
                    "nodes": session.db.execute("SELECT count(*) FROM nodes").fetchone()[0],
                    "evaluations": session.db.execute("SELECT count(*) FROM evaluations").fetchone()[0]}
        if command == "frontier":
            return [compact(n) for n in session.frontier(request.get("limit"), request.get("all", False))]
        if command == "history":
            return [compact(n) for n in session.history(ref)]
        if command == "notes":
            return session.notes(None if request.get("all") else ref)
        if command == "note":
            return session.note(request["text"], ref, request.get("kind", "observation"))
        if command == "mark":
            session.set_frontier(ref, request["state"], request.get("priority", 0), request.get("reason", ""))
            return compact(session.node(ref))
        if command == "focus":
            focus = request.get("lines")
            if focus and (len(focus) != 2 or focus[0] < 1 or focus[1] < focus[0]):
                raise ValueError("focus needs an inclusive source line range LO:HI")
            session.set_metadata(focus=focus)
            return {"focus": focus, "note": "Move enumeration is restricted to edits inside these source lines."}
        if command == "protect":
            _, record, _ = evaluate(session, ref, request)
            regions = record["result"].get("structure", {}).get("regions", [])
            protected = session.metadata().get("protected_regions", {})
            if request.get("clear"):
                protected = {}
            else:
                for name in request["regions"]:
                    region = next((r for r in regions if r.get("target") == name), None)
                    if not region or region.get("local_shape") != "same" or region.get("alignment_ambiguous"):
                        raise ValueError(f"{name} must have unambiguous equal dependency shape before protecting it")
                    if request.get("mode", "shape") == "binding" and region.get("global_binding") != "same":
                        raise ValueError(f"{name} has different hard-register bindings")
                    protected[name] = request.get("mode", "shape")
            session.set_metadata(protected_regions=protected)
            return {"protected_regions": protected, "note": "Future evaluations report violations; branches remain available to the model."}
        if command == "checkout":
            node = session.checkout(ref, request.get("out"))
            return {**compact(node), "out": request.get("out")}
        if command == "diff":
            return session.diff(request.get("a", "ROOT"), request.get("b", ref))
        if command == "source":
            source = session.source(ref)
            if request.get("out"):
                Path(request["out"]).write_text(source)
                return {"out": request["out"]}
            return source
        if command == "values":
            import search_lift
            from search_eval import digest
            node, record, _ = evaluate(session, ref, request)
            implementation = getattr(search_lift, "IMPLEMENTATION_ID", digest(Path(search_lift.__file__).read_bytes()))
            options = {"implementation": implementation}
            cached = None if request.get("force") else session.evaluation(node["id"], evaluator="values", options=options)
            if cached:
                result = cached["result"]
            else:
                compiled = record["result"]
                if not compiled.get("rows"):
                    raise ValueError(compiled.get("diagnostics", "compile first"))
                entry = session.context(node["id"]).get("address")
                target = search_lift.lift(compiled["target_rows"], compiled.get("target_data", ()), entry_address=entry)
                current = search_lift.lift(compiled["rows"], compiled.get("current_data", ()), entry_address=entry)
                result = {"target": target, "current": current, "comparison": search_lift.compare(target, current)}
                session.evaluate(node["id"], result, evaluator="values", options=options)
            if request.get("details"):
                return result
            comparison = result["comparison"]
            limit = request.get("limit") or 16
            names = {a: n for n, a in environment(path).scope.inventory()[1].items()}
            def expression(value):
                return {"text": value.get("text"), "certainty": value.get("certainty")}
            def memory(items):
                return [{"at": f"+0x{m['offset']:X}", "instruction": m.get("instruction"),
                         "kind": m["kind"], "width": m["width"], "signed": m["signed"],
                         "address": expression(m["address"]), "value": expression(m["value"])} for m in items[:limit]]
            def calls(items):
                out = []
                for c in items[:limit]:
                    target = c["target"]["text"]
                    try:
                        target = names.get(int(target, 0), target)
                    except ValueError:
                        pass
                    out.append({"at": f"+0x{c['offset']:X}", "target": target,
                        "register_arguments": [expression(v) for v in c["arguments"]],
                        "stack_argument_candidates": [{"word": v["word"], "value": expression(v["value"])}
                            for v in c.get("stack_arguments", [])], "arity": "unknown"})
                return out
            return {"target_summary": result["target"]["summary"], "current_summary": result["current"]["summary"],
                    "target_only": memory(comparison["target_only"]), "current_only": memory(comparison["current_only"]),
                    "paired_addresses": len(comparison["paired"]),
                    "ambiguous_target_addresses": len(comparison["unresolved"]["target"]),
                    "ambiguous_current_addresses": len(comparison["unresolved"]["current"]),
                    "target_calls": calls(result["target"]["calls"]), "current_calls": calls(result["current"]["calls"]),
                    "target_branches": [{"at": f"+0x{b['offset']:X}", "instruction": b["instruction"],
                        "domain": b["domain"], "relation": b["relation"], "status": b["status"],
                        "arguments": [expression(v) for v in b["arguments"]]} for b in result["target"].get("branches", [])[:limit]],
                    "current_branches": [{"at": f"+0x{b['offset']:X}", "instruction": b["instruction"],
                        "domain": b["domain"], "relation": b["relation"], "status": b["status"],
                        "arguments": [expression(v) for v in b["arguments"]]} for b in result["current"].get("branches", [])[:limit]],
                    "interpretation": comparison["interpretation"]}
        if command in ("evaluate", "inspect", "regions", "compare", "experiment", "listing"):
            if command == "experiment" and not request.get("knobs"):
                raise ValueError("an experiment needs at least one compiler knob")
            node, record, cached = evaluate(session, ref, request)
            result = record["result"]
            if command == "listing":
                return "\n".join(f"{d['tag']} " + (f"{d['target'][0]:04X} {d['target'][2]}" if d['target'] else "").ljust(48) + " | " + (f"{d['current'][0]:04X} {d['current'][2]}" if d['current'] else "") for d in result.get("differences", [])[:request.get("limit")])
            if command == "inspect":
                return {"node": node["id"], **result}
            if command == "regions":
                structure = result.get("structure", {"error": result.get("diagnostics")})
                if request.get("details"):
                    return structure
                return {"summary": structure.get("summary"),
                        "regions": [r for r in structure.get("regions", []) if r.get("local_shape") != "same" or r.get("global_binding") != "same"][:request.get("limit")],
                        "actions": structure.get("actions"), "degrees_of_freedom": structure.get("degrees_of_freedom")}
            if command == "compare":
                from search_eval import decision_delta
                baseline, a, _ = evaluate(session, request.get("a", "ROOT"), {"_no_delta": True})
                return {"baseline": baseline["id"][:12], "node": node["id"][:12],
                        "delta": decision_delta(a["result"], result)}
            return {**compact(node, record), "cache_hit": cached,
                    "experiment_exact": result.get("experiment_exact", False),
                    "experiment_build": result.get("experiment_build") if request.get("knobs") else None}
        if command in ("trace", "demand"):
            from search_trace import collect, delta
            meta = session.metadata()
            evaluator = environment(path)
            evaluator.scope.WORK = str(path.parent / "compiler/traces")
            def snapshot(node_ref):
                node, _, _ = evaluate(session, node_ref, {})
                stored = session.evaluation(node["id"], evaluator="trace")
                if stored and not request.get("force"):
                    return node["id"], stored["result"]
                source_path = evaluator.materialize(session.source(node["id"]), meta["origin"])
                captured = io.StringIO()
                with contextlib.redirect_stdout(captured), contextlib.redirect_stderr(captured):
                    result = collect(evaluator.scope, str(source_path), meta["function"])
                session.evaluate(node["id"], result, evaluator="trace")
                return node["id"], result
            if command == "demand":
                from search_reload import analyze as reload_demand, compact as compact_demand, IMPLEMENTATION_ID
                node, _, _ = evaluate(session, ref, {})
                options = {"implementation": IMPLEMENTATION_ID}
                cached = None if request.get("force") else session.evaluation(node["id"], evaluator="demand", options=options)
                if cached:
                    result = cached["result"]
                    return result if request.get("details") else compact_demand(result, request.get("limit") or 6)
                source_path = evaluator.materialize(session.source(node["id"]), meta["origin"])
                captured = io.StringIO()
                with contextlib.redirect_stdout(captured), contextlib.redirect_stderr(captured):
                    trace = collect(evaluator.scope, str(source_path), meta["function"])
                    result = reload_demand(evaluator.scope, evaluator.scope.workdir(meta["function"]), meta["function"])
                session.evaluate(node["id"], trace, evaluator="trace")
                session.evaluate(node["id"], result, evaluator="demand", options=options)
                return result if request.get("details") else compact_demand(result, request.get("limit") or 6)
            parent_id, parent = snapshot(request.get("a", "ROOT"))
            current_id, current = snapshot(ref)
            changes = delta(parent, current)
            if request.get("details"):
                return {"node": current_id[:12], "baseline": parent_id[:12], "delta": changes, "trace": current}
            named = [{**a, "changes": {k: v for k, v in a["changes"].items()
                        if k in ("allocator", "priority", "refs", "live_length", "rank", "hard_register", "preferred_class")}}
                     for a in changes["allocations"] if not a["ambiguous"] and not a["key"].startswith("=")]
            return {"node": current_id[:12], "baseline": parent_id[:12],
                    "earliest_changed_pass": changes["earliest_changed_pass"],
                    "changed_passes": [p["pass"] for p in changes["changed_passes"]],
                    "allocations": [a for a in named if a["changes"]],
                    **({"values": [a for a in current["allocations"] if a.get("name") == request["argument"]]}
                        if request.get("argument", "all") != "all" else {}),
                    "ambiguous_allocation_changes": sum(a["ambiguous"] for a in changes["allocations"]),
                    "reload_changes": len(changes["reloads"]),
                    "trace_matches_build": changes["trace_matches_build"],
                    "interpretation": changes["interpretation"]}
        if command in ("why", "compiler"):
            meta = session.metadata()
            evaluator = environment(path)
            node, _, _ = evaluate(session, ref, {})
            text = evaluator.diagnose(session.source(node["id"]), meta["function"], meta["origin"],
                                      request.get("view", "why"), str(request.get("argument", "all")))
            session.note(text, node["id"], kind="compiler_evidence", data={"view": request.get("view", "why")})
            return text
        if command in ("moves", "expand"):
            from search_moves import enumerate_moves, apply_move
            source = session.source(ref)
            function = session.metadata()["function"]
            moves = enumerate_moves(source, function, line_range=session.metadata().get("focus"))
            if request.get("kinds"):
                moves = [m for m in moves if serial(m)["kind"] in request["kinds"]]
            if command == "moves":
                items = [serial(m) for m in moves][:request.get("limit")]
                if not request.get("details"):
                    items = [{k: m[k] for k in ("id", "kind", "description", "parameters", "line", "preconditions", "semantic_status")} for m in items]
                return items
            ids = request.get("ids", [])
            if ids:
                selected = [m for m in moves if any(serial(m)["id"].startswith(i) for i in ids)]
                for prefix in ids:
                    if sum(serial(m)["id"].startswith(prefix) for m in selected) != 1:
                        raise ValueError(f"move ID {prefix!r} absent or ambiguous; enumerate this node again")
            else:
                if not request.get("kinds"):
                    raise ValueError("choose explicit move IDs or --kinds; the model chooses what to expand")
                selected = moves[:request.get("limit")]
            parent = session.node(ref)["id"]
            results = []
            for move in selected:
                child = apply_move(source, move, function)
                meta = session.metadata()
                context = environment(path).context(child, function, meta["origin"])
                node = session.add(child, parent, serial(move), context=context,
                                   note=request.get("reason"), checkout=False)
                node, record, cached = evaluate(session, node["id"], request)
                results.append({**compact(node, record), "move": serial(move)["id"], "cache_hit": cached})
            return results
        if command == "import":
            source = Path(request["source"]).read_text()
            import re
            from collections import Counter
            parent = session.node(request.get("parent", ref))["id"]
            pattern = re.compile(r'\b(?:__asm__|__asm|asm)\s*(?:(?:volatile|__volatile__)\s*)?\(\s*((?:"(?:\\.|[^"\\])*"\s*)+)')
            def asm_literals(text):
                clean = re.sub(r"/\*.*?\*/|//[^\n]*", "", text, flags=re.S)
                return ["".join(re.findall(r'"((?:\\.|[^"\\])*)"', match[1])).strip()
                        for match in pattern.finditer(clean)]
            existing = Counter(asm_literals(session.source(parent)))
            for literal in asm_literals(source):
                if existing[literal]:
                    existing[literal] -= 1  # preserve existing ABI asmlabels and SDK declarations
                elif literal and not re.fullmatch(r"r(?:1[0-5]|[0-9])|sl|fp|ip|sb|lr", literal):
                    raise ValueError("handwritten instruction bodies are not matching C moves")
            meta = session.metadata()
            context = environment(path).context(source, meta["function"], meta["origin"])
            # Free edits are first-class moves, never labeled proven equivalent.
            node = session.add(source, parent, {"kind": "model_edit", "reason": request.get("reason", ""),
                "semantic_precondition": "unproven; inspect differential trials and the ROM"},
                context=context, note=request.get("reason"))
            node, record, cached = evaluate(session, node["id"], request)
            return {**compact(node, record), "cache_hit": cached}
        raise ValueError("unknown command: " + command)


def safe_dispatch(request):
    output = io.StringIO()
    try:
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            result = dispatch(request)
        return {"ok": True, "result": result, **({"log": output.getvalue()} if output.getvalue() else {})}
    except (Exception, SystemExit) as error:
        return {"ok": False, "error": str(error), "log": output.getvalue(),
                "traceback": traceback.format_exc() if os.environ.get("SEARCH_DEBUG") else None}
    finally:
        # Scope's standalone commands keep dump locks for process lifetime. Our
        # resident/batch process outlives a request, so release them at its edge.
        scope_module = sys.modules.get("scope")
        if scope_module is not None:
            for directory, fd in list(scope_module._LOCKS.items()):
                os.close(fd)
                del scope_module._LOCKS[directory]


def serve():
    """A filesystem mailbox also works in sandboxes that forbid socket binding."""
    import fcntl
    BASE.mkdir(parents=True, exist_ok=True)
    inbox = BASE / "requests"
    inbox.mkdir(exist_ok=True)
    lock = open(BASE / "serve.lock", "a")
    try:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError:
        return
    ready = BASE / "resident.json"
    loaded_version = runtime_version()
    ready.write_text(json.dumps({"pid": os.getpid(), "version": loaded_version}))
    last = time.monotonic()
    try:
        while time.monotonic() - last < 1800:
            requests = sorted(inbox.glob("*.request"), key=lambda p: p.stat().st_mtime_ns)
            for path in requests:
                last = time.monotonic()
                stopping = False
                try:
                    request = json.loads(path.read_text())
                    if request.get("command") == "stop":
                        reply, stopping = {"ok": True, "result": "stopped"}, True
                    elif loaded_version != runtime_version():
                        reply, stopping = {"ok": False, "restart": True}, True
                    elif request.pop("runtime_version", None) != loaded_version:
                        reply = {"ok": False, "restart": True}
                    else:
                        reply = safe_dispatch(request)
                except Exception as e:
                    reply = {"ok": False, "error": str(e)}
                tmp = path.with_suffix(".tmp")
                tmp.write_text(json.dumps(reply))
                tmp.replace(path.with_suffix(".reply"))
                path.unlink(missing_ok=True)
                if stopping:
                    return
            time.sleep(.01)
    finally:
        ready.unlink(missing_ok=True)
        for evaluator in EVALUATORS.values():
            evaluator.close()


def runtime_version():
    import hashlib
    # Process loads these modules lazily; restart whenever any implementation changes.
    paths = sorted(HERE.glob("search*.py")) + [HERE / "scope.py", HERE / "fast.py",
                                               ROOT / "tools/semcheck.py", ROOT / "tools/build.py", ROOT / "tools/check.py",
                                               ROOT / "tools/romlib.py"]
    return hashlib.sha256(b"".join(p.read_bytes() for p in paths)).hexdigest()


def request_server(request):
    import fcntl
    BASE.mkdir(parents=True, exist_ok=True)
    inbox = BASE / "requests"
    inbox.mkdir(exist_ok=True)

    def active():
        with open(BASE / "serve.lock", "a") as lock:
            try:
                fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
                return False
            except BlockingIOError:
                return True

    def start():
        with open(BASE / "serve.log", "a") as log:
            subprocess.Popen([sys.executable, str(Path(__file__).resolve()), "serve"], cwd=ROOT,
                stdin=subprocess.DEVNULL, stdout=log, stderr=log, start_new_session=True)

    for attempt in range(8):
        if not active():
            start()
        key = uuid.uuid4().hex
        path = inbox / (key + ".request")
        reply_path = path.with_suffix(".reply")
        tmp = path.with_suffix(".writing")
        tmp.write_text(json.dumps(dict(request, runtime_version=runtime_version())))
        tmp.replace(path)
        started, checked = time.monotonic(), time.monotonic()
        while not reply_path.exists():
            now = time.monotonic()
            if now - started > 120:
                raise RuntimeError("resident did not respond within 120 seconds; inspect build/search-sol/serve.log")
            if now - checked > .25:
                checked = now
                if not active():
                    start()  # recover a crashed resident or a startup/shutdown race
            time.sleep(.01)
        reply = json.loads(reply_path.read_text())
        reply_path.unlink()
        if not reply.get("restart"):
            return reply
        time.sleep(.05)
    raise RuntimeError("search implementation changed repeatedly during a request")


def parser():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--session", default="default")
    p.add_argument("--json", action="store_true", help="return machine-readable response envelope")
    sub = p.add_subparsers(dest="command", required=True)
    for command in ("status", "inspect", "regions", "why", "history", "notes", "evaluate", "moves", "frontier", "export", "source", "checkout", "experiment", "compiler", "mark", "focus", "compare", "listing", "trace", "demand", "values"):
        q = sub.add_parser(command)
        q.add_argument("node", nargs="?", default="HEAD")
        q.add_argument("--out")
        q.add_argument("--details", action="store_true")
        q.add_argument("--limit", type=int)
        q.add_argument("--all", action="store_true")
        q.add_argument("--trials", type=int, default=0)
        q.add_argument("--force", action="store_true")
        q.add_argument("--knob", action="append", default=[])
        q.add_argument("--kinds", nargs="+")
        q.add_argument("--view", default="why")
        q.add_argument("--argument", default="all")
        q.add_argument("--state", default="open")
        q.add_argument("--priority", type=float, default=0)
        q.add_argument("--reason", default="")
        q.add_argument("--lines", help="inclusive source line range LO:HI, or clear")
        q.add_argument("--a", default="ROOT")
    q = sub.add_parser("init")
    q.add_argument("source")
    q.add_argument("function", nargs="?")
    q.add_argument("--trials", type=int, default=0)
    for command in ("expand", "import"):
        q = sub.add_parser(command)
        q.add_argument("source" if command == "import" else "ids", nargs=None if command == "import" else "*")
        q.add_argument("--parent", default="HEAD")
        q.add_argument("--reason", default="")
        q.add_argument("--kinds", nargs="+")
        q.add_argument("--limit", type=int)
        q.add_argument("--trials", type=int, default=0)
    q = sub.add_parser("diff")
    q.add_argument("a", nargs="?", default="ROOT")
    q.add_argument("b", nargs="?", default="HEAD")
    q = sub.add_parser("note")
    q.add_argument("text")
    q.add_argument("--node", default="HEAD")
    q.add_argument("--kind", default="observation")
    q = sub.add_parser("protect")
    q.add_argument("regions", nargs="*")
    q.add_argument("--node", default="HEAD")
    q.add_argument("--mode", choices=("shape", "binding"), default="shape")
    q.add_argument("--clear", action="store_true")
    for command in ("serve", "batch", "ping", "stop"):
        sub.add_parser(command)
    return p


def main():
    os.chdir(ROOT)
    # Always use the project's installed parser, capstone and unicorn.
    try:
        import capstone  # noqa: F401
    except ImportError:
        executable = ROOT / "build/venv/bin/python"
        if executable.exists() and not os.environ.get("SEARCH_REEXEC"):
            os.environ["SEARCH_REEXEC"] = "1"
            os.execv(str(executable), [str(executable), *sys.argv])
        raise SystemExit("search needs build/venv")
    args = vars(parser().parse_args())
    if args["command"] == "serve":
        serve()
        return
    machine = args.pop("json")
    if args["command"] == "batch":
        for line in sys.stdin:
            try:
                request = json.loads(line)
                request.setdefault("session", args["session"])
                reply = safe_dispatch(request)
            except Exception as error:
                reply = {"ok": False, "error": str(error)}
            print(json.dumps(reply), flush=True)
        return
    if "knob" in args:
        args["knobs"] = dict(k.split("=", 1) for k in args.pop("knob"))
    if args.get("lines"):
        args["lines"] = None if args["lines"] == "clear" else [int(n) for n in args["lines"].split(":")]
    if args["command"] == "expand":
        args["node"] = args["parent"]
    reply = safe_dispatch(args) if os.environ.get("SCOPE_SEARCH_SERVER") == "0" else request_server(args)
    if machine:
        print(json.dumps(reply))
    elif reply["ok"]:
        value = reply["result"]
        print(value if isinstance(value, str) else json.dumps(value, indent=2))
        if reply.get("log"):
            print(reply["log"], file=sys.stderr)
    else:
        print("search: " + reply["error"], file=sys.stderr)
        if reply.get("log"):
            print(reply["log"], file=sys.stderr)
        if reply.get("traceback"):
            print(reply["traceback"], file=sys.stderr)
        raise SystemExit(1)


if __name__ == "__main__":
    main()

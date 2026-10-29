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
import difflib
import io
import json
import os
from pathlib import Path
import socket
import subprocess
import sys
import time
import traceback
import uuid

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
BASE = ROOT / "build/search-sol"
SOCKET = BASE / "serve.sock"
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
                delta=result.get("delta"), diagnostics=result.get("diagnostics") or None)


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
        return node, cached, True
    result = evaluator.evaluate(source, meta["function"], meta["origin"], options["knobs"], options["semantic_trials"])
    if result.get("rows"):
        result["structure"] = analyze(source, meta["function"], result["target_rows"], result["rows"],
            compiler_root=ROOT / "build/scope/agbcc/gcc", target_data=result.get("target_data", ()),
            current_data=result.get("current_data", ()))
    if node["parents"]:
        parent = session.evaluation(node["parents"][0], options=options)
        if parent:
            result["delta"] = decision_delta(parent["result"], result)
    record = session.evaluate(node["id"], result, options=options)
    if result.get("exact"):
        session.set_frontier(node["id"], "solved", reason="Unmodified build bytes equal ROM bytes.")
    # A failed semantic trial is evidence against the draft; keep it inspectable.
    if result.get("semantic", {}).get("verdict") == "DIFFERENT":
        session.note("Differential execution found a counterexample.", node["id"], kind="counterexample",
                     data=result["semantic"])
    return node, record, False


def focus_moves(moves, source, focus):
    if not focus:
        return moves
    lo, hi = focus
    selected = []
    for move in moves:
        data = serial(move)
        edits = data.get("edits", [])
        if edits and all(lo <= source.count("\n", 0, e["start"]) + 1 <= hi
                         and lo <= source.count("\n", 0, max(e["start"], e["end"]-1)) + 1 <= hi for e in edits):
            selected.append(move)
    return selected


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
            return session.export(request.get("out"))
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
        if command in ("evaluate", "inspect", "regions", "compare", "experiment", "listing"):
            if command == "experiment" and not request.get("knobs"):
                raise ValueError("an experiment needs at least one compiler knob")
            node, record, cached = evaluate(session, ref, request)
            result = record["result"]
            if command == "listing":
                return "\n".join(f"{d['tag']} " + (f"{d['target'][0]:04X} {d['target'][2]}" if d['target'] else "").ljust(48) + " | " + (f"{d['current'][0]:04X} {d['current'][2]}" if d['current'] else "") for d in result.get("differences", []))
            if command == "inspect":
                return {"node": node["id"], **result}
            if command == "regions":
                structure = result.get("structure", {"error": result.get("diagnostics")})
                if request.get("details"):
                    return structure
                return {"summary": structure.get("summary"),
                        "regions": [r for r in structure.get("regions", []) if r.get("local_shape") != "same" or r.get("global_binding") != "same"],
                        "actions": structure.get("actions"), "degrees_of_freedom": structure.get("degrees_of_freedom")}
            if command == "compare":
                from search_eval import decision_delta
                a = session.evaluation(request.get("a", "ROOT"), options={"knobs": {}, "semantic_trials": 0})
                if not a:
                    raise ValueError("comparison node needs a default evaluation")
                return decision_delta(a["result"], result)
            return {**compact(node, record), "cache_hit": cached,
                    "experiment_exact": result.get("experiment_exact", False)}
        if command == "trace":
            from search_trace import collect, delta
            meta = session.metadata()
            evaluator = environment(path)
            evaluator.scope.WORK = str(path.parent / "compiler/traces")
            def snapshot(node_ref):
                node, _, _ = evaluate(session, node_ref, {})
                stored = session.evaluation(node["id"], evaluator="trace")
                if stored and not request.get("force"):
                    return stored["result"]
                source_path = evaluator.materialize(session.source(node["id"]), meta["origin"])
                captured = io.StringIO()
                with contextlib.redirect_stdout(captured), contextlib.redirect_stderr(captured):
                    result = collect(evaluator.scope, str(source_path), meta["function"])
                session.evaluate(node["id"], result, evaluator="trace")
                return result
            parent = snapshot(request.get("a", "ROOT"))
            current = snapshot(ref)
            return {"node": session.node(ref)["id"][:12], "delta": delta(parent, current),
                    **({"trace": current} if request.get("details") else {})}
        if command in ("why", "compiler"):
            meta = session.metadata()
            evaluator = environment(path)
            text = evaluator.diagnose(session.source(ref), meta["function"], meta["origin"],
                                      request.get("view", "why"), str(request.get("argument", "all")))
            session.note(text, ref, kind="compiler_evidence", data={"view": request.get("view", "why")})
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
            parent = session.node(request.get("parent", ref))["id"]
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
                    elif request.pop("runtime_version", None) != loaded_version:
                        reply, stopping = {"ok": False, "restart": True}, True
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
    return hashlib.sha256(b"".join(p.read_bytes() for p in sorted(HERE.glob("search*.py")))).hexdigest()


def request_server(request):
    BASE.mkdir(parents=True, exist_ok=True)
    inbox = BASE / "requests"
    inbox.mkdir(exist_ok=True)
    for attempt in range(2):
        ready = BASE / "resident.json"
        alive = False
        if ready.exists():
            try:
                os.kill(json.loads(ready.read_text())["pid"], 0)
                alive = True
            except (ProcessLookupError, ValueError, OSError):
                pass
        if not alive:
            with open(BASE / "serve.log", "a") as log:
                subprocess.Popen([sys.executable, str(Path(__file__).resolve()), "serve"], cwd=ROOT,
                    stdin=subprocess.DEVNULL, stdout=log, stderr=log, start_new_session=True)
        key = uuid.uuid4().hex
        path = inbox / (key + ".request")
        reply_path = path.with_suffix(".reply")
        tmp = path.with_suffix(".writing")
        tmp.write_text(json.dumps(dict(request, runtime_version=runtime_version())))
        tmp.replace(path)
        started = time.monotonic()
        while not reply_path.exists():
            if time.monotonic() - started > 600:
                raise RuntimeError("resident did not respond; inspect build/search-sol/serve.log")
            time.sleep(.01)
        reply = json.loads(reply_path.read_text())
        reply_path.unlink()
        if not reply.get("restart"):
            return reply
        # Give the prior process time to release its lock before starting the next.
        for _ in range(100):
            if not ready.exists():
                break
            time.sleep(.01)
    raise RuntimeError("search implementation changed repeatedly during a request")


def parser():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--session", default="default")
    p.add_argument("--json", action="store_true", help="return machine-readable response envelope")
    sub = p.add_subparsers(dest="command", required=True)
    for command in ("status", "inspect", "regions", "why", "history", "notes", "evaluate", "moves", "frontier", "export", "source", "checkout", "experiment", "compiler", "mark", "focus", "compare", "listing", "trace"):
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

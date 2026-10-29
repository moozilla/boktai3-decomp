"""CLI and a resident JSON service. The model is the policy, not a subprocess.

One request per JSON line: {"id": 1, "op": "status", "args": {}}.
The same protocol works through a local socket or ``serve --stdio``.
"""
from __future__ import annotations

import argparse
import contextlib
import fcntl
import io
import json
import os
from pathlib import Path
import socket
import subprocess
import sys
import time
import traceback
from typing import Any

from .store import Store, digest


MAX_REQUEST = 16 * 1024 * 1024


def _socket_path(directory: Path) -> Path:
    # Keep socket names below AF_UNIX's 108-byte bound even for long sessions.
    root = Path(__file__).resolve().parents[2] / "build" / "matchsearch" / "sockets"
    root.mkdir(parents=True, exist_ok=True)
    path = root / (digest(str(directory.resolve()))[:20] + ".sock")
    if len(os.fsencode(path)) >= 104:
        raise ValueError("checkout path is too long for a Unix socket; use serve --stdio or request --local")
    return path


def _answer(session, request: dict[str, Any]) -> dict[str, Any]:
    captured = io.StringIO()
    try:
        with contextlib.redirect_stdout(captured), contextlib.redirect_stderr(captured):
            result = session.dispatch(request)
        response = {"id": request.get("id"), "ok": True, "result": result}
    except Exception as exc:
        response = {"id": request.get("id"), "ok": False,
                    "error": {"type": type(exc).__name__, "message": str(exc)}}
        if os.environ.get("MATCHSEARCH_DEBUG"):
            response["error"]["traceback"] = traceback.format_exc()
    if captured.getvalue():
        response["messages"] = captured.getvalue()[-8000:]
    return response


def _read_request(stream) -> dict[str, Any]:
    data = stream.readline(MAX_REQUEST + 1)
    if len(data) > MAX_REQUEST or data and not data.endswith(b"\n"):
        raise ValueError("request too large or missing newline")
    request = json.loads(data)
    if not isinstance(request, dict):
        raise ValueError("request must be a JSON object")
    return request


def serve(directory: Path, *, stdio: bool = False, idle_timeout: float = 1800) -> None:
    from .session import Session
    if stdio:
        with Session(directory) as session:
            for raw in sys.stdin.buffer:
                try:
                    if len(raw) > MAX_REQUEST:
                        raise ValueError("request too large")
                    request = json.loads(raw)
                    if request.get("op") == "shutdown":
                        print(json.dumps({"id": request.get("id"), "ok": True, "result": {"stopped": True}}), flush=True)
                        return
                    response = _answer(session, request)
                except Exception as exc:
                    response = {"ok": False, "error": {"type": type(exc).__name__, "message": str(exc)}}
                print(json.dumps(response), flush=True)
        return
    path = _socket_path(directory)
    lock_path = path.with_suffix(".lock")
    with lock_path.open("a+") as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError:
            return  # another caller won startup; it owns the same compiler
        # Only the process holding the lock may replace a stale socket.
        if path.exists():
            path.unlink()
        with Session(directory) as session, socket.socket(socket.AF_UNIX) as server:
            server.bind(str(path))
            os.chmod(path, 0o600)
            server.listen(16)
            server.settimeout(idle_timeout)
            try:
                while True:
                    try:
                        connection, _ = server.accept()
                    except TimeoutError:
                        return
                    with connection:
                        connection.settimeout(600)
                        request = {}
                        stop = False
                        try:
                            with connection.makefile("rb") as stream:
                                request = _read_request(stream)
                            if request.get("op") == "shutdown":
                                stop = True
                                response = {"id": request.get("id"), "ok": True, "result": {"stopped": True}}
                            else:
                                response = _answer(session, request)
                        except Exception as exc:
                            response = {"id": request.get("id"), "ok": False,
                                        "error": {"type": type(exc).__name__, "message": str(exc)}}
                        try:
                            connection.sendall(json.dumps(response).encode() + b"\n")
                        except (BrokenPipeError, ConnectionResetError):
                            pass  # result remains durable even if the caller disconnected
                    if stop:
                        return
            finally:
                if path.exists():
                    path.unlink()


def request(directory: Path, payload: dict[str, Any], *, local: bool = False) -> dict[str, Any]:
    if local:
        if payload.get("op") == "shutdown":
            return {"id": payload.get("id"), "ok": True, "result": {"already_stopped": True}}
        from .session import Session
        with Session(directory) as session:
            return _answer(session, payload)
    path = _socket_path(directory)
    directory.mkdir(parents=True, exist_ok=True)
    startup = False
    deadline = time.monotonic() + 30
    while True:
        try:
            client = socket.socket(socket.AF_UNIX)
        except PermissionError:
            response = request(directory, payload, local=True)
            response["transport"] = "local fallback; socket creation denied (serve --stdio stays resident)"
            return response
        client.settimeout(600)
        try:
            client.connect(str(path))
            break
        except PermissionError:
            client.close()
            response = request(directory, payload, local=True)
            response["transport"] = "local fallback; socket access denied (serve --stdio stays resident)"
            return response
        except (FileNotFoundError, ConnectionRefusedError):
            client.close()
            if payload.get("op") == "shutdown":
                return {"id": payload.get("id"), "ok": True, "result": {"already_stopped": True}}
            if not startup:
                log = directory / "server.log"
                with log.open("ab") as output:
                    subprocess.Popen([sys.executable, "-m", "tools.matchsearch", "serve", str(directory)],
                        cwd=Path(__file__).resolve().parents[2], stdin=subprocess.DEVNULL,
                        stdout=output, stderr=output, start_new_session=True)
                startup = True
            if time.monotonic() > deadline:
                details = (directory / "server.log").read_text(errors="replace")[-3000:]
                raise RuntimeError("search server failed to start: " + details)
            time.sleep(0.05)
    with client:
        client.sendall(json.dumps(payload).encode() + b"\n")
        client.shutdown(socket.SHUT_WR)
        with client.makefile("rb") as stream:
            line = stream.readline()
        if not line:
            raise RuntimeError("search server closed without a result; inspect server.log and durable node state")
        return json.loads(line)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    init = sub.add_parser("init", help="create one persistent target session")
    init.add_argument("directory", type=Path)
    init.add_argument("source", type=Path)
    init.add_argument("function")
    init.add_argument("--repo", type=Path, default=Path(__file__).resolve().parents[2])
    init.add_argument("--label", default="baseline")
    history = sub.add_parser("history", help="export portable sources/hypotheses without needing the old compiler")
    history.add_argument("directory", type=Path)
    history.add_argument("output", type=Path)
    fork = sub.add_parser("fork", help="carry sources into a fresh physical compiler context")
    fork.add_argument("previous", type=Path)
    fork.add_argument("directory", type=Path)
    fork.add_argument("--repo", type=Path)
    fork.add_argument("--source", type=Path)
    load = sub.add_parser("import", help="import a portable source history and compile its root")
    load.add_argument("history", type=Path)
    load.add_argument("directory", type=Path)
    load.add_argument("--repo", type=Path, default=Path(__file__).resolve().parents[2])
    load.add_argument("--source", type=Path)
    query = sub.add_parser("request", help="send a JSON request; starts a resident compiler automatically")
    query.add_argument("directory", type=Path)
    query.add_argument("json", nargs="?")
    query.add_argument("--file", type=Path)
    query.add_argument("--local", action="store_true", help="one-shot execution without the socket")
    proposal = sub.add_parser('propose', help='evaluate a C source file with a short hypothesis')
    proposal.add_argument('directory', type=Path)
    proposal.add_argument('source', type=Path)
    proposal.add_argument('--hypothesis', required=True)
    proposal.add_argument('--parent', default='root')
    proposal.add_argument('--label', default='')
    proposal.add_argument('--local', action='store_true')
    diff = sub.add_parser('diff', help='readable production/ROM instruction diff with truthful C annotations')
    diff.add_argument('directory', type=Path)
    diff.add_argument('node', nargs='?', default='root')
    diff.add_argument('--limit', type=int, default=100)
    diff.add_argument('--context', type=int, default=2)
    diff.add_argument('--no-annotations', action='store_true', help='skip the optional debug/source-line compile')
    export = sub.add_parser('export', help='write the stored source verbatim, preserving its comments and macros')
    export.add_argument('directory', type=Path)
    export.add_argument('node')
    export.add_argument('output', type=Path)
    landing = sub.add_parser('land', help='merge an exact source into its original unit, format, check, or roll back')
    landing.add_argument('directory', type=Path)
    landing.add_argument('node')
    landing.add_argument('--dry-run', action='store_true', help='save the verified merged proposal without editing src/')
    landing.add_argument('--timeout', type=float, default=120, help='seconds allowed for each landing tool')
    for command in (proposal, diff, export, landing):
        command.add_argument('--json', dest='json_output', action='store_true', help='emit structured output')
    for name in ("status", "stop"):
        command = sub.add_parser(name)
        command.add_argument("directory", type=Path)
    service = sub.add_parser("serve")
    service.add_argument("directory", type=Path)
    service.add_argument("--stdio", action="store_true")
    service.add_argument("--idle-timeout", type=float, default=1800)
    args = parser.parse_args(argv)
    try:
        if args.command == "init":
            from .session import Session
            with Session.create(args.directory, repo=args.repo, function=args.function,
                                source_path=args.source, label=args.label) as session:
                result = {"ok": True, "result": session.status()}
        elif args.command == "history":
            with Store(args.directory) as store:
                history = store.history()
            args.output.parent.mkdir(parents=True, exist_ok=True)
            args.output.write_text(json.dumps(history, indent=2) + "\n")
            result = {"ok": True, "result": {"path": str(args.output.resolve()), "nodes": len(history["nodes"])}}
        elif args.command in {"fork", "import"}:
            from .session import Session
            session = (Session.fork(args.previous, args.directory, repo=args.repo, source_path=args.source)
                       if args.command == "fork" else Session.from_history(args.directory,
                       json.loads(args.history.read_text()), repo=args.repo, source_path=args.source))
            with session:
                result = {"ok": True, "result": session.status()}
        elif args.command == "serve":
            serve(args.directory.resolve(), stdio=args.stdio, idle_timeout=args.idle_timeout)
            return 0
        elif args.command == "request":
            payload = json.loads(args.file.read_text() if args.file else args.json or sys.stdin.read())
            result = request(args.directory.resolve(), payload, local=args.local)
        elif args.command == 'propose':
            result = request(args.directory.resolve(), {'op': 'propose', 'args': {
                'source': args.source.read_text(), 'parent': args.parent,
                'hypothesis': args.hypothesis, 'label': args.label}}, local=args.local)
        elif args.command in {'diff', 'export', 'land'}:
            from .session import Session
            from . import worker
            with Session(args.directory) as session:
                if args.command == 'diff':
                    payload = worker.machine_diff(session, args.node, limit=args.limit, context=args.context,
                                                  annotations=not args.no_annotations)
                elif args.command == 'export':
                    payload = session.export(args.node, args.output)
                else:
                    payload = worker.land(session, args.node, dry_run=args.dry_run, timeout=args.timeout)
            result = {'ok': True, 'result': payload}
        else:
            result = request(args.directory.resolve(), {"op": "shutdown" if args.command == "stop" else "status"})
        if args.command in {'propose', 'diff', 'export', 'land'} and not args.json_output:
            from . import worker
            if not result.get('ok', True):
                print('matchsearch: ' + str(result['error'].get('message', result['error'])), file=sys.stderr)
                return 1
            value = result['result']
            if args.command == 'propose':
                print(worker.summary(value))
            elif args.command == 'diff':
                print(value['text'])
            elif args.command == 'export':
                print('Source exported verbatim: ' + value['path'])
            else:
                print(('Verified proposal saved; source unchanged: ' if value['dry_run'] else 'Unit MATCH; landed: ')
                      + (value['evidence_directory'] if value['dry_run'] else value['path']))
                print('Evidence: ' + value['evidence_directory'])
                print('Concessions: ' + ', '.join(f'{k}={v}' for k, v in value['concessions'].items()))
            return 0
        print(json.dumps(result, indent=2))
        return 0 if result.get("ok", True) else 1
    except Exception as exc:
        if args.command in {'propose', 'diff', 'export', 'land'} and not args.json_output:
            print(f'matchsearch: {type(exc).__name__}: {exc}', file=sys.stderr)
            return 1
        print(json.dumps({"ok": False, "error": {"type": type(exc).__name__, "message": str(exc)}}, indent=2))
        return 1


if __name__ == "__main__":
    raise SystemExit(main())

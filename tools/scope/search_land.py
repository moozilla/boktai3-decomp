"""Land an exact session node through the production campaign verifier.

This is deliberately a thin bridge, not a second source merger. Its artifacts
are ordinary campaign inputs so the existing manual recovery path still works.
"""
from pathlib import Path
import fcntl
import json
import os
import re
import signal
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def write_atomic(path, text):
    path = Path(path)
    with tempfile.NamedTemporaryFile(mode="w", dir=path.parent, prefix=".search-land-", delete=False) as stream:
        stream.write(text)
    os.replace(stream.name, path)


def run_campaign(args, **kwargs):
    """A timed-out formatter/checker must not outlive source restoration."""
    timeout = kwargs.pop("timeout", 90)
    kwargs.pop("capture_output", None)
    with subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                          start_new_session=True, **kwargs) as process:
        try:
            stdout, stderr = process.communicate(timeout=timeout)
        except BaseException:
            # The campaign starts formatter/checker grandchildren. Killing only
            # the coordinator could let a formatter overwrite the restored unit.
            try:
                os.killpg(process.pid, signal.SIGKILL)
            except ProcessLookupError:
                pass
            process.communicate()
            raise
        return subprocess.CompletedProcess(args, process.returncode, stdout, stderr)


def production_concessions(unit):
    """Inspect what production CPP retains, including other matching functions."""
    import scope
    build = scope.build
    from search_eval import preprocessed_concessions
    saved = build.DEFINES
    try:
        # The explicit undef comes after any caller CPP flags and protects this
        # accounting from a library user that previously enabled draft builds.
        build.DEFINES = list(saved) + ["-UNONMATCHING"]
        pre = build.preprocess(unit)
    except (SystemExit, OSError, RuntimeError) as error:
        return dict(verdict="UNKNOWN", pins=None, barriers=None,
                    detail="Production preprocessing unavailable: " + str(error))
    finally:
        build.DEFINES = saved
    return preprocessed_concessions(pre)


def land(session, node, result, unit=None):
    if not result.get("exact") or result.get("experiment") or result.get("experiment_build") not in (None, "normal no-g build"):
        raise ValueError("land requires exact bytes from an unmodified build; compiler experiments cannot land")
    metadata = session.metadata()
    context = session.context(node["id"])
    origin = Path(unit or metadata["origin"])
    target = (ROOT / origin).resolve()
    try:
        relative = target.relative_to(ROOT / "src")
    except ValueError:
        raise ValueError("land needs a source unit under src/; use --unit src/UNIT.c for scratch-origin sessions") from None
    if target.suffix != ".c" or not target.is_file():
        raise ValueError("land needs an existing C source unit")
    import campaign
    source = session.source(node["id"])
    base = session.source("ROOT")
    function = metadata["function"]
    address = f"{context['address']:08X}"
    if campaign.unwrap(source, function, address) is None:
        raise ValueError("the exact candidate must retain its NONMATCHING guard for campaign landing")
    if campaign.unwrap(target.read_text(), function, address) is None:
        raise ValueError("the destination has no draft for this function; it may already be landed")
    directory = ROOT / "build/scope/campaign"
    directory.mkdir(parents=True, exist_ok=True)
    with open(directory / ".search-land.lock", "a") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        queue_path = directory / "queue.json"
        queue = json.loads(queue_path.read_text()) if queue_path.exists() else []
        rel = str(Path("src") / relative)
        row = dict(func=function, addr=address, size=context["size"], src=rel)
        previous = next((r for r in queue if r["func"] == function), None)
        if previous and previous.get("kind") == "draft":
            raise ValueError("existing campaign row uses the undrafted-function landing path; rebuild its queue entry first")
        if previous and (previous["addr"].upper() != address or previous["src"] != rel):
            raise ValueError("existing campaign queue disagrees with this function's source or address")
        if previous is None:
            queue.append(row)
            write_atomic(queue_path, json.dumps(queue, indent=2) + "\n")
        worker = directory / function
        worker.mkdir(exist_ok=True)
        # The session's immutable root, never a later edited candidate, is the
        # three-way merge base. Preserve these artifacts after failed validation.
        write_atomic(worker / "base.c", base)
        write_atomic(worker / "best_match.c", source)
        # An older campaign's revision must not override this session's base.
        revision = worker / ".main_rev"
        saved_revision = revision.read_bytes() if revision.exists() else None
        if saved_revision is not None:
            revision.unlink()
        original = target.read_bytes()
        landed_marker = worker / "landed"
        previous_landed = landed_marker.read_bytes() if landed_marker.exists() else None
        env = {k: v for k, v in os.environ.items()
               if not k.startswith(("AGBCC_", "CSE_", "SCOPE_TRANSFORM"))}
        env["PATH"] = str(ROOT / "build/venv/bin") + os.pathsep + env.get("PATH", "")
        env["SCOPE_RUN"] = "search-land-" + function
        success = False
        try:
            child = run_campaign([sys.executable, "tools/scope/campaign.py", "land", function, "."],
                                   cwd=ROOT, env=env, text=True, capture_output=True, timeout=90)
            log = (child.stdout + child.stderr).strip()
            if child.returncode != 0 or "unit MATCH" not in child.stdout:
                raise ValueError("production landing failed; source restored:\n" + log[-5000:])
            concessions = production_concessions(rel)
            marker = "// MATCHING_CONCESSION: register-pinning"
            landed = target.read_text()
            if concessions.get("pins") or concessions.get("barriers"):
                needs_marker = True
            elif concessions.get("verdict") == "UNKNOWN":
                # Lack of preprocessing evidence cannot certify a depin. Keep
                # either the pre-landing or campaign-generated unit marker.
                needs_marker = marker in original.decode() or marker in landed
            else:
                needs_marker = False
            landed = re.sub(r'^// MATCHING_CONCESSION: register-pinning\n', '', landed, flags=re.M)
            if needs_marker:
                landed = marker + "\n" + landed
            write_atomic(target, landed)
            success = True
        finally:
            if not success:
                target.write_bytes(original)
                if previous_landed is None:
                    landed_marker.unlink(missing_ok=True)
                else:
                    landed_marker.write_bytes(previous_landed)
            if saved_revision is not None:
                revision.write_bytes(saved_revision)
    session.note("Landed through campaign production unit verification.", node["id"], kind="landing",
                 data={"unit": rel, "production_unit_match": True})
    session.set_metadata(landed_node=node["id"], landed_unit=rel)
    return dict(node=node["id"][:12], unit=rel, production_unit_match=True,
                concessions=concessions, candidate_concessions=result.get("concessions"), log=log,
                note="Source is ready for a matching commit; landing does not commit or run a ROM build.")

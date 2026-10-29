#!/usr/bin/env python3
"""Crash-isolated differential execution worker; JSON input/output, no compile.

Native emulator faults must never terminate the matching resident. The parent
also imposes a wall-clock bound on this process. Runtime code stays in pipes.
"""
import json
from pathlib import Path
import sys
import resource

resource.setrlimit(resource.RLIMIT_CORE, (0, 0))

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
sys.path.insert(0, str(ROOT / "build/search-sol/python"))


def run(request):
    import semcheck
    source, pre, function = request["source"], request["pre"], request["function"]
    trials = request["trials"]
    if not isinstance(trials, int) or not 1 <= trials <= 10000:
        raise ValueError("semantic trials must be between 1 and 10000")
    oracle = semcheck.Oracle(request["rom_name"], trials,
        semcheck.returns_value(source, function), arity=semcheck.arities(pre),
        kinds=semcheck.param_kinds(pre, function) or semcheck.param_kinds(source, function))
    verdict, detail = oracle.check(bytes.fromhex(request["code_hex"]), request["address"])
    return dict(verdict=verdict, detail=detail, trials=trials,
                target_coverage=round(oracle.coverage, 2), proof=False,
                isolation="separate process; native faults cannot terminate the search resident")


if __name__ == "__main__":
    try:
        result = run(json.load(sys.stdin))
    except (Exception, SystemExit) as error:
        result = dict(verdict="UNAVAILABLE", trials=0, detail=str(error), proof=False)
    print(json.dumps(result), flush=True)

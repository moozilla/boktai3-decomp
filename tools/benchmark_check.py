#!/usr/bin/env python3
"""Run check.py and retain an auditable per-candidate benchmark record.

  python tools/benchmark_check.py wip/sub_XXXXXXXX.c

Logs and compiler output stay in this worker's ignored build/benchmark/.
This measures check attempts and compiler time, not model tokens or billing.
"""
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[1]


def classify(returncode, output):
    match = re.search(r"^(.+): (MATCH|MISMATCH) \((\d+) bytes at (0x[0-9A-Fa-f]+)\)$",
                      output, re.M)
    if not match:
        return {"status": "error"}
    names, status, size, address = match.groups()
    return {"status": "match" if status == "MATCH" and returncode == 0 else "mismatch",
            "functions": names.split(", "), "bytes": int(size), "address": address}


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    src = Path(sys.argv[1])
    if not src.is_absolute():
        src = ROOT / src
    source_hash = hashlib.sha256(src.read_bytes()).hexdigest()
    outdir = ROOT / "build/benchmark"
    outdir.mkdir(parents=True, exist_ok=True)
    log = outdir / "checks.jsonl"
    seq = len(log.read_text().splitlines()) + 1 if log.exists() else 1
    start = time.time()
    result = subprocess.run([sys.executable, str(ROOT / "tools/check.py"), str(src)],
                            cwd=ROOT, capture_output=True, text=True)
    elapsed = time.time() - start
    output = result.stdout + result.stderr
    transcript = outdir / f"check-{seq:03d}.txt"
    transcript.write_text(output)
    record = {"attempt": seq, "source": str(src.relative_to(ROOT)),
              "source_sha256": source_hash, "started_unix": start,
              "elapsed_seconds": elapsed, "exit_code": result.returncode,
              "transcript": str(transcript.relative_to(ROOT)),
              **classify(result.returncode, output)}
    with log.open("a") as f:
        f.write(json.dumps(record, sort_keys=True) + "\n")
    print(output, end="")
    sys.exit(result.returncode)


if __name__ == "__main__":
    main()

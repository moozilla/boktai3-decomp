#!/usr/bin/env python3
"""Check and checkpoint a candidate; no promotion, commit, or fuzzy credit.

Source, preprocessed input, recipe, and every attempt survive under ignored
build/candidates/. Export only authored source/metadata to a dedicated WIP
branch for durable retention. Never export compiler output or ROM-derived data.
"""
import argparse
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import time

import build
import check

ROOT = Path(build.ROOT)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def classify(returncode, result):
    if isinstance(result, dict):
        if returncode == 0 and result.get('status') == 'match':
            return 'match'
        if returncode == 1 and result.get('status') == 'mismatch':
            return 'mismatch'
    return 'error'


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('source', type=Path)
    p.add_argument('--target-size', type=lambda x: int(x, 0), required=True)
    p.add_argument('--hypothesis', required=True)
    p.add_argument('--next', dest='next_hypothesis', default='')
    p.add_argument('--category', default='unclassified', choices=[
        'unclassified', 'registers', 'control-flow', 'types', 'layout', 'constants'])
    p.add_argument('--validity', default='unreviewed', choices=['unreviewed', 'reviewed', 'known-problem'])
    p.add_argument('--diff', action='store_true',
                   help='show the recorded compact diff without a second compilation')
    a = p.parse_args()
    if a.target_size <= 0:
        p.error('target size must be positive')
    src = a.source.resolve()
    data = src.read_bytes()
    parent = ROOT / 'build/candidates' / sha(data)[:16]
    parent.mkdir(parents=True, exist_ok=True)
    dest = Path(tempfile.mkdtemp(prefix='attempt-', dir=parent))
    snapshot = dest / 'candidate.c'
    snapshot.write_bytes(data)
    record = dict(schema_version=1, source=str(src), source_sha256=sha(data),
                  created_utc=datetime.now(timezone.utc).isoformat(),
                  target_size=a.target_size, hypothesis=a.hypothesis,
                  next_hypothesis=a.next_hypothesis, category=a.category,
                  validity=a.validity, exact_progress_credit=0)
    record['git_head'] = subprocess.run(['git', 'rev-parse', 'HEAD'], cwd=ROOT,
                                        capture_output=True, text=True).stdout.strip()
    started = time.monotonic()
    status = 2
    try:
        # Preprocess at the original location to preserve quoted-include lookup.
        pre = build.preprocess(str(src))
        (dest / 'input.i').write_text(pre)
        record.update(preprocessed_sha256=sha(pre.encode()),
                      compiler=build.compiler_identity(build.compiler_for(str(src))),
                      flags=build.cflags_for(str(src)), preprocessor=build.CPP)
        record['tool_sha256'] = {name: sha((ROOT / 'tools' / name).read_bytes())
                                 for name in ('check.py', 'build.py', 'candidate.py')}
        record['baseline_elf_sha256'] = sha((ROOT / 'build/boktai3.elf').read_bytes())
        # The original source is checked; verify it has not changed underneath us.
        r = subprocess.run([sys.executable, str(ROOT / 'tools/check.py'), str(src),
                            '--json', '--target-size', str(a.target_size)],
                           cwd=ROOT, capture_output=True, text=True)
        (dest / 'stdout.txt').write_text(r.stdout)
        (dest / 'stderr.txt').write_text(r.stderr)
        status = r.returncode
        result = json.loads(r.stdout) if r.stdout.strip().startswith('{') else None
        record['status'] = classify(status, result)
        if record['status'] != 'error':
            record['comparison'] = result
        else:
            status = 2
        if src.read_bytes() != data or build.preprocess(str(src)) != pre:
            record['status'] = 'error'
            record['error'] = 'Source or preprocessing inputs changed during check; retry.'
            status = 2
    except (SystemExit, OSError, ValueError) as exc:
        record.update(status='error', error=str(exc))
        status = 2
    record.update(exit_code=status, elapsed_seconds=time.monotonic() - started)
    (dest / 'record.json').write_text(json.dumps(record, indent=2) + '\n')
    c = record.get('comparison', {})
    with (ROOT / 'build/candidates/index.jsonl').open('a') as f:
        f.write(json.dumps(dict(record=str(dest.relative_to(ROOT) / 'record.json'),
                               source_sha256=record['source_sha256'], status=record['status'],
                               source=str(src), functions=c.get('functions', []),
                               address=f"{c['address']:08X}" if 'address' in c else None,
                               different_bytes=c.get('different_byte_count'),
                               size_delta=c.get('size_delta'), category=a.category,
                               validity=a.validity, hypothesis=a.hypothesis,
                               next_hypothesis=a.next_hypothesis)) + '\n')
    print(f"{record['status'].upper()}: {c.get('different_byte_count', '?')} differing bytes; "
          f"size delta {c.get('size_delta', '?')}; {dest.relative_to(ROOT)}/record.json")
    if a.diff and record['status'] == 'mismatch':
        print(check.render_diff(c))
    if record['status'] == 'error':
        print(record.get('error', 'See retained stderr.txt for compiler/linker failure.'))
    sys.exit(status)


if __name__ == '__main__':
    main()

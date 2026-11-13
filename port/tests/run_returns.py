#!/usr/bin/env python3
"""Rebuild every unit affected by discarded-return repairs, then audit the full link.

Requires a previous complete --link-game object sweep. Outputs remain in build/.
This verifies compilation and ABI boundaries; it does not claim gameplay runs.
"""
import argparse
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'port/core'))
from returns import repair_returns, verify_returns
from audit import audit_link
from link_slice import find_linker


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--hardware-root')
    parser.add_argument('-j', '--jobs', type=int, default=4)
    args = parser.parse_args()
    os.chdir(ROOT)
    verify_returns(ROOT)
    out = Path('build/port/returns')
    out.mkdir(parents=True, exist_ok=True)
    bound = Path('build/port/wasm32-bound')
    response = bound / 'link.rsp'
    if not response.exists(): raise RuntimeError('Run the complete --link-game sweep first')
    saved_response = out / 'full-link.rsp'
    if not saved_response.exists(): shutil.copyfile(response, saved_response)
    baseline = out / 'baseline-link-audit.json'
    if not baseline.exists(): shutil.copyfile(bound / 'link-audit.json', baseline)
    if not (out / 'baseline-compile.json').exists():
        shutil.copyfile(bound / 'compile.json', out / 'baseline-compile.json')

    sources = sorted((ROOT / 'src').rglob('*.c'))
    nodes = sources + list((ROOT / 'include').rglob('*.h'))
    dependents = {}
    changed = set()
    for path in nodes:
        text = path.read_text()
        if repair_returns(text) != text: changed.add(path)
        for include in re.findall(r'^\s*#\s*include\s*[<"]([^>"]+)', text, re.M):
            for candidate in (path.parent / include, ROOT / 'include' / include):
                if candidate.is_file():
                    dependents.setdefault(candidate.resolve(), set()).add(path)
                    break
    pending = list(changed)
    while pending:
        for parent in dependents.get(pending.pop(), ()):
            if parent not in changed:
                changed.add(parent)
                pending.append(parent)
    units = [path for path in sources if path in changed]
    (out / 'affected.json').write_text(json.dumps([str(p.relative_to(ROOT)) for p in units], indent=2) + '\n')
    print(f'{len(units)} affected game units; compiling their generated portable inputs', flush=True)
    command = [sys.executable, 'port/build.py', '--link-game', '--compile-only', '-j', str(args.jobs)]
    if args.hardware_root: command += ['--hardware-root', args.hardware_root]
    for path in units: command += ['--source', str(path.relative_to(ROOT))]
    subprocess.run(command, check=True)
    parts = shlex.split(saved_response.read_text())
    parts[parts.index('-o') + 1] = str(out / 'game.wasm')
    if '--fatal-warnings' not in parts: parts.append('--fatal-warnings')
    final_response = out / 'checked-link.rsp'
    final_response.write_text('\n'.join(shlex.quote(part) for part in parts) + '\n')
    result = subprocess.run([find_linker(), '@' + str(final_response)], capture_output=True, text=True)
    (out / 'link.log').write_text(result.stdout + result.stderr)
    audit_link(out / 'link.log', out / 'strict-link-audit.json')
    # LLD stops after fatal signature diagnostics, before resolving undefined
    # symbols. Collect those separately; never retain the potentially trapping
    # diagnostic module or treat its success as a runnable-game result.
    diagnostic = [part for part in parts if part != '--fatal-warnings']
    diagnostic_module = out / 'diagnostic-game.wasm'
    diagnostic[diagnostic.index('-o') + 1] = str(diagnostic_module)
    diagnostic_response = out / 'diagnostic-link.rsp'
    diagnostic_response.write_text('\n'.join(shlex.quote(part) for part in diagnostic) + '\n')
    diagnostic_result = subprocess.run([find_linker(), '@' + str(diagnostic_response)],
                                       capture_output=True, text=True)
    diagnostic_module.unlink(missing_ok=True)
    (out / 'diagnostic-link.log').write_text(diagnostic_result.stdout + diagnostic_result.stderr)
    audit = audit_link(out / 'diagnostic-link.log', out / 'link-audit.json')
    before = json.loads(baseline.read_text())
    removed = sorted(set(before['signature_conflicts']) - set(audit['signature_conflicts']))
    added = sorted(set(audit['signature_conflicts']) - set(before['signature_conflicts']))
    summary = {'affected_units':len(units), 'conflicts_before':len(before['signature_conflicts']),
               'conflicts_after':len(audit['signature_conflicts']), 'removed':removed, 'added':added,
               'undefined_symbols':audit['undefined_symbols'], 'link_exit':result.returncode}
    (out / 'result.json').write_text(json.dumps(summary, indent=2) + '\n')
    print(json.dumps(summary, indent=2))
    assert not added, 'Return repairs introduced a new signature conflict'
    assert removed, 'No complete signature conflicts were resolved'
    assert audit['undefined_symbols'] == before['undefined_symbols'], 'Missing-symbol set changed'


if __name__ == '__main__': main()

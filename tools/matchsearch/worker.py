"""Small text views and transactional local landing for matching workers.

The session remains the source of truth. These conveniences neither choose
source edits nor substitute graph similarity for production byte identity.
"""
from __future__ import annotations

import contextlib
import fcntl
import importlib.util
import io
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import stat
import subprocess
import sys
import tempfile
import time
from typing import Any

from .evaluator import _concessions, _without_comments, clean_environment
from .moves import MoveError, _aliases, _function_span
from .store import digest


def summary(result: dict[str, Any]) -> str:
    evaluation = result.get('evaluation') or {}
    lines = [f"Node {result.get('id', result.get('node', '?'))}"]
    if evaluation:
        verdict = 'MATCH' if evaluation.get('exact') else evaluation.get('status', 'pending')
        lines.append(f"{verdict}: {evaluation.get('candidate_size', '?')} / "
                     f"{evaluation.get('target_size', '?')} bytes; "
                     f"{evaluation.get('instruction_diff', '?')} differing scope rows")
        concessions = evaluation.get('concessions', {})
        lines.append('Concessions: ' + ', '.join(f'{key}={value}' for key, value in concessions.items()))
        if evaluation.get('error'):
            lines.append(evaluation['error'])
    change = result.get('delta', {}).get('numeric')
    if change:
        lines.append('Change: ' + ', '.join(f'{key}={value:+g}' for key, value in change.items()))
    if result.get('pending_elsewhere'):
        lines.append('Evaluation is pending in another worker.')
    return '\n'.join(lines)


def _annotations(evaluator, source: str, code: bytes, rows: list) -> tuple[dict, dict]:
    """Use scope's line/UID join only when its debug stream is identical."""
    scope = evaluator.scope
    evaluator._input.write_text(source)
    work = evaluator.workdir / 'readable-diff' / digest(source)[:16]
    captured = io.StringIO()
    with evaluator._scope_context(work=work), contextlib.redirect_stdout(captured), contextlib.redirect_stderr(captured):
        try:
            debug_code, _, info = scope.compile_draft_g(str(evaluator._input), evaluator.function, {})
            directory = Path(info['wd'])
            data = info.get('data')
            if data is None:
                data = scope.mapping_ranges(str(directory / 'u.o'), info['obj_offset'], len(debug_code))
            debug_rows = scope.decode(debug_code, evaluator.address, data)
            if debug_code != code or list(map(tuple, debug_rows)) != list(map(tuple, rows)):
                return {}, {'authority': 'withheld: debug compile differs from production',
                            'directory': str(directory), 'debug_changes_code': True}
            annotation = scope.draft_annotations(str(directory), info['obj_name'], rows,
                                                  info['obj_offset'], info.get('lines'), data)
            born = scope.born_passes(str(directory))
            for value in annotation.values():
                value['first_pass'] = born.get(value.get('uid'), ('', ''))[0]
            return annotation, {'authority': 'same bytes and decoded rows as production',
                                'directory': str(directory), 'debug_changes_code': False}
        except (Exception, SystemExit) as exc:
            return {}, {'authority': 'unavailable', 'error': str(exc),
                        'messages': captured.getvalue()[-1000:]}


def machine_diff(session, node: str = 'root', *, limit: int = 100, context: int = 2,
                 annotations: bool = True) -> dict[str, Any]:
    if not 1 <= limit <= 50000 or not 0 <= context <= 20:
        raise ValueError('diff limit must be 1..50000 and context 0..20')
    evaluated = session.evaluate(node)
    if evaluated.get('pending_elsewhere'):
        raise RuntimeError('this node is being evaluated by another worker; retry shortly')
    result = evaluated.get('evaluation') or {}
    stored = session.store.binary(result.get('code_hash', ''))
    if stored is None:
        raise ValueError('node has no compiled bytes: ' + str(result.get('error') or result.get('status')))
    code, rows = stored
    evaluator = session.evaluator
    source = session.store.source(node)
    annotation, note = ({}, {'authority': 'not requested'})
    if annotations:
        annotation, note = _annotations(evaluator, source, code, rows)
    with evaluator._scope_context():
        pairs, _ = evaluator.scope.align(evaluator.target_rows, rows)
    changed = [i for i, (_, _, tag) in enumerate(pairs) if tag != '=']
    visible = sorted({i for at in changed for i in range(max(0, at - context), min(len(pairs), at + context + 1))})
    selected = visible[:limit]
    lines = [f'{evaluator.function}  node {session.store.resolve(node)}',
             f"ROM {len(evaluator.target_code)} B / production {len(code)} B; "
             f"{'MATCH' if code == evaluator.target_code else 'not byte-identical'}; "
             f'{len(changed)} changed scope rows',
             'Alignment is a listing heuristic; it does not establish semantic correspondence.',
             'Source annotations: ' + note['authority'],
             '    ROM address / instruction                         draft offset / instruction                 C line; UID; first RTL pass']
    records = []
    source_lines = source.splitlines()
    previous = -2
    for index in selected:
        left, right, tag = pairs[index]
        rom = evaluator.target_rows[left] if left is not None else None
        draft = rows[right] if right is not None else None
        ann = annotation.get(draft[0], {}) if draft else {}
        loc = ann.get('line')
        location = f'{Path(loc[0]).name}:{loc[1]}' if loc else ''
        extra = '; '.join(part for part in (location, f"uid {ann['uid']}" if ann.get('uid') else '',
                                            ann.get('first_pass', '')) if part)
        if index != previous + 1:
            lines.append('  ...')
        left_text = f'{evaluator.address + rom[0]:08X}  {rom[2]}' if rom else ''
        right_text = f'+{draft[0]:04X}  {draft[2]}' if draft else ''
        lines.append(f'{tag:>2}  {left_text:<49} {right_text:<42} {extra}')
        excerpt = None
        if loc and Path(loc[0]).name == evaluator._input.name and 1 <= loc[1] <= len(source_lines):
            excerpt = source_lines[loc[1] - 1].strip()
            if tag != '=' and excerpt:
                lines.append('      C: ' + excerpt)
        records.append({'tag': tag, 'rom': rom, 'draft': draft, 'annotation': ann, 'source_excerpt': excerpt})
        previous = index
    if len(visible) > len(selected):
        lines.append(f'  ... {len(visible) - len(selected)} additional rows; increase --limit to inspect them.')
    if not changed and code != evaluator.target_code:
        lines.append('Decoded text agrees, but resolved bytes differ; this is not a match.')
    evaluator.validate_environment()
    return {'node': session.store.resolve(node), 'exact': code == evaluator.target_code,
            'changed_rows': len(changed), 'rows_omitted': len(visible) - len(selected),
            'annotations': note, 'rows': records, 'text': '\n'.join(lines)}


def _land_helpers(repo: Path):
    path = repo / 'tools' / 'scope' / 'land.py'
    spec = importlib.util.spec_from_file_location('_matchsearch_land_helpers', path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def _replace(path: Path, contents: bytes, mode: int) -> None:
    fd, temporary = tempfile.mkstemp(prefix='.matchsearch-', suffix='.tmp', dir=path.parent)
    try:
        with os.fdopen(fd, 'wb') as stream:
            stream.write(contents)
            stream.flush()
            os.fsync(stream.fileno())
        os.chmod(temporary, mode)
        os.replace(temporary, path)
    finally:
        Path(temporary).unlink(missing_ok=True)


def land(session, node: str, *, dry_run: bool = False, timeout: float = 120) -> dict[str, Any]:
    """Merge one exact candidate and roll back any unsuccessful unit check.

    This intentionally supports existing src/ units only. A new-unit/rename
    workflow remains scope's responsibility. No commit, push or full ROM build occurs.
    """
    evaluator = session.evaluator
    repo = evaluator.repo
    target = evaluator.source_path.resolve()
    if not target.is_relative_to(repo / 'src') or target.suffix != '.c' or not target.is_file():
        raise ValueError('local land requires a session initialized from an existing src/ C unit')
    if not 0 < timeout <= 600:
        raise ValueError('landing tool timeout must be between 0 and 600 seconds')
    lock_dir = repo / 'build' / 'matchsearch' / 'landing-locks'
    lock_dir.mkdir(parents=True, exist_ok=True)
    with (lock_dir / (digest(str(target))[:20] + '.lock')).open('a+') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        evaluator.validate_environment()
        source = session.store.source(node)
        production = evaluator.evaluate(source)
        if not production.exact or production.diagnostic_knobs or production.error:
            raise ValueError('landing requires a fresh, ordinary production byte match')
        original = target.read_bytes()
        mode = stat.S_IMODE(target.stat().st_mode)
        base = session.store.source('root')
        helper = _land_helpers(repo)
        aliases = set((*evaluator.own_names, evaluator.function))
        for text in (base, source, original.decode()):
            for name in tuple(aliases):
                aliases.update(_aliases(text, name))
        names = tuple(sorted(aliases))
        address = f'{evaluator.address:08X}'

        def unwrap(text):
            for name in names:
                candidate = helper.unwrap(text, name, address)
                if candidate is not None:
                    return candidate
            return text

        merged = helper.merge(unwrap(original.decode()), unwrap(base), unwrap(source))
        if merged is None:
            raise ValueError('source changed since the stored baseline; three-way merge conflicts')
        plain = _without_comments(merged)
        labels = '|'.join(re.escape(name) for name in (*names, 'sub_' + address))
        if re.search(r'\bINCLUDE_ASM\s*\([^;]*\b(?:' + labels + r')\b', plain):
            raise ValueError('target assembly fallback remains; unsupported NONMATCHING wrapper')
        # A unit check could otherwise pass by compiling the original assembly
        # fallback while the supposedly landed C remained behind another guard.
        command = evaluator.cpp + ['-nostdinc', '-undef']
        command += list(getattr(getattr(evaluator, 'build', None), 'DEFINES', []))
        command += ['-UNONMATCHING', '-I', 'include', '-iquote', '.', '-x', 'c', '-']
        normal = subprocess.run(command, input=merged, cwd=repo, env=clean_environment(),
                                capture_output=True, text=True, timeout=30)
        if normal.returncode:
            raise ValueError('normal-build preprocessing failed: ' + normal.stderr[-2000:])
        try:
            _function_span(normal.stdout, set(evaluator.own_names))
        except MoveError:
            raise ValueError('target C definition is not active in the normal build') from None
        expanded = evaluator.preprocess(merged)
        concessions = _concessions(expanded, names)
        if concessions['inline_assembly']:
            raise ValueError('automatic landing of instruction asm is unsupported; review SDK/original asm separately')
        marker = '// MATCHING_CONCESSION: register-pinning'
        if (concessions['register_pins'] or concessions['asm_barriers']) and marker not in merged:
            merged = marker + '\n' + merged
        # Preserve an existing unit marker: other functions may still need it.
        verification = evaluator.verify(merged)
        if not verification.get('exact'):
            raise ValueError('merged source failed independent stock-compiler byte verification: '
                             + str(verification.get('error') or verification.get('byte_mismatches')))
        evidence_dir = session.store.directory / 'landing' / f'{time.time_ns()}-{session.store.resolve(node)[:12]}'
        evidence_dir.mkdir(parents=True)
        (evidence_dir / 'before.c').write_bytes(original)
        (evidence_dir / 'proposed.c').write_text(merged)
        report = {'node': session.store.resolve(node), 'path': str(target), 'dry_run': dry_run,
                  'landed': False, 'verification': verification, 'concessions': concessions,
                  'before_sha256': digest(original), 'evidence_directory': str(evidence_dir)}
        if dry_run:
            (evidence_dir / 'result.json').write_text(json.dumps(report, indent=2) + '\n')
            return report
        formatter = repo / 'build' / 'venv' / 'bin' / 'clang-format'
        formatter_command = str(formatter) if formatter.is_file() else shutil.which('clang-format')
        if not formatter_command:
            raise FileNotFoundError('clang-format is unavailable; source has not been changed')
        if target.read_bytes() != original:
            raise RuntimeError('source changed during landing preparation; retry from the new source')
        env = clean_environment()
        env['AGBCC'] = str(evaluator.production_compiler)
        env['CPP'] = shlex.join(evaluator.cpp)
        successful = False
        try:
            _replace(target, merged.encode(), mode)
            for label, command in (
                    ('format', [formatter_command, '-i', str(target)]),
                    ('unit_check', [sys.executable, str(repo / 'tools' / 'check.py'), str(target), '--quiet'])):
                process = subprocess.run(command, cwd=repo, env=env, capture_output=True, text=True, timeout=timeout)
                (evidence_dir / (label + '.log')).write_text(process.stdout + process.stderr)
                if process.returncode:
                    raise RuntimeError(f'{label} failed ({process.returncode}); original source restored; '
                                       f'see {evidence_dir / (label + ".log")}')
            evaluator.validate_environment()
            final = target.read_bytes()
            (evidence_dir / 'landed.c').write_bytes(final)
            report.update(landed=True, after_sha256=digest(final), unit_check='MATCH')
            (evidence_dir / 'result.json').write_text(json.dumps(report, indent=2) + '\n')
            session.store.add_evidence(node, 'local_landing', report)
            successful = True
            return report
        except BaseException as exc:
            report.update(landed=False, restored=True, error=f'{type(exc).__name__}: {exc}')
            raise
        finally:
            if not successful:
                _replace(target, original, mode)
                with contextlib.suppress(OSError):
                    (evidence_dir / 'result.json').write_text(json.dumps(report, indent=2) + '\n')

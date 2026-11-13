"""Summarize wasm linker boundaries without hiding warnings or importing stubs."""
import json
from pathlib import Path
import re


def audit_link(log, destination):
    warnings = []
    errors = []
    text = Path(log).read_text()
    for match in re.finditer(r'wasm-ld: (warning|error): ([^\n]+)(.*?)(?=\nwasm-ld:|\Z)', text, re.S):
        severity, summary, detail = match.groups()
        item = {'summary': summary, 'detail': detail.strip()}
        (warnings if severity == 'warning' else errors).append(item)
    result = {'warnings': warnings, 'errors': errors,
              'signature_conflicts': sorted(set(re.findall(r'function signature mismatch: (\w+)', text))),
              'undefined_symbols': sorted(set(re.findall(r'undefined symbol: (\w+)', text)))}
    Path(destination).write_text(json.dumps(result, indent=2) + '\n')
    return result

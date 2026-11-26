#!/usr/bin/env python3
"""Run every repository Python unittest directory in its own interpreter."""
import subprocess
import sys
from pathlib import Path


def main():
    root = Path(__file__).resolve().parents[1]
    suites = sorted({path.parent.relative_to(root).as_posix()
                     for tree in ('tests', 'tools', 'port')
                     for path in (root / tree).rglob('test_*.py')})
    failed = []
    for suite in suites:
        print(f'Running unittest discovery in {suite}', flush=True)
        result = subprocess.run([sys.executable, '-m', 'unittest', 'discover', '-s', suite, *sys.argv[1:]],
                                cwd=root)
        if result.returncode:
            failed.append(suite)
    if failed:
        print('Failed test directories: ' + ', '.join(failed), file=sys.stderr)
    return bool(failed)


if __name__ == '__main__':
    raise SystemExit(main())

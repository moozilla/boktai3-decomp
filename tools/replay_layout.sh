#!/bin/sh
# Run from the target branch root. The argument is the reviewed checkout.
set -eu

if [ "$#" -ne 1 ]; then
    echo 'usage: sh tools/replay_layout.sh REVIEWED_CHECKOUT' >&2
    exit 2
fi
reviewed=$(cd "$1" && pwd -P)
if [ "$reviewed" = "$(pwd -P)" ] || [ ! -f symbols/functions.csv ]; then
    echo 'Run replay from the root of a different target checkout.' >&2
    exit 2
fi

tables='source_dirs header_dirs source_files function_renames prefix_renames type_renames function_aliases identity_corrections'
tool_files='reorg.py source_layout.py split.py promote_names.py test_all.py'
for table in $tables; do
    test -f "$reviewed/symbols/$table.csv"
done
for tool in $tool_files; do
    test -f "$reviewed/tools/$tool"
done
for table in $tables; do
    cp "$reviewed/symbols/$table.csv" "symbols/$table.csv"
done
for tool in $tool_files; do
    cp "$reviewed/tools/$tool" "tools/$tool"
done

PY=${PY:-build/venv/bin/python}
if [ ! -x "$PY" ] && ! command -v "$PY" >/dev/null 2>&1; then
    echo "Python executable unavailable: $PY" >&2
    exit 2
fi
BUILD_JOBS=2
export BUILD_JOBS

"$PY" tools/reorg.py
"$PY" tools/reorg.py --apply
"$PY" tools/reorg.py --check
"$PY" tools/reorg.py --function-renames symbols/function_renames.csv --skip-diverged --apply
"$PY" tools/reorg.py --function-prefixes symbols/prefix_renames.csv --skip-collisions --apply
make -j1 PY="$PY" disasm
"$PY" tools/promote_names.py --only-table symbols/function_aliases.csv
"$PY" tools/promote_names.py --only-table symbols/function_aliases.csv --check
"$PY" tools/reorg.py --type-renames symbols/type_renames.csv --apply
"$PY" tools/reorg.py --check
"$PY" tools/reorg.py --function-renames symbols/function_renames.csv --skip-diverged --check
"$PY" tools/reorg.py --function-prefixes symbols/prefix_renames.csv --skip-collisions --warn-types --check
"$PY" tools/reorg.py --type-renames symbols/type_renames.csv --check
make -j1 PY="$PY"

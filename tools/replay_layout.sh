#!/bin/sh
# Run from the target branch root. The first argument is the reviewed checkout.
set -eu

if [ "$#" -lt 1 ]; then
    echo 'usage: sh tools/replay_layout.sh REVIEWED_CHECKOUT [--fork COMMIT] [--branch-ref COMMIT]' >&2
    exit 2
fi
reviewed=$(cd "$1" && pwd -P)
shift
if [ "$reviewed" = "$(pwd -P)" ] || [ ! -f symbols/functions.csv ]; then
    echo 'Run replay from the root of a different target checkout.' >&2
    exit 2
fi

PY=${PY:-build/venv/bin/python}
if [ ! -x "$PY" ] && ! command -v "$PY" >/dev/null 2>&1; then
    echo "Python executable unavailable: $PY" >&2
    exit 2
fi
BUILD_JOBS=2
export BUILD_JOBS

mkdir -p build
state=$(mktemp -d build/layout-replay.XXXXXX)
trap 'code=$?; if [ "$code" -eq 0 ]; then "$PY" -c "import shutil, sys; shutil.rmtree(sys.argv[1])" "$state"; else echo "Replay state saved in $state" >&2; fi' EXIT
"$PY" "$reviewed/tools/replay_support.py" prepare "$reviewed" "$state" "$@"
"$PY" tools/reorg.py
"$PY" tools/replay_support.py place "$state"
"$PY" tools/reorg.py --check
"$PY" tools/reorg.py --function-renames symbols/function_renames.csv --skip-diverged --wrap-renames --apply
"$PY" tools/reorg.py --function-prefixes symbols/prefix_renames.csv --skip-collisions --wrap-renames --apply
make -j1 PY="$PY" disasm
"$PY" tools/promote_names.py --only-table symbols/function_aliases.csv --wrap-renames
"$PY" tools/promote_names.py --only-table symbols/function_aliases.csv --check
"$PY" tools/reorg.py --type-renames symbols/type_renames.csv --wrap-renames --apply
"$PY" tools/replay_support.py sync "$state"
if [ -f "$state/tracked-paths" ]; then
    BOKTAI3_REPLAY_PATHS="$state/tracked-paths"
    export BOKTAI3_REPLAY_PATHS
fi
"$PY" tools/reorg.py --function-renames symbols/function_renames.csv --skip-diverged --settled-references --wrap-renames --apply
"$PY" tools/reorg.py --check
"$PY" tools/reorg.py --function-renames symbols/function_renames.csv --skip-diverged --settled-references --check
"$PY" tools/reorg.py --function-prefixes symbols/prefix_renames.csv --skip-collisions --warn-types --check
"$PY" tools/reorg.py --type-renames symbols/type_renames.csv --check
make -j1 PY="$PY"

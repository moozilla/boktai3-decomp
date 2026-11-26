# Source directory migration

Tags: #tooling #process

The C directory structure follows the report tables. Separate ranges for the same family share a directory. A translation unit spanning ranges stays intact under its first range. The move retains each include's position and resolves nested headers relative to the include root. The ROM linker still orders units by address. SDK imports keep their formatting and linter treatment where upstream code and recovered wrappers share a library directory.

`tools/reorg.py` prints the plan and applies it with `--apply`. The function manifest follows the contents of C files, so paths in the tree and report can change without changing ROM symbols.

## Reviewed layout

Pass 2 applies the reviewed object and header routes. The rule tables are
`symbols/source_dirs.csv` and `symbols/header_dirs.csv`, with prior paths
and ROM ranges recorded in `symbols/source_files.csv`. The companion
[layout decisions](../docs/SOURCE_LAYOUT_DECISIONS.md) records the role
evidence. In-game verification placed the Pile Driver with objects, joined
the chase and grid-attack units to Vanargand and Nidhoggr respectively, and
distinguished Armor from Statue. `tools/reorg.py --check` verifies the settled
layout.

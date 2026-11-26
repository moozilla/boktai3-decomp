# Source layout

Tags: #tooling #process

The C and headers follow the categories and units in the [report hierarchy](REPORT_SUBSYSTEMS.md). Start at `src/<category>/<unit>/` for an object or subsystem and at the corresponding directory under `include/` for its declarations.

| Directory | Contents |
|---|---|
| `src/monsters/ghoul/`, `src/bosses/vanargand/` | Enemy and boss families |
| `src/player/`, `src/menus/pause_menu/` | Player code and individual menus |
| `src/engine/`, `src/system/` | Engine services and hardware, time and save systems |
| `src/lib/m4a/`, `src/lib/rfu/` | SDK and runtime libraries |
| `include/engine/shared/` | Engine types and helpers shared across units |
| `include/gba/` | GBA SDK headers |

Repeated report ranges for the same family share a directory. A translation unit spanning report ranges belongs to the unit containing its first function. Use lowercase snake case for directories and for new files with named functions, for example `ghoul_on_body_collision.c`. An unnamed function keeps its eight-digit ROM address in `sub_XXXXXXXX.c`. Keep upstream library filenames. Name a multi-function translation unit for the code it contains.

Put a header with the unit that owns its API and include it by its path relative to `include/`, for example `#include "monsters/ghoul/ghoul.h"`. Put shared engine headers with the relevant engine service or in `include/engine/shared/`. For a new matching draft, `scope.py new FUNC` opens a workspace and `scope.py land FUNC` places the checked C.

Find a translation unit by ROM address or build symbol:

```sh
build/venv/bin/python tools/source_layout.py 08033568 Ghoul_Create
build/venv/bin/python tools/check.py Ghoul_Create
```

`tools/source_layout.py` derives its mapping from definitions, placeholders, local aliases, and the symbol table. `tools/unit_files.py START END` lists translation units in ROM order within a range. `tools/split.py` derives link order from function addresses.

Bring the layout tooling to the branch where you want to replay the migration. Run `tools/reorg.py` from the repository root to print all moves and reference edits. Add `--apply` to perform them or `--rename` to also name single-function files from the symbol table. Repeating a migration on a migrated tree prints an empty plan.

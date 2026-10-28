# RAM objects: the inventory and shared declarations

Tags: #tooling #matching #progress

Every EWRAM/IWRAM object that C code touches has one row in `symbols/ram.csv`:

| column | meaning |
|---|---|
| `addr` | address, 8 hex digits |
| `name` | the name the sources use: `gUnk_XXXXXXXX` until it has a real one |
| `size` | bytes, hex (blank when unknown) |
| `kind` | `object`, or `interior` for a label inside another object (`03004CC0` is record 3 of `03004C30`) |
| `decl` | the one declaration, e.g. `extern u16 gDispCntShadow;` |
| `header` | the header that holds it |
| `confidence`, `notes` | how sure the name is, and what the object is |

Names in the inventory link without a rebuild: `tools/build.py` and `tools/check.py`
(and everything that uses `check.link_object`: scope, the permuter) define each named
row at its address. `gUnk_` names keep linking from their suffix.

```bash
python3 tools/ramlayout.py check
```

```bash
python3 tools/ramlayout.py show 03004BD8
```

`show` prints the row, every distinct declaration in the sources with a count, and
how many files use the object.

## Finding things by address

Each shared declaration has its address on the line above it:

```c
/* 0x03004290 */
extern u32 gRenderListIndex; /* active list index into the three render list-head pairs */
```

Go-to-definition on a name lands on the comment as well as the declaration.
`python3 tools/ramlayout.py annotate [--range LO-HI]` writes and fixes these
(it is idempotent and keeps clang-formatted headers formatted); `unify` writes
the comment for every declaration it adds, and `check` reports declarations
that lack one.

Which functions touch an address, from layoutmerge's literal-pool scan of the
ROM (run `python3 tools/layoutmerge.py` first; it writes `build/layouts/`):

```bash
python3 tools/ramlayout.py users 03004290            # row name or address; loads, stores, escapes per function
python3 tools/ramlayout.py users 03005418 --size 8   # a range, when the row has no size
python3 tools/ramlayout.py users 02000710 --pointee  # accesses through the pointer stored there
```

Symbols for debuggers and Ghidra come from `symbols/ram.csv`,
`symbols/functions.csv` and `symbols/data.csv`; no build or ROM is needed:

```bash
python3 tools/symexport.py                       # build/symbols/boktai3.sym and ghidra.tsv
python3 tools/symexport.py --sym game.sym        # `ADDR NAME` lines (no$gba, armips style)
python3 tools/symexport.py --ghidra syms.tsv --no-placeholders
```

In Ghidra, run `tools/ghidra/ImportSymbols.java` with the TSV as its argument
(see `tools/ghidra/README.md`): it names functions and RAM objects and adds the
C declaration as a plate comment. mGBA reads the symbols of `build/boktai3.elf`
directly.

## Unifying an object

Drafts declare RAM locally, each with its own spelling: `gUnk_030042E4` was
declared in 118 files as `u8 *`, in seven as `u16 *`, and in a few as `u32` or a
private struct pointer. Unifying gives the object one name and one declaration:

```bash
python3 tools/ramlayout.py unify 030042E4 --decl 'extern u8 *gPaletteBankData;' \
    --header include/render.h --note 'palette-bank data (AnimData_SelectPaletteBank)'
```

```bash
python3 tools/ramlayout.py unify --batch FILE --header include/ram.h
```

A batch file has one `ADDR | DECL | NOTE` line per object. The tool:

1. renames `gUnk_X` (and a file's own `#define NAME gUnk_X`) to the name everywhere
   in `src/` and `include/`. Before writing anything it stops if the name is
   already used for something else. A file whose macro gives another object one
   of the new names gets that macro inlined;
2. puts the declaration in the header and deletes every identical declaration in
   `.c` files, which include the header instead;
3. handles a file that declares the object differently by trying, in order:
   - the shared declaration (often the code is the same: a `u32` read where the
     header says `u8 *`);
   - the file's own declaration, renamed;
   - its own type as a cast at each use, `((struct View *)gGameState)` (only if
     the file can't avoid seeing the header);
4. leaves other headers' declarations in place, renamed. Including the target from
   a header would reach files that can't fall back;
5. compiles every `.c` file whose text changed, or which includes a changed header,
   before and after, with and without `-DNONMATCHING`. The agbcc assembly must be
   identical apart from the name. A file that can't include the header (a struct
   tag clash) falls back to a plain rename. If a file still differs, nothing is
   written;
6. clang-formats the files that were formatted before, and updates the rows.

The header must compile on its own; the tool checks that first.

Options for sweeps:

- `--cast`: a file whose declaration differs gets the shared one with its own type as a cast
  at each use, before the tool falls back to keeping its own declaration. Objects whose shared
  type changes nothing in that file are taken without a cast.
- `--range LO-HI`: only the header's rows in this address range count as shared (so an IWRAM
  sweep of `ram.h` leaves the EWRAM rows' declarations alone).
- `--confidence ''`: keep the rows' confidence (the default sets `low`).
- Re-running a header's rows (`ADDR | DECL |` lines, empty note) adopts whatever is left.
- `src/iwram_code.c` is never edited (`PROTECTED` in `ramlayout.py`); its declarations still
  count as leftovers.

Two things the tool cannot see, which the full build catches: a declaration with parentheses
(`void (*gX[])(void)`: give the type a typedef instead), and a header edit made by hand before
a run (the "before" compile then fails and the file is skipped). Commit hand edits to headers
after checking their includers, then run the sweep.

### Where declarations live

- `include/render.h`: the renderer block `03004280`-`0300523F` (OAM, BG affine
  and scroll shadows, palette fade state), named from the audited address map.
- `include/engine.h`, `include/game_state.h`: engine-wide objects already declared
  there: `gGameState`, `gTerrainMap`, `gRandomIndex`, `gEngineFlags`, ....
- `include/engine_globals.h`: the engine-wide scalars and pointers that `engine.h` and
  `runtime.h` used to declare (`gEngineFlags`, `gRandomIndex`, `gTerrainMap`, `gTerrainWidth`,
  `gTerrainHeight`, `gNextTaskId`, `gScriptEntryProcedure`). Light on purpose, so any draft
  can include it; `engine.h` and `runtime.h` include it for their users.
- `include/input.h`: the pad state `gPadState`.
- `include/rfu_compat.h` and `include/gba/m4a_internal.h`: the librfu and MP2K library objects, under
  their SDK names.
- `include/ram.h`: everything else for now. It is deliberately light: scalars,
  arrays of scalars and pointers to incomplete structs, no definitions. Any draft
  can include it without tag clashes, which `engine.h` doesn't allow for about 190
  drafts. An object moves to its subsystem's header once that header's struct is
  the agreed one.

A struct object (the camera at `030042A0`, the background records at `03004C30`)
needs its struct in the header. Each consumer then has to compile through that
struct's member names, and merging the per-file structs is the job of
[LAYOUTS.md](LAYOUTS.md). Those are unified one by one, not in batches.

## Accounting

`tools/progress.py` reports the inventory in PROGRESS.md and its summary (since
2026-10-27 no longer to decomp.dev, whose data measures are ROM data;
[DATA_TRACKING.md](DATA_TRACKING.md)). It needs no ROM:

- `total_data`: bytes of all `object` rows with a size;
- `matched_data`: objects whose header holds their declaration (unified, even if
  some files still declare their own);
- `complete_data`: those that no other file declares any more.

The denominator is the RAM that C code references by name: objects that only
assembly touches enter the inventory as functions are decompiled (re-run
`seed`, which keeps existing names, declarations and notes). Re-seed after
`make` with:

```bash
python3 tools/ramlayout.py seed
```

`--kit DIR` adds the meanings and spans from a research kit's
`layout/reviewed_address_map.json`. Sizes come from layoutmerge extents
(`build/layouts/layouts.json`, run `tools/layoutmerge.py` first), clipped at the
next object; a unified scalar or pointer gets its exact size.

## Finding names

- The drafts' `#define gSomething gUnk_X` aliases (131 addresses carried one;
  most are unified now).
- Header comments and the audited address map (`notes`).
- `tools/layoutmerge.py ADDR`: the field names each file gives the object.

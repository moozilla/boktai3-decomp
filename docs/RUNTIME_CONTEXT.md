# Runtime function context

`tools/function_context.py` attributes the emulator harness's per-mark `.exec`
files to function starts in `gen/code_sym.s`. It is a navigation aid for
reviewing code by scene or segment. Coverage records that an instruction
halfword ran at least once; it does not record call counts, time spent, or why
the code ran. A function name already present in the generated assembly comes
from the symbol database and is not a semantic conclusion made by this tool.

The harness stores one byte per ROM halfword. Bit 0 marks Thumb execution and
bit 1 marks ARM execution. A set bit at the first halfword of a function is
reported as `selected_entry_observed`; any set bit matching the declared ARM
or Thumb mode in the function's address span is
`selected_any_halfword_observed`. Thus an entry hit and execution only inside
the span remain distinct, and reserved coverage bits cannot produce a hit.
Function spans use the next recognized function start as their end, clipped at
the known data overlays `08000350–0800145C` and `0822F248–0822F648`. Coverage
in those gaps or in an address whose mode does not match a function is counted
under `coverage_diagnostics.*.unattributed_mode_halfwords` instead of being
silently assigned to the preceding function. JSON reports this by tag; CSV
repeats the diagnostic totals and boundary note on each function row.

Other embedded literal pools or data blocks may still fall inside an inferred
function span. The report includes a boundary note for this reason. Inspect
hits near such pools or unusual assembly boundaries in `gen/code_sym.s` and
`tools/asmat.py`; attribution does not prove every address in the span is
executable code.

## Run

Pass each marked coverage file as `TAG=FILE.exec`, choose one or more target
tags with `--segment`, and optionally pass baseline tags. A target function is
ranked exclusive when it was observed in the selected segments and absent from
all specified baseline segments. `other_segments` shows observations in the
other supplied tags, which helps identify common engine work.

```sh
python3 tools/function_context.py \
  --asm gen/code_sym.s \
  --coverage items=build/context/stable-menu-field/cov-menu_items.exec \
  --coverage field=build/context/stable-menu-field/cov-field.exec \
  --segment items --baseline field \
  --format csv -o build/context/items-functions.csv
```

Use `--format json` for machine-readable output. Coverage files are raw,
ROM-derived outputs and should stay in ignored build directories. The tool
requires exactly 8 MiB per `.exec` file (one byte for each halfword in the
16 MiB harness ROM window). It intentionally consumes `.exec`, not `.read`:
data reads and graphics loads do not establish that game code executed.

The tag text comes directly from each script's `mark NAME` command. Verify
that a tag names the scene actually shown by its screenshot/script; a mistagged
segment produces technically valid but misleading comparisons. Compare runs
that share the same boot/save/setup prefix when possible, and interpret the
result as a set difference in observed functions, never as a count or a source
for automatic semantic names. A shared UI, script, or scheduler function may
appear in many segments; a function observed only in one run may simply need
more playthrough coverage.

## Tests

Run the synthetic parser and ranking checks with:

```sh
python3 -m unittest discover -s tests -p 'test_function_context.py'
```

The fixtures cover all three function-start directives, entry versus interior
execution, mode filtering, overlay gaps, and selected-versus-baseline ranking.
They do not need the ROM, generated disassembly, or real coverage files.

The committed menu script labels were verified against screenshots on 2026-10-08.
Tab transitions have separate `transition_to_*` marks so steady-tab comparisons
do not absorb the next tab's loading code. Omit transition segments when
comparing menus. Local screenshots and raw coverage live under `build/context/`.

## Your assignment: phase 4 cleanup of Menus, HUD, dialogue, Link and wireless (last quarter of the list)

Make this part of the code read like the original programmers' source. Your list is
`cloud/ccl-menus-link-c.json`: the subsystems (ROM ranges from `symbols/progress_subsystems.csv`) in
Menus, HUD, dialogue, Link and wireless (last quarter of the list) whose code holds no concession, in ROM order, with how many one-function `src/fn/` files
each still has (21 subsystems). Work them in ROM order. For each subsystem:

1. **One translation unit.** Follow `docs/UNIT_WORKER.md` (read it now; it is the method):
   `build/venv/bin/python tools/unit_files.py START END` lists the files, `tools/merge_unit.py
   src/<subsystem>.c FILES... --write` merges them into `src/<subsystem>.c` with shared types in
   `include/<subsystem>.h` (or an existing header for the same object). Name the file for what the
   code is (`src/sun_gauge.c`), not for an address. A unit must cover a contiguous run of functions
   in ROM order; if the range includes a file listed in `cloud/depin_files.txt` (another worker is
   removing concessions there tonight), stop the unit before it or start after it.
2. **Real types.** Recover the shared structs from field offsets seen across the functions; no
   per-function partial structs, no `u8 pad[]` views of a struct a header defines, no
   `*(u16 *)((u8 *)p + 0x172)` where a field fits. Provisional descriptive field names are fine.
3. **Names.** Give every `sub_XXXXXXXX` in the unit a useful `Prefix_Action` name in
   `symbols/functions.csv` (one prefix per object; `confidence=low` for guesses, with the current
   interpretation in the notes column), and name locals, parameters and fields for what they hold.
   `docs/REPORT_SUBSYSTEMS.md` has the placement and naming rules. Don't run `tools/subsystems.py
   --regroup` or `make disasm` (the coordinator does after integration).
4. **Structure.** Where phase 3 accepted a `goto` loop or a duplicated private helper, look for the
   structured form or the shared helper the original more likely had; keep it only if the unit
   still matches.
5. `python3 tools/check.py src/<subsystem>.c` must MATCH (and every other unit whose header you
   changed). Then one commit for the unit: the new files, the deleted `src/fn/` files, the headers
   and `symbols/functions.csv`. Message: `Unit <name>: N files into src/<file>.c` plus a line on
   the types and names recovered.

Skip a subsystem that resists (a type change that breaks a match you can't resolve in reasonable
time): note why (for your final report) and go on. Quality over count: a unit someone can read beats
three mechanically concatenated ones. If a tool (`merge_unit.py`, `unit_files.py`, the header
helpers) gets in your way or could do more of this automatically, improve it, with a test, as its
own commit.

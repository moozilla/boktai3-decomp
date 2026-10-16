# Shiftable data: how it was proven

Tags: #tooling #assets

Every ROM pointer is a symbol, so all 13.7 MB of data can move: moving it by
64 KiB plays pixel-identical in every scripted test (`make shifttest`). That
is what lets text grow and assets change size in a mod build.

Pointer detection is heuristic (aligned words in ROM range, table shapes,
MP2K structure walking) plus evidence from the emulator: the harness records
which ROM words the program loads with `LDR`/`LDM` (pointers) versus words it
only reads as plain data (halfword loads, DMA, BIOS copies). Bulk-copy loops are
discounted. The shift test then moves all data and diffs screenshots; RAM
snapshots (`tools/ramdiff.py`) pinpoint the first divergence when it fails.
The fix procedure is in `CLAUDE.md` ("When the shift test fails").

The tests so far cover boot/setup, title, new game + intro cutscene, a
late-game save, every pause-menu tab and some field play — about 6% of the
code. Areas no test reaches may still hide a bad pointer guess; extending
`tests/` with more of the game is the way to find them.

Pointer facts live in `symbols/pointers.txt` (proven pointers) and
`symbols/nonpointer_*.txt` (ranges proven or declared not to hold pointers);
`tools/runtime_ptrs.py` folds new emulator evidence into them. Data formats
that are regenerated rather than shifted are described in [DATA.md](DATA.md).

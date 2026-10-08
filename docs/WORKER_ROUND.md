# Standard round brief (for range workers)

1. `git -C /home/user/wt/NAME merge -q main`; resolve add/add conflicts by keeping either
   version; make sure no `<<<<<<<` remains in src/fn or notes; check leftover wip/ files
   (a previous round may have been cut off). `python3 tools/build.py` must print OK.
2. Read docs/WORKER.md (consolidated tricks) and your notes/NAME.md.
3. Worklist: `python3 tools/worklist.py --range START END --max-insns 80` (when thin,
   `--max-insns 150`). Order: shape/clone families, switches, 41-80 insns, small
   leftovers, then 81-150. Skip already-logged skips unless you have a new idea.
4. Inner loop: `python3 tools/check.py wip/sub_X.c`; always confirm the final MATCH line.
   Copy into src/fn/ only on MATCH. Full `python3 tools/build.py` OK before each commit
   (batch ~5 matches per build). Never commit non-matching files, never raw ROM data
   addresses. Named functions keep their gen name.
5. After batches: `python3 tools/clones.py --port --loose --range START END`.
6. Commit after EVERY batch (sessions can be cut off by usage limits; committed work
   survives and gets merged).
7. Budget ~100 matches, then stop. ~10 attempts max per function; log skips in one
   line each in notes/NAME.md. Final report under 8 lines: count, best new tricks.

# Standard round brief (for range workers)

1. Work only in the assigned isolated checkout. The orchestrator supplies its
   branch and baseline; do not merge or reset another worker's work. Activate
   `build/venv`, record a start timestamp in `build/roundN/`, inspect leftover
   WIP and confirm the baseline build. Preserve older benchmark logs.
2. Read docs/WORKER.md (consolidated tricks) and your notes/NAME.md.
3. Worklist: `python3 tools/worklist.py --range START END --max-insns 80` (when thin,
   `--max-insns 150`). Order: shape/clone families, switches, 41-80 insns, small
   leftovers, then 81-150. Confirm every target is still unmatched before drafting;
   rewriting an existing match is not progress. Skip already-logged skips unless
   you have a new idea. Read `docs/COMPILER_REFERENCES.md` for checked community tips.
4. Inner loop: `python3 tools/check.py wip/sub_X.c`; always confirm the final MATCH line.
   Copy into src/fn/ only on MATCH. Full `python3 tools/build.py` OK before each commit
   (batch ~5 matches per build). Never commit non-matching files, never raw ROM data
   addresses. Named functions keep their gen name.
5. After batches: `python3 tools/clones.py --port --loose --range START END`.
6. After every verified build batch, commit each new function separately. The
   orchestrator reviews and merges all worker work as one integration batch;
   only that main push triggers progress Actions. Do not push without assignment.
7. Use the assigned match/time cap (initial production rounds: 15 matches for
   Luna or 25 for Sol, or roughly 15 active minutes). Do not confuse a minimum
   number of attempted targets with a stop condition. Revise promising targets
   rather than abandoning them after one mismatch; cap each at ~10 checks.
   Retain failed candidates and concise skip notes. Record actual start/end
   timestamps and waits; do not estimate elapsed time from memory.
8. Report new verified functions, emitted bytes, automatically propagated versus
   manually written matches, checks, failures, exact commit SHAs and full-build
   evidence. State any continuation or coaching. Different ranges are production
   results, not a controlled model comparison.

# Subsystem scope run, 2026-10-27

Tags: #process #tooling #matching #ram

The second run of [tools/scope/subsystem](../tools/scope/subsystem/README.md), 20:29 to 22:18 PDT,
from 47% of the weekly Codex quota to 2% (the watcher's floor). Queues in ROM order, no
per-function budget (lessons of [the first run](subsystem-run_2026-10-25.md)).

* Workers: 9 gpt-6.1-sol (one per subsystem; Monsters and Scene actors split by ROM half), 2
  gpt-6.1-terra on the other halves of those two as a trial, and 2 Sol RAM layout workers (IWRAM,
  EWRAM; [LAYOUT_BRIEF.md](../tools/scope/subsystem/LAYOUT_BRIEF.md)). At 21:01, 4 more Sol workers
  took the second halves of four queues; at 21:22 Sol replaced Terra (see the README's lessons).
* **29 functions matched**: 78.6% -> 79.7% of code, without register pinning 56.0% -> 56.6%,
  drafts 389 -> 360 ([#862](https://github.com/moozilla/boktai3-decomp/pull/862),
  [#863](https://github.com/moozilla/boktai3-decomp/pull/863)).
* **61 RAM layout commits**: RAM objects with one shared declaration 29.2% -> 74.6% of inventoried
  bytes, including `gCamera` (030042A0) and `gBgRecords[4]` (03004C30) as shared structs with every
  consumer exact ([notes/iwram-layouts_2026-10-27.md](iwram-layouts_2026-10-27.md)).
* Open from the layout workers' trees: `ramlayout.py check` reports size overlaps for
  SoundMainRAM_Buffer / gScriptEntryProcedure (030035F0 / 030039F0) and gSceneSelectionDisplaySlots /
  gLcgState (03005274 / 03005278).

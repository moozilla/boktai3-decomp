# Push20 low range

Base `5ff44e3`; branch `codex/push20-low`; exclusive range `08000000 <= addr < 08100000`.

No new exact matches or source commits. Parent ended this assignment after initial bounded triage to free a slot for a stronger retained-family attempt. No new targets after the drain instruction. No push, PR, symbols/header edits, shared-generation writes or emulator jobs.

Read README, CLAUDE, HANDOFF and WORKER, plus prior Sol and family-push notes. The seeded loose clone sweep (`tools/clones.py --port --loose --range 08000000 08100000`) completed with **0 functions ported**. Larger wholly unseeded families were ranked by aggregate normalized instruction count. Inspected AD270/C6010/D1EA0 (185 x3), 0E5B8/0F4EC (206 x2), and several mid-size bodies. AD270 has extensive derived-literal reuse; no draft or repeated failed family search. Straight-line entry AgbMain and 04940 were read but not drafted.

One fresh representative drafted: `08097BC0` (siblings `0809D0C8`, `080FAD64`, approximately 492 emitted bytes total). It initializes bytes/words, subtracts three halfword coordinates into a stack vector, calls 082215E4 with signed x/z, then fills eight bytes via CpuSet. No semantic name inferred.

Eight manual checks. Safe closest source: ignored `wip/sub_08097BC0-best.c` (164 emitted bytes, exact size). A struct containing a u32 fill scalar followed by s16[3], assigning stack-vector pointer before object-coordinate pointer, and explicit second offset local reproduce all preceding assembly. Explicitly computing destination +0x5FC before assigning the fill scalar reproduces destination setup. Only remaining difference is three adjacent final call-argument setup instructions:

```
original: mov r0,sp; mov r1,r8; ldr r2,control
candidate: ldr r2,control; mov r0,sp; mov r1,r8
```

Explicit pointer/control locals, volatile fill scalar, whole-stack address and u32[3] stack variants did not change this ordering. Original initial byte at offset 2B1 was corrected from a draft misread of 2B3 before retained near-match evidence; all current closest sources use 2B1. No accepted incorrect source.

One bounded 90-second `-j1` permuter run: `build/permute/sub_08097BC0/run-u2knmgto`; base score200, no candidate output saved. No second search, forced assembly, register bindings, raw ROM data literals, data definitions or unsafe output acceptance.

Logs are ignored under `build/push20/`: checks3–8, `97bc0-permute.txt`, final-build.txt. Only notes are committed; ignored WIP preserves candidate variants.

Final complete `build/venv/bin/python tools/build.py` succeeded and printed `build/boktai3.gba: OK` before the notes commit. Source tree remains unchanged from baseline.

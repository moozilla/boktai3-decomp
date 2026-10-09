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

## Resumed larger-body pass

Parent resumed Sol6.1/high for a broader call-heavy70–300 instruction pass, then changed the assignment to exclusive `0822A230` after initial low-range work. No repeat clone sweep or97BC0 attempt.

No accepted functions. `0802CD04` is a fresh460-byte script initializer with a64-byte retained raw Thumb suffix. Six manual drafts/checks: bitfield vector, ordered mask local, hidden third argument to2C91C and pointer-before-loop-count reproduced nearly all instructions. Closest safe source `wip/sub_0802CD04-safe.c` emits464 bytes: original derives address325 by adding198 to the live127 register; candidate loads325 from an extra literal. One90-second `-j1` search stayed score300 and saved no output. No suffix coverage or nonmatching source accepted.

`080AB294`: fresh372-byte unrolled six-call RNG/packed-vector routine. Six checks, all operations and source calls represented. Remaining table-base/mask hoisting and register allocation differ. Safe source remains ignored; no search. `08048E0C`: fresh224-byte two-palette RGB blend loop. Three checks; first draft220B preserved most setup/register allocation but shifted color components directly rather than original16-bit extension pattern. Bitfield RGB variant208B and explicit unsigned-shift variant216B changed allocation; retained only as WIP. Larger suggested bodies FA830,22878,F59E0,ABB18,F20AC,08D64 and additional straight-line candidates were inspected but not drafted. Scope switched before more earlier-range targets.

## Exclusive astronomical routine0822A230

Parent released this one1400-byte function outside the earlier range; adjacent Time_CalculateSunriseSunsetCore remains parent-owned and untouched. Complete C draft retains its six-argument signature: output/context pointer, mode, double argument, signed day, longitude and latitude doubles. Struct uses12 doubles followed by4 integer counters, solely to preserve observed offsets; no semantic naming installed.

Ten checks (including two alias-resolution diagnostics). Initial ordinary expressions evaluated all trigonometric calls before arithmetic helpers, unlike the original call sequence. Locally declared existing libgcc double arithmetic calls in nested expressions reproduced observed sequencing. Nested scopes delayed named intermediates until their actual computation, fixing stack slots. Separate latitude-radian local avoids overwriting the incoming parameter. Closest safe candidate `wip/sub_0822A230-best9.c` emits1384 bytes, frame128 rather than original144. The instruction stream/callees and all internal stack positions match through0822A4BA, apart from total-frame-dependent incoming latitude offsets. Remaining third output fraction lives inr4/r5 rather than spill78; original mode3 sum spill88 is absent, with subsequent branch/register differences. Moving the rise-field reload to the start of mode3 produced1372B and was reverted. No nonmatching source accepted.

Merged accepted parent context from `codex/push20-integrate` as explicitly authorized, including verified __ltdf2/__ledf2 linker aliases. These inherited source matches are **not worker output**. Private complete build after that merge printed `build/boktai3.gba: OK` (`build/push20/math-context-build.txt`). Ordinary double comparisons then checked successfully but did not resolve remaining allocation. No parent files were otherwise edited.

The one90-second `-j1` search improved5151→2685. Semantic audit found one useful change: a final reload of fraction3 from the output field. That field was stored earlier and never changed, so the reload preserves actual behavior. Manually reconstructed safe check11 (`wip/sub_0822A230-best11.c`) now emits1396B versus1400B and exactly matches the144-byte frame, all incoming/internal stack slots and instruction stream through0822A5B8 (aside from branch displacements to later blocks). Remaining mode2 branch bodies cross-jump to a shared date-result copy rather than the original's separate copies and rise reloads; mode3/tail allocation differs afterward. Check12 tried a harmless extra scope suggested by the generated candidate, with no effect, then reverted. Final count12 checks including two alias diagnostics; final two checks audit/reconstruct one search improvement. Original nonmatching drafts and generated output remain ignored, with no accepted source. Parent notified and assignment drained at its20-minute cap.

Final resumed complete build also printed `build/boktai3.gba: OK` (`build/push20/resumed-final-build.txt`) before the notes commit. **Net worker output remains0 functions/0 code bytes**; all context matches were inherited. No active searches or build jobs remain.

## Final A230 continuation: exact match

Parent authorized12 active minutes exclusively on the inherited Astra draft `wip/sub_0822A230-astra.c`. That source already emitted1400B with correct control flow; remaining differences were16 rise/transit stack-slot instructions and4 final temporary instructions. Astra assistance is part of this accepted result, following the original Sol mathematical draft and scope fixes above.

Baseline check confirmed the20 differences. Two ordinary-C lifetime changes solved them:

- The final calculation uses a fresh `value` local initialized in its own inner block before longitude division, instead of reusing the earlier denominator `x`. This preserves the result inr4/r5 rather than storing it into the denominator's old stack slot38. First new check eliminated4 differences.
- Declare `rise,date` in the fraction block, compute/store rise, then introduce `transit` in a nested block at its first use. This allocates rise at68 and transit at70, matching all16 affected loads/stores. No function calls, arithmetic operations, branches, field reads/writes or values changed. Second new check was **MATCH1400B**.

Whitespace-only cleanup was validated by comparing token strings; the formatted source again checked **MATCH1400B**. Four checks total in this continuation (inherited baseline, two new candidates, final formatted verification); no new permuter search. Source was absent from allsrc before acceptance. `src/fn/sub_0822A230.c` is the only new matching translation unit; this function has no trailing retained-code credit issue. No earlier-range work or0822A7A8 edit in this continuation.

Complete build printed `build/boktai3.gba: OK` before the separate one-TU matching commit (`build/push20/a230-matched-full-build.txt`). Final net worker output is **1 function /1400 emitted code bytes**, with inherited Astra assistance recorded above; accepted parent-context merge progress is excluded. No active jobs remain; parent owns combined shift/regression validation.

## Fresh script setup pass: 0802C000–0802E000

Parent authorized a bounded15-minute pass excluding its0802CD04/CED0/CEF8 work. Three fresh functions accepted, **520 emitted bytes total**: `0802C9A0`124B, `0802CA1C`112B and `0802D218`284B. All were absent from allsrc before drafting. The first two initialize neighboring object substructures through the same setup helpers; both first drafts matched. D218 checks callback fields, performs object setup and conditionally creates a packed-vector scratch object. Its second check matched after expressing the RNG table offset before the table base and keeping the byte result in an s32 local for the observed signed branch. No semantic names or metadata edits.

Four successful-target checks total: one per setup helper and two for D218. No trailing retained code after any accepted function: each complete emitted span ends at the next function start. Source matches are separate one-TU commits. Complete batch build printed `build/boktai3.gba: OK` before those commits (`build/push20/script-matched-full-build.txt`).

Retained `wip/sub_0802D44C.c`: fresh292B straight-line initializer calling the accepted setup helpers. Three manual checks; current candidate300B. Natural16-bit packed vector fields reproduce most vector operations. Remaining early zero local goes throughr0 rather thanr2, adding a move before D080; mask allocation differs and callback-field assignments materialize address offsets before function-pointer literals. Typed tail fields did not resolve these. First draft280B used a narrower zero store; revised fields restore the original whole-word masking but still need allocation/order work. No search or nonmatching source accepted. Other routines were inspected but not drafted; no broad sweep or previous97BC0 revisit.

Net worker progress including the assisted A230 match is now **4 functions /1920 emitted bytes**; inherited parent-context matches are excluded. Parent owns integration/regression and all trailing-span metadata. No active jobs remain.

# Boss matching worker: Ratatosk, Vanargand and Hresvelgr

Tags: #matching #bosses #ratatosk #vanargand #hresvelgr #compiler

**10 of the 15 assigned functions match in ordinary C.** Exact coverage of this
queue is **66.5%, a gain of 66.5 percentage points** from the supplied base
(12,640 of 19,000 bytes). The continuation added six matches and **45.7
percentage points** beyond the first pass. Five functions remain guarded drafts;
fuzzy similarity is not exact progress.

Work stopped at the owner's request to preserve at least 20% weekly quota.
The last matching-worker readings were 22% remaining. The earlier 50% floor was
explicitly waived. The requested ordinary-C restriction remained in force:
no new register pins, barriers, handwritten instruction bodies or compiler
overrides were accepted. All fifteen assigned C implementations are free of
those concessions. Broader subsection cleanup remains unfinished.

Branch: `codex/finish-bosses`, based on `a8c44f25d`. Each source commit changes
one translation unit. Exact matches remove their guards; unfinished C remains
behind `#ifdef NONMATCHING` with the original assembly fallback.

| Address | Function | ROM bytes | Result | Deciding source property |
|---|---|---:|---|---|
| 081A4AB4 | `Hresvelgr_EmitImpactExplosions` | 204 | **Exact** | Canonical sample-read operations, captured table/cursor, and one named minimum offset recover the table priority and invariant lifetime. |
| 081AA788 | `Vanargand_UpdateSweepingProjectileAttack` | 1,672 | **Exact** | Separate slot, selected and alternate indices; typed body/contact records; signed span operands and explicit RNG quantities recover the real value boundaries. |
| 081ABA64 | `Vanargand_UpdateChargeSequence` | 3,824 | **Exact** | Scoped animation inputs, typed motion/animation parts, projection operand lifetimes, branch-local palette zeros and one completion/tick path. The return flash window is correctly timer <= 6. |
| 081B0248 | `Vanargand_SpawnPartEmergenceEffects` | 376 | **Exact** | Captured table/cursor and canonical sample reads preserve the first signed remainder and second independent draw without deleting the required copy. |
| 081B03C0 | `PairedSceneActor_UpdatePlayerCapture` | 1,272 | **Exact** | Shared entry values, mask-first player-flag testing, typed motion and a typed sequence owner preserve lifetimes and argument evaluation. |
| 081B18C8 | `ActorStatus_UpdateRisingStrike` | 712 | **Exact** | Inline state setting avoids a retained field address; one motion pointer is reused across phases. |
| 081B2F38 | `Vanargand_UpdatePartOffsetStrike` | 1,240 | **Exact** | Signed Z, a shared entry zero, typed motion and named anchor/angle quantities. |
| 081B4040 | `Vanargand_EmitRadialPartEffects` | 732 | **Exact** | Canonical ZeroFill8, one shared entry pointer and vector Z operand order. |
| 081B431C | `Vanargand_UpdatePartImpact` | 1,128 | Guarded, **99.8%** | Canonical motion/owner fields and a captured position/count recover all registers. One MOV count remains before the signed type load instead of after it. |
| 081F3C68 | `Ratatosk_UpdateRadialSummonAttack` | 1,308 | **Exact** | Adjacent vector workspace, an early derived cursor and late direct address, plus a confined two-word position copy, prevent the erroneous PRE/loop address merge. |
| 081F4CF8 | `Ratatosk_UpdateDefeatSequence` | 1,064 | Guarded, **99.4%** | Shared byte angle, radius and word work values recover every register and spill. Three Y-address instructions remain different; four bytes differ. Canonical body and typed particle parameters retained. |
| 081F5290 | `Ratatosk_UpdateKnockbackRecovery` | 800 | Guarded, **78.7%** | Entry masks/table/cursor and a two-byte ROM sample record restore RNG stores/rematerialization. Corrected successful recovery to write finished at +0xAF, rather than nextState at +0xAD. Scalar lifetimes still differ. |
| 081F7FD0 | `Ratatosk_UpdatePlayerOrbitAttack` | 1,300 | **Exact** | The canonical fixed RAM table address in all three reads recovers the table register and both later reload rotations. Separate vector values and capture order remain essential. |
| 081FAE0C | `Ratatosk_InitializeFromScript` | 2,428 | Guarded, **84.4%** | Typed prefix operations, whole path records and shared palette source recover most structure. Default constant reuse, tail sharing and later camera/PRE ordering still need source recovery. |
| 081FC26C | `Ratatosk_SpawnAnimatedParticle` | 940 | Guarded, **99.6%** | Typed selected-drawable output, separate static draw pointers and captured frame index restore all other instructions/registers. One target copy is absent; draft alignment supplies a NOP. |

Names inferred from behavior remain tentative. Existing symbols are retained;
new descriptive spellings use the repository's worker-local aliases. Alias
promotion and report regeneration remain with batch integration.

## Verification

All ten exact functions passed independent stock compiler checks over their
reviewed extents with zero differing bytes. Matchsearch landing formatted and
verified the active ordinary definitions. Every newly retained guarded draft
was checked with `--nonmatching --record`; default per-unit checks confirm its
assembly fallback still matches. The initializer's retained source was not
changed during this continuation because no reviewed improvement superseded it.

Final continuation checkpoints:

| Function | Stock check artifacts | Commit |
|---|---|---|
| 081B0248 exact | `build/check/run-upnggsrj` | `f4f770723` |
| 081A4AB4 exact | `build/check/run-60tpygnl` | `e722834d3` |
| 081AA788 exact | `build/check/run-jto1zdhc` | `911303b09` |
| 081F7FD0 exact | `build/check/run-dkhv9sg2` | `7ee471a59` |
| 081ABA64 exact | `build/check/run-h8guqhxq` | `9980daeac` |
| 081F3C68 exact | `build/check/run-1q5sfffi` | `cb3ac8cd1` |
| 081B431C guarded | `build/check/run-ldfxawxt` | `0e3bbe312` |
| 081FC26C guarded | `build/check/run-j8r9mniv` | `b8e7038da` |
| 081F4CF8 guarded | `build/check/run-hxr7knib` | `a063d9c96` |
| 081F5290 guarded | `build/check/run-_v0xxl4w` | `1ce372a67` |

The requested tooling commit `02f36efc6` was cherry-picked as `3d91a28a9`.
It fixes current-name fallback in check.py and long matchsearch socket paths.
No ROM, generated assembly or emulator data was committed. No push, PR or full
build was performed. Integration must run the complete combined build and require
`build/boktai3.gba: OK`, then regenerate both PROGRESS reports. Those reports
remain unchanged in this worker branch.

## Resume matching, then cleanup

The five guarded functions remain work to finish. They are not closed as
compiler blockers. Persistent candidates and traces are local ignored artifacts
under `build/matchsearch/`; use `build/venv/bin/python` for tooling.

* **081B431C:** `Vanargand_UpdatePartImpact/resume/WORKING.md` and
  `fresh/WORKING.md`. The clean shaped baseline is exact under diagnostic
  `AGBCC_SPILL=34:1`; the retained ordinary candidate has one count/load order
  difference. Scalar setter and field-mode variants did not close it.
* **081F4CF8:** `Ratatosk_UpdateDefeatSequence/resume/WORKING.md` and `NOTES.md`.
  The remaining Y access retains a cached pointer while ROM uses a fresh SP
  address. X originates as zero-offset ADDRESSOF of a DI object. Additional
  workspace/copy attempts changed storage and were not selected.
* **081F5290:** `sub_081F5290/fresh/WORKING.md`, plus `resume/WORKING.md`.
  Owner, tick and RNG roles improved; particle speed/flags, heading scheduling,
  decay pointer copies and the staged position pointer still need recovery.
  The +0xAF semantic correction is committed.
* **081FAE0C:** `sub_081FAE0C/REPORT.md` and `resume/WORKING.md`. A shared pending
  halfword value/address recovers constant rematerialization but changes operand
  roles. The named-camera hypothesis has a separately proved PRE slot-order
  issue. No exact candidate emerged from the bounded targeted permuter run.
* **081FC26C:** `sub_081FC26C/resume/WORKING.md`. The missing copy has no current
  RTL counterpart. A consumer deleted only during late jump cleanup is a
  plausible source; that provenance is unproved. Do not insert an instruction
  body or artificial enforcement operation to fill it.

After all fifteen match, resume the requested broader pin-removal review,
shared boss structs/headers and data/Studio unification. Existing canonical
body, motion, position and sequence records have been reused in the functions
worked here, but that does not complete subsection-wide cleanup.

The most useful tooling follow-ups are tracing mixed ADDRESSOF/frame origins
through CSE/PRE/loop, explaining copies whose consumer disappears after reload,
and exposing constant-mode/local-allocation boundaries across inlined helpers.

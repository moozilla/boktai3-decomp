# Player cue worker batch — 2026-10-30

Tags: #matching #player #cue #compiler

All nine assigned functions match in ordinary C, without register pins, barriers,
instruction assembly or compiler overrides. This recovers 100% of the assigned
bytes, a gain of 100 percentage points in this queue, and adds 0.549610 percentage
points of exact game code (13,248 bytes). The continuation completed the last two
functions, adding 0.063059 percentage points (1,520 bytes). No assigned drafts or
compiler blockers remain.

Branch: `codex/finish-player-cue`, based on `a8c44f25d`. Matching commits remain
separate, with the continuation landing 0814FC80 before 08153610 and a separate
shared-header commit. Integration, the
combined ROM build and aggregate report regeneration remain with the integrator.

| Address | Function | Bytes | Result | Deciding source property |
|---|---|---:|---|---|
| 0814FC80 | Player_AdvanceScriptDestination | 804 | MATCH | Named compound mask plus a typed generic/player reference union delays pointer equivalence until CSE, enabling the final threading pass to redirect the early edge while retaining the repeated test. |
| 0814FFA4 | PlayerCue_UpdateMoveDestination | 776 | MATCH | Quantized angle supplies both sine lookups; a shared speed local preserves the multiply; a complete inline facing operation retains the byte copy and mask priority. |
| 08152174 | PlayerCue_UpdateMotionFade | 2,280 | MATCH | Canonical packed sequence control and stored increments replace three pinned playback copies; phase-local addresses replace uninitialized temporaries. |
| 08152C3C | PlayerCue_UpdateAlternateFormChange | 1,512 | MATCH | Completion-returning canonical playback, packed particle fields, stored `+= 3` comparisons and explicit alternate-mode branches. |
| 08153610 | PlayerCue_UpdateSpiralMotion | 716 | MATCH | Parameterized comparison limits in the inline counter helper create the implicit halfword mask before inlining; its secondary reload supplies the copy and fourth spill register. |
| 081543F0 | PlayerCue_UpdateSideFade | 1,792 | MATCH | Canonical packed playback removes all pinning while retaining the phase body. |
| 08155008 | PlayerCue_UpdateAlternateFade | 1,752 | MATCH | Canonical playback and a named sequence-selection field replace the uninitialized selector offset. |
| 08155D58 | PlayerCue_UpdateTimedChargePulse | 1,104 | MATCH | Correct initial hold sound, stored elapsed increment, flag operations, and an explicit completion-script field pointer allocated before the loaded procedure. |
| 081561EC | PlayerCue_UpdateTargetDash | 2,512 | MATCH | Typed aligned position copies, source-grounded relative random sampling, distinct planar/vertical pointer lifetimes, typed keyframe selection and animation/data locals shared across phases. |

The three newly descriptive names are tentative (`confidence=low`) in
`symbols/functions.csv`. The naming operation also updates the two callback
installation references at 0815C498 and 0815D5B0.

## Resolved compiler decisions

### 0814FC80: pointer equivalence between the two threading passes

The first pass ended with an 804-byte draft containing one wrong branch byte.
All physical instructions already agreed. At function offset `+0x244`, C branched
to the repeated null check at `+0x26C`; the ROM branched directly to normal sound
selection at `+0x2B0`. Moving or deleting the sound temporary repaired that target
but let CSE delete the ROM's repeated compare/branch pair.

The arrival-direction mismatch was independent: `AGBCC_NOTIE=196` on the earlier
draft proved a regmove tie, and a named wide mask with compound assignment fixed
it in ordinary C. The continuation then traced the control-flow decision through
both calls to `jump.c::thread_jumps`. The first runs before CSE1; the second runs
after CSE2. Scope's `.cse`/`.cse2` filenames hide this ordering, which explains why
its CSE-only canonical/follow probes had not identified a solution.

The final source removes the sound temporary and obtains the optional player
through `PlayerCue_ResolveReference`: a union with generic actor and typed player
pointer members. That record keeps a distinct user-variable pointer in initial
RTL. The first threading pass cannot equate it with the original pointer because
`rtx_equal_for_thread_p` refuses equivalence between distinct user-variable
pseudos. CSE then removes the copy and recognizes the pointers as identical. The
last threading pass can now redirect the early branch. No subsequent CSE pass
removes the repeated comparison, so both the destination and retained pair match.

A trace-only private compiler clone established this ordering on the initial
scalar/pointer-union prototype. The final generic/typed-pointer helper also matches
with the stock production compiler: all 804 bytes, no extra operations or stack
slots. The compiler trace and source variants remain under
`build/scope/drafts/FC80_resume_compiler/` and `build/scope/drafts/FC80_resume/`.
The tracked source is `src/fn/sub_0814FC80.c`.

The useful scope improvement is phase-specific jump-threading provenance and a
trace of the two compared predicates, rejected intervening instructions and user
pseudo equivalences. A plain alias and a union member can reach the same final
register while becoming equivalent at different compiler stages.

### 08153610: the constant's machine mode determines its reload copy

The previous typed draft was 712 bytes, with 26 ROM / 25 draft differing
instructions and shape distance five. It explicitly declared a `u16 counterMask`,
but `stmt.c::expand_decl` promoted this scalar local into an SI register. An SI
constant can load directly into r2, losing the ROM's `ldr r3, =0xFFFF` plus copy
to r2. That also kept r3 out of the whole-function reload register set, causing
unrelated-looking early address reload differences.

The already-correct sequence helper exposed the missing property: its automatic
halfword masks use HI pseudos. Thumb cannot load this pool constant directly with
an HI move. Reload inserts an SI literal load and then a halfword copy. At the
tail, that secondary reload needs r3, expanding the spill set to r0-r3 and repairing
the earlier address reloads as well. Priority and rotation interventions could
not create this mode-dependent secondary reload.

A private APFS compiler clone added a diagnostic-only
`AGBCC_DIAG_LOCAL_MODE=counterMask` intervention to retain the declared HI mode of
that one local. The unchanged former draft then matched all 716 bytes, proving
one sufficient compiler decision without disabling PRE or forcing registers.
Neither this private compiler nor its intervention is used for accepted C.

The ordinary fix is `PlayerCue_AdvanceSpiralCounters(p, trailLimit, timerLimit)`,
called with 32 and 31. Both comparisons are expanded while their limits are still
parameters, naturally creating the implicit HI masks for the stored halfword
increments. Inlining resolves the limits afterward while preserving the required
mode, copy and reload set. Both narrow and wide limit-formal experiments matched;
the final source uses `u16` limits and ordinary `+= 3` / prefix-increment syntax.
All 716 bytes pass the independent production check.

Source and compiler reports remain at
`build/scope/drafts/Spiral_resume_source/REPORT.md` and
`build/scope/Spiral_resume_compiler/REPORT.md`. The tracked source is
`src/fn/sub_08153610.c`. Scope should explain implicit-mask modes, scalar promotion
and secondary reloads before suggesting allocation priority or spill rotation.

## Shared source and recovered behavior

`include/player_cue_sequence.h` supplies ordinary sprite/sequence stepping and
its cycle-completion variant. It reuses `ContactSwarmSprite`,
`DisplaySequenceState` and `DisplaySequenceControl`. The fade callers keep the
same stored increment and duration semantics; no command reads require volatile
or register constraints. The alternate-form callback now uses those actual
sprite/sequence records and `GameState` position fields instead of raw offsets.
The charge callback reuses `ScriptUtilityArguments` for its one-value procedure
call. Existing shared declarations remain unchanged.

The target-dash draft had a reversed previous-position copy, a six-byte local
vector written as eight bytes, and duplicated initial-frame animation guards.
Aligned `ContactPosition` records, per-phase coordinate locals and a single guard
repair those source problems. Its two particle helpers follow the already matched
Dog/Golem relative-sampling family: symbolic heap/resource origins preserve the
address-to-offset boundary without a barrier. Ending the planar vector pointer's
life before sampling vertical motion gives the ROM's stack accesses.

A typed keyframe-selection union prepares the sequence index before outgoing
arguments. Keeping one phase-two animation pointer through the heading join
preserves its flags update. Sharing the main animation/data locals across phases
reproduces the final register preference; diagnostic
`AGBCC_FORCE=data#2:6,data#3:6` proved the allocation issue before the ordinary
source sharing made it exact. No intervention is retained in the source.

## Validation and integration

Every accepted C unit passed a stock compiler check at its complete reviewed
extent with zero differing bytes. The continuation checks compile actual C for
the formerly guarded units, requiring exactly 804 and 716 bytes. The two renamed
callback installers also remain exact
(100 and 216 bytes). All nine assigned units are free of pins and barriers.

`tools/check.py` cannot discover a newly descriptive function name in the worker's
older ELF. The three renamed units therefore use the same production `build_c`,
`check.link_object` and ROM-comparison path with explicit reviewed addresses; the
callback checks add the authoritative CSV names to the old ELF symbol snapshot.
This is private checking code under `build/`, with no source or compiler override.
A CSV fallback in the check CLI would remove this tooling gap.

The long managed worktree path also exceeds the platform's Unix-socket path limit
for matchsearch. `propose --local` and the direct `Session.verify` API preserve
stock verification and durable evidence without the resident socket.

No full ROM build, push, PR, subsystem regrouping or aggregate report regeneration
was performed. `PROGRESS.md` and `PROGRESS.json` stay unchanged. The integrator
must run the combined build, require `build/boktai3.gba: OK`, and regenerate both
reports before merging this batch. No ROM, generated assembly or emulator output
is committed.

## Follow-on cleanup

The owner's follow-up removes concessions from the neighboring callbacks and
consolidates the full subsection's owner/field layouts, playback, editable tables
and Studio wiring. See [the cleanup note](player-cue-cleanup_2026-10-30.md) for
accepted extents, the one remaining transition barrier mechanism and integration
requirements. The nine matches above remain exact after those shared migrations.

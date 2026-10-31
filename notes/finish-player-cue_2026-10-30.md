# Player cue worker batch — 2026-10-30

Tags: #matching #player #cue #compiler

Seven of the nine assigned functions match in ordinary C, without register pins,
barriers, instruction assembly or compiler overrides. This recovers 88.527% of the
assigned bytes, a gain of 88.527 percentage points in this queue, and adds
0.486551 percentage points of exact game code (11,728 bytes). The two remaining
functions have improved ordinary-C drafts behind their original assembly fallbacks.
They add no exact progress.

Branch: `codex/finish-player-cue`, based on `a8c44f25d`. Matching commits remain
separate, in ROM order, with a separate shared-header commit. Integration, the
combined ROM build and aggregate report regeneration remain with the integrator.

| Address | Function | Bytes | Result | Deciding source property |
|---|---|---:|---|---|
| 0814FC80 | Player_AdvanceScriptDestination | 804 | Guarded draft, 99.8%; one branch byte differs | A named `u32 mask = 7; mask &= shifted` prevents the premature two-address tie; a pre-CSE jump-threading choice remains. |
| 0814FFA4 | PlayerCue_UpdateMoveDestination | 776 | MATCH | Quantized angle supplies both sine lookups; a shared speed local preserves the multiply; a complete inline facing operation retains the byte copy and mask priority. |
| 08152174 | PlayerCue_UpdateMotionFade | 2,280 | MATCH | Canonical packed sequence control and stored increments replace three pinned playback copies; phase-local addresses replace uninitialized temporaries. |
| 08152C3C | PlayerCue_UpdateAlternateFormChange | 1,512 | MATCH | Completion-returning canonical playback, packed particle fields, stored `+= 3` comparisons and explicit alternate-mode branches. |
| 08153610 | PlayerCue_UpdateSpiralMotion | 716 | Guarded draft, 89.1%; 26 ROM / 25 draft instructions, plus two moved branches | Correct trail-resource store order and ordinary playback; the shared halfword mask still loses a required copy and changes reload availability. |
| 081543F0 | PlayerCue_UpdateSideFade | 1,792 | MATCH | Canonical packed playback removes all pinning while retaining the phase body. |
| 08155008 | PlayerCue_UpdateAlternateFade | 1,752 | MATCH | Canonical playback and a named sequence-selection field replace the uninitialized selector offset. |
| 08155D58 | PlayerCue_UpdateTimedChargePulse | 1,104 | MATCH | Correct initial hold sound, stored elapsed increment, flag operations, and an explicit completion-script field pointer allocated before the loaded procedure. |
| 081561EC | PlayerCue_UpdateTargetDash | 2,512 | MATCH | Typed aligned position copies, source-grounded relative random sampling, distinct planar/vertical pointer lifetimes, typed keyframe selection and animation/data locals shared across phases. |

The three newly descriptive names are tentative (`confidence=low`) in
`symbols/functions.csv`. The naming operation also updates the two callback
installation references at 0815C498 and 0815D5B0.

## Open compiler decisions

### 0814FC80: pre-CSE jump threading

The landed source is `src/fn/sub_0814FC80.c`. All instructions and offsets match,
except the branch at function offset `+0x244`: the ROM targets `+0x2B0` (ordinary
movement sounds), while C targets `+0x26C` (the repeated null check). The compiled
extent is exactly 804 bytes, with one differing displacement byte.

The arrival-direction mismatch was a separate regmove tie. `AGBCC_NOTIE=196`
on the original draft removed it diagnostically. The ordinary fix gives the
mask its own wide local and compound assignment, preserving the intended input
and output identities. Typed player fields retain that result.

The remaining target change occurs in `jump.c::thread_jumps`, invoked with
`before_loop=1` immediately before `cse_main`; its effects therefore appear in
the `.cse` dump. On the clean diagnostic source, branch UID 913 changes from
label 938 to 966 before `cse_insn` sees it. A scratch compile with
`-fno-thread-jumps` preserves label 938 and adds two instructions (808 bytes),
confirming the responsible pass. This flag is diagnostic only.

The sound temporary assignment at UID 970 stops the backward pattern comparison
before the repeated null test. The ROM needs the earlier edge to bypass that
assignment/test while retaining the test on the other path. Removing or moving
the temporary, or spelling an explicit normal-sound label, fixes the target but
removes the ROM's second compare/branch pair. Helper extraction, optional-player
records, helper return widths, typed aliases, wide null predicates and empty
release-debug blocks did not preserve both properties. Scope's canonical-register
and CSE-follow probes have no hit because they do not control this earlier pass.

Next scope capability: per-edge tracing and an intervention for `thread_jumps`
that identifies which intervening assignment prevents a specific redirection.
Local evidence remains in `build/scope/drafts/FC80_review/REPORT.md` and
`build/scope/drafts/FC80_branch_diag/REPORT.md`.

### 08153610: mask copy and reload register set

The landed source is `src/fn/sub_08153610.c`: typed fields and shared ordinary
playback, no matching concessions. Its 712-byte candidate differs from the
716-byte ROM by 26 ROM / 25 draft instructions, with shape distance five and
two moved branches. The default build retains the original 716-byte fallback.

The ROM loads `0xFFFF` into r3 and copies it to r2. C assigns the shared narrow
counter mask directly to r2, loses the copy, and omits r3 from the reload register
set. Earlier address reloads consequently select different registers or
rematerialize an offset. Forcing the mask into r3 does not restore the copy.
Disabling PRE replacement one for the sequence frame count makes the earlier
r3 reloads agree, proving the connection, but changes the sequence helper and
still fails to produce the full ROM. No diagnostic setting is an exact solution.

Two reviews tried narrow/wide mask locals, raw/halfword unions, repeated
assignments, counter and resource parameters, complete tail-operation helpers,
explicit duration/index values and equivalent increment boundaries. None retained
the required constant-to-user copy with the correct PRE behavior. Next scope
capability: trace and control the creation/removal of that copy separately from
reload rotation and the set of eligible reload registers. Local reports are in
`build/scope/drafts/SpiralMotion/REPORT.md` and
`build/scope/drafts/SpiralMotion_review/REPORT.md`.

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
extent with zero differing bytes. Both guarded units passed their default
assembly-fallback checks. The two renamed callback installers also remain exact
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

# Player cue cleanup — 2026-10-30

Tags: #matching #player #cue #structs #compiler #data #studio

Twelve of thirteen existing concession-bearing functions now match in ordinary C.
The subsection's ordinary-C share rises to 99.199351%, a gain of 16.793352
percentage points (6,628 bytes moved out of the Register-pinned category).
All 90 explicit register pins and 74 of 76 empty barriers are removed. The two
remaining barriers are in `Player_UpdateTransitionAnimation` (`0814FAE8`), with
one sufficient compiler decision proved below. Exact-match coverage is preserved;
this cleanup adds no new exact-code progress.

On `codex/finish-player-cue`, 106 Player cue source units are cleaned, and one
additional caller is updated for a promoted name. Every changed unit passes an
independent stock build/link and complete-extent ROM comparison. The initial
matching batch remains documented in
[the original worker note](finish-player-cue_2026-10-30.md).

## Concession removal

| Address | Function | Bytes | Result and deciding source property |
|---|---|---:|---|
| 0814FAE8 | Player_UpdateTransitionAnimation | 316 | Exact with two existing barriers; constant-first AND commutation remains blocked. Unused compatibility aliases removed. |
| 081516C0 | PlayerCue_UpdateRunTo | 684 | Ordinary C; named angle mask and typed canonical player fields retain the direction copy. |
| 08151D90 | Player_RestartAnimationFromSetup | 296 | Ordinary C; shared packed sequence stepping replaces the pinned playback copy. |
| 08151EB8 | PlayerCue_UpdateWalkTarget | 700 | Ordinary C; shared sprite/sequence operation preserves command masking and flag updates. |
| 08152A5C | PlayerCue_UpdateRecoilSlide | 480 | Ordinary C; shared playback and a typed facing-field pointer preserve the original value lifetimes. |
| 081538DC | Player_RestartAnimationFromResource | 292 | Ordinary C; shared sequence stepping. |
| 08153A00 | PlayerCue_UpdateSideWalkTarget | 1,052 | Ordinary C; complete facing helper with a named mask and a separate heading-field pointer in the lower-facing arm. |
| 08153E1C | PlayerCue_UpdateSideDeparture | 696 | Ordinary C; shared sequence stepping and canonical graphics/position fields. |
| 081540D4 | PlayerCue_UpdateSideArrival | 796 | Ordinary C; pass the existing phase value as the first flip-comparison mask in completion-aware stepping. |
| 08154AF0 | PlayerCue_UpdateSideRecoil | 484 | Ordinary C; canonical playback and typed facing storage. |
| 08154CD4 | PlayerCue_UpdateSpiralRecovery | 528 | Ordinary C; typed stored halfword increment with parameterized limits retains its implicit mask and reload copy. |
| 08154EE4 | PlayerCue_RestartAlternateAnimation | 292 | Ordinary C; shared sequence stepping. |
| 0815DEEC | PlayerCue_ApplyColliderScale | 328 | Ordinary C; capture the mask in a separate local before reading the flags, preserving its early lifetime without barriers. |

### Remaining transition decision

Removing both barriers from `0814FAE8` leaves four ROM/four draft differing
instructions, equal 316-byte extents and no structural difference. In each pulse
condition the ROM copies mask 7 before ANDing the frame; the draft copies the
frame before ANDing mask 7. Initial RTL has the intended operand order. First CSE
folds the known mask and `fold_rtx` in `cse.c` commutes the known constant from the
first AND operand to the second. Regmove then copies the other operand.

`AGBCC_NOTIE=144,266` does not repair it: the operand order has already changed.
A private diagnostic compiler with `CSE_NOSWAP=144,266`, suppressing only that
commutation for these two AND instructions, makes the ordinary draft exact.
Neither the private compiler nor this diagnostic override is used for production
checks or committed source.

Tested source hypotheses include narrow/wide mask and frame locals, complete
inline predicates and side-effect helpers, modulo/division forms, packed control
and scalar-backed unions, bitfields, typed QI/HI field operations and a mask shared
across switch arms. Narrow field operations create separate constants in different
machine modes; a shared mask adds another saved register; predicates materialize
an extra boolean. None removes the final barriers while retaining the ROM bytes.
The remaining source requirement is to keep mask 7 opaque through first CSE while
still allowing its late rematerialization. Private evidence is in
`build/player_cue_cleanup/early/0814FAE8/REPORT.md`.

## Shared ownership, fields and operations

`include/player_cue.h` keeps `struct Player` as the owner and supplies six shared
views for cue-specific fields: graphics, auxiliary display, motion, timers,
messages and aligned coordinates. The broad Player declaration is unchanged.
Eighty-six file-local struct definitions disappear; the two remaining records
are separate pending-notification and outgoing-packet payloads, not player owners.
Tentative interpretations are marked in the shared header.

The views reuse `ContactSwarmSprite`, `DisplaySequenceState`,
`DisplaySequenceSprite`, `SceneEventReceiver`, `SceneVec`, `ContactPosition` and
`ScriptUtilityArguments`. They expose named sequence/resource fields, timer
slots, message completion state, particle state, overlay mode and receiver identity.
The receiver identifier at `0024` must be a word: its consumers load all four
bytes, while Player's older positional-effect field at that location is a halfword.

Aligned `ContactPosition` records preserve eight-byte coordinate copies where
`SceneVec` represents only six bytes. The target ABI layout probe confirms the
unchanged Player size and all new views; compile-time size checks live in the
header. Target rounding makes the timer view `0530` bytes, even though its final
halfword ends at `052E`. Host struct layout is not the authority.

The migrations cover the nine newly matched callbacks, existing animation and
movement callbacks, 59 setup functions, message receivers/sending/dispatch, and
small state/collider callbacks. Five setup functions were already canonical.
The final audit accounts for the three half-open subsystem ranges:
`0814F804`–`08156BBC`, `0815AD24`–`0815C9EC`, and `0815D688`–`0815E034`.
No duplicated player owner records, copied playback routines or numeric player
address accesses remain in these units. The pending-notification and packet
payloads stay distinct.

`include/player_cue_sequence.h` now also offers
`PlayerCue_StepSequenceEndedWithFirstFlipMask`; the existing completion-aware API
wraps it with mask 1. Its prior alternate-form consumer remains stock exact.
SideArrival passes its already-live phase, whose case value is 1. This removes
another copied playback routine without extending that phase's lifetime.

The dispatcher (`0815D6B0`) uses the shared receiver, command and dispatch state.
Its inline argument reader forms each payload address at its use; keeping one
argument pointer live over the dispatch would add a saved register and 16 bytes.
StateMotion (`081556E0`) uses an explicit canonical counter-pointer addition;
array-subscript spelling changes initial load ordering. Both remain ordinary C.

The script setter at `0815AD24` no longer selects `old_agbcc`; default `agbcc`
produces the same 24 bytes. `PlayerCue_Motion17` (`0815BFB8`) retains its original
old-style `ActorSequence_Begin()` declaration and two-argument call: adding an
invented third argument changes the observed call ABI.

Five formerly anonymous functions have descriptive, tentative names in
`symbols/functions.csv`: `Player_EnableStatGaugeFromScript` (`0815AD24`),
`Player_EnableStatGaugeAndHistoryFromScript` (`0815AD3C`),
`Player_RestrictActionRequestsFromScript` (`0815AD60`),
`Player_ConfigureAttachmentEffect` (`0815C73C`) and `Player_SendSceneMessage`
(`0815DBF4`). Their references are rewritten and checked, including the caller at
`0815D524`. No anonymous function names remain in the subsection.

## Editable data and Studio

The animation-base tables at `08610980`–`08610B50` are one manifest-backed editable
source, `data/player_cue/animations.csv`. Modes 0, 1, 2 and 6 each have 58 halfwords;
the codec preserves all 464 bytes and all four internal labels. The lookup at
`081419D4` selects the table; `Player_SetAnimation` at `0813AE18` adds folded
heading to its base sequence. `FFFF` values remain numeric without an invented
sentinel interpretation. Row order, slot identity, extent and halfword range are
validated.

Studio's existing Actors page at `/actors#player-animations` displays the
four-mode matrix, source/range, lookup and selection code links, and 24 direct
cue callers. Names resolve from the current symbol map even with an older
worker disassembly snapshot. Four decoded atlas blocks link to that same view;
cache inputs include the CSV, manifest, adapter and codec. The cutscene player
receiver registration entry now follows its current name. The dispatcher remains
a code switch; no handler-pointer table was invented.

## Validation and integration

- Every landed source unit passed independent production compilation, relocation
  at its reviewed address and full-extent ROM comparison, with zero differing
  bytes. The five unchanged setup candidates were also checked. Renamed functions
  and their changed callers passed again after naming.
- Target layout compilation passed. The existing completion-aware consumer passed
  after the shared wrapper change.
- `tools/datafmt/check.py --extract player_animation_variants` reports exact
  roundtrip for `0x1D0` bytes. Manifest overlap and four internal labels were checked.
- Fifteen targeted unittest cases passed across `test_player_animations`,
  `test_actor_command_tables` and `test_callback_array`.
- Studio was visually checked with all 58 rows, four mode headers and caller links;
  the lookup opened the inline Code view. No browser warnings/errors were observed.
- `git diff --check` and the added-lines/commit-message personal-information check
  pass.

Worker source commits remain separate per unit, with shared-header, naming,
data/Studio and documentation commits separate. No full build, push, PR or merge
was performed. The integrator must run one complete combined build printing
`build/boktai3.gba: OK` and regenerate both PROGRESS files for the integration
commit. Aggregate reports remain unchanged here.

Private handoff evidence is under `build/player_cue_cleanup/` (per-unit stock
proofs, final audit, landing ledger and candidate reports) and
`build/player-cue-data/REPORT.md`. These build artifacts are not committed.

Scope gaps: first-CSE operand commutation needs a targeted provenance/what-if
interface; `land` currently treats an already-matched unit as an existing plain
file rather than an update target; stale worker ELF symbols can reject a newly
promoted callback name before scoring. Existing-unit replacements therefore used
formatted source plus independent stock link/ROM checks, with current CSV names
supplied to the private checker. No scope-tool changes or production compiler
settings were introduced.

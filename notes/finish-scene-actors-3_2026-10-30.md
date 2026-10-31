# Finish scene actors, part 3

Tags: #matching #scene-actors #compiler #scope-gap

Eight of the fifteen reserved targets match in ordinary C, including both routines in the original 0812E118 inventory span: nine physical functions. This converts **51.184% of the assigned code**, a **51.184 percentage-point gain** in this queue, or **0.2869 percentage points** of project code (6,916 bytes secondary). No exact match uses register pins, asm barriers, handwritten instructions or compiler overrides. Six guarded drafts are improved or corrected. Seven targets remain open after the second pass, with their compiler evidence recorded below.

Branch: `codex/finish-scene-actors-3`, based on `a8c44f25d`. One function's implementation per commit; naming commits also carry the required reference renames. The 0812E118 commit covers its contiguous 420-byte body and newly inventoried 288-byte neighbor. No push, PR, combined ROM build, report regeneration or subsystem regrouping was performed; those remain batch-integration work.

## Reserved targets, in ROM order

| Address | Reviewed bytes | Result | Deciding source property or remaining mechanism |
|---|---:|---|---|
| 080C09F0 | 2,300 | MATCH — `Skeleton_UpdateTargetApproach` | Shared typed chase ownership removes the early zero's interference with the partner/turn pointers. Direct saved-control reads, promoted map width and a direct cell-height store produce the correct PRE table/stack order under stock agbcc. |
| 080F2F7C | 560 | MATCH — `PhaseObject_UpdateEntryScatter` | Entry addressing follows offset negation; saved points use short-lived typed indexing. `RANDOM_TABLE_ADDR` produces reload's literal handling, and `origin + offset` operand order preserves the loop's origin alias before another address reuses its register. |
| 080F3420 | 168 | MATCH — `ChildGroup_LaunchNextInactiveMember` | Destructive member-address accumulation and the common typed stride retain both exit-path multiplications. Initialize the stride inside the first count guard and bind the activation value as a byte. |
| 0812B91C | 448 | MATCH — `PursuingActor_AdvanceChase` | Typed heading/speed/velocity fields, correct timer-before-Mod and signed lifetime behavior, unsigned component captures, direct final projection sum and the shared vector-add operand order reproduce allocation/reload. Uses the shared pursuing actor and a typed private parameter view. |
| 0812BADC | 516 | MATCH — `PursuingActor_UpdateRecoil` | Bind masks after Atan2, the second sine pointer before its index, and the unsigned random index through the RAM table literal. Preserve the full-width terrain offset and velocity-first vector-add order. |
| 0812E118 | 708 | MATCH — two physical functions | The traversal routine is 420 bytes; 0812E2BC is a hidden 288-byte forward-floor probe. Removing the draft's invented center initialization preserves the ROM's slope read-before-fill. Direct cell-property reads and branch-local packed offsets match both bodies. |
| 0812E7EC | 1,764 | Guarded draft 76.0% — `SceneActorReaction_DispatchPendingCommands` | Fixes invalid partial actor-field migration and removes its pin. A state argument recovers constant order. Receiver+6/+8 PRE still hoists two field addresses instead of spilling/recomputing the receiver as in ROM; state tails and attachment allocation also differ. Stock: 178 ROM / 156 draft instructions. |
| 0812F5D4 | 800 | Guarded draft 95.4% — `SceneActorReaction_UpdateContactRoute` | Typed flag ownership and a behavior parameter recover the complete physical instruction stream. The remaining 19/19 differences are only a consistent PRE slot permutation; current actor/group pseudo identities admit no feasible table size. Second-pass owner projections do not solve it. |
| 0812FA0C | 1,976 | MATCH — `SceneActorReaction_UpdateRouteReturn` | Reuses `Dog_BeginDelayedRouteRecovery` and shared delayed-response/chase operations, with its wait threshold changed from 20 to 60. |
| 081314E8 | 824 | Guarded draft 99.1% — `SceneActorRoute_UpdateContactRecovery` | Corrects `entryTarget228` versus `savedTarget22C` and recovers typed route/control operations. Only late crossjump's merged callback tail remains: 4 ROM / 1 draft instruction; disabling crossjump 601/897 diagnostically matches. Incompatible callee-return casts were rejected. |
| 08135F70 | 1,156 | Corrected guarded draft 78.6% — `SceneActorReaction_ResolveDamage` | Fixes the inherited lost-flags bug: OR 0x20 into existing flags instead of overwriting them first. Ordinary C has 122/112 differences; the dying `mode >> 2` operand inherits r4 from the HUD argument and changes pending-input, health and flags-pointer allocation. The higher-scoring inherited draft was pinned and semantically wrong. |
| 081363F4 | 1,492 | Guarded draft 87.6% — `SceneActorReaction_ResolvePlacementDamage` | Scoped health/pending ownership and a transient damage-flags pointer recover the main mode/mask sequence. PRE reread suppression, regmove AND ties and coupled owner/collider/HUD allocation remain: 88/78, shape 42. This is not a one-decision near-match. |
| 0815AC04 | 240 | MATCH — `ScriptActor_TestFacingPointFromScript` | Reset the consumed actor cursor after dx/dz. CSE retains the independent player alias; later dead-set elimination removes the reset. A private diagnostic canon_reg hook first proved that preserving pseudo 24 gives the full match. |
| 08181514 | 288 | Retained guarded draft 90.1% — `ActorMarker_LaunchFragments` | No implementation change. ROM stores the full-width Z sum at sp+8. A real fast/index/sum context reproduces that explicit MEM store, but adds index PRE and changes origin/RNG allocation. Baseline: 11/9, 284/288 bytes. |
| 081B8E44 | 272 | Guarded draft 97.0% — `OrbitMotion_UpdateCrossedSines` | A typed X-coordinate pointer bound after the division prevents an eager field-store zero and preserves output aliases. Four register-only differences remain: first coordinate priority 1428 versus second 1578; diagnostic PRI +151 gives full 272-byte MATCH. |

All new descriptive names in `symbols/functions.csv` have low confidence and tentative notes. The hidden neighbor is named `SceneActorTerrain_IsBelowForwardFloor`. It is not an additional 288 bytes beyond the table's 708-byte span.

## What is still open

The command dispatcher has several structural/allocation differences. `pre` proves the two receiver-address hoists. Direct count access, an inline scan helper, current shared narrow/wide state helpers, a local receiver record and a second-pass consumed-receiver reset were tested; none recovers the ROM receiver model. The reset/rebind loop worsens to 203/174 and adds another PRE address. Keep the current guarded source, not that experiment.

Contact-route physical offsets, registers, opcodes, branches and literal values were compared independently before treating the mismatch as a stack permutation. With actor 118/group 122 and N280/281 (both T141), fixed identities have no feasible odd table size; T149 only reduces the residue to eight displacements. Most direct/group-first/reverse-owner projections retain an impossible order or move real copies. The second pass found a stronger identity frontier: staged group projection gives actor 119/group 118, N281/T141, and 15/15 slot-only differences. Its diagnostic T149 gives a complete 800-byte match. Real narrow random-index/sample/direction domains each add two pre-GCSE instructions without changing machine work, but no coherent source reaches the needed N296–299. The checked-in 19/19 source remains the ordinary handoff; the staged frontier and full diagnostic proof are retained in the second-pass graph.

The contact-recovery crossjump blocker is complete and narrow: the stock source loses one duplicated generic callback call. A private `AGBCC_NOCROSS` intervention restores the reviewed 824 bytes. Changing a callee pointer's return type to void/u8 also prevents the merge, but calls through an incompatible function type and was rejected. No such cast is in the tree. A second pass wrapping actual calls in do/while, break and return forms either preserves the same 816-byte merge or changes real control flow; none matches.

The two damage routines share hit-flag expansion issues, but each has a larger allocation frontier. Documented force/priority/NOTIE/PRE interventions confirm individual causes, not a full solution. Their guarded fallback remains the default implementation; fuzzy scores do not count as exact progress.

The marker launcher remains the unchanged ordinary implementation, with its stale fuzzy annotation refreshed to 90.1%. The fullword Z store is an explicit-MEM versus reload problem: forcing sum 88 to spill allocates a slot but reload deletes the dead store. A real twelve-byte context grouping fast, index and Z sum instead produces the observed sp+8 write. PRE then caches its index across the fast/slow split, displacing the random-index pointer. NOPRE 1 restores 288 bytes but leaves 32/32 differences. Narrow/load coalescing diagnostics and an index scalarization pass do not solve the compound origin/sum allocation. No arbitrary padding, volatile local, manual frame operation or diagnostic compiler option is in source.

The orbit projection is the closest remaining ordinary arithmetic draft. Both coordinates have three refs, but flow counts first 21 versus second 19 instructions, giving priority 1428 versus 1578. Raising first by 151 diagnostically matches all 272 bytes. Typed/narrow/getter variants, outer-value reuse, operand forms and snapshots were measured. A later snapshot gives the desired allocation but combine sinks the first signed load past the second; cursor resets do not stop that memory substitution. The typed late X pointer fixes the hidden field-store zero without a cast. Its guarded 97.0% source is retained, with no pin or extra artificial computation.

## Validation and tool observations

Every exact unit was formatted, checked by scope with stock settings, then independently compiled with the production compiler and compared across its complete reviewed extent. Every changed guarded unit retains an exact default fallback; its ordinary draft was checked with `--nonmatching --record` using the project venv. All fifteen caller units affected only by required symbol-reference renames also passed independent stock checks. No emulator/relocation run was needed for byte-identical replacement functions.

The 0812E118 boundary was checked before credit: a return/pool ends at 0812E2BC, another Thumb prologue starts there, nm places the second C definition at +0x1A4, and the entire 708-byte unit matches. Scope's hidden-function registration skips definitions still inside a NONMATCHING guard; unwrapping the reviewed candidate before landing lets the normal registration path add 0812E2BC. No generated assembly or ROM bytes are committed.

Renamed functions are absent from the cloned base ELF. A build-only wrapper around `tools/check.py` supplies the function inventory's verified addresses to its symbol snapshot and maps old fallback labels to renamed C definitions for fuzzy recording. It still uses the production compiler, normal relocation/linking and the full reviewed extent. Old fallback filenames remain usable until integration regenerates assembly names. No compiler option is overridden. The wrapper is `build/brief-work/check_named.py`; checks whose names already existed ran `tools/check.py` directly. Scope landing records fuzzy scores through system python3; venv reruns ensure the Capstone-based metric is recorded consistently.

Useful scope gaps:

- `CSE_KEEP`/`CSE_NOCANON` control class-head selection, not direct canon_reg substitution. For 0815AC04, a build-only compiler hook returning the original expression for `CSE_CANON_KEEP=24` proves the lost copy with a full 240-byte diagnostic match; without the hook it remains 232 bytes. The ordinary cursor reset then matches with stock agbcc. The private compiler and its launcher are under `build/brief-work/` and are not final-source dependencies.
- `pre` attempts to compose comma-separated AGBCC_NOPRE values, but the supplied compiler patch uses atoi and honors only the first. The combined probe is not evidence that both replacements were disabled.
- PRE slot-order constraints are useful only after verifying every physical instruction and a slot bijection. A built-in feasible-order/RTL-count tool would save manual extraction on 080C09F0 and 0812F5D4.
- Allocation reports omit the original dying-mode shift that propagates r4 preference. A regmove tie can be disabled and then recreated by local allocation; unchanged final bytes do not establish that the intervention had no causal effect.
- `probe` omits late crossjump decisions; the manual trace/NOCROSS pair isolated 081314E8. `rasolve` crashes on a missing pseudo record for 081363F4, including the canonical-name scratch variant.
- Matchsearch's default Unix socket path exceeds the platform limit in this managed worktree. `--local` and direct Session APIs preserve the same evidence. Header changes invalidate frozen contexts as intended; forks preserve earlier hypotheses.

## Evidence retained in the worker build

Per-function sources and reports remain under `build/scope/drafts/`, `build/scope/campaign/` and `build/matchsearch/`. Key report paths are:

- `build/scope/campaign/sub_080C09F0/REPORT.md`
- `build/scope/campaign/sub_0812F5D4/REPORT.md`
- `build/scope/campaign/sub_08181514/REPORT.md`
- `build/scope/drafts/sub_080F3420-report/REPORT.md`
- `build/scope/drafts/sub_0812E118-report/REPORT.md`
- `build/scope/drafts/sub_08135F70-report/REPORT.md`
- `build/scope/drafts/sub_0815AC04-second-report/REPORT.md`
- `build/scope/drafts/sub_081B8E44-second-report/REPORT.md`
- `build/matchsearch/sub_080F2F7C/REPORT.md`
- `build/matchsearch/sub_0812BADC/REPORT.md`
- `build/matchsearch/sub_0812FA0C/REPORT.md`
- `build/matchsearch/sub_081314E8/REPORT.md`
- `build/matchsearch/sub_081314E8_second/REPORT.md`
- `build/matchsearch/sub_0812F5D4_second/REPORT.md`
- `build/matchsearch/sub_081363F4/REPORT.md`

The checked-in C and this note are the durable handoff. Build artifacts retain the detailed compiler traces and source frontiers.

The integrator should review this branch with the other worker batches, run one combined build requiring `build/boktai3.gba: OK`, then regenerate and commit both PROGRESS files in the integration PR. Worker checks are sufficient for these commits, and the checked-in reports remain untouched here.

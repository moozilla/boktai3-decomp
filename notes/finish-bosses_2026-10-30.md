# Boss matching worker: Ratatosk, Vanargand and Hresvelgr

Tags: #matching #bosses #ratatosk #vanargand #hresvelgr #compiler #scope-gap

Four of the 15 assigned functions now match in ordinary C. Exact coverage of
this queue increased from 0% to **20.8%, a gain of 20.8 percentage points**
(3,956 of 19,000 bytes). Eleven functions remain guarded drafts. Every assigned
translation unit now has ordinary C without register pins, assembly barriers,
handwritten instructions or compiler overrides. Fuzzy scores below describe
drafts and do not count as exact progress.

Work is on `codex/finish-bosses`, based on `a8c44f25d`, with one source commit per
function. All targets received an initial pass in ROM order; the closest
blockers received independent second passes. Quota remained above the brief's
50% stopping threshold. No combined build, push or PR was performed; integration
and aggregate report regeneration remain with the integrator.

| Address | Function | ROM bytes | Result | Deciding source property |
|---|---|---:|---|---|
| 081A4AB4 | `Hresvelgr_EmitImpactExplosions` | 204 | Guarded, 70.0%; barriers removed | Canonical `ContactPosition` and the masked signed Z offset recover the operations. The RNG table still loses allocation priority to the position pointer. |
| 081AA788 | `Vanargand_UpdateSweepingProjectileAttack` | 1,672 | Guarded, 87.3% (was 29.1%) | Preserve the timer snapshot before calls, signed span division, separate vector stack objects, pool-owner lifetime and field-based selected-collider arithmetic. RNG allocation and two coordinate lifetimes remain wrong. |
| 081ABA64 | `Vanargand_UpdateChargeSequence` | 3,824 | Guarded, 75.6% (was 34.0%) | Correct phase completion gates, signed rounding and separate coordinate/workspace records restore the frame. CSE's owner-pointer takeover still changes the main register roles. |
| 081B0248 | `Vanargand_SpawnPartEmergenceEffects` | 376 | Guarded, 98.9% (was 87.8%) | Reuse one work value for the RNG address and radius remainder. CSE then retargets the subtraction and deletes one copy that the ROM retains; two instructions still differ. |
| 081B03C0 | `PairedSceneActor_UpdatePlayerCapture` | 1,272 | **Exact** | Shared entry values, mask-first player-flag testing, a typed motion track and a typed sequence owner preserve value lifetimes and argument evaluation. |
| 081B18C8 | `ActorStatus_UpdateRisingStrike` | 712 | **Exact** | Inline state setting avoids a retained state-field address; one function-scope motion pointer reused across phases naturally takes the required register. |
| 081B2F38 | `Vanargand_UpdatePartOffsetStrike` | 1,240 | **Exact** | Signed Z coordinates, a shared entry zero, typed `MotionTrack` access and named anchor/angle quantities reproduce the ROM's lifetimes. |
| 081B4040 | `Vanargand_EmitRadialPartEffects` | 732 | **Exact** | Canonical `ZeroFill8`, one entry pointer shared by reset and phase initialization, and the vector Z operand order restore the target instructions. |
| 081B431C | `Vanargand_UpdatePartImpact` | 1,128 | Guarded, 97.1% (was 91.3%) | Typed sequence completion, vector evaluation order and a shared completion value recover the instruction structure. A zero extended from a vector halfword changes inherited reload rotation. |
| 081F3C68 | `Ratatosk_UpdateRadialSummonAttack` | 1,308 | Guarded, 71.4%; four pins removed | Canonical vectors and a verified 32-byte spawn config retain the behavior. The owner must spill while spawn/delta/pattern take different global register roles. The pinned baseline scored 81.3%. |
| 081F4CF8 | `Ratatosk_UpdateDefeatSequence` | 1,064 | Guarded, 92.4% (was 90.1%) | Canonical particle position, the actual configurator signature and capturing `atan2` before the velocity address repair call order. RNG speed and byte-angle values still receive different spill treatment. |
| 081F5290 | `Ratatosk_UpdateKnockbackRecovery` | 800 | Guarded, 63.6%; concessions removed | Canonical body/motion access and the actual particle prototype preserve the ordinary source. The owner alias and particle drift block the desired owner register; tick takes a low register instead of a high one. The constrained baseline scored 71.8%. |
| 081F7FD0 | `Ratatosk_UpdatePlayerOrbitAttack` | 1,300 | Guarded, 93.0% (was 86.5%) | Separate vector values restore first-coordinate address materialization. Capture pointers after player-position calls, preserve signed byte comparison and canonical clear-call evaluation. RNG-table allocation and two reload rotations remain. |
| 081FAE0C | `Ratatosk_InitializeFromScript` | 2,428 | Guarded, 84.4% (was 53.0%) | Typed actor-prefix setters/getter, whole path records and a shared palette-source variable restore the frame and pointer roles. Shared default constants still prevent the ROM's rematerialization and tail merge. |
| 081FC26C | `Ratatosk_SpawnAnimatedParticle` | 940 | Guarded, 92.0% (was 79.2%) | Draw the particle kind before testing it, keep branch-local kind stores, use canonical velocity/sequence records and preserve the hidden halfword zero. A dead copy, RNG allocation and reload differences remain. |

Names newly inferred from behavior are tentative (`confidence=low`). Existing
symbol names are retained; new descriptive spellings use the repository's
worker-local alias convention. The integrator can promote those aliases with
`tools/promote_names.py` before regenerating labels and building the batch.

## Verification and source review

The four exact functions passed independent stock `tools/check.py` checks with
zero differing bytes, including their final readability cleanup. Their complete
guards and assembly fallbacks were removed. Independent source review checked
the sequence, motion, anchor, state, entry and timer field offsets, signedness
and helper argument order.

Each remaining draft was checked with `--nonmatching --record`; its expected
mismatch remains behind `#ifdef NONMATCHING`. Default per-unit checks passed for
the fallback units. The lower scores at 081F3C68 and 081F5290 are deliberate:
the retained alternatives remove the preexisting register concessions and obey
the brief's ordinary-C requirement. Hresvelgr's small fuzzy decrease likewise
removes its two empty assembly barriers. None of these changes adds exact bytes.

Only the assigned source units and work-record documents are committed. Shared
headers, tools, generated assembly, the ROM and PROGRESS reports are unchanged.
The integrator must run one combined build and require `build/boktai3.gba: OK`
before merging the batch, then regenerate both aggregate progress files.

## Remaining work and evidence

Persistent candidates, compiler traces and search graphs remain under
`build/matchsearch/<function>/` in this worktree. They are local working
artifacts, not committed generated outputs. Each function's subtree contains a
`REPORT.md`, at its root or under `review/` or `independent/`.
Use the raw inventory name for matchsearch sessions where a descriptive alias
is not yet in the registry, and `--local` when the checkout path exceeds the
Unix-domain socket limit. All commands below use `build/venv/bin/python`.

The nearest continuation points are:

* **081B0248:** `final-draft.c` has only two differing instructions. On the
  clearer `typed-rem-offset.c`, `AGBCC_PRI=140:+1143` gives a diagnostic exact
  match: table pseudo 140 has priority 2608, behind mask priority 3750, and must
  allocate first. Reusing the work value achieves that priority in stock C but
  lets CSE delete the required copy. `CSE_KEEP=158` restores the copy while
  losing the allocation benefit. Two independent passes tested source
  lifetimes, types and canonical helper forms without satisfying both.
* **081B431C:** the retained draft has the same instruction structure and
  length. `AGBCC_SPILL=34:1` is an exact diagnostic. A halfword zero in the
  success-point addition becomes an SI subregister, survives the burst call
  and is reused for the timer clear; that alters the reload stream. Field,
  array, union, helper and narrow-value alternatives did not reproduce it.
* **081F7FD0:** on `v10.c` (same bytes as the retained best),
  `AGBCC_SPILL=27:3,39:1 AGBCC_FORCE=224:3` is an exact diagnostic. The
  remaining differences are the first RNG table's local register and two
  inherited reload rotations. These knobs are absent from the source.
* **081A4AB4:** on `review/best.c`, `AGBCC_PRI=table:+2133` is an exact
  diagnostic. The table's adjusted lifetime makes its priority 2222 versus
  the position pointer's 4354. Ordinary reuse, lifetime, table qualification
  and shared-helper alternatives did not recover the required order.

The larger unresolved source decisions are:

* **081AA788:** the shifted RNG sample straddles local/global allocation,
  table and cursor assignments diverge, and the two origin halfwords take
  opposite high/low register roles. Selected-collider arithmetic also has two
  additions in the wrong order. No whole-function isolated proof exists.
* **081ABA64:** `CSE_NOCANON=1060` recovers the main owner roles, but other
  differences remain. The repaired completion gates and frame are retained;
  a different shared draw/sequence form improved structure while regressing
  overall correspondence and was rejected.
* **081F3C68 / 081F5290:** neither has a complete exact counterfactual.
  Multiple forced register choices repair some roles but leave substantial
  instruction differences. Recover the owner/alias and polar/RNG value
  lifetimes before investigating stack-slot permutations.
* **081F4CF8:** the ROM's byte-angle stack word is an ordinary spill, not a
  caller-save inserted spill. Raising the RNG speed's priority moves it to
  the expected register but delays the angle spill and changes its slot.
  Aggregate forcing changes frame order instead of recovering the mechanism.
* **081FAE0C:** constant 30 (pseudo 284, priority 3846) is held in one register
  across default halfword stores; the ROM rematerializes it and shares the
  last store between branches. `AGBCC_FORCE=284:0` repairs the first tail
  merge only. A separate typed-camera hypothesis is preserved in `v23.c`;
  diagnostic `AGBCC_GCSE_T=491` restores its early PRE slots but does not
  match the function. The interrupted bounded allocator search completed
  depth one (3,853 compiles) without an exact result; depth two was not
  completed, so it is not an exhaustion proof.
* **081FC26C:** the old uninitialized kind read is repaired. The remaining
  dead copy can be induced by returning a sprite pointer from the sequence
  helper, but that changes other allocations. No complete exact diagnostic
  exists. The selected canonical records avoid the invalid packed/union
  layouts among rejected scratch experiments.

## Scope gaps encountered

`why` sometimes explains a register based on an alignment that loses a
low-to-high copy or mistakes an ordinary spill for caller-save insertion.
Trace the physical value across the call before accepting that explanation.
The local/global obstruction of RNG values currently needs several separate
queries; it does not yet identify a source partition that reverses allocation.
Constant-origin mapping can report unknown, and inherited reload history needs
to be reconstructed across earlier vector operations.

Small DI-mode vector locals become stack addresses only during `addressof`,
after PRE; putting the same coordinates inside an aggregate can therefore
change address rematerialization without changing their source values. PRE
hash/slot permutations should be investigated only after physical instruction
structure and the actual value lifetimes agree. No tool changes were made in
this worker batch.

# Round 15 — priority source recovery

Checkpoint 1, 2026-11-17. This note is updated in place; the accompanying
`notes/round15-open-candidates.patch` retains one complete proposal for each
currently investigated open function. It is an experimental patch, **not** an
accepted source change. No trial logs for completed sites are added.

## State and validation

- Base: `4ce8f0d5d826909e6b9fa1caf60609a0fb7194c3` (`origin/main`).
- Accepted source: `7bb4981ea103b56a2343bf9ef20950b5e78189d0`.
- Branch: `codex/round15`; the delivery bundle is cumulative from the base.
- The instrumented compiler was rebuilt for the updated upstream detector.
  The production compiler remains stock agbcc at
  `da598c1d918402c42c0c0d7128ba14567f3175e9`.
- Independent unit checks passed for every accepted unit change. The combined
  union of changed-header dependents passed **175/175**.
- Full `make` passed, including `tools/check_ram_literals.py`; ROM SHA-1 is
  `2651c5e6875ac60abff734510d152166d211c87c`.
- No detector change, compiler override, assembly, pin, barrier, or newly added
  concession warning is part of this checkpoint. `PROGRESS.md` and
  `PROGRESS.json` are unchanged from the base.
- Independent source review found no substitute allocation steering in the
  accepted changes. The residual source debt listed below is still open.

## Compiler-inclusive census

Both columns are complete `python tools/concessions.py --summary` runs.
The added public API header increases the scanned file count by one.

| Severity / kind | Before | After | Change |
|---|---:|---:|---:|
| Error: volatile | 3 | 3 | 0 |
| Warning: dead-code | 13 | 12 | -1 |
| Warning: proved-zero | 33 | 32 | -1 |
| Warning: once-loop | 114 | 114 | 0 |
| Warning: stmt-expr | 6 | 6 | 0 |
| Warning: const-local | 1,187 | 1,184 | -3 |
| Warning: literal-local | 2,733 | 2,731 | -2 |
| Warning: addr-deref | 41 | 41 | 0 |
| Warning: placeholder-name | 8 | 8 | 0 |
| Warning: volatile-view | 5 | 5 | 0 |
| Warning: local-union | 131 | 131 | 0 |
| **Total errors** | **3** | **3** | **0** |
| **Total warnings** | **4,271** | **4,264** | **-7** |
| Files scanned | 6,586 | 6,587 | +1 |

The priority total falls from 46 to 44 reported sites. **One is a genuine
proved-zero source fix; one is a justified specialization annotation.** The
known Vorn false positive remains reported and untouched. After excluding it,
43 reported priority sites remain. The unreported palette guard removed below
is additional source cleanup and earns no census credit.

## Accepted changes

| Commit | Source change | Evidence and boundary |
|---|---|---|
| `d67ee08d5` | Model actor-controller velocity as signed X/Y/Z fields. | `ActorControllerVelocity` replaces the halfword array; its natural 8-byte size absorbs existing padding. Actor size 4,404 bytes and all probed field offsets stay unchanged. Seven header dependents match. |
| `3950ce30e` | Share the actual polar-velocity calculation in `actor_controller.h`. | Retreat and timed-motion users still match; the old zero local disappears. Three header dependents match. This accounts for one const-local reduction. |
| `4d65a5f05` | Remove Armor's fabricated `u8 >> 8` zero and recover owned velocity/collider writes. | `ArmorActor_UpdateFixedSpeedMotion` at `08183E4C` matches all 448 bytes. Its five warnings become zero: one proved-zero, two const-local and two literal-local. |
| `7bb4981ea` | Give paused particle bursts one public declaration and migrate callers. | `scene_particle_burst_api.h` agrees with the defining wrapper: signed result, three const position pointers, signed count, unsigned priority/flip. All 169 dependents match; the relevant eleven-unit census is unchanged. |
| `23c8ed5d7` | Remove the palette category predicate's impossible negative guard. | The predicate reads its manager's actual flags. All 468 bytes match; the existing five warnings remain. The reported palette index-zero site is still open. |
| `5dbd9f911` | Annotate a legitimate specialization of the existing terrain walker. | One new exemption, detailed separately below; no fabricated-source removal credit. |

### Priority source site cleared: Armor at `08183E4C`

The real operation computes X/Z velocity from heading and speed, clears Y, and
nudges an exactly zero X or Z component toward the terrain-cell centre. The
source now performs those stores through the owning three-component velocity
record. The nudge uses the actual low position byte and signed distance.

The ROM's otherwise unused zero register is produced naturally by agbcc's
halfword field-store preservation mask for `velocity.z`: RTL UID 364 creates the
HI zero, the negative X/Z nudge stores retain its uses through combine, and
allocation assigns r7. No dummy zero expression or fake use replaces the old
concession. This is compiler provenance for a real field operation, not a build
setting.

The existing `*(u16 *)(s - 14)` flag clear remains raw-offset debt. The burst
migration also leaves explicit representation-boundary casts from older
`SceneVec`, array and packed-point owners. Neither is presented as solved.

### The single new `concession-ok`

`src/spread_shot.c`, `SpreadShot_ResolveTerrain` at `08123194`, left-neighbor call:

> Shared bidirectional Seek uses its backward arm for cell - 1; the cell + 1 calls use its forward arm.

This annotates `dead-code`. The existing walker handles real forward and
backward row/column traversals at its other calls. At this call the cell is an
unsigned halfword and the target is `cell - 1`, so its forward arm specializes
away. The instrumented compiler attributes removal of the arm's Mod call to
jump2. Left/right/runtime-target controls distinguish specialization from a
universally dead operation; the pre-existing traversal and its older research
note `notes/cloud-08122DF4.md` establish the operation independently of this
annotation.

A direct backward-only implementation was tried before accepting the exemption:
10,500 bytes versus the unit's 10,704, with 7,372 differing bytes. The annotated
original remains byte-exact at 10,704 bytes and has zero focused warnings. This
exemption does **not** authenticate the separate `SlopeSurface` shaping or
function-scope `cell` allocation choices still documented in that unit.

## Open priority frontiers

All four proposals are preserved together in `notes/round15-open-candidates.patch`.
The patch applies to this checkpoint's accepted source. Its four reconstructed
files were hash-checked and independently stock-compiled in the integrated
checkout; the measurements below reproduced. These are **nonmatching drafts**.
The later functions in each ROM-ordered queue have not been substituted for
these open functions. No easy-kind sweep was performed.

| Function | Ordinary candidate | The decision still to recover |
|---|---|---|
| `FragmentGroup_SpawnRadial`, `08014CA4` | Delete all three zero-address terms; 704 bytes, 531 differing bytes. | The actual group/data lifetimes that avoid hoisting two indexed byte-array bases while retaining the random table's allocation. |
| `SceneActorPalette_ApplyTransition`, `08071F50` | Actual RGB transition and palette binding operations; 468 bytes, 12 differing bytes. | The genuine type/operation structure that gives the six PRE-generated field addresses their ROM spill slots. |
| `TargetRegistry_FindById`, `0815F5B4` | Per-layout ID comparisons, both fabricated forms removed; 68 bytes, 32 differing bytes. | A natural lookup/result CFG that retains the collider comparison tail and shares the actor tail backward. |
| `Hresvelgr_UpdateBoundaryBarrage`, `081A0BC8` | Real widget/palette fields and direct state/flag operations; 732 bytes, 2 differing bytes. | The actual dataflow that keeps sl unavailable when reload relocates the random mask, without manufacturing a use. |

### Fragment group

The direct patch isolates the three reported expressions; inherited parallel
cursor scaffolding is not claimed recovered. GCSE hoists `base + 32` and
`base + 44`; the constant table receives an equivalence priority penalty.
Diagnostic-only `AGBCC_NOPRE=1,2 AGBCC_NOEQUIV=table` reproduces all 704 bytes.

Complete ordinary alternatives were also tested: uniformly indexed fields,
manager-array access, a group cursor, whole-group and whole-sprite operations,
shared random sampling, actual three-field velocity records, and bounded slot
index types. None matched. The three-field velocity record alone changes no
instructions, and narrow slot indices add extensions. The unresolved issue is
the real owner/iteration structure, not a need for another pointer identity.

### Palette transition

The preserved draft removes the index-zero expression, manually ordered channel
bases, private `ReadMasked`/`Remaining` wrappers, and input pointer copies.
`AdvancePaletteTransition` owns RGB accumulation, packing and countdown;
`BindPalette` uses the existing `ActorStatus_BehaviorFlags` API.

Every opcode, register, branch, literal and instruction address agrees; twelve
SP-relative instructions differ by six consistent slot substitutions. The
ordinary PRE table has N=184 / T=93. Diagnostic-only `AGBCC_GCSE_T=103` is exact.
Proper channel domains, RGB packing, aggregates, direct binding, countdown and
structured selection have been tested. No declaration/argument permutation,
instruction-count ballast or compiler override was adopted.

Two well-named literal locals remain open for the owner's eventual policy:
`src/fn/sub_08071F50.c`, `SceneActorPalette_ApplyTransition`, category 23 mask
`0x8000` and category 10 mask `0x4000`. In the preserved draft direct literals
change tail sharing. They retain their warnings and have no ignores. No separate
literal-local sweep or broader list of failed inlining attempts was made.

### Target registry

The draft removes the registry pointer OR-zero and the unreported negative test
of a zero-extended collider ID. All registers and the 68-byte extent agree; jump2
merges the first comparison/result tail into the later actor copy, opposite to
the ROM. Diagnostic-only `AGBCC_NOCROSS=67` keeps the first tail and matches.
The existing pointer-view union remains warned.

A direct inactive-result break gives the correct tail direction but hoists
flags/target-pointer expressions; suppressing those three PRE decisions is
exact diagnostically. A conditional NULL/entry return is 72 bytes because of an
early NULL initialization. Scoped kind switches and captured-flags search loops
were tested without a source solution. The original non-advancing entry behavior
on an ID mismatch is preserved; it has not been repaired.

### Hresvelgr barrage

The initializer establishes the apparent collision fields as actual widget
`paletteBank`/`palette`, with a palette buffer at actor +0x424 and tile map at
+0x704. State helpers establish the complete 16-byte action record. The draft
uses those fields, `ContactPosition`, the shared direct sampler, actual state-one
values and tick parity, and ordinary flag updates. It removes the zero term and
five literal locals, but remains unaccepted.

Only the two transfers of the RNG mask differ: the ROM uses ip and the draft sl.
The mask pseudo initially receives r5; reload evicts it and reuses sl because
the action-offset/palette literal is no longer live. Diagnostic-only
`AGBCC_FORCE=237:12` matches. A real random-position operation was also tried and
did not solve that conflict.

The inherited expanded X remainder still explicitly records an allocation
rationale, and private terrain/helper ownership remains unresolved. This draft
has zero linter warnings; that does **not** establish source authenticity.

## Reproduction boundaries

The production build contains none of the diagnostic settings above. Reproduce
an experimental draft in an isolated copy by applying only its section of the
open patch, then use stock `tools/check.py` with the function extent listed in
the table. The four complete candidate SHA-256 values are:

| Function | Candidate SHA-256 |
|---|---|
| Fragment | `b98cad5f6e1c631186e902afdfe6e01c6a74797edb68a8f9fffdb2841f5695a5` |
| Palette | `a9e9ed6214fd39bafc276c08c8d5be415448eccbeca02fb8bf4c8b7f53d510f0` |
| Registry | `f96451a656715ccb10825f8498cb38ea760ae607bc06ed7643ae278d520ced5d` |
| Barrage | `047f3f150c4067c558381d8f36ad252d5aa4edddd18dba9b194862b7d36bcf90` |

Continue at these open functions. The three volatile errors and all lower-priority
kind queues remain pending behind the dead-code/proved-zero work. Allocation-only
source forms remain concessions even when they are outside the scanner's coverage.

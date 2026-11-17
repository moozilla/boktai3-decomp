# Round 15 — priority source recovery

Checkpoint 2, 2026-11-17. This note is updated in place. The accompanying
`notes/round15-open-candidates.patch` preserves one complete proposal for each
of the four currently investigated open functions. It is an experimental patch,
**not** accepted source. The solved palette proposal has been removed from it.
No trial logs for completed sites are added.

## State and validation

- Base: `4ce8f0d5d826909e6b9fa1caf60609a0fb7194c3` (`origin/main`). A fresh fetch
  found main unchanged; merging it reported already up to date.
- Accepted source and manifest: `3c36c22fa806f20cd4cdaf5a42e015f94f410641`.
- Previous checkpoint: `2511aca4a3e6051b235e6d34f3b11abc45110b6b`.
- Branch: `codex/round15`. Delivery is cumulative from the base and includes
  checkpoint 1.
- The instrumented compiler was rebuilt for this round's upstream detector.
  Production remains stock agbcc at `da598c1d918402c42c0c0d7128ba14567f3175e9`.
- Independent stock checks passed for accepted unit changes. Each changed header's
  dependents were checked: this continuation includes **2,666/2,666** for `ram.h`,
  **2/2** for `fragment_group.h`, and **1/1** for `terrain_sampling.h`. These sets
  overlap; they are not added together. Checkpoint 1's combined check covered 175
  dependents. The cumulative changed-header union contains 2,670 source units.
- Full `make -j8` passed, including `tools/check_ram_literals.py`. After the RAM
  manifest correction, the RAM audit passed again. ROM SHA-1 remains
  `2651c5e6875ac60abff734510d152166d211c87c`.
- Independent read-only review found no new or relocated allocation steering in
  the accepted palette, fragment, or terrain changes.
- No detector change, compiler override, assembly, pin, barrier, or newly added
  concession warning is part of this checkpoint. `PROGRESS.md` and
  `PROGRESS.json` remain unchanged from the base.

## Compiler-inclusive census

All three columns are complete `python tools/concessions.py --summary` runs.
The two added public headers account for the cumulative file-count increase.

| Severity / kind | Round 15 base | Checkpoint 1 | Checkpoint 2 | Change since checkpoint 1 |
|---|---:|---:|---:|---:|
| Error: volatile | 3 | 3 | 3 | 0 |
| Warning: dead-code | 13 | 12 | 12 | 0 |
| Warning: proved-zero | 33 | 32 | 31 | -1 |
| Warning: once-loop | 114 | 114 | 114 | 0 |
| Warning: stmt-expr | 6 | 6 | 6 | 0 |
| Warning: const-local | 1,187 | 1,184 | 1,184 | 0 |
| Warning: literal-local | 2,733 | 2,731 | 2,729 | -2 |
| Warning: addr-deref | 41 | 41 | 41 | 0 |
| Warning: placeholder-name | 8 | 8 | 8 | 0 |
| Warning: volatile-view | 5 | 5 | 5 | 0 |
| Warning: local-union | 131 | 131 | 131 | 0 |
| **Total errors** | **3** | **3** | **3** | **0** |
| **Total warnings** | **4,271** | **4,264** | **4,261** | **-3** |
| Files scanned | 6,586 | 6,587 | 6,588 | +1 |

The reported priority total is now **43**, down from 46 at the round's base:
**two proved-zero source recoveries and one justified specialization annotation**.
The known Vorn false positive remains reported and untouched. Excluding it leaves
**42 priority sites**. The earlier unreported palette guard removal earns no
census credit. Ownership repairs and an unmatched draft's lower warning count
also earn no priority-removal credit.

## New accepted changes

| Commit | Change | Evidence and boundary |
|---|---|---|
| `8f2863fc5` | Recover the complete palette transition and binding operations. | `SceneActorPalette_ApplyTransition`, `08071F50`, matches all 468 bytes. Five warnings become two: one proved-zero and two literal locals disappear. |
| `670fd4bdf` | Give `gFragmentManager` its owning pointer type. | Creator, initializer, destructor and users agree. Destruction clears the singleton and returns status zero explicitly. Both fragment units match; all 2,666 `ram.h` dependents match. No warning reduction. |
| `58d76ab0b` | Use the sprite loader's concrete resource record in `FragmentGroup`. | `SpriteRenderResource` replaces the opaque `SceneAnimation` view; `paletteKey` is the actual field filled by `AnimData_Load`. The 28-byte resource and surrounding offsets are unchanged. Both dependents match. No warning reduction. |
| `fd0ad27e1` | Share terrain cell and height sampling in `terrain_sampling.h`. | The existing implementations move unchanged from `terrain_modifiers.c`. Its 8,896 reviewed bytes match; focused lint remains 0 errors / 0 warnings. The extraction is accepted; the barrage draft using it is still open. |
| `3c36c22fa` | Synchronize the fragment singleton's RAM manifest declaration. | `symbols/ram.csv` now preserves the pointer declaration. Address and four-byte extent are unchanged; the RAM audit passes. |

### New priority source site cleared: palette transition at `08071F50`

`AdvancePaletteTransition` owns the entire 16-color update: add signed per-channel
steps to the fixed-point channels, pack RGB555, and decrement the remaining count.
`BindPalette` owns the behavior-flag check and drawable-specific palette binding.
The caller uses a category switch, actual manager flags, and ordinary early returns.

The availability check now uses the subsystem's existing `ActorStatus_IsDown`.
It has the same null behavior, motion flag `0x40`, status flag `0x20000`, and signed
health test as the previous helper. This is an equivalent existing operation.

The accepted source removes the fabricated zero index, six manually ordered
address locals, the private mask/count wrappers, and input pointer copies.
The real control flow and existing status operation supply the matching PRE
ordering. No declaration permutation, padding operation, weighting loop, artificial
alias, or compiler setting replaces the old form. The ordinary stock result is
exact at 468 bytes.

The two remaining literal-local warnings are the named category masks described
below. Their presence is not hidden or excused.

## Accepted work carried from checkpoint 1

| Commit | Change | Evidence and boundary |
|---|---|---|
| `d67ee08d5` | Model actor-controller velocity as signed X/Y/Z fields. | Natural 8-byte size absorbs existing padding; actor size 4,404 and field offsets stay unchanged. Seven header dependents match. |
| `3950ce30e` | Share the actual polar-velocity calculation in `actor_controller.h`. | Retreat and timed-motion users match; one old zero local disappears. Three header dependents match. |
| `4d65a5f05` | Recover Armor's fixed-speed movement through owned velocity/collider writes. | `ArmorActor_UpdateFixedSpeedMotion`, `08183E4C`, matches 448 bytes. Five warnings become zero: one proved-zero, two const-local and two literal-local. |
| `7bb4981ea` | Canonicalize the paused-particle burst interface and migrate callers. | Signed result, three const position pointers, signed count and unsigned priority/flip. All 169 header dependents match; the relevant eleven-unit census is unchanged. |
| `23c8ed5d7` | Remove the palette category predicate's impossible negative guard. | All 468 bytes match. This unreported guard earned no census credit; its separate reported zero-index site is now solved by `8f2863fc5`. |
| `5dbd9f911` | Annotate a legitimate specialization of the existing terrain walker. | One exemption, listed separately below; no fabricated-source removal credit. |

### Earlier priority source site cleared: Armor at `08183E4C`

The operation computes X/Z velocity from heading and speed, clears Y, and nudges
an exactly zero X or Z component toward the terrain-cell centre. The source now
stores through the owning three-component velocity record. The nudge uses the
actual low position byte and signed distance.

The ROM's otherwise unused zero register is produced naturally by agbcc's halfword
field-store preservation mask for `velocity.z`: RTL UID 364 creates the HI zero,
the negative X/Z nudge stores retain its uses through combine, and allocation
assigns r7. There is no replacement dummy zero expression or fake use.

The existing `*(u16 *)(s - 14)` flag clear remains raw-offset debt. The burst API
migration also leaves explicit representation-boundary casts from older vector,
array and packed-point owners. Neither is presented as solved.

## Every added exemption

**No exemption was added in checkpoint 2.** The cumulative bundle has one:

`src/spread_shot.c`, `SpreadShot_ResolveTerrain`, `08123194`, left-neighbor call:

> Shared bidirectional Seek uses its backward arm for cell - 1; the cell + 1 calls use its forward arm.

This annotates `dead-code`. The existing walker performs real forward and backward
row/column traversals at other calls. Here the cell is an unsigned halfword and the
target is `cell - 1`, so its forward arm specializes away. Left/right/runtime-target
controls distinguish specialization from a universally dead operation. The prior
implementation and `notes/cloud-08122DF4.md` establish the operation independently.

A direct backward-only alternative was tried first: 10,500 bytes versus the unit's
10,704, with 7,372 differing byte positions. The annotated original is exact at
10,704 bytes, with zero focused warnings. This does not authenticate the separate
`SlopeSurface` shaping or function-scope `cell` allocation choices still in the unit.

## Literal locals left for the owner's ruling

The current palette function retains these two warnings, without ignores:

| File / function | Local and use | Natural alternative tried |
|---|---|---|
| `src/fn/sub_08071F50.c`, `SceneActorPalette_ApplyTransition` | Category 23 `mask = 0x8000`, OR into manager flags | Direct literal changed tail sharing in the recovered-operation draft. |
| Same function | Category 10 `mask = 0x4000`, OR into manager flags | Direct literal changed tail sharing in the recovered-operation draft. |

No literal-local sweep or broad ignore pass was performed. The incidental literal
reductions came from priority source recovery. No detector change was made.

## Open priority frontiers

The four proposals in `notes/round15-open-candidates.patch` apply to this accepted
source. Their hashes and ordinary stock measurements were reproduced in the
integrated checkout. They remain **nonmatching drafts** and are not compiled by
the production build. No later easy-kind work replaced these open queue entries.

| Function | Preserved ordinary draft | One source decision still unresolved |
|---|---|---|
| `FragmentGroup_SpawnRadial`, `08014CA4` | All three zero terms removed; 704 bytes, 531 differing byte positions. | Real group/iteration and sampling lifetimes that avoid the two address hoists while retaining the random table's allocation. |
| `Slime_UpdateRandomWait`, `0808E838` | Existing shared animation operation; function remains 360 bytes. Whole 48-function unit is 15,436 bytes with 177 differing byte positions, all within this function. | The real control/flag operation that accounts for materializing mask `0x4` across `Anim_SetKeyframes`. |
| `TargetRegistry_FindById`, `0815F5B4` | Both artificial forms removed; 68 bytes, 32 differing byte positions; all registers correct. | A natural lookup/result flow that keeps the collider tail and shares the actor tail backward. |
| `Hresvelgr_UpdateBoundaryBarrage`, `081A0BC8` | Shared terrain operation and ordinary remainder; 732 bytes, two differing bytes/instructions. | Real dataflow explaining ip, rather than sl, for the relocated random mask. |

### Fragment group

The preserved draft isolates the three reported zero terms. Its inherited parallel
cursors are not claimed recovered. GCSE hoists `base + 32` and `base + 44`, and
constant equivalence halves the table's allocation priority. Earlier diagnostic-only
`AGBCC_NOPRE=1,2 AGBCC_NOEQUIV=table` reproduces all 704 bytes.

New evidence: an ordinary counted/pretested loop has **zero PRE replacements**,
but emits 700 bytes and changes initialization/lifetime allocation. Suppressing
only the table equivalence diagnostically gives 704 bytes with 41 ROM / 41 draft
scope instructions still different. Thus avoiding those two hoists alone is not
the complete source recovery.

The real singleton and resource types are now corrected in production. They do
not clear the three warnings. Natural group preparation/population boundaries,
actual indexed vertical stores, consistent sample widths/capture, a bounded slot
interface, and complete optional-range sampling were tried without an exact source
solution. The next step is the actual owner/iteration structure, not another
pointer identity or a weighted sampler macro.

### Slime animation selection

The preserved draft removes the private helper and calls existing
`SceneActorTurn_SelectProfileAnimation`. Its instruction difference is the missing
ROM `mov r8, r1` and an added alignment NOP, with six relative-offset adjustments.
These shifts account for 177 differing byte positions; this is not a two-byte match.
The unit's draft warnings would fall from 25 to 23, but none is counted as cleared.

Saved RTL establishes the boundary: both relevant branches are conditional in flow
and become unconditional during combine. The baseline mask initializer (insn 193,
pseudo 95) becomes `mov r8, r1` in global allocation/reload. The shared-helper
initializer (insn 223, pseudo 116) survives local allocation but is deleted in
`greg`. Both unreachable OR/store blocks disappear at jump2. The earlier inference
that the word-form branch died before flow was disproved by the saved traces.

The actual sequence ABI widths (`u16`, `u8`, `u8`) and keyframe widths (`u16`, `u16`,
`u16`) do not fix the discrepancy. The complete existing `DisplaySequenceControl`
byte interpretation emits 336 bytes by also simplifying the real mode-selection
branch. A private post-call narrowing would restore the known steering, so it was
not adopted or moved into a header.

### Target registry

The draft removes the registry pointer OR-zero and an unreported negative test of
a zero-extended collider ID. Jump2 merges the first comparison/result tail forward
into the actor copy, opposite to the ROM. Earlier diagnostic-only
`AGBCC_NOCROSS=67` retains the first tail and matches.

The genuine HI collider load does not distinguish the shareable tail: subsequent
comparisons, inactive tests, returns and relevant death notes are word-mode and
agree. Narrow predicates produce 80–84 bytes. Shared success/failure exits change
unconditional sharing into conditional cross-jumping, but still keep the wrong tail.
A clean single-comparison draft remains 68 bytes with five ID/kind register changes.

The requested ID remains a word: no caller establishes a narrower API, and u16/s16
trials add two prologue shifts absent from the ROM. The original non-advancing entry
behavior on an ID mismatch is preserved. The three volatile errors, including the
two elsewhere in this subsystem, remain pending behind priority work.

### Hresvelgr barrage

The initializer establishes the actual widget `paletteBank` and `palette`, palette
buffer at actor +0x424, and tile map at +0x704. State helpers establish the complete
16-byte action record. The draft uses those owners, `ContactPosition`, direct shared
sampling, actual state-one values and tick parity, and ordinary flag updates.

The new draft also uses the terrain module's existing shared operation. Both
expanded remainder expressions are replaced by ordinary `% 512` in `ScatterOffset`.
Its signed-halfword result fits the coordinate-offset domain, but the original
helper API/return width remains an explicit inference. Zero draft warnings do not
establish original-source authenticity.

Only the transfers at `081A0D2C` and `081A0D4E` differ: ROM uses ip, draft sl.
The random-mask pseudo 237 initially gets r5; reload evicts it and reuses sl. The
SI action-offset/palette literal `0x2f0` (pseudo 25) is not live during sampling.
Diagnostic-only `AGBCC_FORCE=237:12` matches; no such setting is in production.
The actual widget-bank receiver and u16 key leave that SI identity unchanged and
introduce additional differences. A cached body component also does not solve it.

## Reproduction boundaries

Apply only the desired section of the open patch in an isolated checkout. For the
single-function proposals, stock `tools/check.py` uses extents 704, 68 and 732 bytes;
for the complete Slime source, use its reviewed 15,436-byte unit extent. The four
complete source SHA-256 values are:

| Proposal | Candidate SHA-256 |
|---|---|
| Fragment | `6eca50f2c7d19d9e1523267e9ffb65ec2fd23771d3124c2b018605da3870aed1` |
| Slime | `5dde640c615006756dde57da768aff3e14f4e9b498a38a6ee145d5fc31547999` |
| Registry | `f96451a656715ccb10825f8498cb38ea760ae607bc06ed7643ae278d520ced5d` |
| Barrage | `eb472344bfcdd6b08a50df8eb51936230d9349853aa0ecfcbcfcb0cfddf15511` |

Continue at these open functions in their ROM-ordered queues. Allocation-only
source forms remain concessions even when they are outside scanner coverage.
The ownership fixes do not waive them, and near matches are not accepted matches.

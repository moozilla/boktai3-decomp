# Round 14: once-loop inventory and checkpoint-three source recovery

Tags: #round14 #matching #once-loop #source-recovery #cleanup

Date: 2026-11-16. The companion TSV preserves the complete inventory snapshot.
Replayable candidates use the canonical root ancestor `1a35f0f22ed5fc7ecfdcf9c7e4e40763d814323b` as their source-blob base.

This pass removes **19 once-loop warnings and four constant-local warnings** in
15 source units. All **135,800 bytes** of those complete units independently
match stock agbcc. Seventeen loops disappear through ordinary source operations;
two sites recover the complete shared random-table draw described below.
No new ignore, compiler setting, or linter exemption is introduced.

## Accepted source changes

| Source | Once-loop sites | Reported constant locals | Exact unit bytes | Commit |
| --- | ---: | ---: | ---: | --- |
| `actor_position_effects.c` | 1 | 0 | 1,340 | `2c5b790ca` |
| `converging_effects.c` | 1 | 0 | 1,196 | `6d4f25abe` |
| `fn/sub_08239480.c` | 1 | 0 | 364 | `7d1cac08c` |
| `fn/sub_080A8144.c` | 2 | 1 | 316 | `90e7f59a6` |
| `dog.c` | 2 | 1 | 37,088 | `18756cb69` |
| `golem_2.c` | 2 | 1 | 12,492 | `abaf7be05` |
| `mummy_entry.c` | 2 | 1 | 49,808 | `42be2887b` |
| `axe.c` | 1 | 0 | 29,148 | `1b9de3e89` |
| `fn/sub_0812F3CC.c` | 1 | 0 | 340 | `3cdeda0cc` |
| `fn/sub_0813FE30.c` | 1 | 0 | 436 | `36880fd91` |
| `contact_burst.c` | 1 | 0 | 1,080 | `9f25dd09d` |
| `fn/sub_081E2910.c` | 1 | 0 | 576 | `9f25dd09d` |
| `fn/sub_0816F6B4.c` | 1 | 0 | 180 | `4343389e9` |
| `fn/sub_0816F8AC.c` | 1 | 0 | 180 | `aa2ba1d15` |
| `fn/sub_081FF454.c` | 1 | 0 | 1,256 | `268d39283` |
| **Total** | **19** | **4** | **135,800** | |

The aggregate compiler-inclusive scans of these source units and the changed
random header fall from **102 warnings to 79**, with zero errors before and
after. Once-loops fall from 22 to 3 and constant locals from 56 to 52. The other
reported kinds in this scope remain unchanged: five statement expressions,
seven address/dereference expressions, and twelve local unions. These are scoped
counts; the coordinator performs the combined tree census and full ROM build.

**Particle sampling.** The three far-particle emitters now use the direct shared
sampler and a halfword radius sample, following the already matched
ConvergingEffects_EmitNearParticle. The sample is an actual table element used to
compute the radius. The X-offset wrapper and obsolete private sampler disappear.
The link-scaled near emitter does not match this structure and remains open.

**Control packets.** Four functions now call the existing
SceneActorChase_SetControl operation. This replaces eight nested wrappers, their
running byte offsets, and the packet's temporary callback/kind/zero setup. The
existing helper names the real step, control-kind, callback, and pending-control
fields. No shared header change was needed.

**Vector and state operations.** Axe_CopyAuxiliaryVector becomes a typed
assignment to the existing AxeAttackTrailContext position through its ChaseActor
owner. Two flag operations and the two pause-menu item-use sequences lose their
wrappers directly. HomingProjectile_UpdateDefaultState uses literals for its
state-entry checks and stores, also removing an unreported `register u32 one`
temporary. That extra local removal is not counted as a fifth warning reduction.

**Complete random draw.** Header commit `b47db9e0f` adds PARTICLE_RANDOM_DRAW;
caller commit `9f25dd09d` uses it in contact bursts and presentation shake. Its
body performs only the cursor advance and selected-halfword read. Both operations
already repeat at both sites and in the existing sampler API. The original macro
spelling is unknown. Ordinary blocks and existing clean inlines were tried first.
All **84 header-dependent units** independently match. See
`round14-random-draw-operation.md` for the source evidence and failed alternatives.

## Delegated families included in this checkpoint

These results are additional to the 15-unit totals above and have their own
complete-unit checks and source-candidate records.

| Family | Once-loop sites removed | Exact complete-unit bytes | Source commits |
| --- | ---: | ---: | --- |
| Flash and interaction markers | 5 | 904 | `72405755d`, `1a35f0f22`, `79400dfdf`, `bd5581747` |
| Palette-bank traversal | 3 | 5,416 | `8ff81912e`, `de7e3bcc1`, `7a8fec89e` |

The marker functions use direct input predicates. The flash functions use their
actual frame-range branches and display flags. The three palette routines use
indexed 16-color banks, consistent with the existing palette resource type and
interpolator. Their ordinary counted cursor versions did not match. No shared
header change or replacement wrapper was required.

Details are in `round14-flash-marker_2026-11-16.md` and
`round14-palette-traversal_2026-11-16.md`. The decimal family remains open; its
three independently checked renderers and 30 source states are documented in
`round14-decimal-rendering_2026-11-16.md`.

## Inventory by actual operation

The snapshot contains **151 physical sites: 150 unexcused warnings and one
previously annotated SDK expansion**. The TSV inventory identifies every site,
its function, operation, source evidence, disposition, and associated trial IDs.
After the accepted work above and the two delegated successful families, this
snapshot has 27 recovered/removed sites, 123 open sites, and the retained SDK site.
This is an inventory disposition, not a replacement for the coordinator's final
tree-wide count.

| Actual operation | Snapshot sites | Recovered/removed | Still open |
| --- | ---: | ---: | ---: |
| Cursor traversal or loop setup | 26 | 3 | 23 |
| Terrain coordinates or particle geometry | 10 | 3 | 7 |
| Flag, display, or control-bit updates | 23 | 7 | 16 |
| Auxiliary vector copy | 1 | 1 | 0 |
| Decimal digit remainder | 3 | 0 | 3 |
| State, parameter, animation, or UI updates | 56 | 3 | 53 |
| Highlight-pulse calculation | 4 | 0 | 4 |
| Random-table step, generation, or sampling | 7 | 2 | 5 |
| Scripted control-packet initialization | 8 | 8 | 0 |
| Standalone empty `while (0)` | 1 | 0 | 1 |
| Defeat statistics and saturation | 2 | 0 | 2 |
| Original libgcc wide-product expansion | 1 | 0 | 0 |
| Heap or registry allocation | 3 | 0 | 3 |
| Text copying or staged password validation | 2 | 0 | 2 |
| Modifier gate result | 3 | 0 | 3 |
| Collision update delay | 1 | 0 | 1 |

The SDK row remains in source with its prior annotation. Its original expansion
and unsuccessful ordinary alternatives are documented in
`round14-libgcc-multiply-source.md`; it is not part of this pass's 19 reductions.

Source provenance is separate from operation classification. One site's original
SDK source is verified. Sixteen sites have nearby source comments explicitly
describing allocation or block-layout intent. The remaining 134 have no such
proof in this inventory. A failed literal substitution or a register difference
does not establish that a named mask or local was artificial. The known
compiler-purpose comments are preserved in the TSV instead of generalized to
every warning.

## Reviewed holdouts and remaining decisions

* **Link-scaled converging particles (`082392E8`).** The matched sibling's
  halfword sample and direct operations produce 412 bytes against 408. Removing
  the redundant draw pointer gives 400 bytes; direct angle-mask literals give
  388. The closest full operation changes draw/mask allocation and final sampler
  reloads. No compensating copy or weighting construct was introduced.
* **Random draws.** TimeResult_UpdateDigitReveal's clean sampler keeps its
  2,056-byte unit but changes four bytes: the table and graphics resource swap
  r8/r9. Indexed digits and direct field arguments do not restore it. The shared
  complete-draw macro also fails there. Random_InitValueTable's ordinary store
  differs by seven bytes; matching the real unsigned Random_Next15 declaration
  and separating the shift produces nine changed bytes. Both stay open.
* **Highlight pulses.** All four ordinary removals swap the object and color
  registers and change eleven bytes. The existing eight-bit-counter pulse helper
  supplies real sibling evidence for the algorithm. Separate color computation,
  a full pulse helper, and actual frame/output field pointers were tried for the
  halfword-counter family; none matched. Four representative helper/pointer
  candidates are independently stock-checked and preserved.
* **Terrain and placement.** Ordinary shadow-height clamp blocks and an explicit
  height delta both change nineteen bytes in Actor_UpdateShadowSpriteParts.
  The grid-position sibling structure reduces the difference to three register
  choices in its 104-byte function but does not match. The ground-bob direct
  height assignment, tested after the accepted volatile-pointer cleanup, changes
  twenty-nine bytes in 480. Tether's bounds-and-row lookup remains open.
* **Text and state control flow.** Password's text loop retains a six-byte
  difference after ordinary outer-loop removal, including a candidate that
  removes its redundant character copy. Nested validation and a shared cleanup
  label each change forty-seven bytes in Password_DecodeEntry. Ordinary switch
  control flow changes eight bytes in both Actor_SelectPathNodeAction and
  ModifierMenu_DrawBackground. No ignore was inferred from the cleanup idiom.
* **Input and empty loops.** Removing the navigation-key wrapper changes twelve
  bytes while retaining the existing volatile reads; ordinary array reads produce
  64 bytes against 72. SceneActorReaction_BeginReactionSound contains a standalone
  empty `while (0)`, not a `do` wrapper. Deleting it changes five bytes. Its source
  remains visible; the detector was not weakened for that example.
* **Traversal and allocation.** Ordinary source trials cover the remaining
  cursor, pool, state-update, heap, and registry sites. Their results are in the
  evidence catalogue. The newly assigned SpriteEffectController/RouteSprite/
  Feather pool investigations continue separately for checkpoint four.
* **Previously deep-reviewed families.** The four BoardPiece_Wander wrappers,
  three palette-controller wrappers, three modifier-result wrappers, and three
  retreat/random wrappers in `0804373C` remain covered by the earlier wrapper
  and proved-zero reviews. Their status stays open; this pass does not recast
  their allocation evidence as original-source evidence.

## Durable source evidence

`round14-once-loop-inventory_2026-11-16.tsv` preserves all 151 classified sites.
`evidence/round14-once-loop-candidates_2026-11-16.jsonl` preserves **112 complete
nonmatching source candidates for 94 functions/helpers in 86 units**. Each record
contains the source path, function, hypothesis, canonical source-blob base,
unified patch, source hashes, and independent stock-check status/size/difference.
It contains no ROM data or generated assembly.

Every patch was applied independently to its named baseline source blob and the
resulting candidate SHA-256 verified: **112 of 112 replays pass**. These are
independent candidate snapshots, not a patch series. Some retain other accepted
changes present when the candidate was tested. Reconstruct a record's source
copy, apply that record's patch, and check the resulting draft against the
checkpoint's headers and stock toolchain. Production source is never replaced
by the failed candidate catalogue.

Ordinary trials were completed for 131 sites here. The other twenty sites are
covered by thirteen prior deep reviews, the five-site delegated flag review, the
delegated HUD decimal case, and the verified SDK expansion. Further source wins
after the accepted set above belong to the next checkpoint.

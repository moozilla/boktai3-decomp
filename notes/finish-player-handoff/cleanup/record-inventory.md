# Player cleanup inventory supplement

Tags: #player #structs #data #studio #register-pinning

Read-only supplement to `build/scope/campaign/sub_0814D264/round5/CLEANUP_INVENTORY.md`, which already contains the seven removal diagnostics, main callback-table decode evidence and broad type families. No cleanup source/header/data/Studio mutations or new deconcession probes were run here. Only scratch inventory/CSV artifacts were written. Remaining function matching must finish before cleanup changes.

## Scope and deconcession families

The four requested report units contain 552 functions: Player 499, burst 18, attack effect 15, clone 20; 476 source files. Current source audit has 77 concession-affected functions (73 Player,2 attack effect,2 clone), not 77 individual pins. Their total footprint is 43,040 bytes. Many functions contain both operand constraints and pins; 18 are reached through hinted helpers. This is an audit count, not a regenerated progress report.

`pinned-rom-order.tsv` sorts the existing audit by ROM address. The first is0813B18C ActorDirection_ComputeRelativeCardinal (one memory barrier); then0813B4C8 and0813B7C4. The prior inventory's small/easy table is useful evidence, but should not silently replace the user's ROM-order workflow with size order.

Largest reusable family: 25 concession-bearing files contain the same packed DisplaySequence playback operations (`playback-family.tsv`): 24 Player handlers plus081606C0 PlayerAttackEffect_AllocateParticle. Ordinary prior art already exists in08148318 Step_UpdateWaveringMovement,0816233C StepParticles, and include/display_sequence_inline.h. Their control-byte union, typed index/command readers, halfword timing and explicit value boundaries replace the old volatile/pin/barrier pattern. Some callers need the ended result and some pass a live one-bit value; preserve those interfaces and verify each caller. This is strong prior art, not a claim that all 25 removals are free.

The two history selectors08141A1C/0823C368 are source/ROM twins. The newly ordinary08141BAC/0823C4B4 smoother supplies signed-halfword sample/result boundary prior art, but its shorter loop and fallback differ: adapt the property, not the entire body. The existing removal probes remain the starting evidence.

## Additional layout ownership

Player's repeated PRecover/ReleaseAnimation views describe a drawable at14C–178 and playback at3FC–40C, followed by resource fields410/414. Canonical Player currently splits the drawable into secondaryDrawFlags, colliderPosition and padding. PlayerCueCollider spans3A8–418, so its unknown tail includes the54-byte collider followed by sequence/resource storage. An additive shared cue/display view can reconcile these overlays without replacing unrelated fields blindly.

Keep signedness, bitfield access mode and alignment explicit: SceneVec is signed and2-aligned; ContactPosition is signed and4-aligned; initializer BurstPosition uses unsigned components and4-alignment; SceneExtent is packed into u32 bitfields. Equal byte offsets are not enough evidence to substitute one for another.

The burst task extends into the neighboring Player orbit effect unit (7 functions), beyond the four requested units. Existing PlayerOrbitParticle in0815F934/0815FA78 is the same 0x3C record: position18, active28, angle29, frame2C, radius30. Task owner18, resourceF8, particlesFC, particle frame4BC and callback4C4 explain the full 0x4C8 allocation. The initializer's local task view ends4BC; it is not a complete owner declaration. Read these seven consumers before choosing the shared header. Their existing ordinary RNG code is useful matching prior art; it has been sent to the Burst finisher.

Clone consolidation and AttackMotionContext/cue-descriptor/emission-argument families remain as listed in the prior inventory. Global singleton consumers are confined to their effect/pool families in current source, so typed forward declarations for gPlayerAttackEffect and gDirectionalSpritePool are plausible once owner layouts agree. Remove the old file-local gPlayerCloneManager alias instead of adding another RAM symbol.

## Data beyond the original seven callback tables

The direct address-named ROM-reference audit finds 25 addresses in the 476 selected files (`direct-rom-references.json`). None currently lies in a data/manifest.csv entry. This is a direct-reference audit, not proof that it covers every transitive/shared dependency.

Three additional physical callback regions are08E87A10–08E87A90 (32 words),08E87A90–08E87B0C (31), and08E87B0C–08E87B78 (27). Read-only callback_array decode/encode reproduced all 360 bytes exactly; every non-null entry is a known Thumb function. Scratch CSVs and validation.json are in callbacks/.

Do not present these as independent complete logical arrays yet. Dispatchers0824261C and082429AC explicitly use indices1F/20: A10+32 points at A90, while A90+31/32 point at the first two words of B0C. A combined 90-word physical range with three alias/index views, or an explicitly documented adjacent fallback, preserves this behavior without overlapping manifest entries. The current generic callback format is sufficient for physical bytes; Studio needs alias-aware logical bounds.

Other candidate record families, pending extent/field review:

- 08E5FA6C–08E6092C:59 records of64 bytes, using WeaponCueRecord's speed/header plus five 12-byte WeaponCuePhase records.
- 08E6092C–08E60F4C:49 records of32 bytes, matching PlayerActionDescriptor; index47 is the special default. Interior assembly labels are not record boundaries.
- 08E60F4C–08E60F80:13 four-byte PlayerIndexedAction records.
- 08E6290C: short action-configuration preset passed to Player_InitializeActionState. Its next label is8 bytes later, while some PlayerActionConfig variants read farther: resolve tag-dependent readers before imposing one full struct extent.

The prior unknown08610E24–08610E2C region has a consumer:0815A97C Player_SetSlotValueFromScript, in Scene actors and NPCs / Script actor outside these four units. The first two u16 values are1800/1800 and are slot defaults; the remaining four bytes are900/0 as halfwords with no direct consumer found. Do not label all four as known slot defaults.

086141BC and086149C4 are broadly shared random/trig data, not Player-owned merely because Clone or movement reads them. Preserve their shared ownership. Assembly .4byte symbol guesses are not field typing: e.g. the random table contains a word symbolized as a ROM pointer even though it is read as two numeric samples. Typed numeric encoders must use pointer=False; real callbacks must remain relocatable pointers.

Current datafmt modules have no generic numeric-array/record codec. A reviewed fixed-width numeric/record format is needed for direction parameters, descriptor bytes/halfwords and lookup arrays; keep unknown fields visible rather than infer semantics from pointer-like numbers.

## Studio integration points

- tools/assets/actors.py command_tables() filters callback_array entries to data/actor_commands/. Player action/mode/element arrays are not script command[0] tables: generalize ownership/category/index meaning rather than hiding their files under that folder.
- tools/studio/serve.py /api/actors already returns command_tables; tools/studio/atlas.py command_table_blocks() creates exact decoded extents but hard-codes /actors#commands links. Add Player/code/table links and alias-aware views for the linked tables.
- Callback names already come from current function symbols, not the CSV informational name. Retain that behavior.
- actors.cached is process-lifetime memoization, with no manifest/table invalidation. A server restart or targeted invalidation is needed to reflect newly installed/edited tables in an existing Studio process.
- script_targets.progress() generates a temporary fresh progress report; do not rely on the intentionally stale checked-in PROGRESS.json for matching state during cleanup. It was used here only for stable unit membership and function-start addresses.

The original inventory's seven validated callback regions and small removal probes remain authoritative. This supplement adds families, dependencies and bounds caveats; it does not authorize a broad header rewrite or claim cleanup progress.

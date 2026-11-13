# Field objects and hazards: dp-field-b run

Tags: #phase3 #actors #particles #animation #tools

The run stopped at the coordinator's end-of-run instruction after processing
18 of 23 queue items. Eight functions are now concession-free (34.8% of the
assigned queue), and four additional functions have matching partial removals.
Every changed source unit passed its per-unit byte comparison. No full ROM
build was run; combined integration validation remains with the coordinator.

Concession-free functions:

- BreakableProp_EmitBreakParticles (080515DC): union random-table initializer
  prevents loop hoisting; separate shifts preserve coordinate allocation.
- ProximityEmitter_EmitParticle (08051A8C): split cursor updates and halfword
  field masks preserve the required register lifetimes and reload copy.
- ProximityEmitter_TickActive (08051BD8) and ProximityEmitter_TickRise
  (08051CB4): direct field preincrements preserve the cadence mask copies.
- StackedActor_InitRisingParticle (080521DC): halfword-returning raised-height
  helper preserves constant/load order.
- StackedActor_InitFallingParticle (0805228C): direct height-field expression
  separates its lifetime from horizontal coordinate temporaries.
- StackedActor_UpdateSegments (08052418): halfword offset helper and early
  world-height pointer recover allocation and reload order.
- WoodenCrate_CreateFromScript (080FF87C): shared DisplaySequence_StepSprite
  replaces the pinned local step while retaining a separate sprite pointer.

Matching partials:

- ProximityEmitter_InitializeFromScript (08051EA8): eight pins and four
  barriers removed; two terrain pointers remain.
- InteractionMarker_InitializeFromScript (08053AEC): nine pins and two
  barriers removed; two terrain pointers remain.
- SavedPositionMarker_InitializeFromScript (08054028): one copy barrier
  removed using a ContactPosition temporary.
- BouncingObject_SpawnFromScript (080697C8): one CpuSet destination pin and
  one barrier removed using an inline argument-preparation helper.

Ten remaining holdouts, including those four partials, have drafts under
`notes/phase3-drafts/dp-field-b/` and evidence under the dp-field-b heading in
`notes/phase3-holdouts.md`. The final item, NamedObject_RegisterFollowCells
(08101B1C), has an exact diagnostic match with object allocation priority
raised by 1027; the ordinary-C lifetime form remains unresolved.

Five queue entries were not started: ActorGroup_CreateFromScript (08104350),
Collectible_StartDeath (081153B4), Collectible_UpdateEntry (08115674),
SpriteDrop_UpdateEntry (08115E00), and sub_0817AEC8.

Tool work: guarded split-masked-update spelling with synthetic and real
compiler fixtures; diagnosis for ROM-only inverse reload copies. The root
suite passed 410 tests (two skipped), and the tools suite passed 127 tests.
Compiler facts 220–223 record the successful forms. Scope's `try` command
also exposed an unresolved alias-name crash in put_definition when a variant
uses the canonical symbol while its base defines a macro alias; retaining
the base's existing definition spelling worked around it.

Commit requests 0001–0026 contain individual matches, partials, tool changes,
and holdout records. The run summary is a final separate documentation commit.

# Field objects and hazards: dp-field-b run

Tags: #phase3 #actors #particles #animation #tools

All 23 queue items have been processed. Eleven functions are now concession-free
(47.8% of the assigned queue), and five additional functions have matching partial
removals. Twelve functions retain concessions and have documented holdouts.
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
- ActorGroup_CreateFromScript (08104350): command loads repeated in both
  animation direction arms, a mask after the join, and a do loop recover allocation.
- Collectible_StartDeath (081153B4): shared sprite playback and byte-valued
  sequence arguments preserve the call argument setup order.
- SpriteDrop_UpdateEntry (08115E00): shared sprite playback replaces the
  local animation constraints; camera and spatial-entry fields are named.

Matching partials:

- ProximityEmitter_InitializeFromScript (08051EA8): eight pins and four
  barriers removed; two terrain pointers remain.
- InteractionMarker_InitializeFromScript (08053AEC): nine pins and two
  barriers removed; two terrain pointers remain.
- SavedPositionMarker_InitializeFromScript (08054028): one copy barrier
  removed using a ContactPosition temporary.
- BouncingObject_SpawnFromScript (080697C8): one CpuSet destination pin and
  one barrier removed using an inline argument-preparation helper.
- Collectible_UpdateEntry (08115674): fourteen concessions removed using
  shared sprite playback and named fields; one hit-value pin remains.

Twelve remaining holdouts, including those five partials, have drafts under
`notes/phase3-drafts/dp-field-b/` and evidence under the dp-field-b heading in
`notes/phase3-holdouts.md`. NamedObject_RegisterFollowCells (08101B1C),
Collectible_UpdateEntry (08115674), and sub_0817AEC8 have exact diagnostic
matches from allocation-priority changes. Their ordinary-C lifetime forms
remain unresolved. The last emitter's union table initializer already prevents
loop hoisting; allocation order is its remaining blocker. No queue entries
remain unstarted. These priority diagnoses are the next places to resume.

Tool work: guarded split-masked-update spelling with synthetic and real
compiler fixtures; diagnosis for ROM-only inverse reload copies; consistent
definition matching for legacy aliases in scope's snippet replacement. The
alias fix has four regression tests covering both replacement directions,
prototypes, repeated replacement, and multiline pointer returns. The real
bouncing-object `try` reproducer now completes without an exception.
Compiler facts 220–225 record the successful forms.
Final tool validation ran 414 root tests (two skipped) and 127 tools tests;
both suites passed.

Commit requests 0001–0036 contain individual matches, partials, tool changes,
holdout records, and the earlier summary. The final summary refresh is a
separate documentation commit.

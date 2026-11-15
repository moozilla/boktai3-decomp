# SceneActorPlacement_EmitTimedParticles (08077C48): holdout

Tags: #matching #phase3

1072 bytes; 9 `index` pins, 2 `angleMask` pins, a `table` pin and 3 clobber barriers on r8/r9.
`scope.py pins` removes all 15: 165 ROM / 177 draft (`...no_pins.c` here). The index pins
group alone costs 113. The body is three switch cases of inlined random-table steps
(`index = (index + 1) & 1023` over gRandomTable, gRandomIndex written back) feeding
sub_080148CC, plus a packed-word update of the copied position's y. The likely original is a
shared random-step inline used at each draw; the nearest matched relatives
(SceneActors_SpawnDestructionParticles 08028414 and 0817ABEC) spell the step by hand with
pointer aliases, so they don't settle the helper's form. Untried: a helper per draw
(`ParticleRandom_NextSample` in monster_particle_random.h is the candidate), matchsearch.

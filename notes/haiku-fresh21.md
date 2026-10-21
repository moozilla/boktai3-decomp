# fresh21 notes (task 0801404C, particle groups)

* sub_08014330 (ParticleGroup_UpdateFrozen): NONMATCHING draft in src/fn/sub_08014330.c, 98.3%. Only the ROM's `ldrb r0; adds r3,r0,#0` copy of g->type differs.
* sub_0801404C (ParticleGroup_UpdateActive): notes/haiku-fresh21/sub_0801404C.c.txt, ~73%. Struct layout is right (PGroup/PParticle/PFx, 0x38 stride, fx at +0x10). Needed `asm("" ::: "memory")` before `p->fx.x += p->vx` to stop agbcc reusing the early x load. Open problems: the ROM keeps the rand table (0x0203B400), &gUnk_03005308 and the Effect pointer in sl/r8/sb and does not hoist the 0xFFFFFF00 mask (mine hoists 0xFF00); e lives in r3 with a caller-save spill. scale() factor is evaluated before the velocity load.
* sub_08014634 (spawn): notes/haiku-fresh21/sub_08014634.c.txt, ~56%. ROM keeps count/type/pos in stack slots; mine uses registers.
* sub_080148CC, sub_08014CA4: untouched (11-arg spawn functions of the same shape).

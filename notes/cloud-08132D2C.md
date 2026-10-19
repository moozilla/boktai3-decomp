# Cloud task 08132D2C

Matched:
* sub_08135D74 SceneActorReaction_TryStartControl1D
* sub_0813599C SceneActorReaction_ApplyEscalatingHit (two pins and one empty
  `asm("" : "+r"(ptr))` barrier; without it agbcc merges two `flags2D0 &=` updates
  around a u16 store to +0x2D6, which the ROM keeps separate)

Drafted (NONMATCHING, 84.4%): sub_08135F70. Core work lines up (regions.py core
distance 6/444). What's left is spills/allocation: the ROM spills the health and +0x2D8
pointers to sp+0x14/0x18, keeps mode in sl and a constant 1 in sb across
SceneHud_SetTimedState. rasolve with the ROM's registers forced still differs, so
some value lifetime is still wrong. Observations that got it this far:
* the hit-flags update is inlined twice. The first copy computes +0x2CC itself; the
  second reuses the caller's pointer (r7), which the caller reassigns
  (`flags2CC = &actor->flags2CC`) before each conditional branch.
* constant-before-load order needs mask locals throughout (`u32 bit = 0x8000; *p |= bit;`).

Not attempted:
* sub_081363F4 (663): a larger sibling of sub_08135F70 (same inlined flag block,
  the damage-number code from sub_0813599C, calls sub_0813599C/sub_08135D74). Start from
  the sub_08135F70 draft.
* sub_08132D2C (2149), sub_08134398 (2103): too large for this pass.

# Cloud task 080B7C10 (Skeleton states)

Drafts: sub_080B8B84 (SceneActorReaction_TrySink, 94.5%), sub_080BB61C
(Skeleton_BeginKnockedFlight, 86.5%). Their comments say what differs.

## sub_080BA268 (Skeleton_BeginBoneScatter), 63.8%, not a draft

`cloud-080B7C10/sub_080BA268.c`: 343 instructions vs the ROM's 346. Leads:

* The polar helper's z index keeps `& 0xFF` in the ROM even for the random heading. The
  heading is `NextRandom() & 0xFF`, which combine later turns into `ldrb`.
* Collider_SetAttackProperties: the ROM evaluates the field arguments through one walking
  base register (actor+0xBA, +8, +0x16, -0xE) and computes the collider argument (r0) last.
* The first facing vector (sp+0x2C) is computed and never used, but its stores stay.

Not attempted (rationed): sub_080B7C10, sub_080B9158 (about 1600 instructions each) and
sub_080BBDD0 (agent draft on main).

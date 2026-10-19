# Cloud task 08231A0C

Matched: sub_08231A0C EffectPart_UpdateAnimation, sub_08231C84 (clone), sub_08231B3C
EffectPart_UpdateBlink, sub_08231DB4 (clone), sub_082327E0 ColliderSpritePool_Update
(FallingObjectManager_Update with its own object layout).

Not matched: sub_08233760 (Task_Kill after a burst of particles when the object is above the
terrain height). The structure is right (core distance 2/133); allocation is not. rasolve gets
to 51 bytes with the ROM registers forced. The best attempt is in wip/ locally and not
committed. Pinning table/seed/index (r6/sl/r2) got to 201 differing bytes.

Also matched from relative ports outside this task (same PR): sub_080B04F4, sub_080D211C,
sub_080D622C, sub_0809C834. sub_08183D9C is close (ROM rematerializes the +0xB32 address;
agbcc keeps it in r5 and pushes r6).

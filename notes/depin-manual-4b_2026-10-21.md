# depin-manual-4b worklog (2026-10-21)

Branch cloud/depin-manual-4b. List: wip/depin_list4.tsv (40 functions). Method: `depin.py --write` over all 40, then permuter (stripped copies in wip/depin4/, 240 s, -j 1, queue script wip/depin4/queue.sh), ported by hand and re-checked with check.py. Hand attempts were only productive when the permuter had shown the shape.

## Clean (no pins, no asm)

* sub_0808E780: depin.py alone (the `+r` barrier on q was unneeded).
* Wireless_OnChildTransferEvent (0803614C): by hand. `*slots |= (u8)gUnk_03005390.code;` directly (no pinned r1 temp); drop the pinned `key` copy and use the u8 parameter `command` for the late 0xF2/0xFF compares.
* BouncingObject_UpdatePool (08069510): permuter. Reuse the `offset` local as the return value: `offset = 0;` after computing guard, `return offset;`.
* SceneHud_AnimateActionResult (0816F340): permuter. `short index;` and the mode constant 1 via `u16 k2 = 1; mode = k2;`.
* ActorAttachment_UpdateValueFrame (08065904): permuter. `s16 new_var; new_var = *value; current = new_var;`.
* SceneInventory_BeginConsumableRemoveMove / SpecialMove (0806097C, 08060B94): permuter. `start[1].x` through the local pointer but `starts[1].y/.z` through the array.
* Vanargand_UpdatePartFadeOut (081B5CCC): permuter. `(u8 *)p - (-offset)` / `- (-off)` address forms and a `u32 k2 = 0x194` local for the 0x194 load.
* LevelAdvanceDisplay_StartParticle (08056F88): permuter. `index = ((*(&index)) + 1) & mask;` for the fourth increment.
* Lun_EnterIdleOrCopyCell (0809D424): permuter. `zero_2 = s->hflags; zero = zero_2 & mask;` (separate u16 temp).
* Hresvelgr_UpdateRandomAnimation (081A117C): permuter. Address-taken temp `new_var3 = &new_var2` (new_var2 = actor) used for the motion stores and the SetAnimation call, `new_var = 0` stored to motion[2].
* Slime_FollowPlayer (0808FD9C): permuter. `int new_var = 0` as the Chase_SetState args, `new_var2 = -0x2C; mask = st + new_var2`.

## Not solved

Everything else in the list (28 functions). 300 s of permuter found nothing for them; notable near misses:
* ScenePanelRig_CheckContactTransition: only dz lands in r1 instead of r0.
* SceneHud_TickActionResult: commutative operand order (`adds r2,r1,#0; ands r2,r0` vs the reverse), the known unsolved pattern.
* TransientSpriteEffects_Update: unpinned stripped copy is the right size; `sprite` is derived from the view (r1+0x44) instead of from `slot` (r2+0x2c) and one temp lands in r1 instead of r0.
* Wireless_SelectParentFromInput: stripped is 4 bytes over; pressed/old swap r5/r6.
* sub_081D0864, ActorImpact_CopyContact: single barrier kept, nothing tried (deref, do-while, copy removal) gave a match.

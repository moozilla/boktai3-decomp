# Field objects and hazards: open near misses (wip/field2)

- 08104F98 CallbackPool_ApplyDamage (99.3%, pinned): unpinned, rasolve wants target's priority above
  the &entry->damage pseudo (needs ~1 more use or a longer damage-pointer life). A local copy after
  the hitFlags test gets the allocation right but moves `mov r8, r1` below the test.
- 08017684 (98.3%): the ROM puts `movs r7, #0x1f` between the hoisted &p->anim and -4; every loop
  form tried puts the counter before or after both.
- 081055C0 ProximityActor_SetState (93.8%): the ROM leaves the 0x0203B400 table and 127 spilled
  (rematerialized per iteration); ours caller-saves r3. A numeric table address gives 77%.
- 08113920 NativeEffect_UpdateAcceleratingTrail (95.4%): the constant 1 is CSE'd between `timer & 1`
  and the particle argument; a u8 helper un-CSEs it but spills &timer (frame 0x28 vs 0x24).
- 0810621C RegionActor_UpdateHealthAnimation (98.4%): the ROM copies the sprite alias (mov r7, ip)
  before the gUnk_020001B4 test; the alias comes from the inlined Step's `sprite = sp`.
- 08010298 (67%, wip only): the ROM does not hoist the timer zero or the `active` 1 (r8), or the
  random table; ours hoists all three.
- 0817AEC8 (60%, wip only): the sine table pointer is hoisted (4*1*28 >= 106 insns); the ROM keeps
  it in the loop. Order of hoisted values already matches (i, table, &v, cursor, 1023).
- 0812D064 (66%, wip only): sibling of SceneSlot_EmitPulse (same dead 0x0403B714 literal); a port
  of that function's pinning still differs in the second RNG half.
- 0817B2DC: draft committed at 89.6%.

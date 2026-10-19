# Cloud task 08132748: drafts

`sub_0813599C` was matched exactly by cloud task 08132D2C (PR #281): see its `Fill(&zero, dst)` inline, its HUD arguments computed into locals in ROM order, and its `asm("" : "+r")` barrier on `flags2D0`. `sub_08134398` is a NONMATCHING draft (90%). `sub_08132904` stays a draft here (below 80%).

* `sub_0813599C` (matched in PR #281; the notes below are what my draft got wrong). This is the category-1 hit reaction: hit history at 0x5B8, damage `1 << *(u8 *)0x47E`, the HUD timed state, the damage number, and the route restart.
  The control flow and instruction count are right. Remaining differences:
  * In the ROM, `flags2D0` is reloaded after the `flags2D6` store. The draft keeps it in a register.
  * The `field2E4` store is followed by a reload (`strh r4; ldrh r1`) that the draft doesn't produce.
  * `SceneHud_SetTimedState` loads its arguments in a different order. The ROM derives the 0xB2 and 0x9A addresses from the 0xB0 address.
  * The CpuSet control literal is loaded last in the ROM.
  * The zero word must sit below the size buffer. A `struct { u32 zero; u32 size[2]; }` gets that layout.
  A 15-minute permuter run (`--focus auto`) found no improvement over score 2890.
* `sub_08132904.c` (489 insns; fuzzy about 58%). This is the generic hit reaction: area-effect gain, random knockback probe, Atan2 heading, then either the defeat path (the same tail as `sub_08132748`) or stagger control kind 18.
  The structure is drafted. Register and spill allocation differ a lot: the ROM spills dx/dz to sp+0x14/0x18 and the collider/position/timer pointers to sp+0x1C..0x28.

Both share the `ShowsDamage` inline (a link-mode owner bit test through a switch on `gUnk_020004B4->localPlayer`).
`sub_08132D2C` (2149 insns) and `sub_08134398` (2103 insns) were not attempted.

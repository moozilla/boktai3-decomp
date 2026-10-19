# Cloud task 08122DF4

* `08122DF4` (`SequencedSprite_StepAnimation`) and `08126170` (`DriftingSprite_Update`) both inline the
  display-sequence Step that `SceneParticleBurst_UpdateSlot` and `include/route_sprite_playback.h` also use.
  Start any `ldrh [rS, #8]; lsls #1; ldr [..]; ... ands #0x30; lsrs #4` function from those.
  Two details mattered:
  * the sprite pointer is a separate variable from the object pointer (`adds r5, r7, #0`);
  * the first `commands` read goes through the object (`[r5, #0x48]`), not the state pointer. Copy the inline
    and give it the extra pointer.
* **Technique: one pointer variable reused for several fields** (08126170). The `delay` and `riseSteps`
  pointers were global pseudos that got r1, while the ROM has r2. regsolve reported "class or call-crossing; no reordering
  reaches it". Writing `counter = &s->hidden; ... counter = &s->delay; ... counter = (u16 *)&s->riseSteps;`
  (one variable for all three) makes them one pseudo, which then takes r2 from the first use.
  The first use then needed `asm("" ::: "memory")` after the flag store, because the ROM reloads the counter there.

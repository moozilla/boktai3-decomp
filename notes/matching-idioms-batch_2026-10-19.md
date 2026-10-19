# Matching idioms from the 2026-10-19 cloud batch

Tags: #matching #agbcc #techniques

Idioms that closed functions in cloud tasks 08180FE0, 08182918, 08183E4C,
0811BD30, 08120DAC, 0813A620 and neighbours. Each line names a function that
uses it.

* **OR with constant in the destination.** ROM `ldrh r1,[rX]; movs r0,#4;
  orrs r0,r1; strh r0,[rX]` comes from
  `u32 old = *fl; u32 set = 4; set |= old; *fl = set;`, not `*fl |= 4`.
  Fixed the last 2 bytes of sub_08183078 and sub_081846F4.
* **Dead zero load.** A `movs rN,#0` with no visible use: an `s16 zero = 0`
  assigned between a compute and its store, then used only in later compares
  (sub_081826C8, sub_08182918). The same shape appears in ~30 unmatched ROM
  functions.
* **Constant locals declared first** hoist the `movs`/`ldr` before the loads
  that use them: `u32 mask = 0x60000;`, `u8 one = 1; obj->flags |= one;`,
  `u32 four`, `u32 bit = 4` (sub_08184D18, sub_0811FAB8, sub_08120DAC).
* **State advance with reset:** `u8 v = *s + 1; u8 zero = 0; *s = v;
  p[0xB42] = zero;` orders the zero load as the ROM does (sub_081842B0).
* **static inline helper taking the pointer as a parameter** stops agbcc
  reusing an address it just computed, giving the ROM's second address load
  (sub_081842B0), or splits a variable into a second pseudo so it gets a
  different register (sub_081200C8).
* **Shared pointer locals across branches:** declare `dest`, `source`,
  `saved` once at function scope and reuse them in every branch; argument
  registers then stay put until the call (sub_08181E78, 48% → 100%).
* **Struct padding:** a struct-typed s16 x,y,z member takes 8 bytes; vectors
  addressed straight off sp must sit in one struct of locals, a separate
  array gives `add rX,sp,#N; adds r0,r0,rX` (sub_0811FAB8, sub_08120DAC).
* **Loop and switch shapes:** `if (i < n) do { … } while (++i < n);`
  (sub_08120DAC); a `loop:` label before `switch` with `goto loop` for a case
  that re-dispatches (sub_081866E0); `song = X; goto play;` shares one tail.
* **Small expressions:** `& 0xFF` instead of a `(u8)` cast avoids
  `lsl`/`lsr`; `(s16)(cam->x - 16)` keeps the subtraction on the camera
  value; `s8 slot = self->slot;` gives `ldrb; lsl; asr`; read a bit index into
  a local before the mask address (`s32 b = p->bit;`) to order the loads
  (sub_08182B14, sub_0813A620, sub_0812266C).
* **Packed bitfield** `struct Slot { u8 v:8; } __attribute__((packed));`
  keeps `ldrb; orrs; strb` for `|= 0xff`.

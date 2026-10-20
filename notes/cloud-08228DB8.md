# Cloud task 08228DB8: SunClock_GetPhase (and Camera_SetPosition)

Both matched exactly on 2026-10-20; the work is written up as a devlog artifact
(project thread "Decomp devlog artifact").

* `08225F88` Camera_SetPosition: the projection tail is Camera_Initialize's
  `updateScroll` inline (tools/relatives.py found it at 46% coverage). Writing
  it out by hand put the output pointer load in the wrong place; calling the
  inline matched on the second draft.
* `08228DB8` SunClock_GetPhase took 13 drafts. What mattered, in order:

## Techniques

* **Nest by the first comparison the ROM makes.** The ROM compares the base
  hour with hour A first, so the source is a three-way split on that
  (`<`, `==`, `>`), with B handled inside each arm. A boolean formulation
  (`A <= B ? now >= A && now < B : ...`) tests A against B first and never
  recovers (realigned 128 -> 53 differing instructions with the nesting).
* **Store-flag presets come from statements, not expressions.** `day = x < y`
  and `if (...) day = 1; else day = 0;` all canonicalize to the same RTL here;
  `day = 0; if (x < y) day = 1;` and `day = 1; if (x < y) day = 0;` give the
  `movs rD, #K` before the compare that the ROM has (53 -> 18).
* **A first operand loaded before the flag is a local.** The ROM's
  `ldrb r0, [m]; movs r3, #0; ldrb r1, [mB]` needs `u8 m = C.baseMinute;
  day = 0; if (m < C.minuteB) ...`: the local's load comes first and
  local-alloc's update_equiv_regs moves the second load down to its use.
* **`ldr r0, =X; ...; adds r4, r0, #0` is a reaching-register copy.**
  Accessing `gUnk_03005430.field` directly (no pointer local) gave the copy;
  with `struct SunClock *c` there is none.
* **Branch targets of a same-hour window depend on its if/else shape.**
  With registers forced, two `blo` targets still differed. Of ten spellings,
  only `if (m < b) day = 1; else { if (m < a) day = 0; else day = 1; }`
  threads the first compare straight to the result (tools/variants.py with
  `AGBCC=build/agbcc-instr/gcc/agbcc AGBCC_FORCE_LABEL=day:3`, which scores
  structure with the allocation fixed).
* **Pins made it worse, compiler-space forcing did not.**
  `register int day asm("r3")` broke the flag threading (30 structural
  differences); `AGBCC_FORCE_LABEL=day:3` in the instrumented compiler kept the
  structure and matched. Forcing the pointer to r4 by a pin broke CSE of the
  field loads (126 differing instructions).
* **The last swap is a priority inequality.** rasolve.py: the clock address
  reaching register (26 uses, live 296, 0.351) outranked `day` (19 refs,
  live 240, 0.317). Empty `do { day = 0; } while (0)` around the four presets
  (three suffice) add a loop-depth weight to those refs: 23 refs, 0.383, and
  day takes r3. translate.py searched for a lower address priority instead and
  did not find a C edit within its budget.

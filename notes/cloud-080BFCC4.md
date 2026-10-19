# Cloud task 080BFCC4

Matched: sub_080C44E4 (Actor_StepTowardTargetAxis), sub_080C2BE0
(SceneActorControl_UpdateProbeApproach; one empty `asm("" : "+r"(actor))` barrier,
marked as a register-pinning concession).

Not attempted, boundary problems in the split:

* sub_080BFCC4 (1077 insns) has no epilogue of its own: it continues into
  sub_080C0644, which is just its tail (`adds; str; add sp,#0x9c; pop ...`).
  The real function is 080BFCC4..080C0650.
* sub_080C12FC (1903) and sub_080C23FC (884) are one function: 080C23FC has no
  prologue, uses r7/sl/sp+0x58 set up by 080C12FC, and 080C12FC reaches it with
  far `bl sub_080C23FC` / `bl _080C2BC2` jumps. Together about 2790 instructions,
  ending just before sub_080C2BE0.
* sub_080C09F8 (1013) starts with `push {r5, r6, r7}` and no `lr`, so its start
  is probably also misplaced.

These need boundary fixes in the split before anyone tries to match them.

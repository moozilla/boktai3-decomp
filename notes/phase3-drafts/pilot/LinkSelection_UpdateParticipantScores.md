# LinkSelection_UpdateParticipantScores (0810BBD4): holdout

Tags: #matching #phase3

The landed match has 7 pins and 6 operand barriers. The plain rewrite here (for loop, the
file's Time/Positive/Negative/MarkAbsent inlines called directly) is 21 ROM / 19 draft, shape
20. What is left:

* `bonus` and `positive` swap registers (ROM bonus r5, positive ip; draft bonus ip, positive r5).
  Writing `bonus = -10; bonus -= Div(...)` (the ROM sets -10 before the call) makes bonus r5 but
  pushes p into sl (class STACK_REG, `why all`) and i into r7: 53/55; `AGBCC_PRI=p:+354,i:+138`
  brings that to 33/35, and then the giv for `i * 2` starts as a copy of i instead of `movs r7, #0`.
* Positive's two zero paths: the ROM keeps `movs r0, #0; b` for null results and
  `movs r1, #0; mov ip, r1; b` for an invalid index; the draft threads the null case.
* The absent loop: the ROM loads 0xFFFF into r2 and copies it to r1 (`adds r1, r2, #0`); a
  hoisted `u32 absent = 0xFFFF` local makes it worse (21/19, shape 22).

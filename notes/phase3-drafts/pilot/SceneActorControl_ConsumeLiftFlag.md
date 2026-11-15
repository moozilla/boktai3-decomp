# SceneActorControl_ConsumeLiftFlag (080837D4): holdout

Tags: #matching #phase3

The landed match pins `bit`/`old` to r0/r1 and keeps `byteZero` opaque with
`asm("" : "+r"(byteZero) : "r"(old))`.

The best plain draft here (`SceneActorControl_ConsumeLiftFlag.c`) writes both ORs constant
first (README fact 33: `bit = 8; old = ...; bit |= old;`), which fixes the operand ties: 23 ROM /
21 draft, shape 4. What remains is one mechanism: the ROM keeps two zero registers, `movs r4, #0`
(set between the parameter byte load and its orr, used by `str` to counter2AC) and `movs r3, #0`
(set between the flags2D4 load and its orr, used by the two `strb` to state2A8/phase2A9). In
every draft cse merges the second zero into the first (the byte stores read the word zero's
register), so wordZero lands in r1 instead of r4 and the push loses r4.

Tried with no change: every typed pairing (u32/u16/u8/s8/s16 for either zero, the narrower one
first); the header state setters (Chase_SetState, SceneActorTerrain_EnterState) don't place a zero
early. `probe` finds no cse/gcse decision that helps (CSE_NOCANON=23 makes it worse).
